/*
 * AST.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_AST_H__
#define __AC_AST_H__


#include "Visitor.h"
#include "StreamPosition.h"

#include <string>
#include <memory>


namespace Ac
{


#define __AC_AST_INTERFACE__(name)                              \
    name##Expr(const StreamPosition& pos) :                     \
        Expr( pos )                                             \
    {                                                           \
    }                                                           \
    Types Type() const override                                 \
    {                                                           \
        return Types::name;                                     \
    }                                                           \
    void Visit(Visitor* visitor, void* args = nullptr) override \
    {                                                           \
        visitor->Visit##name##Expr(this, args);                 \
    }

struct Expr
{
    enum class Types
    {
        Unary,
        Binary,
        Literal,
        Ident,
    };

    Expr(const StreamPosition& pos) :
        pos( pos )
    {
    }

    virtual ~Expr()
    {
    }

    virtual Types Type() const = 0;
    virtual void Visit(Visitor* visitor, void* args = nullptr) = 0;

    StreamPosition pos;
};

using ExprPtr = std::shared_ptr<Expr>;

struct UnaryExpr : public Expr
{
    __AC_AST_INTERFACE__(Unary);

    enum class Operators
    {
        Negate,
    };

    std::shared_ptr<Expr>   expr;
    Operators               op = Operators::Negate;
};

struct BinaryExpr : public Expr
{
    __AC_AST_INTERFACE__(Binary);

    enum class Operators
    {
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Pow,
        LShift,
        RShift,
    };

    std::shared_ptr<Expr>   exprL;
    Operators               op = Operators::Add;
    std::shared_ptr<Expr>   exprR;
};

struct LiteralExpr : public Expr
{
    __AC_AST_INTERFACE__(Literal);

    std::string value;
};

struct IdentExpr : public Expr
{
    __AC_AST_INTERFACE__(Ident);

    std::string value;
};


#undef __AC_AST_INTERFACE__


} // /namespace Ac


#endif



// ================================================================================
