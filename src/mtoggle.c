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

#include "mtouchcomm.h"
#include "mtouchrdr.h"
#include "mtoggle.h"

#ifndef WIN32
#   include <unistd.h>
#endif

#define NDEBUG    1
#include "mtouchdebug.h"

#define TOGGLE_BORDER             2

static void mToggle_construct(mToggle* self, DWORD param)
{
    Class(mWidget).construct((mWidget*)self, param);
#if BACKUP_BACKGROUND_DC
    self->backup_of_bg_dc = 0;
#endif
    self->mouseX = TOGGLE_BORDER;
    self->status = NCS_TOGGLE_OFF;
    self->checked_bgcolor = 0xFF83DD46;
    self->checked_bordercolor = 0xFFFFFFFF;
    self->unchecked_bgcolor = 0xFFD4D4D4;
    self->unchecked_bordercolor = 0xFFD4D4D4;
    self->bgcolor = self->unchecked_bgcolor;
    self->bordercolor = self->unchecked_bordercolor;
    IncludeWindowStyle(self->hwnd, NCSS_NOTIFY);
}

static void mToggle_destroy(mToggle* self)
{
#if BACKUP_BACKGROUND_DC
    if(self->backup_of_bg_dc)
        DeleteCompatibleDC(self->backup_of_bg_dc);
#endif
    Class(mWidget).destroy((mWidget*)self);
}

static BOOL mToggle_onCreate(mToggle* self, DWORD addData)
{
    Class(mWidget).onCreate((mWidget*)self, addData);
    _M(self, setProperty, NCSP_WIDGET_RDR, (DWORD)NCS4TOUCH_RENDERER);

    return TRUE;
}

static MPStatus MGPlusFillCircleWithShadowI (HGRAPHICS graphics, HBRUSH brush, int x, int y, int r)
{
    HBRUSH brushShadow = MGPlusBrushCreate (MP_BRUSH_TYPE_PATHGRADIENT);
    if(!brushShadow)
    {
        MGPlusBrushDelete (brushShadow);
        return MP_GENERIC_ERROR;
    }
    
    HPATH pathShadow = MGPlusPathCreate (MP_PATH_FILL_MODE_ALTERNATE);
    if(!pathShadow)
    {
        MGPlusPathDelete (pathShadow);
        return MP_GENERIC_ERROR;
    }
    
    MGPlusPathAddEllipseI (pathShadow, x+2, y+2, r, r, TRUE);
    
    ARGB SurroundColors [] = {0, 0, 0};
    MPPOINT point = {x, y};
    MGPlusSetPathGradientBrushCenterPoint (brushShadow, &point);
    MGPlusSetPathGradientBrushCenterColor (brushShadow, 0xFF000000);
    MGPlusSetPathGradientBrushSurroundColors (brushShadow, SurroundColors, 2);
    
    RECT rect = {0, 0, r*2, r*2};
    OffsetRect(&rect, x - r + 2, y - r + 2);
    MGPlusSetPathGradientBrushSurroundRect (brushShadow, &rect);
    MGPlusFillPath (graphics, brushShadow, pathShadow);
    
    MGPlusFillEllipseI(graphics, brush, x, y, r, r);
    
    MGPlusBrushDelete(brushShadow);
    MGPlusPathDelete(pathShadow);
    return MP_OK;
}

static void mToggle_onPaint(mToggle* self, HDC hdc, const PCLIPRGN pClip)
{
    RECT rc;
    GetClientRect(self->hwnd, &rc);
    int w = RECTW(rc);
    int h = RECTH(rc);

#if BACKUP_BACKGROUND_DC
    if(!self->backup_of_bg_dc) {
        HWND mainHwnd = GetMainWindowHandle(self->hwnd);
        self->backup_of_bg_dc = CreateCompatibleDC(hdc);
        assert(self->backup_of_bg_dc);
        if(!self->backup_of_bg_dc)
            return;
        
        RECT wrc;
        GetWindowRect(self->hwnd, &wrc);
        int x = wrc.left, y = wrc.top;
        BitBlt (GetClientDC(GetMainWindowHandle(self->hwnd)), x, y, w, h, self->backup_of_bg_dc, 0, 0, 0);
        // for test
        //BitBlt(self->backup_of_bg_dc, 0, 0, w, h, HDC_SCREEN, w, 0, 0);
    }
    
    // ?a¨¤?¡À?D?¨º?¡À¡ê¡ä???¨¤¡ä¦Ì?¡À3?¡ãDC, o¨®??MGPlusGraphicSave¨º??¨²?a??DC¡Á?????
    HGRAPHICS graphic = MGPlusGraphicCreateFromDC(self->backup_of_bg_dc);
#else
    HGRAPHICS graphic = MGPlusGraphicCreateFromDC(hdc);
#endif

    int r = h/2 - TOGGLE_BORDER;

    if(!graphic)
        return;
    
    HBRUSH brushSolid = MGPlusBrushCreate (MP_BRUSH_TYPE_SOLIDCOLOR);
    if(!brushSolid)
        goto fail;
    
    MGPlusSetPathRenderingHint(graphic, MP_PATH_RENDER_HINT_ANTIALIAS_ON);
    MGPlusSetSolidBrushColor (brushSolid, self->bgcolor);
    MGPlusFillRoundRectI(graphic, brushSolid, 0, 0, w, h, h/2);
    if(self->mouseX < w/2) {
        int dh = self->mouseX * 2 * h / w;
        InflateRect(&rc, -self->mouseX,  dh >= TOGGLE_BORDER ? -dh : -TOGGLE_BORDER);
        
        MGPlusSetSolidBrushColor (brushSolid, self->bordercolor);
        MGPlusFillRoundRectI(graphic, brushSolid, rc.left, rc.top, RECTW(rc), RECTH(rc), RECTH(rc)/2);
    }

    MGPlusSetSolidBrushColor (brushSolid, 0xFFFFFFFF);
    MGPlusFillCircleWithShadowI(graphic, brushSolid, self->mouseX + r, h/2, r);
    MGPlusGraphicSave(graphic, hdc, 0, 0, 0, 0, 0, 0);
    MGPlusBrushDelete(brushSolid);
fail:
    MGPlusGraphicDelete(graphic);
    return;
}

#if BACKUP_BACKGROUND_DC
static int mToggle_onSizeChanged(mToggle* self, RECT* rtClient)
{
    if(self->backup_of_bg_dc) {
        DeleteCompatibleDC(self->backup_of_bg_dc);
        self->backup_of_bg_dc = 0;
    }
    return 0;
}
#endif

static int mToggle_onLButtonDown(mToggle* self, int x, int y, DWORD keyFlags)
{
    SetCapture(self->hwnd);
    return 0;
}


static int mToggle_onLButtonUp(mToggle* self, int x, int y, DWORD keyFlags)
{
    RECT rc, rcBlock;

    if (GetCapture() == self->hwnd) {
        int x1 = x, y1 = y;
        ScreenToClient(self->hwnd, &x1, &y1);
        GetClientRect(self->hwnd, &rc);
        if(PtInRect(&rc, x1, y1)) {
            if (self->status == NCS_TOGGLE_ON) {
                self->bgcolor = self->unchecked_bgcolor;
                self->bordercolor = self->unchecked_bordercolor;
                _M(self, runAnimation, RECTW(rc)-RECTH(rc)+TOGGLE_BORDER, TOGGLE_BORDER, 300);
                self->status = NCS_TOGGLE_OFF;
                ncsNotifyParentEx((mWidget*)self, NCSN_TOGGLE_STATUSCHANGED, self->status);
            } else if (self->status == NCS_TOGGLE_OFF) {
                self->bgcolor = self->checked_bgcolor;
                self->bordercolor = self->checked_bordercolor;
                _M(self, runAnimation, TOGGLE_BORDER, RECTW(rc)-RECTH(rc)+TOGGLE_BORDER, 300);
                self->status = NCS_TOGGLE_ON;
                ncsNotifyParentEx((mWidget*)self, NCSN_TOGGLE_STATUSCHANGED, self->status);
            }
        }
        ReleaseCapture();
        //InvalidateRect(self->hwnd, NULL, FALSE);
    }

    return 0;
}


static BOOL mToggle_setProperty(mToggle* self, int id, DWORD value)
{
    if (id > NCSP_TOGGLE_MAX)
        return FALSE;

    switch (id) {
        case NCSP_WIDGET_RDR:
            if (strcmp((char*)value, NCS4TOUCH_RENDERER) == 0)
                break;
            else
                return FALSE;
        case NCSP_TOGGLE_CHECKED_BGCOLOR:
            self->checked_bgcolor = value;
            InvalidateRect(self->hwnd, NULL, FALSE);
            return TRUE;
        case NCSP_TOGGLE_CHECKED_BORDERCOLOR:
            self->checked_bordercolor = value;
            InvalidateRect(self->hwnd, NULL, FALSE);
            return TRUE;
        case NCSP_TOGGLE_UNCHECKED_BGCOLOR:
            self->unchecked_bgcolor = value;
            InvalidateRect(self->hwnd, NULL, FALSE);
            return TRUE;
        case NCSP_TOGGLE_UNCHECKED_BORDERCOLOR:
            self->unchecked_bordercolor = value;
            InvalidateRect(self->hwnd, NULL, FALSE);
            return TRUE;
        case NCSP_TOGGLE_STATUS:
            if (self->status != value) {
                self->status = value;
                
                if(self->status == NCS_TOGGLE_ON) {
                    self->bgcolor = self->checked_bgcolor;
                    self->bordercolor = self->checked_bordercolor;
                    
                    RECT rc;
                    GetClientRect(self->hwnd, &rc);
                    self->mouseX = RECTW(rc) - RECTH(rc) + TOGGLE_BORDER;
                } else {
                    self->bgcolor = self->unchecked_bgcolor;
                    self->bordercolor = self->unchecked_bordercolor;
                    self->mouseX = TOGGLE_BORDER;
                }
                InvalidateRect(self->hwnd, NULL, TRUE);
            }
            return TRUE;
        default:
            break;
    }

    return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mToggle_getProperty(mToggle* self, int id)
{
    if (id >= NCSP_TOGGLE_MAX)
        return (-1);

    if (id == NCSP_TOGGLE_STATUS) {
        return (DWORD)self->status;
    }

    return Class(mWidget).getProperty((mWidget*)self, id);
}

static void mToggle_mGEffPropCallback(MGEFF_ANIMATION handle, mToggle* self, int id, int *value)
{
    /* set animation property */
    self->mouseX = *value;
    
    /* update */
    InvalidateRect (self->hwnd, NULL, TRUE);
}

static void mToggle_runAnimation(mToggle* self, int start, int end, int duration)
{
    if (duration < 0)
        duration = 1;

    MGEFF_ANIMATION animation;
    int animation_id = 1;
    
    /* init mGEff library */
    //mGEffInit ();
    
    /* create animation object */
    animation = mGEffAnimationCreate ((void *) self, (void *) mToggle_mGEffPropCallback, animation_id, MGEFF_INT);

    /* set animation property */
    /* duration */
    mGEffAnimationSetDuration (animation, duration);

    /* start value */
    mGEffAnimationSetStartValue (animation, &start);

    /* end value */
    mGEffAnimationSetEndValue (animation, &end);

    /* set curve */
    mGEffAnimationSetCurve (animation, Linear);

    /* running */
    mGEffAnimationAsyncRun (animation);

    /* wait animation stop */
    mGEffAnimationWait ((void *) &self->hwnd, animation);

    /* delete the animation object */
    mGEffAnimationDelete (animation);
    
   /* deinit mGEff library */
    //mGEffDeinit ();
}

#ifdef _MGNCS4TOUCH_GUIBUILDER_SUPPORT
static BOOL mToggle_refresh(mToggle* self)
{
    UpdateWindow(self->hwnd, TRUE);

    return TRUE;
}
#endif


BEGIN_CMPT_CLASS(mToggle, mWidget)
    CLASS_METHOD_MAP(mToggle, construct)
    CLASS_METHOD_MAP(mToggle, destroy)
    CLASS_METHOD_MAP(mToggle, onCreate)
#if BACKUP_BACKGROUND_DC
    CLASS_METHOD_MAP(mToggle, onSizeChanged)
#endif
    CLASS_METHOD_MAP(mToggle, onPaint)
    CLASS_METHOD_MAP(mToggle, onLButtonUp)
    CLASS_METHOD_MAP(mToggle, onLButtonDown)
    CLASS_METHOD_MAP(mToggle, setProperty)
    CLASS_METHOD_MAP(mToggle, getProperty)
    CLASS_METHOD_MAP(mToggle, runAnimation)
#ifdef _MGNCS4TOUCH_GUIBUILDER_SUPPORT
    CLASS_METHOD_MAP(mToggle, refresh)
#endif
END_CMPT_CLASS

