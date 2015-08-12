/*
 * Abacus.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_ABACUS_H__
#define __AC_ABACUS_H__


#include "Export.h"
#include "AST.h"
#include "Log.h"

#include <string>
#include <map>
#include <memory>


namespace Ac
{


struct ComputeState
{
    std::map<std::string, std::string> vars;
};


AC_EXPORT ExprPtr ParseExpression(const std::string& expr, Log* log = nullptr);

AC_EXPORT std::string Compute(const std::string& expr, Log* log = nullptr);
AC_EXPORT std::string Compute(const std::string& expr, ComputeState& state, Log* log = nullptr);


} // /namespace Ac


#endif



// ================================================================================
