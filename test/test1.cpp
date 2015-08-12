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

    private:
        
        LogOutput& log_;

};

int main()
{
    LogOutput log;

    // parsing test
    auto ast = ParseExpression(
        //"3 + 2 ^ -1 mod 7.2 - 4 + 1"
        //"sin(x) * cos(2 mod e^log(2, 10))"
        "3 mod 5"
        ,&log
    );

    if (ast)
    {
        Printer printer(log);
        ast->Visit(&printer);
    }

    // constants test
    ConstantsSet constants;
    
    Compute("x = y = sin(pi/2)", constants, &log);
    Compute("z = 0.5", constants, &log);

    std::cout << "constants:" << std::endl;

    for (const auto& c : constants.constants)
        std::cout << c.first << " = " << c.second << std::endl;

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}


// ================================================================================
