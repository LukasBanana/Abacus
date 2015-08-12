/*
 * StreamPosition.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Abacus/StreamPosition.h>


namespace Ac
{


const StreamPosition StreamPosition::ignore;

StreamPosition::StreamPosition(unsigned int row, unsigned int column) :
    row_   ( row    ),
    column_( column )
{
}

std::string StreamPosition::ToString() const
{
    return std::to_string(row_) + ":" + std::to_string(column_ > 0 ? column_ - 1 : 0);
}

void StreamPosition::IncRow()
{
    ++row_;
    column_ = 0;
}

void StreamPosition::IncColumn()
{
    ++column_;
}

bool StreamPosition::IsValid() const
{
    return row_ > 0 && column_ > 0;
}


} // /namespace Ac



// ================================================================================