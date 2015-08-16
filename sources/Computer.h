/*
 * Computer.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_COMPUTER_H__
#define __AC_COMPUTER_H__


#include "precpkg/iprecision.h"
#include "precpkg/fprecision.h"
#include "Variable.h"

#include <Abacus/Abacus.h>
#include <stack>
#include <string>


namespace Ac
{


class Computer : private Visitor
{
    
    public:
        
        std::string ComputeExpr(
            const std::string& expr, ConstantsSet& constantsSet, Log* log = nullptr
        );

    private:
        
        void Error(const std::string& msg);

        void VisitUnaryExpr     ( UnaryExpr*    ast, void* args ) override;
        void VisitBinaryExpr    ( BinaryExpr*   ast, void* args ) override;
        void VisitLiteralExpr   ( LiteralExpr*  ast, void* args ) override;
        void VisitIdentExpr     ( IdentExpr*    ast, void* args ) override;
        void VisitFuncExpr      ( FuncExpr*     ast, void* args ) override;
        void VisitFoldExpr      ( FoldExpr*     ast, void* args ) override;
        void VisitVectorExpr    ( VectorExpr*   ast, void* args ) override;
        void VisitDefExpr       ( DefExpr*      ast, void* args ) override;

        void Push(const Variable& value);
        Variable Pop();
        Variable& Top();

        void StoreConst(const std::string& ident, std::string value);

        std::stack<Variable>    values_;
        ConstantsSet*           constantsSet_;

};


} // /namespace Ac


#endif



// ================================================================================