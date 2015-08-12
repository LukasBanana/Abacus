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
    //todo...

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
    //todo...

    /* Push result onto stack */
    Push(valueL);//!!!!!!!!
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
    {
        std::vector<char> s(value.size() + 1);
        memcpy(s.data(), value.c_str(), value.size() + 1);
        iprec_ = int_precision(s.data());
    }
}

Computer::Value::operator std::string ()
{
    return isFloat_ ? fprec_.toString() : iprec_.toString();
}


} // /namespace HTLib



// ================================================================================