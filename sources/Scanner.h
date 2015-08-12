/*
 * Scanner.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_SCANNER_H__
#define __AC_SCANNER_H__


#include "ExprStream.h"
#include "Token.h"

#include <Abacus/StreamPosition.h>
#include <Abacus/Log.h>
#include <string>
#include <functional>


namespace Ac
{


//! This class stores the position in a source code file.
class Scanner
{
    
    public:
        
        Scanner(Log* log = nullptr);

        bool Scan(const std::shared_ptr<ExprStream>& stream);

        //! Scanns the next token.
        TokenPtr Next();

        //! Returns the current stream position.
        StreamPosition Pos() const;

        //! Returns the current expression stream.
        inline ExprStream* GetSource() const
        {
            return stream_.get();
        }

    private:
        
        /* === Functions === */

        char Take(char chr);
        char TakeIt();

        void Error(const std::string& msg);
        void ErrorUnexpected();
        void ErrorUnexpected(char expectedChar);
        void ErrorEOF();
        void ErrorLetterInNumber();

        //! Ignores all characters which comply the specified predicate.
        void Ignore(const std::function<bool (char)>& pred);

        void IgnoreWhiteSpaces();

        TokenPtr Make(const Token::Types& type, bool takeChr = false);
        TokenPtr Make(const Token::Types& type, std::string& spell, bool takeChr = false);
        TokenPtr Make(const Token::Types& type, std::string& spell, const StreamPosition& pos, bool takeChr = false);

        TokenPtr ScanToken();

        TokenPtr ScanIdentifier();
        TokenPtr ScanShiftOperator(const char Chr);
        TokenPtr ScanNumber();

        void ScanDecimalLiteral(std::string& spell);

        inline bool Is(char chr) const
        {
            return chr_ == chr;
        }

        inline unsigned char UChr() const
        {
            return static_cast<unsigned char>(chr_);
        }

        /* === Members === */

        Log*                        log_ = nullptr;

        std::shared_ptr<ExprStream> stream_;
        char                        chr_ = 0;

};


} // /namespace Ac


#endif



// ================================================================================