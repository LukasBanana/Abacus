/*
 * Log.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_LOG_H__
#define __AC_LOG_H__


#include <cstddef>
#include <string>


namespace Ac
{


//! Log output interface.
class Log
{

    public:
        
        class ScopedIndent
        {
            
            public:
                
                ScopedIndent(Log& log) :
                    log_( &log )
                {
                    log_->IncIndent();
                }
                ScopedIndent(ScopedIndent&& rhs) :
                    log_( rhs.log_ )
                {
                    rhs.log_ = nullptr;
                }
                ~ScopedIndent()
                {
                    if (log_)
                        log_->DecIndent();
                }

            private:
                
                Log* log_;

        };

        virtual ~Log()
        {
        }

        virtual void Error(const std::string& message) = 0;

        void IncIndent()
        {
            ++indent_;
        }

        void DecIndent()
        {
            if (indent_ > 0)
                --indent_;
        }

        ScopedIndent Indent()
        {
            return std::move(ScopedIndent(*this));
        }

    protected:
        
        std::string GetIndent(std::size_t indentSize = 2) const
        {
            return std::string(indent_*indentSize, ' ');
        }

    private:
        
        std::size_t indent_ = 0;

};


} // /namespace Ac


#endif



// ================================================================================
