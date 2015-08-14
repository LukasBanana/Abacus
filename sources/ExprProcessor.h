/*
 * ExprProcessor.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_EXPR_PROCESSOR_H__
#define __AC_EXPR_PROCESSOR_H__


#include <Abacus/Log.h>
#include <string>


namespace Ac
{


//! Base class for scanner and parser.
class ExprProcessor
{
    
    public:
        
        ExprProcessor(Log* errHandler = nullptr);

    protected:
        
        //! Returns which kind of expression processor this is (e.g. "syntax", or "lexical").
        virtual std::string GetContextInfo() const = 0;

        //! Throws a std::runtime_error excpetion with the specififed message.
        void Error(const std::string& msg);

        //! Throws a std::runtime_error excpetion with the information about an invalid end-of-file.
        void ErrorEOF();

        //! Throws a std::runtime_error excpetion with the specified message.
        void ErrorInternal(const std::string& msg);

        //! Sends the specified error message to the error handler.
        void SendError(const std::string& msg);

    private:
        
        Log* errHandler_ = nullptr;

};


} // /namespace Ac


#endif



// ================================================================================