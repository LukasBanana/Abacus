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


struct ConstantsSet
{
    ConstantsSet()
    {
        constants["pi"] = "3.1415926535897932384626433832795";
        constants["e"]  = "2.7182818284590452353602874713526";
    }

    std::map<std::string, std::string> constants;
};


AC_EXPORT ExprPtr ParseExpression(const std::string& expr, Log* log = nullptr);

AC_EXPORT std::string Compute(const std::string& expr, Log* log = nullptr);
AC_EXPORT std::string Compute(const std::string& expr, ConstantsSet& constantsSet, Log* log = nullptr);


} // /namespace Ac


#endif



// ================================================================================
