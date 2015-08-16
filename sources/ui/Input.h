/*
 * Input.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_INPUT_H__
#define __AC_INPUT_H__


#include "History.h"

#include <wx/textctrl.h>
#include <wx/font.h>
#include <functional>
#include <string>


class Input : public wxTextCtrl
{

    public:
        
        using Callback = std::function<void(const std::string&)>;
        using HistoryType = History<std::string>;

        Input(
            wxWindow* parent, const wxPoint& pos, const wxSize& size,
            const wxFont& font, const Callback& callback
        );

        void Replace(std::string s);

        std::string Get() const;

        const HistoryType& GetHistory() const
        {
            return history_;
        }
        HistoryType& GetHistory()
        {
            return history_;
        }

    private:
        
        long ClampPos(long pos) const;

        void MoveCursorLeft(bool shift);
        void MoveCursorRight(bool shift);
        void LocateCursor(long pos, bool shift);
        
        void Insert(char chr);
        void Insert(const wxString& str);
        void Erase(long dir);

        void Enter();
        void HistoryPrev();
        void HistoryNext();

        std::pair<long, long> GetSelectionRange() const;
        bool IsAllSelected() const;
        void SwitchSelectAll();

        void StoreTemp();

        void OnChar(wxKeyEvent& event);

        wxTextPos   selStart_ = 0l;
        Callback    callback_;

        HistoryType history_;
        std::string tempInput_;

};


#endif



// ================================================================================