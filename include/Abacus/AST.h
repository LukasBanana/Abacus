/*
 * AST.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_AST_H__
#define __AC_AST_H__


#include "Export.h"
#include "Visitor.h"
#include "StreamPosition.h"

#include <string>
#include <memory>
#include <vector>


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
        Func,
        Fold,
        Vector,
        Def,
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
        __Unknown__,
        Negate,
        Factorial,
        Norm,
    };

    static AC_EXPORT Operators GetOperator(const std::string& spell);
    static AC_EXPORT std::string GetOperatorSpell(const Operators op);

    std::shared_ptr<Expr>   expr;
    Operators               op = Operators::Negate;
};

struct BinaryExpr : public Expr
{
    __AC_AST_INTERFACE__(Binary);

    enum class Operators
    {
        __Unknown__,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Pow,
        LShift,
        RShift,
    };

    static AC_EXPORT Operators GetOperator(const std::string& spell);
    static AC_EXPORT std::string GetOperatorSpell(const Operators op);

    std::shared_ptr<Expr>   exprL;
    Operators               op = Operators::Add;
    std::shared_ptr<Expr>   exprR;
};

struct LiteralExpr : public Expr
{
    __AC_AST_INTERFACE__(Literal);

    std::string value;
    bool        isFloat = false;
};

struct IdentExpr : public Expr
{
    __AC_AST_INTERFACE__(Ident);

    std::string value;
};

struct FuncExpr : public Expr
{
    __AC_AST_INTERFACE__(Func);

    std::string             name;
    std::vector<ExprPtr>    args;
};

struct FoldExpr : public Expr
{
    __AC_AST_INTERFACE__(Fold);

    std::string             func;       // fold function name (either "sum" or "product").
    std::string             index;      // identifier of the index variable
    ExprPtr                 initExpr;   // expression to initialize the index
    ExprPtr                 iterExpr;   // expression for the index iterationp
    ExprPtr                 loopExpr;   // expression inside the fold loop
};

struct VectorExpr : public Expr
{
    __AC_AST_INTERFACE__(Vector);

    std::vector<ExprPtr> components; // expressions of the vector components
};

struct DefExpr : public Expr
{
    __AC_AST_INTERFACE__(Def);

    std::string ident;
    ExprPtr     expr;
};


#undef __AC_AST_INTERFACE__


} // /namespace Ac


#endif



// ================================================================================
