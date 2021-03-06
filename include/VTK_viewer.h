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
#ifndef VTK_VIEWER_VTK_VIEWER_H
#define VTK_VIEWER_VTK_VIEWER_H

#include "surface.h"
#include "histogram.h"
#include "line_plot.h"
#include <variant>
#include <string>

namespace VTK_viewer
{
    class VTK_viewer
    {
      public:

        explicit
        VTK_viewer(const std::string & p_file_name);

        void display() const;

      private:

        void display(const surface & p_surface) const;

        void display(const histogram & p_histogram) const;

        void display(const line_plot & p_line_plot) const;

        void display(const graph_based & p_graph_base
                    ,int p_type
                    ,float p_width
                    ) const;

        std::variant<surface,histogram,line_plot> m_displayable_object;
    };

}
#endif //VTK_VIEWER_VTK_VIEWER_H
