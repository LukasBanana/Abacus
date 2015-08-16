/*
 * Beautifier.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_BEAUTIFIER_H__
#define __AC_BEAUTIFIER_H__


#include <string>


namespace Ac
{


void BeautifyLiteral(std::string& s, std::size_t maxExp = 10);


} // /namespace Ac


#endif



// ================================================================================