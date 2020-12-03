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

#include <vtkActor.h>
#include <vtkChartXYZ.h> // Module vtkChartsCore
#include <vtkPlotSurface.h>
#include <vtkContextView.h> // Module vtkViewsContext2D
#include <vtkContextScene.h>
#include <vtkFloatArray.h>
#include <vtkContextMouseEvent.h>
#include <vtkPen.h>
#include <vtkTable.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkColorSeries.h>
#include <vtkChartXY.h>
#include <vtkAxis.h>
#include <vtkTextProperty.h>
#include <vtkPlot.h>

#include <array>
#include <fstream>
#include <sstream>

//-----------------------------------------------------------------------------
void size_table( vtkSmartPointer<vtkTable> p_table
               , unsigned int p_nb_row
               , unsigned int p_nb_column
               )
{
    for(vtkIdType l_index = 0; l_index < p_nb_column; ++l_index)
    {
        p_table->AddColumn(vtkNew<vtkTypeInt32Array>());
    }
    p_table->SetNumberOfRows(p_nb_row);
    for(vtkIdType l_row_index = 0; l_row_index < p_nb_row; ++l_row_index)
    {
        for(vtkIdType l_column_index = 0; l_column_index < p_nb_column; ++l_column_index)
        {
            p_table->SetValue(l_row_index, l_column_index, 0);
        }
    }

}

//-----------------------------------------------------------------------------
void display_surface(vtkSmartPointer<vtkTable> p_table
                    , unsigned int p_max_x
                    , unsigned int p_max_y
                    )
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
    l_plot->SetXRange(0, (float)p_max_x);
    l_plot->SetYRange(0, (float)p_max_y);
    l_plot->SetInputData(p_table);
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

//-----------------------------------------------------------------------------
void display_histogram( const vtkStdString & p_title
                      , const vtkStdString & p_X_title
                      , const vtkStdString & p_Y_title
                      , vtkSmartPointer<vtkTable> p_table
                      , unsigned int p_nb_series
                      )
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
    l_x_axis->SetTitle(p_X_title);
    l_x_axis->GetTitleProperties()->SetColor(l_axis_color.GetData());
    l_x_axis->GetTitleProperties()->SetFontSize(16);
    l_x_axis->GetTitleProperties()->ItalicOn();
    l_x_axis->GetLabelProperties()->SetColor(l_axis_color.GetData());

    vtkAxis * l_y_axis = l_chart->GetAxis(vtkAxis::LEFT);
    l_y_axis->SetTitle(p_Y_title);
    l_y_axis->GetTitleProperties()->SetColor(l_axis_color.GetData());
    l_y_axis->GetTitleProperties()->SetFontSize(16);
    l_y_axis->GetTitleProperties()->ItalicOn();
    l_y_axis->GetLabelProperties()->SetColor(l_axis_color.GetData());

    l_chart->SetTitle(p_title);
    l_chart->GetTitleProperties()->SetFontSize(24);
    l_chart->GetTitleProperties()->SetColor(l_title_color.GetData());
    l_chart->GetTitleProperties()->BoldOn();

    if(l_color_series->GetNumberOfColors() < (int)p_nb_series)
    {
        throw std::logic_error("to many series compared to number of color");
    }

    vtkPlot * l_line;
    for(int l_index_serie = 0; l_index_serie < (int)p_nb_series; ++l_index_serie)
    {
        l_line = l_chart->AddPlot(vtkChart::BAR);
        l_line->SetColor( l_color_series->GetColor(l_index_serie).GetRed() / 255.0
                        , l_color_series->GetColor(l_index_serie).GetGreen() / 255.0
                        , l_color_series->GetColor(l_index_serie).GetBlue() / 255.0
                        );
        l_line->SetInputData(p_table, 0, l_index_serie + 1);

    }

    //Finally render the scene and compare the image to a reference image
    l_view->GetRenderWindow()->SetMultiSamples(0);
    l_view->GetRenderWindow()->Render();
    l_view->GetInteractor()->Initialize();
    l_view->GetInteractor()->Start();
}

//-----------------------------------------------------------------------------
int main( int p_argc
        , char * p_argv[]
        )
{
    if(p_argc != 2)
    {
        std::cerr << "Usage is VTK_viewer <input_file>" << std::endl;
        return -1;
    }
    std::ifstream l_input_file;
    l_input_file.open(p_argv[1]);

    typedef enum class file_type {SURFACE, HISTOGRAM, NONE} t_file_type;
    t_file_type l_file_type = t_file_type::NONE;
    typedef enum class mode {SURFACE_PARAM, HISTOGRAM_PARAM, TABLE_DATA, HEADER} t_mode;
    t_mode l_mode = t_mode::HEADER;

    vtkSmartPointer<vtkTable> l_table;
    unsigned int l_current_table_row = 0;
    std::pair<unsigned int, unsigned int> l_table_data_dim;
    std::string l_histogram_title;
    std::string l_histogram_x_axis;
    std::string l_histogram_y_axis;
    unsigned int l_histogram_series;
    unsigned int l_histogram_width;
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
                    l_line_stream >> l_table_data_dim.first >> l_table_data_dim.second;
                    size_table(l_table, l_table_data_dim.second, l_table_data_dim.first);
                    l_mode = t_mode::TABLE_DATA;
                    break;
                }
                case t_mode::HISTOGRAM_PARAM:
                {
                    std::stringstream l_line_stream;
                    l_line_stream << l_line;
                    l_line_stream >> l_histogram_title >> l_histogram_x_axis >> l_histogram_y_axis >> l_histogram_width >> l_histogram_series;
                    l_table_data_dim = std::make_pair(l_histogram_width, l_histogram_series);
                    size_table(l_table, l_histogram_width, l_histogram_series + 1);
                    for(int l_column_index = 0; l_column_index < ((int)l_histogram_series) + 1; ++l_column_index)
                    {
                        dynamic_cast<vtkTypeInt32Array*>(l_table->GetColumn((int)l_column_index))->SetName(std::to_string(l_column_index).c_str());
                    }
                    for(int l_row_index = 0; l_row_index < (int)l_histogram_width; l_row_index++)
                    {
                        l_table->SetValue(l_row_index, 0, l_row_index + 1);
                    }
                    l_mode = t_mode::TABLE_DATA;
                    break;
                }
                case t_mode::TABLE_DATA:
                {
                    if(l_current_table_row >= l_table_data_dim.second)
                    {
                        std::cerr << "to many data compared to declared line number" << std::endl;
                        return EXIT_FAILURE;
                    }
                    std::stringstream l_line_stream;
                    l_line_stream << l_line;
                    unsigned int l_value;
                    for(vtkIdType l_column_index = 0; l_column_index < l_table_data_dim.first; ++l_column_index)
                    {
                        l_line_stream >> l_value;
                        if(l_file_type == t_file_type::SURFACE)
                        {
                            l_table->SetValue(l_current_table_row, l_column_index,l_value);
                        }
                        else
                        {
                            l_table->SetValue(l_column_index, l_current_table_row + 1, l_value);
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
                    }
                    else if("histogram" == l_line)
                    {
                        l_file_type = t_file_type::HISTOGRAM;
                        l_mode = t_mode::HISTOGRAM_PARAM;
                    }
                    else
                    {
                        std::cerr << R"(Unsupported header ")" << l_line << R"(")" << std::endl;
                        return EXIT_FAILURE;
                    }
                    l_table = vtkSmartPointer<vtkTable>::New();
                    break;
            }
        }
    }
    l_input_file.close();

    switch(l_file_type)
    {
        case t_file_type::SURFACE:
            display_surface(l_table, l_table_data_dim.first, l_table_data_dim.second);
            break;
        case t_file_type::HISTOGRAM:
            display_histogram(l_histogram_title, l_histogram_x_axis, l_histogram_y_axis, l_table, l_histogram_series);
            break;
        default:
            std::cerr << "Unsupported mode" << std::endl;
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
// EOF
