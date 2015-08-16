/*
 * Input.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Input.h"

#include <wx/utils.h>
#include <wx/valtext.h>
#include <wx/clipbrd.h>

#include <string>
#include <vector>
#include <algorithm>


static const char charMul = char(215);
static const char charDiv = char(247);

static void AdjustExprOut(std::string& s)
{
    for (auto& c : s)
    {
        if (c == charMul)
            c = '*';
        else if (c == charDiv)
            c = '/';
    }
}

static void AdjustExprIn(std::string& s)
{
    for (auto& c : s)
    {
        if (c == '*')
            c = charMul;
        else if (c == '/')
            c = charDiv;
    }
}

Input::Input(wxWindow* parent, const wxPoint& pos, const wxSize& size, const wxFont& font, const Callback& callback) :
    wxTextCtrl  ( parent, wxID_ANY, "", pos, size, wxTE_PROCESS_ENTER ),
    callback_   ( callback                                            )
{
    SetFont(font);
    Bind(wxEVT_CHAR, &Input::OnChar, this);
}

void Input::Replace(std::string s)
{
    /* Get previous insertion position */
    auto pos = GetCursorPos();
    
    /* Set new text input */
    AdjustExprIn(s);
    SetValue(s);

    /* Reset insertion position */
    LocateCursor(pos);
}

void Input::Cut()
{
    Copy();

    /* Remove selected text */
    long from, to;
    GetSelection(&from, &to);
    Remove(from, to);
}

void Input::Copy()
{
    /* Copy adjusted selection to clipboard */
    std::string s = GetStringSelection().ToStdString();
    AdjustExprOut(s);
    wxClipboard::Get()->SetData(new wxTextDataObject(s));
}

void Input::Paste()
{
    wxTextDataObject data;
    if (wxClipboard::Get()->GetData(data))
    {
        /* Paste text from clipboard */
        std::string s = data.GetText().ToStdString();
        AdjustExprIn(s);
        Insert(s);
    }
}

std::string Input::Get() const
{
    auto s = GetValue().ToStdString();
    AdjustExprOut(s);
    return s;
}


/*
 * ======= Private: =======
 */

long Input::ClampPos(long pos) const
{
    return std::max(0l, std::min(pos, GetLastPosition()));
}

wxTextPos Input::GetCursorPos() const
{
    return IsSelected() ? cursorPos_ : GetInsertionPoint();
}

void Input::MoveCursorLeft(bool shift)
{
    cursorPos_ = std::max(0l, GetCursorPos() - 1l);
    
    if (shift)
        SetSelection(cursorPos_, selStart_);
    else
    {
        SetInsertionPoint(cursorPos_);
        selStart_ = cursorPos_;
    }
}

void Input::MoveCursorRight(bool shift)
{
    cursorPos_ = std::min(GetCursorPos() + 1l, GetLastPosition());

    if (shift)
        SetSelection(selStart_, cursorPos_);
    else
    {
        SetInsertionPoint(cursorPos_);
        selStart_ = cursorPos_;
    }
}

void Input::LocateCursor(long pos, bool shift)
{
    cursorPos_ = ClampPos(pos);

    if (shift)
    {
        if (selStart_ < cursorPos_)
            SetSelection(selStart_, cursorPos_);
        else
            SetSelection(cursorPos_, selStart_);
    }
    else
    {
        SetInsertionPoint(cursorPos_);
        selStart_ = cursorPos_;
    }
}

void Input::Insert(char chr)
{
    WriteText(wxString(1, chr));
    StoreTemp();
}

void Input::Insert(const wxString& str)
{
    WriteText(str);
    StoreTemp();
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
        auto pos = GetCursorPos();
        
        if (dir < 0 && pos > 0)
            Remove(std::max(0l, pos + dir), pos);
        else if (dir > 0 && pos < GetLastPosition())
            Remove(pos, std::min(pos + dir, GetLastPosition()));
    }

    StoreTemp();
}

void Input::Enter()
{
    auto s = Get();

    /* Commit callback */
    if (callback_)
        callback_(s);

    if (!s.empty())
    {
        #if 0//todo -> make this optional for the user!!!
        /* Select all to make removal for user easier */
        SelectAll();
        #endif

        /* Store value in history */
        history_.Add(s);
    }
}

void Input::HistoryPrev()
{
    history_.Prev();
    std::string s;
    if (history_.Get(s))
        Replace(s);
}

void Input::HistoryNext()
{
    history_.Next();
    std::string s;
    if (history_.Get(s))
        Replace(s);
    else
        Replace(tempInput_);
}

std::pair<long, long> Input::GetSelectionRange() const
{
    long from, to;
    GetSelection(&from, &to);
    return { from, to };
}

bool Input::IsSelected() const
{
    auto range = GetSelectionRange();
    return range.first != range.second;
}

bool Input::IsAllSelected() const
{
    auto range = GetSelectionRange();
    return range.first == 0 && range.second == GetLastPosition();
}

void Input::SwitchSelectAll()
{
    if (IsAllSelected())
    {
        SelectNone();
        LocateCursor(selStart_);
    }
    else
        SelectAll();
}

void Input::StoreTemp()
{
    tempInput_ = Get();
}

static bool IsValidInputChar(char c)
{
    return
        ( c >= '0' && c <= '9' ) ||
        ( c >= 'a' && c <= 'z' ) ||
        ( c >= 'A' && c <= 'Z' ) ||
        ( c == '(' || c == ')' ) ||
        ( c == '[' || c == ']' ) ||
        //( c == '{' || c == '}' ) ||
        ( c == '+' || c == '-' ) ||
        ( c == '=' || c == '!' ) ||
        ( c == ',' || c == '.' ) ||
        ( c == '^' || c == '|' ) ||
        ( c == ' ' || c == '_' ) ||
        ( c == '>' || c == '<' );
}

void Input::OnChar(wxKeyEvent& event)
{
    auto key = event.GetKeyCode();
    auto chr = static_cast<char>(key);

    bool shift = event.ShiftDown();
    bool ctrl = event.ControlDown();

    if (ctrl)
    {
        switch (key)
        {
            case WXK_CONTROL_A:
                SwitchSelectAll();
                break;
            case WXK_CONTROL_X:
                Cut();
                break;
            case WXK_CONTROL_C:
                Copy();
                break;
            case WXK_CONTROL_V:
                Paste();
                break;
            case WXK_CONTROL_Z:
                Undo();
                break;
            case WXK_CONTROL_Y:
                Redo();
                break;
        }
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
            case WXK_UP:
                HistoryPrev();
                break;
            case WXK_DOWN:
                HistoryNext();
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
                Enter();
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