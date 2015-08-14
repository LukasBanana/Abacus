/*
 * WinFrame.coo
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "WinFrame.h"

#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/valtext.h>


WinFrame::WinFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame( nullptr, wxID_ANY, title, pos, size, GetStyle() )
{
    computing_ = false;

    #ifdef _WINDOWS
    SetIcon(wxICON(app_icon));
    #endif

    Ac::SetFloatPrecision(50);

    /* Create window layout */
    CreateStatBar();
    CreateFont();
    CreateInputCtrl();
    CreateOutputCtrl();

    Centre();

    ShowInfo();
}

using ComputeCallback = std::function<void(const std::string&)>;

class ErrorCollector : public Ac::Log
{
    
    public:
        
        void Error(const std::string& msg) override
        {
            errors_.push_back(msg);
        }

        const std::vector<std::string>& GetErrors() const
        {
            return errors_;
        }

        bool HasErrors() const
        {
            return !errors_.empty();
        }

    private:
        
        std::vector<std::string> errors_;

};

void WinFrame::ComputeThreadProc(const std::string& expr)
{
    constantsSet_.ResetStd();

    /* Compute math expression */
    ErrorCollector errHandler;
    auto result = Ac::Compute(expr, constantsSet_, &errHandler);

    /* Output result */
    if (errHandler.HasErrors())
    {
        wxArrayString s;
        for (const auto& e : errHandler.GetErrors())
            s.Add(e);
        ShowOutput(s, true);
    }
    else
        ShowOutput(result, true);

    computing_ = false;
}

bool WinFrame::ExecExpr(const wxString& expr)
{
    /* If expression is empty -> show information */
    if (expr.empty())
    {
        ShowInfo();
        return false;
    }

    /* Show status message */
    ShowOutput("computing ...", true);

    if (computing_)
        return false;

    /* Wait until previous thread has successfully terminated */
    JoinThread();

    /* Compute expression */
    computing_ = true;

    thread_ = std::unique_ptr<std::thread>(new std::thread(&WinFrame::ComputeThreadProc, this, expr.ToStdString()));

    return true;
}


/*
 * ======= Private: =======
 */

static const int textFieldSize = 25;
static const int border = 10;

void WinFrame::CreateFont()
{
    /* Create standard font */
    stdFont_ = new wxFont(
        textFieldSize/2,
        wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_LIGHT,
        false,
        "courier new"
    );
    
    /* Create error font */
    smallFont_ = new wxFont(
        textFieldSize/3,
        wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_LIGHT,
        false,
        "courier new"
    );
}

void WinFrame::CreateInputCtrl()
{
    auto clientSize = GetClientSize();

    wxTextValidator validator(wxFILTER_EMPTY | wxFILTER_EXCLUDE_LIST);
    validator.SetCharExcludes("~φόδ\"§$%&{}[]°#\'\\΄`");

    inCtrl_ = new wxTextCtrl(
        this, wxID_ANY, "",
        wxPoint(border, border),
        wxSize(clientSize.GetWidth() - border*2, textFieldSize),
        wxTE_PROCESS_ENTER,
        validator
    );

    inCtrl_->SetFont(*stdFont_);

    inCtrl_->Bind(wxEVT_TEXT_ENTER, &WinFrame::OnTextEnter, this);
    Bind(wxEVT_CLOSE_WINDOW, &WinFrame::OnClose, this);
}

void WinFrame::CreateOutputCtrl()
{
    auto clientSize = GetClientSize();
    auto posY = border*2 + textFieldSize;

    outCtrl_ = new wxTextCtrl(
        this, wxID_ANY, "",
        wxPoint(border, posY),
        wxSize(
            clientSize.GetWidth() - border*2,
            clientSize.GetHeight() - posY - border
        ),
        wxTE_READONLY | wxTE_MULTILINE
    );

    outCtrl_->SetFont(*stdFont_);
}

void WinFrame::CreateStatBar()
{
    statusBar_ = CreateStatusBar();
    statusBar_->PushStatusText("Version 1.00 Alpha", 0);
}

long WinFrame::GetStyle() const
{
    return wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN | wxMINIMIZE_BOX | wxCLOSE_BOX;
}

void WinFrame::ShowOutput(const wxString& out, bool smallView)
{
    wxArrayString s;
    s.Add(out);
    ShowOutput(s, smallView);
}

void WinFrame::ShowOutput(const wxArrayString& out, bool smallView)
{
    wxString str;

    for (std::size_t i = 0; i < out.GetCount(); ++i)
        str += out[i] + "\n";

    outCtrl_->SetValue(str);
    outCtrl_->SetFont(smallView ? *smallFont_ : *stdFont_);
}

void WinFrame::ShowInfo()
{
    /* Startup text */
    wxArrayString Info;
    {
        Info.Add("Abacus Calculator");
        Info.Add("");
        Info.Add("Copyright (C) 2015  Lukas Hermanns");
        Info.Add("Licensed under the terms of the 3-Clause BSD License");
        Info.Add("");
        Info.Add("Enter arithmetic expressions in the above text field");
        Info.Add("");
        Info.Add("Supported Operators:");
        Info.Add("  A + B           A plus B");
        Info.Add("  A - B           A minus B");
        Info.Add("  A * B           A multiplied by B");
        Info.Add("  A / B           A divided by B");
        Info.Add("  A ^ B           A power of B");
        Info.Add("  A mod B         A modulo B");
        Info.Add("  X << S          X left-shifted by S");
        Info.Add("  X >> S          X right-shifted by S");
        Info.Add("  X!              Factorial of X");
        Info.Add("  | X |           Norm of X");
        Info.Add("");
        Info.Add("Standard Constants:");
        Info.Add("  pi              Number pi ~ 3.14...");
        Info.Add("  e               Euler's number e ~ 2.71...");
        Info.Add("");
        Info.Add("Supported Functions:");
        Info.Add("  sin(X)          Sine of X");
        Info.Add("  cos(X)          Cosine of X");
        Info.Add("  tan(X)          Tangent of X");
        Info.Add("  asin(X)         Arc sine of X");
        Info.Add("  acos(X)         Arc cosine of X");
        Info.Add("  atan(X)         Arc tangent of X");
        Info.Add("  atan2(X, Y)     Arc tangent of Y/X");
        Info.Add("  sinh(X)         Hyperbolic sine of X");
        Info.Add("  cosh(X)         Hyperbolic cosine of X");
        Info.Add("  tanh(X)         Hyperbolic tangent of X");
        Info.Add("  asinh(X)        Hyperbolic arc sine of X");
        Info.Add("  acosh(X)        Hyperbolic arc cosine of X");
        Info.Add("  atanh(X)        Hyperbolic arc tangent of X");
        Info.Add("  pow(B, E)       B power of E");
        Info.Add("  exp(X)          Exponetial function of X (e^X)");
        Info.Add("  sqrt(X)         Square root of X");
        Info.Add("  log(X)          Logarithm of X to base e");
        Info.Add("  log10(X)        Logarithm of X to base 10");
        Info.Add("  abs(X)          Absolute value of X");
        Info.Add("");
        Info.Add("Special Input:");
        Info.Add("  exit            quit application");
        Info.Add("  demo            shows the next expresion for demonstration");
    }
    ShowOutput(Info, true);
}

void WinFrame::OnTextEnter(wxCommandEvent& Event)
{
    auto s = Event.GetString();

    if (s == "exit")
        Close();
    else
        ExecExpr(s);
}

void WinFrame::OnClose(wxCloseEvent& event)
{
    if (thread_)
    {
        if (computing_)
        {
            auto answer = wxMessageBox(
                "A computation process has not been completed!\n\nDo you want to terminate this process?",
                "Computation Pending",
                wxYES | wxNO | wxNO_DEFAULT
            );

            if (answer == wxYES)
            {
                /* Terminate abnormal */
                exit(0);
            }
            else
                return;
        }
        else
            JoinThread();
    }

    /* Close frame */
    wxFrame::OnCloseWindow(event);
}

void WinFrame::JoinThread()
{
    if (thread_)
    {
        thread_->join();
        thread_ = nullptr;
    }
}



// ================================================================================