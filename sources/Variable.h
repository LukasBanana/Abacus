/*
 * Variable.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_VARIABLE_H__
#define __AC_VARIABLE_H__


#include "precpkg/iprecision.h"
#include "precpkg/fprecision.h"

#include <string>
#include <vector>


namespace Ac
{


class Variable
{
            
    public:
        
        Variable() = default;
        Variable(std::vector<Variable>&& vector);
        Variable(const int_precision& iprec);
        Variable(const float_precision& fprec);
        Variable(const std::string& value);

        void Add(Variable& rhs);
        void Sub(Variable& rhs);
        void Mul(Variable& rhs);
        void Div(Variable& rhs);
        void Mod(Variable& rhs);
        void Pow(Variable& rhs);
        void LShift(Variable& rhs);
        void RShift(Variable& rhs);
        void Min(Variable& rhs);
        void Max(Variable& rhs);

        void ToFloat();
        void ToInt();

        void Unify(Variable& rhs);

        void Negate();
        void Factorial();
        void Normalize();
        void Sign();

        std::string ToString() const;

        operator std::string () const;

        const int_precision& GetInt() const
        {
            return iprec_;
        }

        const float_precision& GetFloat() const
        {
            return fprec_;
        }

        const std::vector<Variable>& GetVector() const
        {
            return vector_;
        }

        std::vector<Variable>& GetVector()
        {
            return vector_;
        }

        bool IsVector() const
        {
            return !vector_.empty();
        }

        bool IsScalar() const
        {
            return !IsVector();
        }

        bool IsFloat() const
        {
            return isFloat_;
        }

    private:

        int_precision           iprec_;
        float_precision         fprec_;
        bool                    isFloat_ = false;
        std::vector<Variable>   vector_;

};


} // /namespace Ac


#endif



// ================================================================================