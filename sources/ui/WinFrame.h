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


// enable multi-threaded computation (to avoid deadlocks in UI)
#define AC_MULTI_THREADED

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

        void MoveCursorLeft(bool shift);
        void MoveCursorRight(bool shift);
        void LocateCursor(long pos, bool shift);
        void Insert(char chr);
        void Remove(long dir);

        void OnTextEnter(wxCommandEvent& event);
        void OnChar(wxKeyEvent& event);
        void OnClose(wxCloseEvent& event);

        void ComputeThreadProc(const std::string& expr);

        #ifdef AC_MULTI_THREADED
        void JoinThread();
        #endif

        wxFont*                         stdFont_    = nullptr;
        wxFont*                         smallFont_  = nullptr;

        wxTextCtrl*                     inCtrl_     = nullptr;
        wxTextCtrl*                     outCtrl_    = nullptr;

        wxStatusBar*                    statusBar_  = nullptr;

        wxTextPos                       selStart_   = 0l;

        Ac::ConstantsSet                constantsSet_;

        #ifdef AC_MULTI_THREADED
        std::unique_ptr<std::thread>    thread_;
        std::atomic_bool                computing_;
        #endif

};


#endif



// ================================================================================