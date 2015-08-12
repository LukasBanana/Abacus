/*
 * Parser.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Parser.h"


namespace Ac
{


Parser::Parser(Log* log) :
    scanner_( log ),
    log_    ( log )
{
}

std::shared_ptr<Expr> Parser::Parse(const std::shared_ptr<ExprStream>& stream)
{
    if (!scanner_.Scan(stream))
        return false;

    AcceptIt();

    try
    {
        return ParseExpr();
    }
    catch (const std::exception& err)
    {
        if (log_)
            log_->Error(err.what());
    }

    return nullptr;
}


/*
 * ======= Private: =======
 */

void Parser::Error(const std::string& msg)
{
    throw std::runtime_error("syntax error (" + scanner_.Pos().ToString() + ") : " + msg);
}

void Parser::ErrorUnexpected()
{
    Error("unexpected token '" + tkn_->Spell() + "'");
}

void Parser::ErrorUnexpected(const std::string& hint)
{
    Error("unexpected token '" + tkn_->Spell() + "' (" + hint + ")");
}

void Parser::ErrorInternal(const std::string& msg)
{
    throw std::runtime_error("internal error (" + scanner_.Pos().ToString() + ") : " + msg);
}

TokenPtr Parser::Accept(const Tokens type)
{
    if (tkn_->Type() != type)
        ErrorUnexpected();
    return AcceptIt();
}

TokenPtr Parser::Accept(const Tokens type, const std::string& spell)
{
    if (tkn_->Type() != type)
        ErrorUnexpected();
    if (tkn_->Spell() != spell)
        Error("unexpected token spelling '" + tkn_->Spell() + "' (expected '" + spell + "')");
    return AcceptIt();
}

TokenPtr Parser::AcceptIt()
{
    auto prevTkn = tkn_;
    tkn_ = scanner_.Next();
    return prevTkn;
}

/* ------- Parse functions ------- */

BinaryExpr::Operators Parser::GetBinaryOperator(const std::string& spell)
{
    using Op = BinaryExpr::Operators;

    if (spell == "+")
        return Op::Add;
    if (spell == "-")
        return Op::Sub;
    if (spell == "*")
        return Op::Mul;
    if (spell == "/")
        return Op::Div;
    if (spell == "mod")
        return Op::Mod;
    if (spell == "^")
        return Op::Pow;
    if (spell == "<<")
        return Op::LShift;
    if (spell == ">>")
        return Op::RShift;

    Error("invalid binary operator: " + spell);
    return Op::Add;
}

ExprPtr Parser::ParseExpr()
{
    return ParseAddExpr();
}

ExprPtr Parser::ParseAbstractBinaryExpr(
    const std::function<ExprPtr(void)>& parseFunc, const Tokens binaryOperatorToken)
{
    /* Parse sub expressions */
    std::vector<ExprPtr> exprs;
    std::vector<BinaryExpr::Operators> ops;

    /* Parse primary expression */
    exprs.push_back(parseFunc());

    while (Is(binaryOperatorToken))
    {
        /* Parse binary operator */
        auto spell = AcceptIt()->Spell();
        ops.push_back(GetBinaryOperator(spell));

        /* Parse next sub-expression */
        exprs.push_back(parseFunc());
    }

    /* Build (left-to-rigth) binary expression tree */
    return BuildBinaryExprTree(exprs, ops);
}

ExprPtr Parser::ParseAddExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseSubExpr, this), Tokens::AddOp);
}

ExprPtr Parser::ParseSubExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseMulExpr, this), Tokens::SubOp);
}

ExprPtr Parser::ParseMulExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseDivExpr, this), Tokens::MulOp);
}

ExprPtr Parser::ParseDivExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParsePowExpr, this), Tokens::DivOp);
}

ExprPtr Parser::ParsePowExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseShiftExpr, this), Tokens::PowOp);
}

ExprPtr Parser::ParseShiftExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseValueExpr, this), Tokens::ShiftOp);
}

ExprPtr Parser::ParseValueExpr()
{
    switch (Type())
    {
        case Tokens::IntLiteral:
            return ParseIntLiteral();
        case Tokens::FloatLiteral:
            return ParseFloatLiteral();
        case Tokens::OpenBracket:
            return ParseBracketExpr();
        case Tokens::SubOp:
            return ParseUnaryExpr();
    }
    return ParseIdentExpr();
}

ExprPtr Parser::ParseIntLiteral()
{
    auto ast = Make<LiteralExpr>();

    ast->value = Accept(Tokens::IntLiteral)->Spell();

    return ast;
}

ExprPtr Parser::ParseFloatLiteral()
{
    auto ast = Make<LiteralExpr>();

    ast->value = Accept(Tokens::FloatLiteral)->Spell();
    ast->isFloat = true;

    return ast;
}

ExprPtr Parser::ParseBracketExpr()
{
    Accept(Tokens::OpenBracket);

    auto ast = ParseExpr();

    Accept(Tokens::CloseBracket);

    return ast;
}

ExprPtr Parser::ParseUnaryExpr()
{
    auto ast = Make<UnaryExpr>();

    Accept(Tokens::SubOp);

    ast->expr = ParseValueExpr();

    return ast;
}

ExprPtr Parser::ParseIdentExpr()
{
    auto ast = Make<IdentExpr>();

    ast->value = Accept(Tokens::Ident)->Spell();

    return ast;
}

ExprPtr Parser::BuildBinaryExprTree(std::vector<ExprPtr>& exprs, std::vector<BinaryExpr::Operators>& ops)
{
    if (exprs.empty())
        ErrorInternal("sub-expressions must not be empty (" __FUNCTION__ ")");

    if (exprs.size() > 1)
    {
        if (exprs.size() != ops.size() + 1)
            ErrorInternal("sub-expressions and operators have uncorrelated number of elements (" __FUNCTION__ ")");

        auto ast = Make<BinaryExpr>();

        /* Build right hand side */
        ast->exprR = exprs.back();
        ast->op = ops.back();

        exprs.pop_back();
        ops.pop_back();

        /* Build left hand side */
        ast->exprL = BuildBinaryExprTree(exprs, ops);

        return ast;
    }

    return exprs.front();
}


} // /namespace HTLib



// ================================================================================