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
struct FuncExpr;
struct FoldExpr;
struct VectorExpr;
struct DefExpr;

class Visitor
{
    
    public:
        
        virtual ~Visitor()
        {
        }

        virtual void VisitUnaryExpr     ( UnaryExpr*    ast, void* args ) = 0;
        virtual void VisitBinaryExpr    ( BinaryExpr*   ast, void* args ) = 0;
        virtual void VisitLiteralExpr   ( LiteralExpr*  ast, void* args ) = 0;
        virtual void VisitIdentExpr     ( IdentExpr*    ast, void* args ) = 0;
        virtual void VisitFuncExpr      ( FuncExpr*     ast, void* args ) = 0;
        virtual void VisitFoldExpr      ( FoldExpr*     ast, void* args ) = 0;
        virtual void VisitVectorExpr    ( VectorExpr*   ast, void* args ) = 0;
        virtual void VisitDefExpr       ( DefExpr*      ast, void* args ) = 0;

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
