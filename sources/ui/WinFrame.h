/*
 * WinFrame.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_WIN_FRAME_H__
#define __AC_WIN_FRAME_H__


#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/font.h>
#include <wx/statusbr.h>
#include <Abacus/Abacus.h>
#include <functional>
#include <thread>
#include <memory>
#include <atomic>


class WinFrame : public wxFrame
{

    public:
        
        WinFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

        bool ExecExpr(const wxString& expr);

    private:
        
        void CreateFont();
        void CreateInputCtrl();
        void CreateOutputCtrl();
        void CreateStatBar();

        long GetStyle() const;

        void SetOutput(const wxString& out, bool largeView = false);
        void SetOutput(const wxArrayString& out, bool largeView = false);

        void SetInput(const wxString& in);

        void ShowInfo();
        void ShowDemo();

        void OnTextEnter(wxCommandEvent& event);
        void OnTextChange(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);

        void ComputeThreadProc(const std::string& expr);

        void JoinThread();

        wxFont*                         stdFont_    = nullptr;
        wxFont*                         smallFont_  = nullptr;

        wxTextCtrl*                     inCtrl_     = nullptr;
        wxTextCtrl*                     outCtrl_    = nullptr;

        wxStatusBar*                    statusBar_  = nullptr;

        Ac::ConstantsSet                constantsSet_;

        std::unique_ptr<std::thread>    thread_;
        std::atomic_bool                computing_;

};


#endif



// ================================================================================