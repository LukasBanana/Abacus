/*
 * Computer.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Computer.h"


namespace Ac
{


std::string Computer::ComputeExpr(const std::string& expr, ConstantsSet* constantsSet, Log* log)
{
    constantsSet_ = constantsSet;

    try
    {
        auto ast = ParseExpression(expr, log);
        Visit(ast);
        return Pop();
    }
    catch (const std::exception& err)
    {
        if (log)
            log->Error(err.what());
    }

    return "ERR";
}


/*
 * ======= Private: =======
 */

static bool IsFloat(const std::string& s)
{
    return s.find('.') != std::string::npos;
}

void Computer::Error(const std::string& msg)
{
    throw std::runtime_error("math error: " + msg);
}

void Computer::VisitUnaryExpr(UnaryExpr* ast, void* args)
{
    /* Evaluate sub expressions */
    Visit(ast->expr);
    auto value = Pop();

    /* Compute unary operation */
    using Op = UnaryExpr::Operators;

    switch (ast->op)
    {
        case Op::Negate:
            value.Negate();
            break;
    }

    /* Push result onto stack */
    Push(value);
}

void Computer::VisitBinaryExpr(BinaryExpr* ast, void* args)
{
    /* Evaluate sub expressions */
    Visit(ast->exprL);
    auto valueL = Pop();

    Visit(ast->exprR);
    auto valueR = Pop();

    /* Compute binary operation */
    using Op = BinaryExpr::Operators;

    switch (ast->op)
    {
        case Op::Add:
            valueL.Add(valueR);
            break;
        case Op::Sub:
            valueL.Sub(valueR);
            break;
        case Op::Mul:
            valueL.Mul(valueR);
            break;
        case Op::Div:
            valueL.Div(valueR);
            break;
        case Op::Mod:
            valueL.Mod(valueR);
            break;
        case Op::Pow:
            valueL.Pow(valueR);
            break;
        case Op::LShift:
            valueL.LShift(valueR);
            break;
        case Op::RShift:
            valueL.RShift(valueR);
            break;
    }

    /* Push result onto stack */
    Push(valueL);
}

void Computer::VisitLiteralExpr(LiteralExpr* ast, void* args)
{
    /* Push literal value onto stack */
    Push(Value(ast->value, IsFloat(ast->value)));
}

void Computer::VisitIdentExpr(IdentExpr* ast, void* args)
{
    /* Try to find constant */
    if (constantsSet_)
    {
        auto it = constantsSet_->constants.find(ast->value);
        if (it != constantsSet_->constants.end())
        {
            /* Push value onto stack */
            Push(Value(it->second, IsFloat(it->second)));
        }
        else
            Error("undefined constant 'x" + ast->value + "'");
    }
    else
        Error("no constants defined");
}

void Computer::VisitFuncExpr(FuncExpr* ast, void* args)
{
    /* Find function */
    auto f = ast->name;

    //if (f == "sin")
    //todo...
}

void Computer::Push(const Value& value)
{
    values_.push(value);
}

Computer::Value Computer::Pop()
{
    if (values_.empty())
        Error("stack underflow");
    auto val = values_.top();
    values_.pop();
    return val;
}


/*
 * Value class
 */

Computer::Value::Value(const std::string& value, bool isFloat) :
    isFloat_( isFloat )
{
    if (isFloat_)
        fprec_ = float_precision(value.c_str());
    else
        iprec_ = int_precision(value.c_str());
}

void Computer::Value::Add(Value& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ += rhs.fprec_;
    else
        iprec_ += rhs.iprec_;
}

void Computer::Value::Sub(Value& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ -= rhs.fprec_;
    else
        iprec_ -= rhs.iprec_;
}

void Computer::Value::Mul(Value& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ *= rhs.fprec_;
    else
        iprec_ *= rhs.iprec_;
}

void Computer::Value::Div(Value& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ /= rhs.fprec_;
    else
        iprec_ /= rhs.iprec_;
}

void Computer::Value::Mod(Value& rhs)
{
    ToInt();
    rhs.ToInt();

    iprec_ %= rhs.iprec_;

    if (iprec_ < "0")
        iprec_ += rhs.iprec_;
}

void Computer::Value::Pow(Value& rhs)
{
    ToFloat();
    rhs.ToFloat();
    fprec_ = pow(fprec_, rhs.fprec_);
}

void Computer::Value::LShift(Value& rhs)
{
    ToInt();
    rhs.ToInt();
    iprec_ <<= rhs.iprec_;
}

void Computer::Value::RShift(Value& rhs)
{
    ToInt();
    rhs.ToInt();
    iprec_ >>= rhs.iprec_;
}

void Computer::Value::ToFloat()
{
    if (!isFloat_)
    {
        auto ival = iprec_.toString();
        fprec_ = float_precision(ival.c_str());
        isFloat_ = true;
    }
}

void Computer::Value::ToInt()
{
    if (isFloat_)
    {
        iprec_ = fprec_.to_int_precision();
        isFloat_ = false;
    }
}

void Computer::Value::Unify(Value& rhs)
{
    if (isFloat_ && !rhs.isFloat_)
        rhs.ToFloat();
    else if (!isFloat_ && rhs.isFloat_)
        ToFloat();
}

void Computer::Value::Negate()
{
    if (isFloat_)
        fprec_ = -fprec_;
    else
        iprec_ = -iprec_;
}

Computer::Value::operator std::string ()
{
    return isFloat_ ? fprec_.toString() : iprec_.toString();
}


} // /namespace HTLib



// ================================================================================