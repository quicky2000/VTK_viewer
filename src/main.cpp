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

//-----------------------------------------------------------------------------
int main( int p_argc
        , char * p_argv[]
        )
{
    try
    {
        if (p_argc != 2)
        {
            std::cerr << "Usage is VTK_viewer <input_file>" << std::endl;
            return -1;
        }

        VTK_viewer::VTK_viewer l_viewer(p_argv[1]);
        l_viewer.display();
    }
    catch(quicky_exception::quicky_runtime_exception & e)
    {
        std::cerr << "ERROR : " << e.what() << " from " << e.get_file() << ":" << e.get_line() << std::endl ;
        return EXIT_FAILURE;
    }
    catch(quicky_exception::quicky_logic_exception & e)
    {
        std::cerr << "ERROR : " << e.what() << " from " << e.get_file() << ":" << e.get_line() << std::endl ;
        return EXIT_FAILURE;
    }


   return EXIT_SUCCESS;
}
// EOF
