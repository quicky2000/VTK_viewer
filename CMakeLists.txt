cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(VTK_viewer)

find_package(VTK COMPONENTS
             vtkChartsCore
             vtkViewsContext2D
             vtkCommonColor
             vtkCommonCore
             vtkFiltersSources
             vtkInteractionStyle
             vtkRenderingContextOpenGL2
             vtkRenderingCore
             vtkRenderingFreeType
             vtkRenderingGL2PSOpenGL2
             vtkRenderingOpenGL2 QUIET
            )
if (NOT VTK_FOUND)
  message("Skipping ${PROJECT_NAME}: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()

message (STATUS "VTK_VERSION: ${VTK_VERSION}")

if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE main.cpp)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE main.cpp)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(TARGETS ${PROJECT_NAME}
                      MODULES ${VTK_LIBRARIES}
                     )
endif () 
