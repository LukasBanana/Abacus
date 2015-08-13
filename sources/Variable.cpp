/*
 * Variable.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Variable.h"


namespace Ac
{


Variable::Variable(const int_precision& iprec) :
    iprec_  ( iprec ),
    isFloat_( false )
{
}

Variable::Variable(const float_precision& fprec) :
    fprec_  ( fprec ),
    isFloat_( true  )
{
}

static bool IsFloat(const std::string& s)
{
    return s.find('.') != std::string::npos || s.find('E') != std::string::npos;
}

Variable::Variable(const std::string& value) :
    isFloat_( IsFloat(value) )
{
    
    if (isFloat_)
        fprec_ = float_precision(value.c_str());
    else
        iprec_ = int_precision(value.c_str());
}

void Variable::Add(Variable& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ += rhs.fprec_;
    else
        iprec_ += rhs.iprec_;
}

void Variable::Sub(Variable& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ -= rhs.fprec_;
    else
        iprec_ -= rhs.iprec_;
}

void Variable::Mul(Variable& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ *= rhs.fprec_;
    else
        iprec_ *= rhs.iprec_;
}

void Variable::Div(Variable& rhs)
{
    ToFloat();
    rhs.ToFloat();
    fprec_ /= rhs.fprec_;
}

void Variable::Mod(Variable& rhs)
{
    ToInt();
    rhs.ToInt();

    iprec_ %= rhs.iprec_;

    if (iprec_ < "0")
        iprec_ += rhs.iprec_;
}

void Variable::Pow(Variable& rhs)
{
    Unify(rhs);
    if (isFloat_)
        fprec_ = pow(fprec_, rhs.fprec_);
    else
    {
        if (rhs.iprec_ < 0)
        {
            ToFloat();
            Pow(rhs);
        }
        else
            iprec_ = ipow(iprec_, rhs.iprec_);
    }
}

void Variable::LShift(Variable& rhs)
{
    ToInt();
    rhs.ToInt();
    iprec_ <<= rhs.iprec_;
}

void Variable::RShift(Variable& rhs)
{
    ToInt();
    rhs.ToInt();
    iprec_ >>= rhs.iprec_;
}

void Variable::ToFloat()
{
    if (!isFloat_)
    {
        auto ival = iprec_.toString();
        fprec_ = float_precision(ival.c_str());
        isFloat_ = true;
    }
}

void Variable::ToInt()
{
    if (isFloat_)
    {
        iprec_ = fprec_.to_int_precision();
        isFloat_ = false;
    }
}

void Variable::Unify(Variable& rhs)
{
    if (isFloat_ && !rhs.isFloat_)
        rhs.ToFloat();
    else if (!isFloat_ && rhs.isFloat_)
        ToFloat();
}

void Variable::Negate()
{
    if (isFloat_)
        fprec_ = -fprec_;
    else
        iprec_ = -iprec_;
}

// simple factorial function for big-integers in imperative style
static void IntPrecFactorial(int_precision& n)
{
    if (n == 0)
        n = 1;
    else
    {
        bool isNeg = false;
        if (n < 0)
        {
            isNeg = true;
            n = -n;
        }

        auto m = n;
        while (m > 1)
        {
            --m;
            n *= m;
        }

        if (isNeg)
            n = -n;
    }
}

void Variable::Factorial()
{
    ToInt();
    IntPrecFactorial(iprec_);
}

void Variable::Normalize()
{
    if (isFloat_)
        fprec_ = abs(fprec_);
    else
        iprec_ = abs(iprec_);
}

Variable::operator std::string ()
{
    return isFloat_ ? fprec_.toString() : iprec_.toString();
}


} // /namespace HTLib



// ================================================================================