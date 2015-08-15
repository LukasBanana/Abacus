/*
 * Scanner.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Scanner.h"

#include <cctype>


namespace Ac
{


Scanner::Scanner(Log* errHandler) :
    ExprProcessor( errHandler )
{
}

bool Scanner::Scan(const std::shared_ptr<ExprStream>& stream)
{
    if (stream && stream->IsValid())
    {
        /* Store source stream and take first character */
        stream_ = stream;
        TakeIt();
        succeeded_ = true;
        return true;
    }
    succeeded_ = false;
    return false;
}

TokenPtr Scanner::Next()
{
    while (!Is(0))
    {
        try
        {
            /* Scan next token */
            IgnoreWhiteSpaces();
            return ScanToken();
        }
        catch (const std::exception& err)
        {
            /* Add to error and scan next token */
            SendError(err.what());
            succeeded_ = false;
        }
    }

    return nullptr;
}

StreamPosition Scanner::Pos() const
{
    return stream_ != nullptr ? stream_->Pos() : StreamPosition::ignore;
}


/*
 * ======= Private: =======
 */

std::string Scanner::GetContextInfo() const
{
    return "lexical";
}

char Scanner::Take(char chr)
{
    if (chr_ != chr)
        ErrorUnexpected(chr);
    return TakeIt();
}

char Scanner::TakeIt()
{
    /* Get next character and return previous one */
    auto prevChr = chr_;
    chr_ = stream_->Next();
    return prevChr;
}

void Scanner::ErrorUnexpected()
{
    auto chr = TakeIt();
    if (chr != 0 && chr != '\n')
        Error("invalid character '" + std::string(1, chr) + "'");
    else
        ErrorEOF();
}

void Scanner::ErrorUnexpected(char expectedChar)
{
    auto chr = TakeIt();
    if (chr != 0 && chr != '\n')
        Error("invalid character '" + std::string(1, chr) + "' (expected '" + std::string(1, expectedChar) + "')");
    else
        ErrorEOF();
}

void Scanner::ErrorLetterInNumber()
{
    Error("letter '" + std::string(1, chr_) + "' is not allowed within a number");
}

void Scanner::Ignore(const std::function<bool (char)>& pred)
{
    while (pred(chr_))
        TakeIt();
}

void Scanner::IgnoreWhiteSpaces()
{
    Ignore(
        [](char chr)
        {
            return chr == ' ' || chr == '\t';
        }
    );
}

TokenPtr Scanner::Make(const Token::Types& type, bool takeChr)
{
    if (takeChr)
    {
        std::string spell;
        spell += TakeIt();
        return std::make_shared<Token>(Pos(), type, std::move(spell));
    }
    return std::make_shared<Token>(Pos(), type);
}

TokenPtr Scanner::Make(const Token::Types& type, std::string& spell, bool takeChr)
{
    if (takeChr)
        spell += TakeIt();
    return std::make_shared<Token>(Pos(), type, std::move(spell));
}

TokenPtr Scanner::Make(const Token::Types& type, std::string& spell, const StreamPosition& pos, bool takeChr)
{
    if (takeChr)
        spell += TakeIt();
    return std::make_shared<Token>(pos, type, std::move(spell));
}

TokenPtr Scanner::ScanToken()
{
    std::string spell;

    /* Scan identifier */
    if (std::isalpha(UChr()) || Is('_'))
        return ScanIdentifier();

    /* Scan number */
    if (std::isdigit(UChr()))
        return ScanNumber();

    /* Scan operators, punctuation, special characters and brackets */
    switch (chr_)
    {
        case '+': return Make( Token::Types::AddOp,         true );
        case '-': return Make( Token::Types::SubOp,         true );
        case '*': return Make( Token::Types::MulOp,         true );
        case '/': return Make( Token::Types::DivOp,         true );
        case '^': return Make( Token::Types::PowOp,         true );
        case ',': return Make( Token::Types::Comma,         true );
        case '=': return Make( Token::Types::Equal,         true );
        case '!': return Make( Token::Types::FactOp,        true );
        case '|': return Make( Token::Types::NormOp,        true );
        case '(': return Make( Token::Types::OpenBracket,   true );
        case ')': return Make( Token::Types::CloseBracket,  true );
        case '[': return Make( Token::Types::OpenParen,     true );
        case ']': return Make( Token::Types::CloseParen,    true );
        case '{': return Make( Token::Types::OpenCurly,     true );
        case '}': return Make( Token::Types::CloseCurly,    true );
    }

    if (Is('<') || Is('>'))
        return ScanShiftOperator(chr_);
        
    /* Scan punctuation, special characters and brackets */
    if (Is('('))
        return Make(Token::Types::OpenBracket, true);
    if (Is(')'))
        return Make(Token::Types::CloseBracket, true);
    if (Is('\n'))
        return Make(Token::Types::EndOfStream, true);

    ErrorUnexpected();

    return nullptr;
}

TokenPtr Scanner::ScanIdentifier()
{
    /* Scan identifier string */
    std::string spell;
    spell += TakeIt();

    while (std::isalnum(UChr()) || Is('_'))
        spell += TakeIt();
        
    /* Scan reserved words */
    if (spell == "mod")
        return Make(Token::Types::DivOp, spell);
    if (spell == "sum" || spell == "product")
        return Make(Token::Types::FoldFunc, spell);

    /* Return as identifier */
    return Make(Token::Types::Ident, spell);
}

TokenPtr Scanner::ScanShiftOperator(const char chr)
{
    std::string spell;

    spell += TakeIt();
    spell += Take(chr);

    return Make(Token::Types::ShiftOp, spell);
}

TokenPtr Scanner::ScanNumber()
{
    if (!std::isdigit(UChr()))
        Error("expected digit");
    
    /* Take first number (literals like ".0" are not allowed) */
    std::string spell;

    const auto startChr = TakeIt();
    spell += startChr;

    /* Parse integer or floating-point number */
    auto type = Token::Types::IntLiteral;

    ScanDecimalLiteral(spell);

    if (Is('.'))
    {
        spell += TakeIt();
        
        if (std::isdigit(UChr()))
            ScanDecimalLiteral(spell);
        else
            Error("floating-point literals must have a decimal on both sides of the dot (e.g. '0.0' but not '0.' or '.0')");

        type = Token::Types::FloatLiteral;
    }

    if (std::isalpha(UChr()) || Is('.'))
        ErrorLetterInNumber();

    /* Create number token */
    return Make(type, spell);
}

void Scanner::ScanDecimalLiteral(std::string& spell)
{
    while (std::isdigit(UChr()))
        spell += TakeIt();
}


} // /namespace HTLib



// ================================================================================