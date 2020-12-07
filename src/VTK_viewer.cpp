/*
      This file is part of VTK_viewer
      Copyright (C) 2020 Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "VTK_viewer.h"
#include "quicky_exception.h"
#include <vtkNamedColors.h>
#include <vtkChartXYZ.h> // Module vtkChartsCore
#include <vtkPlotSurface.h>
#include <vtkContextView.h> // Module vtkViewsContext2D
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkContextScene.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContextMouseEvent.h>
#include <vtkColorSeries.h>
#include <vtkChartXY.h>
#include <vtkAxis.h>
#include <vtkTextProperty.h>
#include <vtkChartLegend.h>
#include <vtkPlot.h>
#include <vtkPen.h>
#include <iostream>
#include <type_traits>

namespace VTK_viewer
{
    template<class... TYPE> struct visitor_helper : TYPE... { using TYPE::operator()...; };
    // Deduction guide ( not used in C++20)
    template<class... TYPE> visitor_helper(TYPE...) -> visitor_helper<TYPE...>;

    //-------------------------------------------------------------------------
    VTK_viewer::VTK_viewer(const std::string & p_file_name)
    {
        std::ifstream l_input_file;
        l_input_file.open(p_file_name);

        if(!l_input_file.is_open())
        {
            throw quicky_exception::quicky_runtime_exception(R"(File ")" + p_file_name +R"(" do not exist)", __LINE__, __FILE__);
        }

        typedef enum class file_type {SURFACE, HISTOGRAM, LINE_PLOT, NONE} t_file_type;
        t_file_type l_file_type = t_file_type::NONE;
        typedef enum class mode {SURFACE_PARAM, GRAPH_PARAM, GRAPH_LEGEND, TABLE_DATA, HEADER} t_mode;
        t_mode l_mode = t_mode::HEADER;

        unsigned int l_current_table_row = 0;
        std::pair<unsigned int, unsigned int> l_table_data_dim;

        while(!l_input_file.eof())
        {
            std::string l_line;
            std::getline(l_input_file, l_line);
            if(!l_line.empty())
            {
                std::cout << R"(Line = ")" << l_line << R"(")" << std::endl;
                switch(l_mode)
                {
                    case t_mode::SURFACE_PARAM:
                    {
                        std::stringstream l_line_stream;
                        l_line_stream << l_line;
                        auto & l_surface =std::get<surface>(m_displayable_object);
                        l_surface.set_header(l_line_stream);
                        l_table_data_dim = std::make_pair(l_surface.get_dim_X(), l_surface.get_dim_Y());
                        l_mode = t_mode::TABLE_DATA;
                        break;
                    }
                    case t_mode::GRAPH_PARAM:
                    {
                        std::stringstream l_line_stream;
                        l_line_stream << l_line;
                        graph_based & l_graph_based = t_file_type::HISTOGRAM == l_file_type ?  (graph_based&)std::get<histogram>(m_displayable_object) : (graph_based&)std::get<line_plot>(m_displayable_object);
                        l_graph_based.set_header(l_line_stream);
                        l_table_data_dim = std::make_pair(l_graph_based.get_width(), l_graph_based.get_nb_series());
                        l_mode = t_mode::GRAPH_LEGEND;
                        break;
                    }
                    case t_mode::GRAPH_LEGEND:
                    {
                        std::stringstream l_line_stream;
                        l_line_stream << l_line;
                        graph_based & l_graph_based = t_file_type::HISTOGRAM == l_file_type ?  (graph_based&)std::get<histogram>(m_displayable_object) : (graph_based&)std::get<line_plot>(m_displayable_object);
                        l_graph_based.set_legend(l_line_stream);
                        l_mode = t_mode::TABLE_DATA;
                        break;
                    }
                    case t_mode::TABLE_DATA:
                    {
                        if(l_current_table_row >= l_table_data_dim.second)
                        {
                            throw quicky_exception::quicky_logic_exception("to many data compared to declared line number", __LINE__, __FILE__);
                        }
                        std::stringstream l_line_stream;
                        l_line_stream << l_line;
                        unsigned int l_value;
                        for(vtkIdType l_column_index = 0; l_column_index < l_table_data_dim.first; ++l_column_index)
                        {
                            l_line_stream >> l_value;
                            switch(l_file_type)
                            {
                                case t_file_type::SURFACE:
                                    std::get<surface>(m_displayable_object).set_value(l_current_table_row, l_column_index,l_value);
                                    break;
                                case t_file_type::HISTOGRAM:
                                    std::get<histogram>(m_displayable_object).set_value(l_current_table_row, l_column_index, l_value);
                                    break;
                                case t_file_type::LINE_PLOT:
                                    std::get<line_plot>(m_displayable_object).set_value(l_current_table_row, l_column_index, l_value);
                                    break;
                                case t_file_type::NONE:
                                    throw quicky_exception::quicky_logic_exception("File tpye is not defined", __LINE__, __FILE__);
                            }
                        }
                        ++l_current_table_row;
                        break;
                    }
                    case t_mode::HEADER:
                        if("surface" == l_line)
                        {
                            l_mode = t_mode::SURFACE_PARAM;
                            l_file_type = t_file_type::SURFACE;
                            m_displayable_object = surface();
                        }
                        else if("histogram" == l_line)
                        {
                            l_file_type = t_file_type::HISTOGRAM;
                            l_mode = t_mode::GRAPH_PARAM;
                            m_displayable_object = histogram();
                        }
                        else if("line_plot" == l_line)
                        {
                            l_file_type = t_file_type::LINE_PLOT;
                            l_mode = t_mode::GRAPH_PARAM;
                            m_displayable_object = line_plot();
                        }
                        else
                        {
                            throw quicky_exception::quicky_logic_exception(R"(Unsupported header ")" + l_line + R"(")", __LINE__, __FILE__);
                        }
                        break;
                }
            }
        }
        l_input_file.close();
    }

    //-------------------------------------------------------------------------
    void
    VTK_viewer::display() const
    {
        std::visit(visitor_helper{ [=](const surface & p_surface)
                                   {this->display(p_surface);}
                                 , [=](const histogram & p_histogram)
                                   {this->display(p_histogram);}
                                 , [=](const line_plot & p_line_plot)
                                   {this->display(p_line_plot);}
                                 }
                  ,m_displayable_object
                  );
    }

    //-------------------------------------------------------------------------
    void
    VTK_viewer::display(const surface & p_surface) const
    {
        vtkNew<vtkNamedColors> l_colors;

        vtkNew<vtkChartXYZ> l_chart;
        l_chart->SetGeometry(vtkRectf(10.0, 10.0, 630, 470));

        vtkNew<vtkPlotSurface> l_plot;

        vtkNew<vtkContextView> l_view;
        l_view->GetRenderer()->SetBackground(l_colors->GetColor3d("Silver").GetData());
        l_view->GetRenderWindow()->SetSize(640, 480);
        l_view->GetScene()->AddItem(l_chart);

        // Set up the surface plot we wish to visualize and add it to the chart.
        l_plot->SetXRange(0, (float)p_surface.get_dim_X());
        l_plot->SetYRange(0, (float)p_surface.get_dim_Y());
        l_plot->SetInputData(p_surface.get_table());
        l_plot->GetPen()->SetColorF(l_colors->GetColor3d("Tomato").GetData());
        l_chart->AddPlot(l_plot);

        l_view->GetRenderWindow()->SetMultiSamples(0);
        l_view->GetInteractor()->Initialize();
        l_view->GetRenderWindow()->Render();

        // rotate
        vtkContextMouseEvent l_mouse_event;
        l_mouse_event.SetInteractor(l_view->GetInteractor());

        vtkVector2i pos;
        vtkVector2i lastPos;
        l_mouse_event.SetButton(vtkContextMouseEvent::LEFT_BUTTON);
        lastPos.Set(100, 50);
        l_mouse_event.SetLastScreenPos(lastPos);
        pos.Set(150, 100);
        l_mouse_event.SetScreenPos(pos);

        l_chart->MouseMoveEvent(l_mouse_event);

        l_view->GetInteractor()->Start();
    }

    //-------------------------------------------------------------------------
    void
    VTK_viewer::display(const histogram & p_histogram) const
    {
        display(p_histogram, vtkChart::BAR, 0);
    }

    //-------------------------------------------------------------------------
    void
    VTK_viewer::display(const line_plot & p_line_plot) const
    {
        display(p_line_plot, vtkChart::LINE, 2);
    }

    //-------------------------------------------------------------------------
    void
    VTK_viewer::display(const graph_based & p_graph_base,
                        int p_type,
                        float p_width
                       ) const
    {
        // Colors
        vtkNew<vtkColorSeries> l_color_series;
        l_color_series->SetColorScheme(vtkColorSeries::BREWER_SEQUENTIAL_BLUE_PURPLE_3);

        vtkNew<vtkNamedColors> l_colors;
        vtkColor3d l_background_color = l_colors->GetColor3d("Grey");
        vtkColor3d l_axis_color = l_colors->GetColor3d("Black");
        vtkColor3d l_title_color = l_colors->GetColor3d("White");

        // Set up a 2D scene, add an XY chart to it
        vtkNew<vtkContextView>  l_view;
        l_view->GetRenderer()->SetBackground(l_background_color.GetData());
        l_view->GetRenderWindow()->SetSize(640, 480);
        vtkNew<vtkChartXY>  l_chart;
        l_view->GetScene()->AddItem(l_chart);

        // Set various properties
        vtkAxis * l_x_axis = l_chart->GetAxis(vtkAxis::BOTTOM);
        l_x_axis->SetTitle(p_graph_base.get_x_axis());
        l_x_axis->GetTitleProperties()->SetColor(l_axis_color.GetData());
        l_x_axis->GetTitleProperties()->SetFontSize(16);
        l_x_axis->GetTitleProperties()->ItalicOn();
        l_x_axis->GetLabelProperties()->SetColor(l_axis_color.GetData());

        vtkAxis * l_y_axis = l_chart->GetAxis(vtkAxis::LEFT);
        l_y_axis->SetTitle(p_graph_base.get_y_axis());
        l_y_axis->GetTitleProperties()->SetColor(l_axis_color.GetData());
        l_y_axis->GetTitleProperties()->SetFontSize(16);
        l_y_axis->GetTitleProperties()->ItalicOn();
        l_y_axis->GetLabelProperties()->SetColor(l_axis_color.GetData());

        l_chart->SetTitle(p_graph_base.get_title());
        l_chart->GetTitleProperties()->SetFontSize(24);
        l_chart->GetTitleProperties()->SetColor(l_title_color.GetData());
        l_chart->GetTitleProperties()->BoldOn();
        l_chart->SetShowLegend(true);
        l_chart->GetLegend()->SetHorizontalAlignment(vtkChartLegend::LEFT);
        l_chart->GetLegend()->SetVerticalAlignment(vtkChartLegend::TOP);

        vtkPlot * l_line;
        for(int l_index_serie = 0; l_index_serie < (int)p_graph_base.get_nb_series(); ++l_index_serie)
        {
            l_line = l_chart->AddPlot(p_type);
            l_line->SetColor( l_color_series->GetColorRepeating(l_index_serie).GetRed() / 255.0
                            , l_color_series->GetColorRepeating(l_index_serie).GetGreen() / 255.0
                            , l_color_series->GetColorRepeating(l_index_serie).GetBlue() / 255.0
                            );
            l_line->SetInputData(p_graph_base.get_table(), 0, l_index_serie + 1);
            if(p_width)
            {
                l_line->SetWidth(p_width);
            }
        }

        //Finally render the scene and compare the image to a reference image
        l_view->GetRenderWindow()->SetMultiSamples(0);
        l_view->GetRenderWindow()->Render();
        l_view->GetInteractor()->Initialize();
        l_view->GetInteractor()->Start();
    }
}

// EOF