
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>

#include "mtextview.h"

#define print_rect(prefix, rect) printf("%s : (%d, %d, %d, %d)\n", prefix, rect.left, rect.top, rect.right, rect.bottom)

static mObject* mTextView_createBody(mTextView *self)
{
    mPanelPiece *body;
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    
    if(dwStyle & NCSS_TEXTVIEW_VERTICAL)
        body = (mPanelPiece*)NEWPIECE(mScrollViewPiece);
    else
        body = (mPanelPiece*)NEWPIECE(mHScrollViewPiece);
    
    return (mObject*)body;
}

static SIZE* mTextView_getMaxSize(mTextView *self, SIZE *size)
{
    RECT rc;
    GetClientRect(self->hwnd, &rc);

    DWORD dwStyle = GetWindowStyle(self->hwnd);
    if(dwStyle & NCSS_TEXTVIEW_VERTICAL) {
        size->cx = RECTW(rc);
        size->cy = INT_MAX;
    }
    else {
        size->cx = INT_MAX;
        size->cy = RECTH(rc);
    }

    return size;
}


static BOOL mTextView_onCreate(mTextView* self, DWORD addData)
{
    mPanelPiece *body = (mPanelPiece*)self->body;
    // set owner for body
    _c(body)->setOwner(body, (mWidget*)self);
      
    RECT rc;
    GetClientRect(self->hwnd, &rc);
    _c(body)->setRect (body, &rc);
    
    mPanelPiece *content = (mPanelPiece*)NEWPIECE(mPanelPiece);
    _c(body)->addContent(body, (mHotPiece*)content, 0, 0);

    mTextPiece *textpiece = (mTextPiece*)NEWPIECE(mTextPiece);
    _c(textpiece)->setProperty(textpiece, NCSP_LABELPIECE_LABEL,
                                (DWORD)GetWindowCaption(self->hwnd));
    _c(textpiece)->setProperty(textpiece, NCSP_LABELPIECE_AUTOWRAP, 1);
    _c(textpiece)->setProperty(textpiece, NCSP_TEXTPIECE_TEXTCOLOR,
                                (DWORD)MakeRGBA(0x00, 0x00, 0x00, 0xff));

    _SUPER(mWidget, self, onCreate, addData);

    SIZE max;
    _c(self)->getMaxSize(self, &max);
    _c(textpiece)->autoSize(textpiece, (mObject *)self, NULL, &max);
    self->textpiece = textpiece;
    _c(content)->addContent(content, (mHotPiece*)textpiece, 0, 0);
    _c(content)->autoSize(content, (mObject *)self, NULL, NULL);

    RECT bound;
    _c(content)->getRect(content, &bound);

    mShapeTransRoundPiece* backPiece = (mShapeTransRoundPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backPiece)->setRect (backPiece, &bound);
    _c(backPiece)->setProperty(backPiece, NCSP_TRANROUND_BKCOLOR, MakeRGBA(0xff, 0xff, 0xff, 0xff));
    _c(content)->setBkgndPiece(content, backPiece);

    return TRUE;
}

static BOOL mTextView_setProperty(mTextView *self, int id, DWORD value)
{
    if(id >= NCSP_TEXTVIEW_MAX)
        return FALSE;
    
    mPanelPiece *body = (mPanelPiece*)self->body;
    if (body) {
        mShapeTransRoundPiece *backpiece = _c(body)->getBkgndPiece(body);
        assert(backpiece);
        switch (id) {
            case NCSP_TEXTVIEW_RADIUS:
                return _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_RADIUS, value);
            case NCSP_TEXTVIEW_BKCOLOR:
                return _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_BKCOLOR, value);
            case NCSP_TEXTVIEW_CORNERFLAG:
                return _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_CORNERFLAG, value);
            default:
                break;
        }
    }
    return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mTextView_getProperty(mTextView *self, int id)
{
    if(id >= NCSP_TEXTVIEW_MAX)
        return 0;
    
    mPanelPiece *body = (mPanelPiece*)self->body;
    if (body) {
        mShapeTransRoundPiece *backpiece = _c(body)->getBkgndPiece(body);
        assert(backpiece);
        switch (id) {
            case NCSP_TEXTVIEW_RADIUS:
                return _c(backpiece)->getProperty(backpiece, NCSP_TRANROUND_RADIUS);
            case NCSP_TEXTVIEW_BKCOLOR:
                return _c(backpiece)->getProperty(backpiece, NCSP_TRANROUND_BKCOLOR);
            case NCSP_TEXTVIEW_CORNERFLAG:
                return _c(backpiece)->getProperty(backpiece, NCSP_TRANROUND_CORNERFLAG);
            default:
                break;
        }
    }

    return Class(mWidget).getProperty((mWidget*)self, id);
}


static LRESULT mTextView_wndProc(mTextView* self, UINT message, WPARAM wParam, LPARAM lParam)
{	
    switch(message){
        
        case MSG_SETTEXT:
            SetWindowCaption (self->hwnd, (const char*)lParam);
            _c(self->textpiece)->setProperty(self->textpiece,
                NCSP_LABELPIECE_LABEL, (DWORD)GetWindowCaption(self->hwnd));
        case MSG_FONTCHANGED:
        {
            RECT oldrc, newrc;
            _c(self->textpiece)->getRect(self->textpiece, &oldrc);

            SIZE max;
            _c(self)->getMaxSize(self, &max);
            _c(self->textpiece)->autoSize(self->textpiece, (mObject *)self, NULL, &max);

            mPanelPiece *parent = (mPanelPiece *)self->textpiece->parent;
            assert(parent);

            _c(parent)->autoSize(parent, (mObject *)self, NULL, NULL);
            _c(parent)->getRect(parent, &newrc);

            RECT bounds;
            GetBoundRect(&bounds, &oldrc, &newrc);
            print_rect("oldrc:", oldrc);
            print_rect("newrc:", newrc);
            print_rect("bounds:", bounds);
            mShapeTransRoundPiece* backPiece = _c(parent)->getBkgndPiece(parent);
            _c(backPiece)->setRect (backPiece, &bounds);

            PanelPiece_invalidatePiece((mHotPiece *)self->textpiece, &bounds);
            return 0;
        }
        default:
            break;
    }
	
	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}


BEGIN_CMPT_CLASS(mTextView, mWidget)
    CLASS_METHOD_MAP(mTextView, onCreate)
	CLASS_METHOD_MAP(mTextView, wndProc)
    CLASS_METHOD_MAP(mTextView, createBody)
    CLASS_METHOD_MAP(mTextView, getMaxSize)
    CLASS_METHOD_MAP(mTextView, getProperty)
    CLASS_METHOD_MAP(mTextView, setProperty)
END_CMPT_CLASS

