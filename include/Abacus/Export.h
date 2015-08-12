/*
 * Export.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_EXPORT_H__
#define __AC_EXPORT_H__


#ifdef _MSC_VER
#   define AC_EXPORT __declspec(dllexport)
#else
#   define AC_EXPORT
#endif


#endif



// ================================================================================
