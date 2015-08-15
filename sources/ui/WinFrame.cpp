/*
 * WinFrame.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "WinFrame.h"
#include "Input.h"

#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/valtext.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>


static const std::string configFilename = "abacus-config.ini";

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

WinFrame::WinFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame( nullptr, wxID_ANY, title, pos, size, GetStyle() )
{
    #ifdef AC_MULTI_THREADED
    computing_ = false;
    #endif

    #ifdef _WINDOWS
    SetIcon(wxICON(app_icon));
    #endif

    Ac::SetFloatPrecision(50);

    /* Create window layout */
    CreateStatBar();
    CreateFont();
    CreateInputCtrl();
    CreateOutputCtrl();

    SetMinClientSize(wxSize(300, 100));

    Bind(wxEVT_CLOSE_WINDOW, &WinFrame::OnClose, this);
    Bind(wxEVT_SIZE, &WinFrame::OnResize, this);

    Centre();

    ShowInfo();

    LoadConfig(configFilename);
}

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
        SetOutput(s);
    }
    else
        SetOutput(result);

    #ifdef AC_MULTI_THREADED
    computing_ = false;
    #endif
}

bool WinFrame::ExecExpr(const std::string& expr)
{
    /* If expression is empty -> show information */
    if (expr.empty())
        ShowInfo();
    else if (expr == "exit")
    {
        /* Clear input to avoid storing "exit" in the config-file */
        SetInput("");
        Close();
    }
    else if (expr == "demo")
        ShowDemo();
    else if (expr == "const")
        ShowConstants();
    else
    {
        /* Show status message */
        SetOutput("computing ...");

        #ifdef AC_MULTI_THREADED
    
        /* Wait until previous thread has successfully terminated */
        if (computing_)
            return false;
        JoinThread();
    
        #endif

        /* Compute expression */
        #ifdef AC_MULTI_THREADED

        computing_ = true;
        thread_ = std::unique_ptr<std::thread>(new std::thread(&WinFrame::ComputeThreadProc, this, expr));
    
        #else
    
        ComputeThreadProc(expr.ToStdString());
    
        #endif
    }

    return true;
}


/*
 * ======= Private: =======
 */

static const int textFieldSize  = 25;
static const int border         = 10;

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

    inCtrl_ = new Input(
        this,
        wxPoint(border, border),
        wxSize(clientSize.GetWidth() - border*2, textFieldSize),
        *stdFont_,
        [&](const std::string& s){ ExecExpr(s); }
    );
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
        wxTE_READONLY | wxTE_MULTILINE | wxTE_CHARWRAP
    );

    outCtrl_->SetFont(*stdFont_);
}

void WinFrame::CreateStatBar()
{
    statusBar_ = CreateStatusBar();
    statusBar_->PushStatusText("Version " __AC_VERSION_STR__, 0);
}

long WinFrame::GetStyle() const
{
    return wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCLOSE_BOX;
}

void WinFrame::SetOutput(const wxString& out, bool largeView)
{
    wxArrayString s;
    s.Add(out);
    SetOutput(s, largeView);
}

void WinFrame::SetOutput(const wxArrayString& out, bool largeView)
{
    wxString str;

    for (std::size_t i = 0; i < out.GetCount(); ++i)
        str += out[i] + "\n";

    outCtrl_->SetValue(str);
    outCtrl_->SetFont(largeView ? *stdFont_ : *smallFont_);
}

void WinFrame::SetInput(const std::string& in)
{
    inCtrl_->Replace(in);
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
        Info.Add("  A + B               A plus B");
        Info.Add("  A - B               A minus B");
        Info.Add("  A * B               A multiplied by B");
        Info.Add("  A / B               A divided by B");
        Info.Add("  A ^ B               A power of B");
        Info.Add("  A mod B             A modulo B");
        Info.Add("  X << S              X bitwise left-shifted by S");
        Info.Add("  X >> S              X bitwise right-shifted by S");
        Info.Add("  X!                  Factorial of X");
        Info.Add("  | X |               Norm of X");
        Info.Add("");
        Info.Add("Standard Constants:");
        Info.Add("  pi                  Number pi ~ 3.14...");
        Info.Add("  e                   Euler's number e ~ 2.71...");
        Info.Add("");
        Info.Add("Supported Functions:");
        Info.Add("  sin(X)              Sine of X");
        Info.Add("  cos(X)              Cosine of X");
        Info.Add("  tan(X)              Tangent of X");
        Info.Add("  asin(X)             Arc sine of X");
        Info.Add("  acos(X)             Arc cosine of X");
        Info.Add("  atan(X)             Arc tangent of X");
        Info.Add("  atan2(X, Y)         Arc tangent of Y/X");
        Info.Add("  sinh(X)             Hyperbolic sine of X");
        Info.Add("  cosh(X)             Hyperbolic cosine of X");
        Info.Add("  tanh(X)             Hyperbolic tangent of X");
        Info.Add("  asinh(X)            Hyperbolic arc sine of X");
        Info.Add("  acosh(X)            Hyperbolic arc cosine of X");
        Info.Add("  atanh(X)            Hyperbolic arc tangent of X");
        Info.Add("  pow(B, E)           B power of E");
        Info.Add("  exp(X)              Exponetial function of X (e^X)");
        Info.Add("  sqrt(X)             Square root of X");
        Info.Add("  log(X)              Logarithm of X to base e");
        Info.Add("  log10(X)            Logarithm of X to base 10");
        Info.Add("  abs(X)              Absolute value of X");
        Info.Add("  ceil(X)             Rounds X upward");
        Info.Add("  floor(X)            Rounds X downward");
        Info.Add("  rand()              Pseudo random number in [0, 1]");
        Info.Add("  min(X1, ..., Xn)    Minimum of all values");
        Info.Add("  max(X1, ..., Xn)    Maximum of all values");
        Info.Add("");
        Info.Add("Special Input:");
        Info.Add("  exit                Quit application");
        Info.Add("  const               Shows all stored constants");
        Info.Add("  demo                Shows the next expresion for demonstration");
    }
    SetOutput(Info);
}

void WinFrame::ShowDemo()
{
    static int n;

    static const char* demos[] =
    {
        "5 + (6! - 8^3)*2.5",
        "x = 6",
        "y = z = x*2",
        "x+y+z",
        "log 10 + 26",
        "100!",
        "10^10^10",
        "3---8",
        "|3---8|",
        "sin pi/2 + sqrt(1 + (2^-12)*2)",
        "sum[i=1, 10](i*2 - 2^i)",
        "cross([1, 2, 3], [4, 5, 6])",
        "| [0, -1, 2] - [4, 0.5, 3] |",
        nullptr
    };

    /* Set next demo */
    if (!demos[n])
        n = 0;

    SetInput(demos[n]);
    ExecExpr(demos[n]);

    ++n;
}

void WinFrame::ShowConstants()
{
    wxArrayString s;

    std::size_t maxLen = 0;
    for (const auto& c : constantsSet_.constants)
        maxLen = std::max(maxLen, c.first.size());

    for (const auto& c : constantsSet_.constants)
        s.Add(c.first + std::string(maxLen - c.first.size(), ' ') + " = " + c.second);

    SetOutput(s);
}

void WinFrame::OnClose(wxCloseEvent& event)
{
    #ifdef AC_MULTI_THREADED

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

    #endif

    /* Save configuration */
    SaveConfig(configFilename);

    /* Close frame */
    wxFrame::OnCloseWindow(event);
}

void WinFrame::OnResize(wxSizeEvent& event)
{
    auto clientSize = GetClientSize();
    auto posY = border*2 + textFieldSize;

    inCtrl_->SetSize(wxSize(clientSize.GetWidth() - border*2, textFieldSize));
    outCtrl_->SetSize(wxSize(clientSize.GetWidth() - border*2, clientSize.GetHeight() - posY - border));
}

#ifdef AC_MULTI_THREADED

void WinFrame::JoinThread()
{
    if (thread_)
    {
        thread_->join();
        thread_ = nullptr;
    }
}

#endif

bool WinFrame::SaveConfig(const std::string& filename)
{
    /* Open file for writing */
    std::ofstream file(filename);
    if (!file.good())
        return false;

    /* Define [W]rite function */
    auto W = [&file](const std::string& ident, const std::string& value)
    {
        file << '\"' << ident << "\" = \"" << value << '\"' << std::endl;
    };

    /* Store window location */
    W("$window_x", std::to_string(GetPosition().x));
    W("$window_y", std::to_string(GetPosition().y));
    W("$window_width", std::to_string(GetSize().GetWidth()));
    W("$window_height", std::to_string(GetSize().GetHeight()));

    /* Store last input */
    W("$input", inCtrl_->GetValue().ToStdString());

    /* Store history */
    std::size_t historyIdx = 0;
    for (const auto& v : inCtrl_->GetHistory().GetValues())
        W("$history_" + std::to_string(historyIdx++), v);

    /* Store constants */
    for (const auto& c : constantsSet_.constants)
        W(c.first, c.second);

    return true;
}

template <typename T>
T Num(const std::string& s)
{
    T val = T(0);
    std::istringstream stream(s);
    stream >> val;
    return val;
}

bool WinFrame::LoadConfig(const std::string& filename)
{
    /* Open file for writing */
    std::ifstream file(filename);
    if (!file.good())
        return false;

    /* Define [R]ead function */
    std::string line, ident, value;

    auto R = [&line](std::size_t& pos) -> std::string
    {
        pos = line.find('\"', pos);
        if (pos != std::string::npos)
        {
            auto start = ++pos;
            pos = line.find('\"', pos);
            if (pos != std::string::npos)
            {
                auto end = pos++;
                return line.substr(start, end - start);
            }
        }
        return "";
    };

    auto RInt = [&value]()
    {
        return Num<int>(value);
    };

    /* Read file content */
    while (!file.eof())
    {
        /* Read next line from file */
        std::getline(file, line);
        
        std::size_t pos = 0;
        ident = R(pos);
        value = R(pos);

        /* Map values to configurations */
        if (ident.empty())
            continue;

        if (ident.front() == '$')
        {
            if (ident == "$window_x")
                SetPosition(wxPoint(RInt(), GetPosition().y));
            else if (ident == "$window_y")
                SetPosition(wxPoint(GetPosition().x, RInt()));
            else if (ident == "$window_width")
                SetSize(wxSize(RInt(), GetSize().GetHeight()));
            else if (ident == "$window_height")
                SetSize(wxSize(GetSize().GetWidth(), RInt()));
            else if (ident == "$input")
                SetInput(value);
            else if (ident.size() > 9 && ident.substr(0, 9) == "$history_")
                inCtrl_->GetHistory().Add(value);
        }
        else
            constantsSet_.constants[ident] = value;
    }

    return true;
}



// ================================================================================