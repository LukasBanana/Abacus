/*
 * ExprProcessor.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "ExprProcessor.h"


namespace Ac
{


ExprProcessor::ExprProcessor(Log* errHandler) :
    errHandler_( errHandler )
{
}

ExprProcessor::~ExprProcessor()
{
}

void ExprProcessor::Error(const std::string& msg)
{
    throw std::runtime_error(GetContextInfo() + " error: " + msg);
}

void ExprProcessor::ErrorEOF()
{
    Error("premature end of expression");
}

void ExprProcessor::ErrorInternal(const std::string& msg)
{
    throw std::runtime_error("internal error: " + msg);
}

void ExprProcessor::SendError(const std::string& msg)
{
    if (errHandler_)
        errHandler_->Error(msg);
}


} // /namespace HTLib



// ================================================================================