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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>

#include "mgncs4touch.h"

static void mFlowLayout_construct(mFlowLayout *self, DWORD add_data)
{
    Class(mLayoutManager).construct((mLayoutManager*)self, add_data);
    _c(self)->setHgap(self, 0);
    _c(self)->setVgap(self, 0);
    _c(self)->setHAlignment(self, FLOW_LAYOUT_HALIGN_CENTER);
    _c(self)->setVAlignment(self, FLOW_LAYOUT_VALIGN_CENTER);
}

void mFlowLayout_setHgap(mFlowLayout *self, int hgap)
{
    self->hgap = hgap;
}

void mFlowLayout_setVgap(mFlowLayout *self, int vgap)
{
    self->vgap = vgap;
}

void mFlowLayout_setHAlignment(mFlowLayout *self, enum FlowLayoutHAlign halign)
{
    self->halign = halign;
}

void mFlowLayout_setVAlignment(mFlowLayout *self, enum FlowLayoutVAlign valign)
{
    self->valign = valign;
}

int mFlowLayout_getHgap(mFlowLayout *self)
{
    return self->hgap;
}

int mFlowLayout_getVgap(mFlowLayout *self)
{
    return self->vgap;
}

enum FlowLayoutHAlign mFlowLayout_getHAlignment(mFlowLayout *self)
{
    return self->halign;
}

enum FlowLayoutVAlign mFlowLayout_getVAlignment(mFlowLayout *self)
{
    return self->valign;
}

static void mFlowLayout_reLayoutOneRow(
    mFlowLayout *self, mItemIterator* iter, 
     int offset, int width, int row_y, int row_h)
{
    mPieceItem *item = NULL;
    RECT rc;

    while ((width > 0) && (item = _c(iter)->next(iter))) {
        if (item->underLayout) {
            _c(item->piece)->getRect(item->piece, &rc);
            item->x = offset;
            item->y = (row_h - RECTH(rc)) / 2 + row_y;

            int w = RECTW(rc) + self->hgap;
            offset += w;
            width -= w;
        }
    }
}

static int mFlowLayout_measureHeight(mFlowLayout *self, mItemIterator* iter, int w)
{
    mItemIterator *iter_duplicate = _c(iter)->duplicate(iter);
    mPieceItem *item = NULL;
    int w_total = 0;
    int h_total = 0;
    int max_height = 0;
    RECT rc;

    while ((item = _c(iter)->next(iter))) {
        if (item->underLayout) {
           _c(item->piece)->getRect(item->piece, &rc);

            w_total += RECTW(rc);
            if(w_total < w) {
                if(RECTH(rc) > max_height)
                    max_height = RECTH(rc);
                w_total += self->hgap;
            } else {
                h_total += self->vgap;
                h_total += max_height;
                // next row
                w_total = 0;
                max_height = 0;
                _c(iter)->prev(iter);
            }
        }
    }

    h_total += max_height;
    DELETE(iter);
    return h_total;
}


static void mFlowLayout_reLayout(mFlowLayout *self, mItemIterator* iter, int w, int h)
{
    mItemIterator *iter_duplicate = _c(iter)->duplicate(iter);
    mPieceItem *item = NULL;
    int offset = 0;
    int w_total = 0;
    int max_height = 0;
    int row_y = 0;
    RECT rc;

    if(self->valign == FLOW_LAYOUT_VALIGN_TOP) {
        row_y = 0;
    } else if(self->valign == FLOW_LAYOUT_VALIGN_CENTER) {
        int h_total = mFlowLayout_measureHeight(self, _c(iter)->duplicate(iter), w);
        if(h_total < h)
            row_y = (h - h_total)/2;
    } else if(self->valign == FLOW_LAYOUT_VALIGN_BOTTOM) {
        int h_total = mFlowLayout_measureHeight(self, _c(iter)->duplicate(iter), w);
        if(h_total < h)
            row_y = h - h_total;
    }
    
    while ((item = _c(iter)->next(iter))) {
        if (item->underLayout) {
            _c(item->piece)->getRect(item->piece, &rc);

            w_total += RECTW(rc);
            if(w_total < w) {
                if(RECTH(rc) > max_height)
                    max_height = RECTH(rc);
                
                w_total += self->hgap;
            } else {
                w_total -= (RECTW(rc) + self->hgap);
                offset = (w - w_total) / 2;
                mFlowLayout_reLayoutOneRow(self, iter_duplicate, 
                    offset, w_total, row_y, max_height);

                row_y += (max_height + self->vgap);

                w_total = 0;
                max_height = 0;
                _c(iter)->prev(iter);
            }
        }
    }

    w_total -= self->hgap;
    offset = (w - w_total) / 2;
    mFlowLayout_reLayoutOneRow(self, iter_duplicate, 
        offset, w_total, row_y, max_height);

    DELETE(iter_duplicate);
    return;
}

BEGIN_MINI_CLASS(mFlowLayout, mLayoutManager)
    CLASS_METHOD_MAP(mFlowLayout, construct)
    CLASS_METHOD_MAP(mFlowLayout, reLayout)
    CLASS_METHOD_MAP(mFlowLayout, setHgap)
    CLASS_METHOD_MAP(mFlowLayout, setVgap)
    CLASS_METHOD_MAP(mFlowLayout, setHAlignment)
    CLASS_METHOD_MAP(mFlowLayout, setVAlignment)
    CLASS_METHOD_MAP(mFlowLayout, getHgap)
    CLASS_METHOD_MAP(mFlowLayout, getVgap)
    CLASS_METHOD_MAP(mFlowLayout, getHAlignment)
    CLASS_METHOD_MAP(mFlowLayout, getVAlignment)
END_MINI_CLASS
