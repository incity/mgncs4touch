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

#define ID_DIALPAD     101
#define ID_PASSWORDBOX     102



static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

const static char *dialpad_key_numbers[] = {
    "1", "2", "3",  "[res/del.png]",
    "4", "5", "6", "0",
    "7", "8", "9", "#"
};

static void dialpad_notify(mWidget *self, int id, int nc, DWORD add_data)
{
    printf ("DIALPAD (%d, %d)\n", id, add_data);
}

static void passwordbox_notify(mPasswordBox *self, int id, int nc, DWORD add_data)
{
    printf ("PASSWORDBOX (%d, %d)\n", id, add_data);
    _c(self)->clear(self);
}


static NCS_EVENT_HANDLER dialpad_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_DIALPAD_KEYPAD_CLICKED, dialpad_notify),
    {0, NULL}
};


static NCS_EVENT_HANDLER passwordbox_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_PASSWORDBOX_ENTERED, passwordbox_notify),
    {0, NULL}
};

static NCS_PROP_ENTRY dialpad_props [] = {
	{NCSP_DIALPAD_KEYPAD_NUMBER_ARRAY, dialpad_key_numbers},
	{0, 0}
};


//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = 
{
	{
		NCSCTRL_DIALPAD, 
		ID_DIALPAD,
		120, 120, 350, 260,
		WS_VISIBLE,
		WS_EX_TRANSPARENT,
		NULL,
		dialpad_props, //props,
		NULL, //rdr_info
		dialpad_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
        NCSCTRL_PASSWORDBOX, 
        ID_PASSWORDBOX,
        120, 20, 350, 50,
        WS_VISIBLE,
        WS_EX_TRANSPARENT,
        NULL,
        NULL, //props,
        NULL, //rdr_info
        passwordbox_handlers, //handlers,
        NULL, //controls
        0,
        0 //add data
    }

};
BITMAP bgbmp;
static void mymain_onPaint(mWidget *self, HDC hdc, const CLIPRGN* inv)
{
    RECT rcBg;
    
    GetClientRect(self->hwnd, &rcBg);
    //SetBrushColor(hdc, 0xFFFFFFFF);
    //FillBox(hdc, rcBg.left, rcBg.top, RECTW(rcBg), RECTH(rcBg));
    
    FillBoxWithBitmap(hdc, rcBg.left, rcBg.top, RECTW(rcBg), RECTH(rcBg), &bgbmp);
}

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
    mPasswordBox * pb = (mPasswordBox*)_c(self)->getChild (self, ID_PASSWORDBOX);
    mDialPad   * dp = (mDialPad*) _c(self)->getChild (self, ID_DIALPAD);

    _c(dp)->setProperty(dp, NCSP_DIALPAD_TARGET_HWND, pb->hwnd);
	return TRUE;
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
    {MSG_CREATE, mymain_onCreate},

    //{MSG_ERASEBKGND, mymain_onEraseBackground},
    {0, NULL}
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_MAINWND, 
	1,
	0, 0, 480, 480,
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
        int ret = LoadBitmap(HDC_SCREEN, &bgbmp, "res/bg.jpg");
	mDialogBox* mydlg = (mDialogBox*)ncsCreateMainWindowIndirect 
									(&mymain_templ, HWND_DESKTOP);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncs4TouchUninitialize();
	ncsUninitialize();
	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

