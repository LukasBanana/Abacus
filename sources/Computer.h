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

#include <Abacus/Abacus.h>
#include <stack>
#include <string>


namespace Ac
{


class Computer : private Visitor
{
    
    public:
        
        std::string ComputeExpr(
            const std::string& expr, ConstantsSet* constantsSet = nullptr, Log* log = nullptr
        );

    private:
        
        class Value
        {
            
            public:
                
                Value(const std::string& value, bool isFloat = false);

                void Add(Value& rhs);
                void Sub(Value& rhs);
                void Mul(Value& rhs);
                void Div(Value& rhs);
                void Mod(Value& rhs);
                void Pow(Value& rhs);
                void LShift(Value& rhs);
                void RShift(Value& rhs);

                void ToFloat();
                void ToInt();

                void Unify(Value& rhs);

                void Negate();

                operator std::string ();

            private:
                
                int_precision   iprec_;
                float_precision fprec_;
                bool            isFloat_ = false;

        };

        void Error(const std::string& msg);

        void VisitUnaryExpr     ( UnaryExpr*    ast, void* args ) override;
        void VisitBinaryExpr    ( BinaryExpr*   ast, void* args ) override;
        void VisitLiteralExpr   ( LiteralExpr*  ast, void* args ) override;
        void VisitIdentExpr     ( IdentExpr*    ast, void* args ) override;
        void VisitFuncExpr      ( FuncExpr*     ast, void* args ) override;

        void Push(const Value& value);
        Value Pop();

        std::stack<Value>   values_;
        ConstantsSet*       constantsSet_;

};


} // /namespace Ac


#endif



// ================================================================================