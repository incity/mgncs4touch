/*
 *   This file is part of mGNCS4Touch, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>

#include "mgncs4touch.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

const static char _keypad_scancode_array[] = {
    SCANCODE_1, SCANCODE_2, SCANCODE_3, SCANCODE_BACKSPACE,
    SCANCODE_4, SCANCODE_5, SCANCODE_6, SCANCODE_0,
    SCANCODE_7, SCANCODE_8, SCANCODE_9, SCANCODE_ENTER
};

const static char *_keypad_number_array[] = {
    "1", "2", "3", "*",
    "4", "5", "6", "0",
    "7", "8", "9", "#"
};

const static char *_keypad_letters_array[] = {
    "", "ABC", "DEF", "",
    "GHI", "JKL", "MNO", "+",
    "PQRS", "TUV", "WXYZ", ""
};

#define print_rect(prefix, rect) printf("%s : (%d, %d, %d, %d)\n", prefix, rect.left, rect.top, rect.right, rect.bottom)

static void mDialPad_construct(mDialPad* self, DWORD param)
{
    Class(mWidget).construct((mWidget*)self, param);
    self->keypad_scancode_array = (char*)_keypad_scancode_array;
    self->keypad_number_array = (char**)_keypad_number_array;
    self->keypad_letters_array = (char**)_keypad_letters_array;
    self->keypad_count = ARRAY_SIZE(_keypad_number_array);

    self->keypad_radius = 40;
    self->keypad_bgcolor = 0x4C000000;
    self->keypad_hi_bgcolor = 0xBEFFFFFF;
    self->keypad_number_color = 0xFFFFFFFF;
    self->keypad_letters_color = 0xFFFFFFFF;
    self->keypad_number_font =  NULL;
    self->keypad_letters_font = NULL;
    self->last_scancode = 0;
    self->hwnd_target = GetParent(self->hwnd);
    IncludeWindowStyle(self->hwnd, NCSS_NOTIFY);
}

static void mDialPad_destroy(mDialPad* self)
{
    Class(mWidget).destroy((mWidget*)self);
}

static BOOL mDialPad_onPiece(mDialPad *self, mHotPiece *piece, int event_id, DWORD param)
{
    mPieceGridItem *item;
    if(event_id == NCSN_ABP_CLICKED) {
        mPanelPiece *body = (mPanelPiece *)self->body;
        item = (mPieceGridItem *)_c(body)->searchItem(body, piece);

        int row = _c(item)->getRow(item);
        int colum = _c(item)->getColum(item);

        mGridLayout *body_layout = (mGridLayout *)body->layout;
        int cols = _c(body_layout)->getColums(body_layout);

        self->last_scancode = row * cols + colum;
        if(self->keypad_scancode_array) {
            self->last_scancode = self->keypad_scancode_array[self->last_scancode];
            ncsNotifyParentEx((mWidget*)self, NCSN_DIALPAD_KEYPAD_CLICKED,
                (DWORD)self->last_scancode);
        }
        else {
            ncsNotifyParentEx((mWidget*)self, NCSN_DIALPAD_KEYPAD_CLICKED,
                (DWORD)self->last_scancode);
        }
        SendNotifyMessage(self->hwnd_target, MSG_KEYDOWN,
                    (WPARAM)self->last_scancode, 0);
    }
    return FALSE;
}

static BOOL mDialPad_createKeypad(mDialPad* self)
{
    mPanelPiece *body = (mPanelPiece *)self->body;
    _c(body)->clearContents(body);

    mGridLayout *body_layout = (mGridLayout *)body->layout;
    mGridLayout *layout = (mGridLayout *)NEW(mGridLayout);
    assert(layout);
    _c(layout)->setRows(layout, 3);
    _c(layout)->setColums(layout, 1);
            
    int i, row = 0, colum = 0;
    for(i = 0; i < self->keypad_count; i++) {
        mButtonPanelPiece* keypadpiece =(mButtonPanelPiece*)NEWPIECE(mButtonPanelPiece);
        assert(piece);
        
        mShapeTransRoundPiece *backpiece = _c(keypadpiece)->getBkgndPiece(keypadpiece);
        _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_BORDERSIZE, 0);
        _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_RADIUS, self->keypad_radius);
        _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_FILLENGINE, TRANROUND_FILLENGINE_PLUS);        
        do {
            if(self->keypad_number_array[i][0] == '[') {
                char imagepath[256];
                sscanf(self->keypad_number_array[i], "[%[^]]]", imagepath);
                BITMAP *bmp = (BITMAP *)malloc(sizeof(BITMAP));
                assert(bmp);
                if(bmp && LoadBitmap(HDC_SCREEN, bmp, imagepath) == ERR_BMP_OK) {
                    mGridLayout *layout = (mGridLayout*)NEW(mGridLayout);
                    _c(layout)->setRows(layout, 3);
                    _c(layout)->setColums(layout, 1);
                    _c(layout)->setRows(layout, 1);
                    _c(layout)->setColums(layout, 1);
                    _c(keypadpiece)->setLayoutManager (keypadpiece, (mLayoutManager*)layout);
                    
                    mHotPiece *imagepiece = (mHotPiece*)NEWPIECE(mImagePiece);
                    assert(imagepiece);
                    _c(imagepiece)->setProperty (imagepiece, NCSP_IMAGEPIECE_IMAGE, (DWORD)bmp);
                    _c(keypadpiece)->addContentToLayout(keypadpiece, imagepiece);                  
                    break;
                }
                printf("%s load failure!\n", imagepath);
            }
            _c(keypadpiece)->setLayoutManager (keypadpiece, (mLayoutManager*)layout);
            mTextPiece *keypad_number_piece = (mTextPiece*)NEWPIECE(mTextPiece);
            assert(keypad_number_piece);
            _c(keypad_number_piece)->setProperty (keypad_number_piece,
                NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)self->keypad_number_color);
            _c(keypad_number_piece)->setProperty (keypad_number_piece,
                NCSP_TEXTPIECE_LOGFONT, (DWORD)self->keypad_number_font);            
            _c(keypad_number_piece)->setProperty (keypad_number_piece,
                NCSP_LABELPIECE_LABEL, (DWORD)self->keypad_number_array[i]);
            mPieceGridItem *item = (mPieceGridItem *)_c(keypadpiece)->addContentToLayout(
                keypadpiece, (mHotPiece*)keypad_number_piece);
            _c(item)->setRow(item, 0);
            _c(item)->setColum(item, 0);
            _c(item)->setRowSpan(item, 2);
                
            mTextPiece *keypad_letters_piece = (mTextPiece*)NEWPIECE(mTextPiece);
            assert(keypad_letters_piece);
            _c(keypad_letters_piece)->setProperty (keypad_letters_piece,
                NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)self->keypad_letters_color);
            _c(keypad_letters_piece)->setProperty (keypad_letters_piece,
                NCSP_TEXTPIECE_LOGFONT, (DWORD)self->keypad_letters_font);
            _c(keypad_letters_piece)->setProperty (keypad_letters_piece,
                NCSP_LABELPIECE_LABEL, (DWORD)self->keypad_letters_array[i]);
            item = (mPieceGridItem *)_c(keypadpiece)->addContentToLayout(keypadpiece, (mHotPiece*)keypad_letters_piece);
            _c(item)->setRow(item, 2);
            _c(item)->setColum(item, 0);
        }while(0);

        mPieceGridItem* item = (mPieceGridItem*)_c(body)->addContentToLayout(body, (mHotPiece*)keypadpiece);
        _c(item)->setRow(item, row);
        _c(item)->setColum(item, colum++);
        if(colum+1 > _c(body_layout)->getColums(body_layout)) {
            row++;
            colum = 0;
        }
        
        _c(keypadpiece)->setStateTable(keypadpiece, (mHotPiece*)backpiece, 
            NCSP_TRANROUND_BKCOLOR, self->keypad_bgcolor, self->keypad_hi_bgcolor);
        _c(keypadpiece)->setStates(keypadpiece);
        ncsAddEventListener((mObject*)keypadpiece, (mObject*)self,
                (NCS_CB_ONPIECEEVENT)mDialPad_onPiece, NCSN_ABP_CLICKED);
    }

    return TRUE;
}

static mObject* mDialPad_createBody(mDialPad *self)
{
    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    
    RECT rc;
    GetClientRect(self->hwnd, &rc);
    _c(body)->setRect (body, &rc);

    mGridLayout *layout = (mGridLayout*)NEW(mGridLayout);
    _c(layout)->setRows(layout, 3);
    _c(layout)->setColums(layout, 4);
    _c(layout)->setVgap(layout, 20);
    _c(layout)->setHgap(layout, 20);
    
    _c(body)->setLayoutManager (body, (mLayoutManager*)layout);
    
    // set owner for body
    _c(body)->setOwner(body, (mWidget*)self);
    return (mObject*)body;
}

static BOOL mDialPad_onCreate(mDialPad* self, DWORD addData)
{
    _SUPER(mWidget, self, onCreate, addData);

    mDialPad_createKeypad(self);
    return TRUE;
}

static void mDialPad_onPaint(mDialPad* self, HDC hdc, const PCLIPRGN pClip)
{
    _SUPER(mWidget, self, onPaint, hdc, pClip);
    return;
}

static BOOL mDialPad_setProperty(mDialPad* self, int id, DWORD value)
{
    if (id > NCSP_DIALPAD_MAX)
        return FALSE;

    switch (id) {
    case NCSP_DIALPAD_KEYPAD_NUM:
        if (self->keypad_count != (int)value) {
            self->keypad_count = (int)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_RADIUS:
        if (self->keypad_radius != (int)value) {
            self->keypad_radius = (int)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_HSPACING:
    {
        mPanelPiece *body = (mPanelPiece *)self->body;
        assert(body);
        mGridLayout *layout = (mGridLayout *)body->layout;
        assert(layout);

        _c(layout)->setHgap(layout, (int)value);
        return TRUE;
    }
    case NCSP_DIALPAD_KEYPAD_VSPACING:
    {
        mPanelPiece *body = (mPanelPiece *)self->body;
        assert(body);
        mGridLayout *layout = (mGridLayout *)body->layout;
        assert(layout);
    
        _c(layout)->setVgap(layout, (int)value);
        return TRUE;
    }
    case NCSP_DIALPAD_KEYPAD_BGCOLOR:
        if (self->keypad_bgcolor != (ARGB)value) {
            self->keypad_bgcolor = (ARGB)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_HI_BGCOLOR:
        if (self->keypad_hi_bgcolor != (ARGB)value) {
            self->keypad_hi_bgcolor = (ARGB)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_NUMBER_FONT:
        if (self->keypad_number_font != (PLOGFONT)value) {
            self->keypad_number_font = (PLOGFONT)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_NUMBER_FONTNAME:
    {
        PLOGFONT font;
        font = (PLOGFONT)LoadResource((const char*)value, RES_TYPE_FONT, 0L);
    
        // TODO: ???
        if(self->keypad_number_font) {
            ReleaseRes(((FONT_RES *)self->keypad_number_font)->key);
        }
        
        if(font) {
            self->keypad_number_font = font;
            return TRUE;
        }
        return FALSE;
    }
    case NCSP_DIALPAD_KEYPAD_NUMBER_COLOR:
        if (self->keypad_number_color != (ARGB)value) {
            self->keypad_number_color = (ARGB)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_LETTERS_FONT:
        if (self->keypad_letters_font != (PLOGFONT)value) {
            self->keypad_letters_font = (PLOGFONT)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_LETTERS_FONTNAME:
    {
        PLOGFONT font;
        font = (PLOGFONT)LoadResource((const char*)value, RES_TYPE_FONT, 0L);
    
        // TODO: ???
        if(self->keypad_letters_font) {
            ReleaseRes(((FONT_RES *)self->keypad_letters_font)->key);
        }
        
        if(font) {
            self->keypad_letters_font = font;
            return TRUE;
        }
        return FALSE;
    }
    case NCSP_DIALPAD_KEYPAD_LETTERS_COLOR:
        if (self->keypad_letters_color != (ARGB)value) {
            self->keypad_letters_color = (ARGB)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_KEYPAD_NUMBER_ARRAY:
        if (self->keypad_number_array != (char**)value) {
            self->keypad_number_array = (char**)value;
            return TRUE;
        }
    case NCSP_DIALPAD_KEYPAD_LETTERS_ARRAY:
        if (self->keypad_letters_array != (char**)value) {
            self->keypad_letters_array = (char**)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_DIALPAD_TARGET_HWND:
        if (self->hwnd_target != (HWND)value) {
            self->hwnd_target = (HWND)value;
            return TRUE;
        }
        return FALSE;

        default:
            break;
    }
    return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mDialPad_getProperty(mDialPad* self, int id)
{
    if (id >= NCSP_DIALPAD_MAX)
        return (-1);
    switch (id) {
    case NCSP_DIALPAD_KEYPAD_NUM:
        return (DWORD)self->keypad_count;
    case NCSP_DIALPAD_KEYPAD_RADIUS:
        return (DWORD)self->keypad_radius;
    case NCSP_DIALPAD_KEYPAD_HSPACING:
    {
        mPanelPiece *body = (mPanelPiece *)self->body;
        assert(body);
        mGridLayout *layout = (mGridLayout *)body->layout;
        assert(layout);
    
        return (DWORD)_c(layout)->getHgap(layout);
    }
    case NCSP_DIALPAD_KEYPAD_VSPACING:
    {
        mPanelPiece *body = (mPanelPiece *)self->body;
        assert(body);
        mGridLayout *layout = (mGridLayout *)body->layout;
        assert(layout);
    
        return (DWORD)_c(layout)->getVgap(layout);
    }
    case NCSP_DIALPAD_KEYPAD_BGCOLOR:
        return (DWORD)self->keypad_bgcolor;
    case NCSP_DIALPAD_KEYPAD_HI_BGCOLOR:
        return (DWORD)self->keypad_hi_bgcolor;
    case NCSP_DIALPAD_KEYPAD_NUMBER_FONT:
        return (DWORD)self->keypad_number_font;
    case NCSP_DIALPAD_KEYPAD_NUMBER_COLOR:
        return (DWORD)self->keypad_number_color;
    case NCSP_DIALPAD_KEYPAD_LETTERS_FONT:
        return (DWORD)self->keypad_letters_font;
    case NCSP_DIALPAD_KEYPAD_LETTERS_COLOR:
        return (DWORD)self->keypad_letters_color;
    case NCSP_DIALPAD_KEYPAD_NUMBER_ARRAY:
        return (DWORD)self->keypad_number_array;
    case NCSP_DIALPAD_KEYPAD_LETTERS_ARRAY:
        return (DWORD)self->keypad_letters_array;
    case NCSP_DIALPAD_LAST_SCANCODE:
        return (DWORD)self->last_scancode;
    case NCSP_DIALPAD_TARGET_HWND:
        return (DWORD)self->hwnd_target;
    default:
        break;
    }
    return Class(mWidget).getProperty((mWidget*)self, id);
}

static LRESULT mDialPad_wndProc (mDialPad *self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
	{
		case MSG_SETFOCUS:
		case MSG_KILLFOCUS:
		case MSG_MOUSEACTIVE:
            return 0;
    }

    return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

#ifdef _MGNCS4TOUCH_GUIBUILDER_SUPPORT
static BOOL mDialPad_refresh(mDialPad* self)
{
    UpdateWindow(self->hwnd, TRUE);

    return TRUE;
}
#endif


BEGIN_CMPT_CLASS(mDialPad, mWidget)
    CLASS_METHOD_MAP(mDialPad, construct)
    CLASS_METHOD_MAP(mDialPad, createBody)
    CLASS_METHOD_MAP(mDialPad, destroy)
    CLASS_METHOD_MAP(mDialPad, onCreate)
	CLASS_METHOD_MAP(mDialPad, wndProc)
    CLASS_METHOD_MAP(mDialPad, onPaint)
    CLASS_METHOD_MAP(mDialPad, setProperty)
    CLASS_METHOD_MAP(mDialPad, getProperty)
#ifdef _MGNCS4TOUCH_GUIBUILDER_SUPPORT
    CLASS_METHOD_MAP(mDialPad, refresh)
#endif
END_CMPT_CLASS

