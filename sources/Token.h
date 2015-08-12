/*
 * Token.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_TOKEN_H__
#define __AC_TOKEN_H__


#include <Abacus/StreamPosition.h>
#include <string>
#include <memory>


namespace Ac
{


//! Token classes used by the scanner and parser.
class Token
{
    
    public:
        
        //! Token type enumeration.
        enum class Types
        {
            __Unknown__,
            
            Ident,          //!< (letter | '_') (letter | '_' | digit)*
            IntLiteral,     //!< digit+
            FloatLiteral,   //!< digit+ '.' digit+
            
            AddOp,          //!< +
            SubOp,          //!< -
            MulOp,          //!< *
            DivOp,          //!< /
            ModOp,          //!< mod
            PowOp,          //!< ^
            ShiftOp,        //!< <<, >>
            
            OpenBracket,    //!< (
            CloseBracket,   //!< )

            EndOfStream,
        };

        Token(Token&& other);

        Token(const StreamPosition& pos, const Types type);
        Token(const StreamPosition& pos, const Types type, const std::string& spell);
        Token(const StreamPosition& pos, const Types type, std::string&& spell);

        //! Returns the token type.
        inline Types Type() const
        {
            return type_;
        }

        //! Returns the token source position.
        inline const StreamPosition& Pos() const
        {
            return pos_;
        }

        //! Returns the token spelling.
        inline const std::string& Spell() const
        {
            return spell_;
        }

    private:

        Types           type_;  //!< Type of this token.
        StreamPosition  pos_;   //!< Source area of this token.
        std::string     spell_; //!< Token spelling.

};

using TokenPtr = std::shared_ptr<Token>;


} // /namespace Ac


#endif



// ================================================================================