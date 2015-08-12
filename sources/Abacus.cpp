/*
 * Abacus.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Abacus/Abacus.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <functional>

#include "Parser.h"
#include "Computer.h"


namespace Ac
{


AC_EXPORT ExprPtr ParseExpression(const std::string& expr, Log* log)
{
    /* Setup input stream */
    auto stream = std::make_shared<std::stringstream>();
    *stream << expr << '\n';

    /* Parse expression */
    Parser parser(log);
    return parser.Parse(std::make_shared<ExprStream>(stream));
}

static bool IsWhiteSpace(char c)
{
    return c == ' ' || c == '\t';
}

static bool IsNotWhiteSpace(char c)
{
    return !IsWhiteSpace(c);
}

static std::string& LeftTrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), IsNotWhiteSpace));
    return s;
}

static std::string& RightTrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), IsNotWhiteSpace).base(), s.end());
    return s;
}

static std::string& Trim(std::string& s)
{
    return LeftTrim(RightTrim(s));
}

static bool IsIdentValid(const std::string& s)
{
    if (s.empty() || std::find_if(s.begin(), s.end(), IsWhiteSpace) != s.end())
        return false;
    return std::isalpha(static_cast<int>(s[0])) || s[0] == '_';
}

static std::string ComputeIntern(const std::string& expr, ConstantsSet* constantsSet, Log* log)
{
    /* Check if a new constant should be added */
    auto pos = expr.find('=');

    if (pos != std::string::npos)
    {
        /* Compute sub expression (right hand side) */
        auto rhsExpr = expr.substr(pos + 1);
        auto rhsExprResult = ComputeIntern(rhsExpr, constantsSet, log);

        /* Set new constant value */
        if (constantsSet)
        {
            auto lhsExpr = expr.substr(0, pos);
            Trim(lhsExpr);

            /* Validate constant name */
            if (!IsIdentValid(lhsExpr))
            {
                if (log)
                    log->Error("invalid constant name \"" + lhsExpr + "\"");
                return "0";
            }

            constantsSet->constants[lhsExpr] = rhsExprResult;
        }

        return rhsExprResult;
    }

    /* Compute expression */
    Computer comp;
    return comp.ComputeExpr(expr, constantsSet, log);
}

AC_EXPORT std::string Compute(const std::string& expr, Log* log)
{
    return ComputeIntern(expr, nullptr, log);
}

AC_EXPORT std::string Compute(const std::string& expr, ConstantsSet& constantsSet, Log* log)
{
    return ComputeIntern(expr, &constantsSet, log);
}


} // /namespace Ac



// ================================================================================
