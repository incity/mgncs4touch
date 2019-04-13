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

#define ID_BTN1     101



static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

extern DWORD ConfirmDialog (HWND hParent, const char* title,
                                const char* ok, const char* cancel,
                                const char* text, ...);

static void btn_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    printf ("TOGGLE (%d, %d) notify : %s\n", id, nc, add_data == 0 ? "OFF" : "ON");
    if(add_data == 1) {
        //ConfirmDialog(GetMainWindowHandle(self->hwnd), "Test Title", "Yes", "No", "Are you handsome?");
    }
}

static NCS_EVENT_HANDLER btn_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_TOGGLE_STATUSCHANGED, btn_notify),
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
		NCSCTRL_TOGGLE, 
		ID_BTN1,
		20, 20, 52, 32,
		WS_VISIBLE,
		WS_EX_TRANSPARENT,
		"I\nO",
		toggle_props, //props,
		NULL, //rdr_info
		btn_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},	
    {
		NCSCTRL_TOGGLE, 
		ID_BTN1+1,
		20, 70, 100, 50,
		WS_VISIBLE,
		WS_EX_TRANSPARENT,
		"ON\nOFF",
		NULL, //props,
		NULL, //rdr_info
		btn_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
		NCSCTRL_TOGGLE, 
		ID_BTN1+2,
		20, 140, 150, 50,
		WS_VISIBLE,
		WS_EX_TRANSPARENT,
		NULL,
		NULL, //props,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
		NCSCTRL_TOGGLE, 
		ID_BTN1+3,
		20, 210, 150, 50,
		WS_VISIBLE,
		WS_EX_TRANSPARENT,
		"OPEN\nCLOSE",
		NULL, //props,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	}
};
static void mymain_onPaint(mWidget *self, HDC hdc, const CLIPRGN* inv)
{
    RECT rcBg;
    GetClientRect(self->hwnd, &rcBg);
    SetBrushColor(hdc, 0xFFFFFFFF);
    FillBox(hdc, rcBg.left, rcBg.top, RECTW(rcBg), RECTH(rcBg));
}

BOOL mymain_onEraseBackground(mWidget* self, HDC hdc, const PRECT clip)
{
    RECT rcBg;
    GetClientRect(self->hwnd, &rcBg);
    SetBrushColor(hdc, 0xFFFFFFFF);
    if(clip) {
        FillBox(hdc, clip->left, clip->top, RECTWP(clip), RECTHP(clip));
    }
    else FillBox(hdc, rcBg.left, rcBg.top, RECTW(rcBg), RECTH(rcBg));
    return 1;
}

static NCS_EVENT_HANDLER mymain_handlers[] = {
    {MSG_CLOSE, mymain_onClose},
    {MSG_PAINT, mymain_onPaint},
    //{MSG_ERASEBKGND, mymain_onEraseBackground},
    {0, NULL}
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_MAINWND, 
	1,
	160, 164, 480, 320,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_AUTOSECONDARYDC,
    "SwitchButton Test ......",
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
    mGEffInit ();
	_c(mydlg)->doModal(mydlg, TRUE);
    mGEffDeinit ();
	ncs4TouchUninitialize();
	ncsUninitialize();
	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

