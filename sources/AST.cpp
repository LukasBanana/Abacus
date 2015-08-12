/*
 * AST.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Abacus/AST.h>


namespace Ac
{


/* --- UnaryExpr --- */

using UnaryOp = UnaryExpr::Operators;

AC_EXPORT UnaryExpr::Operators UnaryExpr::GetOperator(const std::string& spell)
{
    if (spell == "-")
        return UnaryOp::Negate;
    return UnaryOp::__Unknown__;
}

AC_EXPORT std::string UnaryExpr::GetOperatorSpell(const Operators op)
{
    switch (op)
    {
        case UnaryOp::Negate:
            return "-";
    }
    return "";
}


/* --- BinaryExpr --- */

using BinaryOp = BinaryExpr::Operators;

AC_EXPORT BinaryExpr::Operators BinaryExpr::GetOperator(const std::string& spell)
{
    if (spell == "+")
        return BinaryOp::Add;
    if (spell == "-")
        return BinaryOp::Sub;
    if (spell == "*")
        return BinaryOp::Mul;
    if (spell == "/")
        return BinaryOp::Div;
    if (spell == "mod")
        return BinaryOp::Mod;
    if (spell == "^")
        return BinaryOp::Pow;
    if (spell == "<<")
        return BinaryOp::LShift;
    if (spell == ">>")
        return BinaryOp::RShift;
    return BinaryOp::__Unknown__;
}

AC_EXPORT std::string BinaryExpr::GetOperatorSpell(const Operators op)
{
    switch (op)
    {
        case BinaryOp::Add:
            return "+";
        case BinaryOp::Sub:
            return "-";
        case BinaryOp::Mul:
            return "*";
        case BinaryOp::Div:
            return "/";
        case BinaryOp::Mod:
            return "mod";
        case BinaryOp::Pow:
            return "^";
        case BinaryOp::LShift:
            return "<<";
        case BinaryOp::RShift:
            return ">>";
    }
    return "";
}


} // /namespace Ac



// ================================================================================
