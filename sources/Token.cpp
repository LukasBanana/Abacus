/*
 * Token.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Token.h"


namespace Ac
{


Token::Token(Token&& other) :
    type_   ( other.type_             ),
    pos_    ( other.pos_              ),
    spell_  ( std::move(other.spell_) )
{
}

Token::Token(const StreamPosition& pos, const Types type) :
    type_   ( type ),
    pos_    ( pos  )
{
}
Token::Token(const StreamPosition& pos, const Types type, const std::string& spell) :
    type_   ( type  ),
    pos_    ( pos   ),
    spell_  ( spell )
{
}
Token::Token(const StreamPosition& pos, const Types type, std::string&& spell) :
    type_   ( type             ),
    pos_    ( pos              ),
    spell_  ( std::move(spell) )
{
}


} // /namespace Ac



// ================================================================================