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

#include <array>
#include <fstream>
#include <sstream>

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
    l_plot->SetXRange(0, p_max_x);
    l_plot->SetYRange(0, p_max_y);
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

    typedef enum class mode {SURFACE, SURFACE_DATA, NONE} t_mode;
    t_mode l_mode = t_mode ::NONE;

    vtkSmartPointer<vtkTable> l_table;
    unsigned int l_surface_row = 0;
    std::pair<unsigned int, unsigned int> l_surface_dim;
    while(!l_input_file.eof())
    {
        std::string l_line;
        std::getline(l_input_file, l_line);
        if(!l_line.empty())
        {
            std::cout << R"(Line = ")" << l_line << R"(")" << std::endl;
            switch(l_mode)
            {
                case t_mode::SURFACE:
                {
                    std::stringstream l_line_stream;
                    l_line_stream << l_line;
                    l_line_stream >> l_surface_dim.first >> l_surface_dim.second;
                    for(vtkIdType l_index = 0; l_index < l_surface_dim.first; ++l_index)
                    {
                        l_table->AddColumn(vtkNew<vtkTypeInt32Array>());
                    }
                    l_table->SetNumberOfRows(l_surface_dim.second);
                    for(vtkIdType l_row_index = 0; l_row_index < l_surface_dim.second; ++l_row_index)
                    {
                        for(vtkIdType l_column_index = 0; l_column_index < l_surface_dim.first; ++l_column_index)
                        {
                            l_table->SetValue(l_row_index, l_column_index, 0);
                        }
                    }
                    l_mode = t_mode::SURFACE_DATA;
                    break;
                }
                case t_mode::SURFACE_DATA:
                {
                    if(l_surface_row >= l_surface_dim.second)
                    {
                        std::cerr << "to many data compared to declared line number" << std::endl;
                        return EXIT_FAILURE;
                    }
                    std::stringstream l_line_stream;
                    l_line_stream << l_line;
                    unsigned int l_value;
                    for(vtkIdType l_column_index = 0; l_column_index < l_surface_dim.first; ++l_column_index)
                    {
                        l_line_stream >> l_value;
                        l_table->SetValue(l_surface_row, l_column_index, l_value);
                    }
                    ++l_surface_row;
                    break;
                }
                case t_mode::NONE:
                    if("surface" == l_line)
                    {
                        l_mode = t_mode::SURFACE;
                        l_table = vtkSmartPointer<vtkTable>::New();
                    }
            }
        }
    }
    l_input_file.close();

    display_surface(l_table, l_surface_dim.first, l_surface_dim.second);

    return EXIT_SUCCESS;
}
// EOF
