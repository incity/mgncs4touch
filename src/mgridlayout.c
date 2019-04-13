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

static void mGridLayout_construct(mGridLayout *self, DWORD add_data)
{
    Class(mLayoutManager).construct((mLayoutManager*)self, add_data);
    _c(self)->setHgap(self, 0);
    _c(self)->setVgap(self, 0);
    _c(self)->setColums(self, 1);
    _c(self)->setRows(self, 1);
}

void mGridLayout_setHgap(mGridLayout *self, int hgap)
{
    self->hgap = hgap;
}

void mGridLayout_setVgap(mGridLayout *self, int vgap)
{
    self->vgap = vgap;
}

void mGridLayout_setRows(mGridLayout *self, int rows)
{
    self->rows = rows;
}

void mGridLayout_setColums(mGridLayout *self, int cols)
{
    self->colums = cols;
}

int mGridLayout_getHgap(mGridLayout *self)
{
    return self->hgap;
}

int mGridLayout_getVgap(mGridLayout *self)
{
    return self->vgap;
}

int mGridLayout_getRows(mGridLayout *self)
{
    return self->rows;
}

int mGridLayout_getColums(mGridLayout *self)
{
    return self->colums;
}

static void mGridLayout_reLayout(mGridLayout *self, mItemIterator* iter, int w, int h)
{
    mPieceGridItem *item = NULL;
    RECT rc;
    int w1, h1;
    
    SetRect(&rc, 0, 0, 0, 0);
    w1 = (w - (self->colums - 1) * self->hgap) / self->colums;
    h1 = (h - (self->rows - 1) * self->vgap) / self->rows;
    //printf("%s %d: w1:%d h1:%d\n", __func__, __LINE__, w1, h1);
    while ((item = (mPieceGridItem *)_c(iter)->next(iter))) {
        if (item->underLayout) {
            rc.right = w1 * item->columSpan + self->hgap * (item->columSpan - 1);
            rc.bottom = h1 * item->rowSpan + self->vgap * (item->rowSpan - 1);
            //printf("cell(%d %d) span(%d, %d) (0, 0, %d, %d)\n", 
                //item->row, item->colum, item->rowSpan, item->columSpan, 
                //rc.right, rc. bottom);
            _c(item->piece)->setRect(item->piece, &rc);
            item->x = item->colum * (RECTW(rc) + self->hgap);
            item->y = item->row * (RECTH(rc) + self->vgap);
        }
    }

    return;
}

static mPieceItem*  mLayoutManager_newPieceItem(mGridLayout *self)
{
    mPieceItem* item = (mPieceItem*)NEW(mPieceGridItem);
    return item;
}

BEGIN_MINI_CLASS(mGridLayout, mLayoutManager)
    CLASS_METHOD_MAP(mGridLayout, construct)
    CLASS_METHOD_MAP(mGridLayout, reLayout)
    CLASS_METHOD_MAP(mGridLayout, setHgap)
    CLASS_METHOD_MAP(mGridLayout, setVgap)
    CLASS_METHOD_MAP(mGridLayout, setRows)
    CLASS_METHOD_MAP(mGridLayout, setColums)
    CLASS_METHOD_MAP(mGridLayout, getHgap)
    CLASS_METHOD_MAP(mGridLayout, getVgap)
    CLASS_METHOD_MAP(mGridLayout, getRows)
    CLASS_METHOD_MAP(mGridLayout, getColums)
    CLASS_METHOD_MAP(mLayoutManager, newPieceItem)
END_MINI_CLASS
