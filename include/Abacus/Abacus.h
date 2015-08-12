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

#include <string>
#include <map>
#include <memory>


namespace Ac
{


struct ComputeState
{
    std::map<std::string, std::string> vars;
};


AC_EXPORT std::shared_ptr<Expr> ParseExpression(const std::string& expr);

AC_EXPORT std::string Compute(const std::string& expr);
AC_EXPORT std::string Compute(const std::string& expr, ComputeState& state);


} // /namespace Ac


#endif



// ================================================================================
