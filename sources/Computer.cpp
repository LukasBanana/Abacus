/*
 * Computer.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Computer.h"
#include "Beautifier.h"

#include <random>


namespace Ac
{


static const char* expConst = "2.7182818284590452353602874713526";

static void LogError(Log* log, const std::string& msg)
{
    if (log)
        log->Error(msg);
}

std::string Computer::ComputeExpr(const std::string& expr, ConstantsSet& constantsSet, Log* log)
{
    /* Setup constant set */
    constantsSet_ = &constantsSet;

    try
    {
        /* Parse expression stream */
        auto ast = ParseExpression(expr, log);
        if (ast)
        {
            /* Compute AST */
            Visit(ast);
            return Pop();
        }
    }
    catch (const std::exception& err)
    {
        LogError(log, err.what());
    }
    catch (int_precision::bad_int_syntax)
    {
        LogError(log, "bad integer syntax");
    }
    catch (int_precision::out_of_range)
    {
        LogError(log, "out of range");
    }
    catch (int_precision::divide_by_zero)
    {
        LogError(log, "division by zero");
    }
    catch (float_precision::bad_int_syntax)
    {
        LogError(log, "bad integer syntax");
    }
    catch (float_precision::bad_float_syntax)
    {
        LogError(log, "bad float syntax");
    }
    catch (float_precision::out_of_range)
    {
        LogError(log, "out of range");
    }
    catch (float_precision::divide_by_zero)
    {
        LogError(log, "division by zero");
    }
    catch (float_precision::domain_error)
    {
        LogError(log, "domain error");
    }
    catch (float_precision::base_error)
    {
        LogError(log, "base error");
    }

    return "";
}


/*
 * ======= Private: =======
 */

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
        case Op::Norm:
            value.Normalize();
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
    Push(Variable(ast->value));
}

void Computer::VisitIdentExpr(IdentExpr* ast, void* args)
{
    /* Try to find constant */
    auto it = constantsSet_->constants.find(ast->value);
    if (it != constantsSet_->constants.end())
    {
        /* Push value onto stack */
        Push(Variable(it->second));
    }
    else
        Error("undefined constant '" + ast->value + "'");
}

void Computer::VisitFuncExpr(FuncExpr* ast, void* args)
{
    /* Find function */
    auto f = ast->name;
    
    auto ParamCount = [&](std::size_t n)
    {
        auto m = ast->args.size();

        if (m != n)
        {
            /* Setup well readable output message */
            std::string required = (n == 1 ? "1 argument" : std::to_string(n) + " arguments");
            std::string given = (m == 1 ? "1 is" : std::to_string(m) + " are");

            Error(
                "function '" + ast->name + "' requires exactly " +
                required + ", but " + given + " specified"
            );
        }
    };

    auto ParamCountNot0 = [&]()
    {
        if (ast->args.empty())
            Error("function '" + ast->name + "' requires at least 1 argument, but 0 are specified");
    };

    auto Param = [&](std::size_t i) -> float_precision
    {
        /* Evaluate argument */
        Visit(ast->args[i]);
        auto val = Pop();

        /* Return float precision value */
        val.ToFloat();
        return val.GetFloat();
    };

    if (f == "sin")
    {
        ParamCount(1);
        Push(sin(Param(0)));
    }
    else if (f == "cos")
    {
        ParamCount(1);
        Push(cos(Param(0)));
    }
    else if (f == "tan")
    {
        ParamCount(1);
        Push(tan(Param(0)));
    }
    else if (f == "sinh")
    {
        ParamCount(1);
        Push(sinh(Param(0)));
    }
    else if (f == "cosh")
    {
        ParamCount(1);
        Push(cosh(Param(0)));
    }
    else if (f == "tanh")
    {
        ParamCount(1);
        Push(tanh(Param(0)));
    }
    else if (f == "asin")
    {
        ParamCount(1);
        Push(asin(Param(0)));
    }
    else if (f == "acos")
    {
        ParamCount(1);
        Push(acos(Param(0)));
    }
    else if (f == "atan")
    {
        ParamCount(1);
        Push(atan(Param(0)));
    }
    else if (f == "atan2")
    {
        ParamCount(2);
        Push(atan2(Param(0), Param(1)));
    }
    else if (f == "asinh")
    {
        ParamCount(1);
        Push(asinh(Param(0)));
    }
    else if (f == "acosh")
    {
        ParamCount(1);
        Push(acosh(Param(0)));
    }
    else if (f == "atanh")
    {
        ParamCount(1);
        Push(atanh(Param(0)));
    }
    else if (f == "pow")
    {
        ParamCount(2);
        Push(pow(Param(0), Param(1)));
    }
    else if (f == "sqrt")
    {
        ParamCount(1);
        Push(sqrt(Param(0)));
    }
    else if (f == "exp")
    {
        ParamCount(1);
        Push(exp(Param(0)));
    }
    else if (f == "log")
    {
        ParamCount(1);
        Push(log(Param(0)));
    }
    else if (f == "log10")
    {
        ParamCount(1);
        Push(log10(Param(0)));
    }
    else if (f == "abs")
    {
        ParamCount(1);
        Push(abs(Param(0)));
    }
    else if (f == "ceil")
    {
        ParamCount(1);
        Push(ceil(Param(0)).to_int_precision());
    }
    else if (f == "floor")
    {
        ParamCount(1);
        Push(floor(Param(0)).to_int_precision());
    }
    else if (f == "rand")
    {
        ParamCount(0);
        
        /* Generate random number in the range [0, 1] */
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_real_distribution<> dist(0, 1);

        Push(float_precision(dist(gen)));
    }
    else if (f == "min")
    {
        ParamCountNot0();

        /* Get first value */
        auto it = ast->args.begin();
        Visit(*it);
        auto result = Pop();

        /* Apply minification with all other arguments */
        for (++it; it != ast->args.end(); ++it)
        {
            Visit(*it);
            result.Min(Pop());
        }

        Push(result);
    }
    else if (f == "max")
    {
        ParamCountNot0();

        /* Get first value */
        auto it = ast->args.begin();
        Visit(*it);
        auto result = Pop();

        /* Apply minification with all other arguments */
        for (++it; it != ast->args.end(); ++it)
        {
            Visit(*it);
            result.Max(Pop());
        }

        Push(result);
    }
    else
        Error("unknown function '" + f + "'");
}

void Computer::VisitFoldExpr(FoldExpr* ast, void* args)
{
    /* Check if initialization and iteration expressions have integer type */
    Visit(ast->initExpr);
    auto initVal = Pop();

    Visit(ast->iterExpr);
    auto iterVal = Pop();

    if (initVal.IsFloat() || iterVal.IsFloat())
        Error("fold function '" + ast->func + "' can only have discrete iterations");

    /* Fold loop expression */
    auto idx = initVal.GetInt();
    auto idxEnd = iterVal.GetInt();

    bool isSum = (ast->func == "sum");
    Variable result(std::string(isSum ? "0" : "1"));

    while (idx <= idxEnd)
    {
        /* Setup new value for index variable */
        StoreConst(ast->index, idx.toString());

        /* Compute current iteration */
        Visit(ast->loopExpr);
        auto val = Pop();
        
        /* Fold with result */
        if (isSum)
            result.Add(val);
        else
            result.Mul(val);

        ++idx;
    }

    /* Return result */
    Push(result);
}

void Computer::VisitVectorExpr(VectorExpr* ast, void* args)
{
    //todo...
}

void Computer::VisitDefExpr(DefExpr* ast, void* args)
{
    /* Compute definition value */
    Visit(ast->expr);
    auto value = Top();

    /* Store (beautified) result in constant */
    StoreConst(ast->ident, value);
}

void Computer::Push(const Variable& value)
{
    values_.push(value);
}

Variable Computer::Pop()
{
    if (values_.empty())
        Error("stack underflow");
    auto val = values_.top();
    values_.pop();
    return val;
}

Variable Computer::Top()
{
    if (values_.empty())
        Error("empty stack");
    return values_.top();
}

void Computer::StoreConst(const std::string& ident, std::string value)
{
    BeautifyLiteral(value);
    constantsSet_->constants[ident] = value;
}


} // /namespace HTLib



// ================================================================================