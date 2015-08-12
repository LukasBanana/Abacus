/*
 * Log.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_LOG_H__
#define __AC_LOG_H__


#include <string>


namespace Ac
{


//! Log output interface.
class Log
{

    public:
        
        virtual ~Log()
        {
        }

        virtual void Error(const std::string& message) = 0;

};


} // /namespace Ac


#endif



// ================================================================================
