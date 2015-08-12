/*
 * ExprStream.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_EXPR_STREAM_H__
#define __AC_EXPR_STREAM_H__


#include <Abacus/StreamPosition.h>
#include <istream>
#include <string>
#include <memory>


namespace Ac
{


class ExprStream
{
    
    public:
        
        ExprStream(const std::shared_ptr<std::istream>& stream);

        //! Returns true if this is a valid source code stream.
        bool IsValid() const;

        //! Returns the next character from the source.
        char Next();

        //! Ignores the current character.
        inline void Ignore()
        {
            Next();
        }

        //! Returns the current stream position.
        inline const StreamPosition& Pos() const
        {
            return pos_;
        }
        //! Returns the current source line.
        inline const std::string& Line() const
        {
            return line_;
        }

    protected:
        
        ExprStream() = default;

        std::shared_ptr<std::istream>   stream_;
        std::string                     line_;
        StreamPosition                  pos_;

};


} // /namespace Ac


#endif



// ================================================================================