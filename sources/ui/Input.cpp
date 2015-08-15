/*
 * Input.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Input.h"

#include <wx/utils.h>
#include <wx/valtext.h>
#include <string>
#include <vector>
#include <algorithm>


Input::Input(wxWindow* parent, const wxPoint& pos, const wxSize& size, const wxFont& font, const Callback& callback) :
    wxTextCtrl  ( parent, wxID_ANY, "", pos, size, wxTE_PROCESS_ENTER ),
    callback_   ( callback                                            )
{
    SetFont(font);
    Bind(wxEVT_CHAR, &Input::OnChar, this);
}


/*
 * ======= Private: =======
 */

void Input::MoveCursorLeft(bool shift)
{
    auto pos = std::max(GetInsertionPoint() - 1l, 0l);
    
    /* Get previous selection */
    long from, to;
    GetSelection(&from, &to);

    /* Set new insertion position */
    if (from == to)
        selStart_ = to;
    else
        pos = from;

    SetInsertionPoint(pos);
    selStart_ = pos;

    /* Update selection */
    if (shift)
        SetSelection(std::max(from - 1l, 0l), to);
}

void Input::MoveCursorRight(bool shift)
{
    auto pos = std::min(GetInsertionPoint() + 1l, GetLastPosition());

    /* Get previous selection */
    long from, to;
    GetSelection(&from, &to);

    /* Set new insertion position */
    if (from == to)
        selStart_ = from;
    else
        pos = to;

    SetInsertionPoint(pos);
    selStart_ = pos;

    /* Update selection */
    if (shift)
        SetSelection(from, std::min(to + 1l, GetLastPosition()));
}

void Input::LocateCursor(long pos, bool shift)
{
    pos = std::max(0l, std::min(pos, GetLastPosition()));

    /* Get previous selection */
    long from, to;
    GetSelection(&from, &to);

    if (shift)
    {
        if (selStart_ < pos)
            SetSelection(selStart_, pos);
        else
            SetSelection(pos, selStart_);
    }
    else
    {
        SetInsertionPoint(pos);
        selStart_ = pos;
    }
}

void Input::Insert(char chr)
{
    WriteText(wxString(1, chr));
}

void Input::Erase(long dir)
{
    /* Get previous selection */
    long from, to;
    GetSelection(&from, &to);

    if (from != to)
        Remove(from, to);
    else
    {
        auto pos = GetInsertionPoint();
    
        if (dir < 0 && pos > 0)
            Remove(std::max(0l, pos + dir), pos);
        else if (dir > 0 && pos < GetLastPosition())
            Remove(pos, std::min(pos + dir, GetLastPosition()));
    }
}

static const char charMul = char(215);
static const char charDiv = char(247);

static bool IsValidInputChar(char c)
{
    return
        ( c >= '0' && c <= '9' ) ||
        ( c >= 'a' && c <= 'z' ) ||
        ( c >= 'A' && c <= 'Z' ) ||
        ( c == '(' || c == ')' ) ||
        ( c == '[' || c == ']' ) ||
        ( c == '{' || c == '}' ) ||
        ( c == '+' || c == '-' ) ||
        ( c == '=' || c == '!' ) ||
        ( c == ',' || c == '.' ) ||
        ( c == '^' || c == '|' ) ||
        ( c == ' ' || c == '_' ) ||
        ( c == '>' || c == '<' );
}

static void AdjustExpr(std::string& s)
{
    for (auto& c : s)
    {
        if (c == charMul)
            c = '*';
        else if (c == charDiv)
            c = '/';
    }
}

void Input::OnChar(wxKeyEvent& event)
{
    auto key = event.GetKeyCode();
    auto chr = static_cast<char>(key);

    bool shift = event.ShiftDown();
    bool ctrl = event.ControlDown();

    if (ctrl)
    {
        if (key == WXK_CONTROL_A)
            SetSelection(0, GetLastPosition());
    }
    else
    {
        switch (key)
        {
            case WXK_LEFT:
                MoveCursorLeft(shift);
                break;
            case WXK_RIGHT:
                MoveCursorRight(shift);
                break;
            case WXK_HOME:
                LocateCursor(0, shift);
                break;
            case WXK_END:
                LocateCursor(GetLastPosition(), shift);
                break;
            case WXK_ESCAPE:
                SetValue("");
                break;
            case WXK_BACK:
                Erase(-1);
                break;
            case WXK_DELETE:
                Erase(1);
                break;
            case WXK_RETURN:
                if (callback_)
                {
                    auto s = GetValue().ToStdString();
                    AdjustExpr(s);
                    callback_(s);
                }
                break;
            default:
                if (key > 0 && key < 128)
                {
                    if (IsValidInputChar(chr))
                        Insert(chr);
                    else if (chr == '/')
                        Insert(charDiv);
                    else if (chr == '*')
                        Insert(charMul);
                }
                break;
        }
    }
}



// ================================================================================