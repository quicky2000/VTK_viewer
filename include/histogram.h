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
#ifndef VTK_VIEWER_HISTOGRAM_H
#define VTK_VIEWER_HISTOGRAM_H

#include "table_based.h"
#include <sstream>

namespace VTK_viewer
{
    class histogram: public table_based
    {
      public:
        inline
        histogram();

        inline
        const std::string & get_title()const;

        inline
        const std::string & get_x_axis()const;

        inline
        const std::string & get_y_axis()const;

        inline
        unsigned int get_nb_series()const;

        inline
        unsigned int get_width()const;

        inline
        void set_header( std::stringstream & p_stream);

        inline
        void set_value( vtkIdType p_file_row
                      , vtkIdType p_column
                      , unsigned int p_value
                      );

      private:

        std::string m_title;
        std::string m_x_axis;
        std::string m_y_axis;
        unsigned int m_width;
        unsigned int m_nb_series;
    };

    //-------------------------------------------------------------------------
    histogram::histogram()
    : m_width(0)
    , m_nb_series(0)
    {

    }

    //-------------------------------------------------------------------------
    void
    histogram::set_header(std::stringstream & p_stream)
    {
        p_stream >> m_title >> m_x_axis >> m_y_axis >> m_width >> m_nb_series;
        size_table(m_width, m_nb_series + 1);
        for(int l_column_index = 0; l_column_index < ((int)m_nb_series) + 1; ++l_column_index)
        {
            dynamic_cast<vtkTypeInt32Array*>(get_table()->GetColumn((int)l_column_index))->SetName(std::to_string(l_column_index).c_str());
        }
        for(int l_row_index = 0; l_row_index < (int)m_width; l_row_index++)
        {
            get_table()->SetValue(l_row_index, 0, l_row_index + 1);
        }

    }

    //-------------------------------------------------------------------------
    void
    histogram::set_value( vtkIdType p_file_row
                        , vtkIdType p_column
                        , unsigned int p_value
                        )
    {
        get_table()->SetValue(p_column, p_file_row + 1, p_value);
    }

    //-------------------------------------------------------------------------
    const std::string &
    histogram::get_title() const
    {
        return m_title;
    }

    //---------------------------------------------------------------------------
    const std::string &
    histogram::get_x_axis() const
    {
        return m_x_axis;
    }

    //---------------------------------------------------------------------------
    const std::string &
    histogram::get_y_axis() const
    {
        return m_y_axis;
    }

    //---------------------------------------------------------------------------
    unsigned int
    histogram::get_nb_series() const
    {
        return m_nb_series;
    }

    //---------------------------------------------------------------------------
    unsigned int
    histogram::get_width() const
    {
        return m_width;
    }
}
#endif //VTK_VIEWER_HISTOGRAM_H
