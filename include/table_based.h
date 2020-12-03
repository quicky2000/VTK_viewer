//
// Created by quickux on 04/12/2020.
//

#ifndef VTK_VIEWER_TABLE_BASED_H
#define VTK_VIEWER_TABLE_BASED_H


#include "vtkSmartPointer.h"
#include "vtkTable.h"
#include "vtkTypeInt32Array.h"

namespace VTK_viewer
{
    class table_based
    {
      public:

        inline
        table_based() = default;

        inline
        void size_table( unsigned int p_nb_row
                       , unsigned int p_nb_column
                       );
        inline
        vtkSmartPointer<vtkTable> get_table();

        inline
        const vtkSmartPointer<vtkTable> get_table() const;

      private:

        vtkSmartPointer<vtkTable> m_table;
    };

    //-------------------------------------------------------------------------
    vtkSmartPointer<vtkTable>
    table_based::get_table()
    {
        return m_table;
    }

    //-------------------------------------------------------------------------
    const vtkSmartPointer<vtkTable>
    table_based::get_table() const
    {
        return m_table;
    }

    //-------------------------------------------------------------------------
    void table_based::size_table( unsigned int p_nb_row
                                , unsigned int p_nb_column
                                )
    {
        m_table = vtkSmartPointer<vtkTable>::New();
        for(vtkIdType l_index = 0; l_index < p_nb_column; ++l_index)
        {
            m_table->AddColumn(vtkNew<vtkTypeInt32Array>());
        }
        m_table->SetNumberOfRows(p_nb_row);
        for(vtkIdType l_row_index = 0; l_row_index < p_nb_row; ++l_row_index)
        {
            for(vtkIdType l_column_index = 0; l_column_index < p_nb_column; ++l_column_index)
            {
                m_table->SetValue(l_row_index, l_column_index, 0);
            }
        }

    }

}
#endif //VTK_VIEWER_TABLE_BASED_H
// EOF