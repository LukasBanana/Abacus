/*
 * WinFrame.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_WIN_FRAME_H__
#define __AC_WIN_FRAME_H__


#include "Input.h"

#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/font.h>
#include <wx/statusbr.h>
#include <Abacus/Abacus.h>
#include <functional>
#include <thread>
#include <string>
#include <memory>
#include <atomic>


// enable multi-threaded computation (to avoid deadlocks in UI)
#define AC_MULTI_THREADED

class WinFrame : public wxFrame
{

    public:
        
        WinFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

        bool ExecExpr(const std::string& expr);

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
        void ShowConstants();

        void OnClose(wxCloseEvent& event);
        void OnResize(wxSizeEvent& event);

        void ComputeThreadProc(const std::string& expr);

        #ifdef AC_MULTI_THREADED
        void JoinThread();
        #endif

        bool SaveConfig(const std::string& filename);
        bool LoadConfig(const std::string& filename);

        wxFont*                         stdFont_    = nullptr;
        wxFont*                         smallFont_  = nullptr;

        Input*                          inCtrl_     = nullptr;
        wxTextCtrl*                     outCtrl_    = nullptr;

        wxStatusBar*                    statusBar_  = nullptr;

        Ac::ConstantsSet                constantsSet_;

        #ifdef AC_MULTI_THREADED
        std::unique_ptr<std::thread>    thread_;
        std::atomic_bool                computing_;
        #endif

};


#endif



// ================================================================================