/*
 * StreamPosition.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_STREAM_POSITION_H__
#define __AC_STREAM_POSITION_H__


#include <Abacus/Export.h>
#include <string>


namespace Ac
{


//! This class stores the position in a source code file.
class AC_EXPORT StreamPosition
{
    
    public:
        
        StreamPosition() = default;
        StreamPosition(unsigned int row, unsigned int column);

        //! Returns the source position as string in the format "Row:Column", e.g. "75:10".
        std::string ToString() const;

        //! Increases the row by 1 and sets the column to 0.
        void IncRow();
        //! Increases the column by 1.
        void IncColumn();

        //! Returns ture if this is a valid source position. False if row and column are 0.
        bool IsValid() const;

        //! Returns the row of the source position, beginning with 1.
        inline unsigned int Row() const
        {
            return row_;
        }
        //! Returns the colummn of the source position, beginning with 1.
        inline unsigned int Column() const
        {
            return column_;
        }

        //! Invalid source position.
        static const StreamPosition ignore;

    private:
        
        unsigned int row_ = 0, column_ = 0;

};


} // /namespace Ac


#endif



// ================================================================================