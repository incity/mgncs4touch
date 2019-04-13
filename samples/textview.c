/*
 * \file toggle.c
 * \author FMSoft
 * \date 2010/10/09
 *
 \verbatim

    This file is part of mGNCS4Touch, one of MiniGUI components.

    Copyright (C) 2008-2018 FMSoft (http://www.fmsoft.cn).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include <mgncs4touch/mgncs4touch.h>
// #define NDEBUG	1
#include <mgncs4touch/mtouchdebug.h>

#define ID_TEXTVIEW     101
#define ID_HTEXTVIEW    102
#define ID_TOGGLE 103

char *newtext = "##################1#################";

static void toggle_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    HWND hwnd = GetDlgItem(GetParent(self->hwnd), ID_TEXTVIEW);
    SendMessage(hwnd, MSG_SETTEXT, 0, (LPARAM)newtext);
}


static NCS_EVENT_HANDLER toggle_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_TOGGLE_STATUSCHANGED, toggle_notify),
	{0, NULL}	
};

static NCS_PROP_ENTRY toggle_props [] = {
    {NCSP_TOGGLE_STATUS, NCS_TOGGLE_ON},
    {0, NULL}
};
    

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = 
{
	{
		NCSCTRL_TEXTVIEW, 
		ID_TEXTVIEW,
		120, 220, 100, 50,
		WS_VISIBLE,
		WS_EX_TRANSPARENT,
		"012345678909876543210\n-=qwertyuiop\n[]asdfghjkl\n;'zxcvbnm\n,./~!@#$%^\n&*()_+",
		NULL, //props,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
        NCSCTRL_TEXTVIEW, 
        ID_HTEXTVIEW,
        120, 80, 200, 50,
        WS_VISIBLE | NCSS_TEXTVIEW_VERTICAL,
        WS_EX_TRANSPARENT,
        "012345678909876543210-=qwertyuiop[]asdfghjkl\n;'zxcvbnm\n,./~!@#$%^\n&*()_+",
        NULL, //props,
        NULL, //rdr_info
        NULL, //handlers,
        NULL, //controls
        0,
        0 //add data
    },
	{
	NCSCTRL_TOGGLE, 
	ID_TOGGLE,
	20, 20, 52, 32,
	WS_VISIBLE,
	WS_EX_TRANSPARENT,
	"I\nO",
	toggle_props, //props,
	NULL, //rdr_info
	toggle_handlers, //handlers,
	NULL, //controls
	0,
	0 //add data
},	
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
    {MSG_LBUTTONDOWN, SpeedMeterMessageHandler },
    {MSG_LBUTTONUP, SpeedMeterMessageHandler },
    {MSG_MOUSEMOVE, SpeedMeterMessageHandler },
    {0, NULL}
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
    NCSCTRL_MAINWND, 
    1,
    0, 0, 480, 480,
    WS_CAPTION | WS_BORDER | WS_VISIBLE,
    WS_EX_AUTOSECONDARYDC,
    "TextView",
    NULL,
    NULL,
    mymain_handlers,
    _ctrl_templ,
    sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
    0,
    0,
    0,
};

int MiniGUIMain(int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, NULL, 0, 0);
#endif
	
    ncsInitialize();
    ncs4TouchInitialize();
    mDialogBox* mydlg = (mDialogBox*)ncsCreateMainWindowIndirect 
    						(&mymain_templ, HWND_DESKTOP);
    mGEffInit();
    _c(mydlg)->doModal(mydlg, TRUE);
    mGEffDeinit();
    ncs4TouchUninitialize();
    ncsUninitialize();
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

