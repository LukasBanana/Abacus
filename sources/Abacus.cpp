/*
 * Abacus.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Abacus/Abacus.h>
#include <iostream>
#include <sstream>

#include "Parser.h"


namespace Ac
{


AC_EXPORT ExprPtr ParseExpression(const std::string& expr, Log* log)
{
    /* Setup input stream */
    auto stream = std::make_shared<std::stringstream>();
    *stream << expr;

    /* Parse expression */
    Parser parser(log);
    return parser.Parse(std::make_shared<ExprStream>(stream));
}

static std::string ComputeIntern(const std::string& expr, ComputeState* state, Log* log)
{
    //todo...
    return "";
}

AC_EXPORT std::string Compute(const std::string& expr, Log* log)
{
    return ComputeIntern(expr, nullptr, log);
}

AC_EXPORT std::string Compute(const std::string& expr, ComputeState& state, Log* log)
{
    return ComputeIntern(expr, &state, log);
}


} // /namespace Ac



// ================================================================================
