/*
 * Parser.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Parser.h"


namespace Ac
{


Parser::Parser(Log* errHandler) :
    ExprProcessor   ( errHandler ),
    scanner_        ( errHandler )
{
}

std::shared_ptr<Expr> Parser::Parse(const std::shared_ptr<ExprStream>& stream)
{
    if (!scanner_.Scan(stream))
        return nullptr;

    AcceptIt();

    try
    {
        auto ast = ParseExpr();
        
        if (!scanner_.HasSucceeded())
            return nullptr;

        if (!Is(Tokens::EndOfStream))
            Error("invalid continuation with token '" + tkn_->Spell() + "'");

        return ast;
    }
    catch (const std::exception& err)
    {
        SendError(err.what());
    }

    return nullptr;
}


/*
 * ======= Private: =======
 */

std::string Parser::GetContextInfo() const
{
    return "syntax";
}

void Parser::ErrorUnexpected()
{
    if (Is(Tokens::EndOfStream))
        ErrorEOF();
    else
        Error("invalid token '" + tkn_->Spell() + "'");
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
    auto op = BinaryExpr::GetOperator(spell);
    if (op == BinaryExpr::Operators::__Unknown__)
        Error("invalid binary operator: " + spell);
    return op;
}

// expr: add_expr;
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

// add_expr: sub_expr ('+' sub_expr)*;
ExprPtr Parser::ParseAddExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseSubExpr, this), Tokens::AddOp);
}

// sub_expr: mul_expr ('-' mul_expr)*;
ExprPtr Parser::ParseSubExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseMulExpr, this), Tokens::SubOp);
}

// mul_expr: div_expr ('*' div_expr)*;
ExprPtr Parser::ParseMulExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseDivExpr, this), Tokens::MulOp);
}

// div_expr: pow_expr (('/' | 'mod') pow_expr)*;
ExprPtr Parser::ParseDivExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParsePowExpr, this), Tokens::DivOp);
}

// pow_expr: shift_expr ('^' shift_expr)*;
ExprPtr Parser::ParsePowExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseShiftExpr, this), Tokens::PowOp);
}

// shift_expr: fact_expr (SHIFT_OP fact_expr)*;
ExprPtr Parser::ParseShiftExpr()
{
    return ParseAbstractBinaryExpr(std::bind(&Parser::ParseFactExpr, this), Tokens::ShiftOp);
}

// value_expr: literal_expr | ident_expr | bracket_expr | unary_expr | func_expr | fact_expr;
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
        case Tokens::NormOp:
            return ParseNormExpr();
    }
    return ParseIdentExpr();
}

// fact_expr: value_expr | fact_expr '!';
ExprPtr Parser::ParseFactExpr()
{
    auto ast = ParseValueExpr();

    while (Is(Tokens::FactOp))
    {
        AcceptIt();
        
        auto factExpr = Make<UnaryExpr>();

        factExpr->expr = ast;
        factExpr->op = UnaryExpr::Operators::Factorial;

        ast = factExpr;
    }

    return ast;
}

// INT_LITERAL: BIN_LITERAL | OCT_LITERAL | DEC_LITERAL | HEX_LITERAL;
ExprPtr Parser::ParseIntLiteral()
{
    auto ast = Make<LiteralExpr>();

    ast->value = Accept(Tokens::IntLiteral)->Spell();

    return ast;
}

// FLOAT_LITERAL: DEC_LITERAL '.' DEC_LITERAL;
ExprPtr Parser::ParseFloatLiteral()
{
    auto ast = Make<LiteralExpr>();

    ast->value = Accept(Tokens::FloatLiteral)->Spell();
    ast->isFloat = true;

    return ast;
}

// bracket_expr: '(' expr ')';
ExprPtr Parser::ParseBracketExpr()
{
    Accept(Tokens::OpenBracket);

    auto ast = ParseExpr();

    Accept(Tokens::CloseBracket);

    return ast;
}

// unary_expr: '-' value_expr;
ExprPtr Parser::ParseUnaryExpr()
{
    auto ast = Make<UnaryExpr>();

    Accept(Tokens::SubOp);

    ast->expr = ParseValueExpr();

    return ast;
}

// ident_expr: IDENT;
ExprPtr Parser::ParseIdentExpr()
{
    auto value = Accept(Tokens::Ident)->Spell();

    if (!Is(Tokens::OpenBracket))
    {
        if (Is(Tokens::Ident) || Is(Tokens::FloatLiteral) || Is(Tokens::IntLiteral))
        {
            /* Create function expression */
            return ParseFuncExpr(std::move(value), true);
        }
        else
        {
            /* Create identifier expression */
            auto ast = Make<IdentExpr>();

            ast->value = value;

            return ast;
        }
    }

    /* Create function expression */
    return ParseFuncExpr(std::move(value));
}

// norm_expr: '|' expr '|';
ExprPtr Parser::ParseNormExpr()
{
    auto ast = Make<UnaryExpr>();

    ast->op = UnaryExpr::Operators::Norm;

    Accept(Tokens::NormOp);

    ast->expr = ParseExpr();

    Accept(Tokens::NormOp);

    return ast;
}

// func_expr: IDENT (argument_list | value_expr);
ExprPtr Parser::ParseFuncExpr(std::string&& name, bool singleParam)
{
    auto ast = Make<FuncExpr>();

    ast->name = name;

    if (!singleParam)
    {
        Accept(Tokens::OpenBracket);
        ast->args = ParseExprList();
        Accept(Tokens::CloseBracket);
    }
    else
        ast->args.push_back(ParseMulExpr());

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

// expr_list: expr (',' expr)*;
std::vector<ExprPtr> Parser::ParseExprList()
{
    std::vector<ExprPtr> exprList;

    while (true)
    {
        exprList.push_back(ParseExpr());
        if (Is(Tokens::Comma))
            AcceptIt();
        else
            break;
    }

    return exprList;
}


} // /namespace HTLib



// ================================================================================