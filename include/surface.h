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
#ifndef VTK_VIEWER_SURFACE_H
#define VTK_VIEWER_SURFACE_H

#include "table_based.h"
#include <sstream>

namespace VTK_viewer
{
    class surface: public table_based
    {
      public:

        inline
        surface();

        inline
        void set_header(std::stringstream & p_stream);

        inline
        unsigned int get_dim_X()const;

        inline
        unsigned int get_dim_Y()const;

        inline
        void set_value( vtkIdType p_file_row
                      , vtkIdType p_column
                      , unsigned int p_value
                      );

      private:
        unsigned int m_dim_X;
        unsigned int m_dim_Y;

    };

    //-------------------------------------------------------------------------
    surface::surface()
    : m_dim_X{0}
    , m_dim_Y{0}
    {

    }

    //-------------------------------------------------------------------------
    void
    surface::set_header(std::stringstream & p_stream)
    {
        p_stream >> m_dim_X >> m_dim_Y;
        size_table(m_dim_Y, m_dim_X);

    }

    //-------------------------------------------------------------------------
    unsigned int
    surface::get_dim_X() const
    {
        return m_dim_X;
    }

    //-------------------------------------------------------------------------
    unsigned int
    surface::get_dim_Y() const
    {
        return m_dim_Y;
    }

    //-------------------------------------------------------------------------
    void
    surface::set_value(vtkIdType p_file_row,
                       vtkIdType p_column,
                       unsigned int p_value
                      )
    {
        get_table()->SetValue(p_file_row, p_column,p_value);
    }



}
#endif //VTK_VIEWER_SURFACE_H
