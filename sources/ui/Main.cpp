/*
 * Main.cpp
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "WinFrame.h"

#include <wx/app.h>


class AbacusApp : public wxApp
{
    
    public:

        bool OnInit() override
        {
            if (!wxApp::OnInit())
                return false;
                
            auto frame = new WinFrame("Abacus", wxDefaultPosition, wxSize(600, 250));
            frame->Show(true);

            return true;
        }

};


IMPLEMENT_APP(AbacusApp)



// ================================================================================