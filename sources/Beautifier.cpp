/*
 * Beautifier.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Beautifier.h"
#include "precpkg/iprecision.h"


namespace Ac
{


static bool Replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t pos = str.find(from);

    if (pos != std::string::npos)
    {
        str.replace(pos, from.size(), to);
        return true;
    }

    return false;
}

void BeautifyLiteral(std::string& s)
{
    if (s.empty())
        return;

    /* Remove pre-fix '+' */
    if (s.front() == '+')
        s.erase(s.begin());

    /* Find 'E' */
    auto pos = s.find('E');
    if (pos == std::string::npos)
        return;

    /* Get exponent value */
    auto expStr = s.substr(pos + 1);

    if (expStr == "0")
    {
        /* Remove last two characters "E0" */
        s.resize(s.size() - 2);
        return;
    }

    /* Replace 'E' by " * 10^" */
    s.replace(pos, 1, " * 10^");
}


} // /namespace Ac



// ================================================================================