/*
 * Visitor.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_VISITOR_H__
#define __AC_VISITOR_H__


#include <memory>


namespace Ac
{


struct UnaryExpr;
struct BinaryExpr;
struct LiteralExpr;
struct IdentExpr;

class Visitor
{
    
    public:
        
        virtual ~Visitor()
        {
        }

        void VisitUnaryExpr     ( UnaryExpr*    ast, void* args );
        void VisitBinaryExpr    ( BinaryExpr*   ast, void* args );
        void VisitLiteralExpr   ( LiteralExpr*  ast, void* args );
        void VisitIdentExpr     ( IdentExpr*    ast, void* args );

    protected:

        template <class T>
        void Visit(T ast, void* args = nullptr)
        {
            if (ast)
                ast->Visit(this, args);
        }

};


} // /namespace Ac


#endif



// ================================================================================
