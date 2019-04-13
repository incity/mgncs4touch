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
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgeff/mgeff.h>

#include "mgncs4touch.h"

static void mPieceGridItem_construct(mPieceGridItem *self, DWORD add_data)
{
	Class(mPieceItem).construct((mPieceItem*)self, add_data);
    self->row = 0;
    self->colum = 0;
    self->rowSpan = 1;
    self->columSpan = 1;
}

static void mPieceGridItem_destroy(mPieceGridItem *self)
{
    Class(mPieceItem).destroy((mPieceItem*)self);
}

void mPieceGridItem_setGravity(mPieceGridItem *self, enum LayoutGravity gravity)
{
    self->gravity = gravity;
}

int mPieceGridItem_getGravity(mPieceGridItem *self)
{
    return self->gravity;
}

void mPieceGridItem_setRow(mPieceGridItem *self, int row)
{
    self->row = row;
}

int mPieceGridItem_getRow(mPieceGridItem *self)
{
    return self->row;
}

void mPieceGridItem_setColum(mPieceGridItem *self, int colum)
{
    self->colum = colum;
}

int mPieceGridItem_getColum(mPieceGridItem *self)
{
    return self->colum;
}

void mPieceGridItem_setRowSpan(mPieceGridItem *self, int span)
{
    self->rowSpan = span;
}

int mPieceGridItem_getRowSpan(mPieceGridItem *self)
{
    return self->rowSpan;
}

void mPieceGridItem_setColumSpan(mPieceGridItem *self, int span)
{
    self->columSpan = span;
}

int mPieceGridItem_getColumSpan(mPieceGridItem *self)
{
    return self->columSpan;
}


BEGIN_MINI_CLASS(mPieceGridItem, mPieceItem)
	CLASS_METHOD_MAP(mPieceGridItem, construct)
	CLASS_METHOD_MAP(mPieceGridItem, destroy)
	CLASS_METHOD_MAP(mPieceGridItem, setGravity)
	CLASS_METHOD_MAP(mPieceGridItem, getGravity)
	CLASS_METHOD_MAP(mPieceGridItem, setRow)
	CLASS_METHOD_MAP(mPieceGridItem, getRow)
	CLASS_METHOD_MAP(mPieceGridItem, setColum)
	CLASS_METHOD_MAP(mPieceGridItem, getColum)
	CLASS_METHOD_MAP(mPieceGridItem, setRowSpan)
	CLASS_METHOD_MAP(mPieceGridItem, getRowSpan)
	CLASS_METHOD_MAP(mPieceGridItem, setColumSpan)
	CLASS_METHOD_MAP(mPieceGridItem, getColumSpan)
END_MINI_CLASS

