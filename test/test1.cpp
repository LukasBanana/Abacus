/*
 * test1.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Abacus/Abacus.h>
#include <iostream>


using namespace Ac;

class LogOutput : public Log
{
    
    public:
        
        void Error(const std::string& msg) override
        {
            std::cerr << msg << std::endl;
        }

        void Info(const std::string& s)
        {
            std::cout << GetIndent() << s << std::endl;
        }

};

class Printer : public Visitor
{
    
    public:
        
        Printer(LogOutput& log) :
            log_( log )
        {
        }

        void VisitUnaryExpr(UnaryExpr* ast, void* args) override
        {
            log_.Info("Unary Expr");
            auto dummy = log_.Indent();
            log_.Info(UnaryExpr::GetOperatorSpell(ast->op));
            Visit(ast->expr);
        }

        void VisitBinaryExpr(BinaryExpr* ast, void* args) override
        {
            log_.Info("Binary Expr");
            auto dummy = log_.Indent();
            Visit(ast->exprL);
            log_.Info(BinaryExpr::GetOperatorSpell(ast->op));
            Visit(ast->exprR);
        }

        void VisitLiteralExpr(LiteralExpr* ast, void* args) override
        {
            log_.Info("Literal Expr: " + ast->value);
        }

        void VisitIdentExpr(IdentExpr* ast, void* args) override
        {
            log_.Info("Ident Expr: " + ast->value);
        }

        void VisitFuncExpr(FuncExpr* ast, void* args) override
        {
            log_.Info("Func Expr: " + ast->name);
            auto dummy = log_.Indent();
            for (const auto& arg : ast->args)
                Visit(arg);
        }

        void VisitFoldExpr(FoldExpr* ast, void* args) override
        {
            log_.Info("Fold Expr: " + ast->func);
            auto dummy = log_.Indent();
            log_.Info("index: '" + ast->index + "'");
            Visit(ast->initExpr);
            Visit(ast->iterExpr);
            Visit(ast->loopExpr);
        }

        void VisitVectorExpr(VectorExpr* ast, void* args) override
        {
            log_.Info("Vector Expr");
            auto dummy = log_.Indent();
            for (const auto& comp : ast->components)
                Visit(comp);
        }

        void VisitDefExpr(DefExpr* ast, void* args) override
        {
            log_.Info("Def Expr: " + ast->ident);
            auto dummy = log_.Indent();
            Visit(ast->expr);
        }

    private:
        
        LogOutput& log_;

};

int main()
{
    LogOutput log;

    // parsing test
    std::cout << "parsing:" << std::endl << "--------" << std::endl;

    std::string expr =
        //"34834^32"
        //"1.3^1234.0"
        //"x*2 + y*0.1"
        //"-5*(sin(pi/3) + e^2)"
        //"---3!!!"
        //"5^-10"
        //"|-5|"
        //"1.2"
        "[dot([5, 1, 2], [5, 1, -1]), 2, 3]"
        //"1/3"
        //"-3 mod 5"
        //"9876123^34"
    ;

    auto ast = ParseExpression(expr, &log);

    if (ast)
    {
        Printer printer(log);
        ast->Visit(&printer);
    }

    // constants test
    std::cout << std::endl << "constants:" << std::endl << "----------" << std::endl;

    ConstantsSet constants;

    SetFloatPrecision(50);
    
    Compute("x = 2", constants, &log);
    Compute("y = -123.456", constants, &log);
    Compute("z = 5.1", constants, &log);

    for (const auto& c : constants.constants)
        std::cout << c.first << " = " << c.second << std::endl;

    // compute test
    std::cout << std::endl << "computing:" << std::endl << "----------" << std::endl;

    std::cout << expr << " = " << Compute(expr, constants, &log) << std::endl;

    // variable test
    std::cout << std::endl << "variables:" << std::endl << "----------" << std::endl;

    for (int i = 0; i < 10; ++i)
    {
        Compute("x = x+1", constants, &log);
        std::cout << "x = " << Compute("x", constants, &log) << std::endl;
    }

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}


// ================================================================================
