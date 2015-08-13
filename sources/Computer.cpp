/*
 * Computer.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Computer.h"


namespace Ac
{


static const char* expConst = "2.7182818284590452353602874713526";

static void LogError(Log* log, const std::string& msg)
{
    if (log)
        log->Error(msg);
}

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
        LogError(log, err.what());
    }
    catch (int_precision::bad_int_syntax)
    {
        LogError(log, "bad integer syntax (interger)");
    }
    catch (int_precision::out_of_range)
    {
        LogError(log, "out of range (interger)");
    }
    catch (int_precision::divide_by_zero)
    {
        LogError(log, "division by zero (interger)");
    }
    catch (float_precision::bad_int_syntax)
    {
        LogError(log, "bad integer syntax (float)");
    }
    catch (float_precision::bad_float_syntax)
    {
        LogError(log, "bad float syntax (float)");
    }
    catch (float_precision::out_of_range)
    {
        LogError(log, "out of range (float)");
    }
    catch (float_precision::divide_by_zero)
    {
        LogError(log, "division by zero (float)");
    }
    catch (float_precision::domain_error)
    {
        LogError(log, "domain error (float)");
    }
    catch (float_precision::base_error)
    {
        LogError(log, "base error (float)");
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
        case Op::Factorial:
            value.Factorial();
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
            Push(Value(it->second, true));
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
    
    auto Param = [&](std::size_t i) -> float_precision
    {
        if (i >= ast->args.size())
            Error("not enough parameters for function '" + ast->name + "'");

        /* Evaluate argument */
        Visit(ast->args[i]);
        auto val = Pop();

        /* Return float precision value */
        val.ToFloat();
        return val.GetFloat();
    };

    if (f == "sin")
        Push(sin(Param(0)));
    else if (f == "cos")
        Push(cos(Param(0)));
    else if (f == "tan")
        Push(tan(Param(0)));
    else if (f == "sinh")
        Push(sinh(Param(0)));
    else if (f == "cosh")
        Push(cosh(Param(0)));
    else if (f == "tanh")
        Push(tanh(Param(0)));
    else if (f == "asin")
        Push(asin(Param(0)));
    else if (f == "acos")
        Push(acos(Param(0)));
    else if (f == "atan")
        Push(atan(Param(0)));
    else if (f == "atan2")
        Push(atan2(Param(0), Param(1)));
    else if (f == "asinh")
        Push(asinh(Param(0)));
    else if (f == "acosh")
        Push(acosh(Param(0)));
    else if (f == "atanh")
        Push(atanh(Param(0)));
    else if (f == "pow")
        Push(pow(Param(0), Param(1)));
    else if (f == "sqrt")
        Push(sqrt(Param(0)));
    else if (f == "exp")
        Push(exp(Param(0)));
    else if (f == "log")
        Push(log(Param(0)));
    else if (f == "log10")
        Push(log10(Param(0)));
    else if (f == "ln")
        Push(log(Param(0))/log(float_precision(expConst)));
    else if (f == "abs")
        Push(abs(Param(0)));
    else if (f == "ceil")
        Push(ceil(Param(0)));
    else if (f == "floor")
        Push(floor(Param(0)));
    else
        Error("unknown function '" + f + "'");
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

Computer::Value::Value(const int_precision& iprec) :
    iprec_  ( iprec ),
    isFloat_( false )
{
}

Computer::Value::Value(const float_precision& fprec) :
    fprec_  ( fprec ),
    isFloat_( true  )
{
}

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
    ToFloat();
    rhs.ToFloat();
    fprec_ /= rhs.fprec_;
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
    Unify(rhs);
    if (isFloat_)
        fprec_ = pow(fprec_, rhs.fprec_);
    else
    {
        if (rhs.iprec_ < 0)
        {
            ToFloat();
            Pow(rhs);
        }
        else
            iprec_ = ipow(iprec_, rhs.iprec_);
    }
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

static void IntPrecFactorial(int_precision& n)
{
    if (n == 0)
        n = 1;
    else
    {
        bool isNeg = false;
        if (n < 0)
        {
            isNeg = true;
            n = -n;
        }

        auto m = n;
        while (m > 1)
        {
            --m;
            n *= m;
        }

        if (isNeg)
            n = -n;
    }
}

void Computer::Value::Factorial()
{
    ToInt();
    IntPrecFactorial(iprec_);
}

Computer::Value::operator std::string ()
{
    return isFloat_ ? fprec_.toString() : iprec_.toString();
}


} // /namespace HTLib



// ================================================================================