
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

#include "mgncs4touch.h"


#define print_rect(prefix, rect) printf("%s : (%d, %d, %d, %d)\n", prefix, rect.left, rect.top, rect.right, rect.bottom)

static BOOL mPasswordBox_onEraseBkgnd(mPasswordBox *self, HDC hdc, const RECT *pinv);
static void mPasswordBox_onPaint (mPasswordBox *self, HDC hdc, const PCLIPRGN pinv_clip);


static void mPasswordBox_construct(mPasswordBox *self, DWORD param)
{
    Class(mWidget).construct((mWidget*)self, param);
    
    self->max_password_length = 8;
    self->password_pieces = NULL;
    self->password_char = "";
    IncludeWindowStyle(self->hwnd, NCSS_NOTIFY);
}

static void mPasswordBox_destroy(mPasswordBox *self)
{
    if(self->password_pieces)
        free(self->password_pieces);

    if(self->password)
        free(self->password);

    if(self->password2)
        free(self->password2);

    _SUPER(mWidget, self, destroy);
}

static void mPasswordBox_repaintPassword(mPasswordBox *self, int where, int offset)
{
    int i;

    if(self->password_char && strlen(self->password_char)) {
        for(i = where; i < where+offset; i++) {
            mTextPiece *textpiece = self->password_pieces[i];
            if(self->password[i])
                _c(textpiece)->setProperty (textpiece,
                        NCSP_LABELPIECE_LABEL, (DWORD)self->password_char);
            else
                _c(textpiece)->setProperty (textpiece,
                        NCSP_LABELPIECE_LABEL, (DWORD)"");

            PanelPiece_invalidatePiece((mHotPiece *)textpiece, NULL);
        }
        return;
    }
    
    for(i = where; i < where+offset; i++) {
        mTextPiece *textpiece = self->password_pieces[i];
        self->password2[i*2] = self->password[i];
        _c(textpiece)->setProperty (textpiece,
                    NCSP_LABELPIECE_LABEL, (DWORD)&self->password2[i*2]);
        PanelPiece_invalidatePiece((mHotPiece *)textpiece, NULL);
    }
}

static int mPasswordBox_append(mPasswordBox *self, const char* str)
{
    assert(str && self->password);
    int len = strlen(self->password);
    int offset = strlen(str);
    
    if((offset + len) > self->max_password_length)
        return 0;

    strcat(self->password, str);
    mPasswordBox_repaintPassword(self, len, offset);
    return offset;
}

static int mPasswordBox_subtract(mPasswordBox *self, int num) 
{
    assert(self->password);
    assert(num > 0);
    
    int len = strlen(self->password);
    if(len <= 0)
        return 0;

    if(num > len) {
        num = len;
    }

    int where = len - num;
    bzero(self->password + where, num);
    
    mPasswordBox_repaintPassword(self, where, num);
    return num;
}

static void mPasswordBox_onPaint(mPasswordBox *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    _SUPER(mWidget, self, onPaint, hdc, pinv_clip);
}

static mObject* mPasswordBox_createBody(mPasswordBox *self)
{
    mPanelPiece *body = (mPanelPiece*)NEWPIECE(mPanelPiece);
    RECT rc;
    GetClientRect(self->hwnd, &rc);
    //print_rect("Rect:", rc);
    _c(body)->setRect (body, &rc);
    _c(body)->setBkgndPiece(body, NEWPIECE(mShapeTransRoundPiece));
    _c(body->bkgndPiece)->setRect(body->bkgndPiece, &rc);
    _c(body->bkgndPiece)->setProperty(body->bkgndPiece, NCSP_TRANROUND_BKCOLOR, 0x4C000000);
    _c(body->bkgndPiece)->setProperty(body->bkgndPiece, NCSP_TRANROUND_RADIUS, 8);

    self->cell_backpiece = (mShapeTransRoundPiece*)NEWPIECE(mShapeTransRoundPiece);
    SetRect(&rc, 0, 0, 30, 40);
    _c(self->cell_backpiece)->setRect(self->cell_backpiece, &rc);
    _c(self->cell_backpiece)->setProperty(self->cell_backpiece, NCSP_TRANROUND_RADIUS, 4);
    _c(self->cell_backpiece)->setProperty(self->cell_backpiece, NCSP_TRANROUND_BKCOLOR, 0x80FFFFFF);
    
    mFlowLayout *layout = (mFlowLayout*)NEW(mFlowLayout);
    _c(layout)->setHgap(layout, 5);
    _c(layout)->setHAlignment(layout, FLOW_LAYOUT_HALIGN_CENTER);
    _c(layout)->setVAlignment(layout, FLOW_LAYOUT_VALIGN_CENTER);

    _c(body)->setLayoutManager (body, (mLayoutManager*)layout);
      
    // set owner for body
    _c(body)->setOwner(body, (mWidget*)self);
    return (mObject*)body;
}

static BOOL mPasswordBox_createCells(mPasswordBox* self)
{
    mPanelPiece *body = (mPanelPiece *)self->body;
    _c(body)->clearContents(body);

    self->password_pieces = (mTextPiece **)calloc(
        self->max_password_length, sizeof(mTextPiece *));

    int i;
    for(i = 0; i < self->max_password_length; i++) {
        mPanelPiece* panelpiece =(mPanelPiece*)NEWPIECE(mPanelPiece);
        assert(panelpiece);

        RECT rc;
        _c(self->cell_backpiece)->getRect(self->cell_backpiece, &rc);

        _c(panelpiece)->setBkgndPiece(panelpiece, self->cell_backpiece);
        _c(panelpiece)->setRect(panelpiece, &rc);
        self->password_pieces[i] = (mTextPiece*)NEWPIECE(mTextPiece);
        assert(self->password_pieces[i]);
        _c(self->password_pieces[i])->setProperty (self->password_pieces[i],
                NCSP_TEXTPIECE_TEXTCOLOR, (DWORD)0xFF000000);
        _c(self->password_pieces[i])->setRect(self->password_pieces[i], &rc);

        _c(panelpiece)->addContentToLayout(panelpiece,
                (mHotPiece*)self->password_pieces[i]);
        _c(body)->addContentToLayout(body, (mHotPiece*)panelpiece);
    }
    return TRUE;
}

static BOOL mPasswordBox_onCreate(mPasswordBox* self, DWORD addData)
{
    _SUPER(mWidget, self, onCreate, addData);

    if(self->max_password_length > 0) {
        self->password = (char *)calloc(
            self->max_password_length + 1, sizeof(char));

        if(!self->password_char || strlen(self->password_char) == 0) {
            self->password2 = (char *)calloc(
            self->max_password_length, sizeof(char)*2);
        }
    }
    mPasswordBox_createCells(self);
    return TRUE;
}

static BOOL mPasswordBox_setProperty(mPasswordBox *self, int id, DWORD value)
{
    if(id >= NCSP_PASSWORDBOX_MAX)
        return FALSE;
    
    mPanelPiece *body = (mPanelPiece*)self->body;
    if (body) {
        mShapeTransRoundPiece *backpiece = _c(body)->getBkgndPiece(body);
        assert(backpiece);
        switch (id) {
            case NCSP_PASSWORDBOX_RADIUS:
                return _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_RADIUS, value);
            case NCSP_PASSWORDBOX_BKCOLOR:
                return _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_BKCOLOR, value);
            case NCSP_PASSWORDBOX_CORNERFLAG:
                return _c(backpiece)->setProperty(backpiece, NCSP_TRANROUND_CORNERFLAG, value);
            default:
                break;
        }
    }

    switch (id) {
    case NCSP_PASSWORDBOX_MAX_LENGTH:
        if (self->max_password_length != (int)value) {
            self->max_password_length = (int)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_PASSWORDBOX_CELL_RADIUS:
        if (self->cell_backpiece->corner_radius != (int)value) {
            self->cell_backpiece->corner_radius = (int)value;
            return TRUE;
        }
        return FALSE;
    case NCSP_PASSWORDBOX_CELL_WIDTH:
    {
        RECT rc;
        _c(self->cell_backpiece)->getRect(self->cell_backpiece, &rc);
        if (RECTW(rc) != (int)value) {
            rc.left = 0;
            rc.right = (int)value;
            _c(self->cell_backpiece)->setRect(self->cell_backpiece, &rc);
            return TRUE;
        }
        return FALSE;

    }
    case NCSP_PASSWORDBOX_CELL_HEIGHT:
    {
        RECT rc;
        _c(self->cell_backpiece)->getRect(self->cell_backpiece, &rc);
        if (RECTW(rc) != (int)value) {
            rc.top = 0;
            rc.bottom = (int)value;
            _c(self->cell_backpiece)->setRect(self->cell_backpiece, &rc);
            return TRUE;
        }
        return FALSE;
    
    }
    case NCSP_PASSWORDBOX_CELL_BKCOLOR:
    if (self->cell_backpiece->bk_color != (ARGB)value) {
        self->cell_backpiece->bk_color = (ARGB)value;
        return TRUE;
    }
    return FALSE;
    default:
        break;
   }
    return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mPasswordBox_getProperty(mPasswordBox *self, int id)
{
    if(id >= NCSP_PASSWORDBOX_MAX)
        return 0;
    
    mPanelPiece *body = (mPanelPiece*)self->body;
    if (body) {
        mShapeTransRoundPiece *backpiece = _c(body)->getBkgndPiece(body);
        assert(backpiece);
        switch (id) {
            case NCSP_PASSWORDBOX_RADIUS:
                return _c(backpiece)->getProperty(backpiece, NCSP_TRANROUND_RADIUS);
            case NCSP_PASSWORDBOX_BKCOLOR:
                return _c(backpiece)->getProperty(backpiece, NCSP_TRANROUND_BKCOLOR);
            case NCSP_PASSWORDBOX_CORNERFLAG:
                return _c(backpiece)->getProperty(backpiece, NCSP_TRANROUND_CORNERFLAG);
            default:
                break;
        }
    }
    
     switch (id) {
     case NCSP_PASSWORDBOX_MAX_LENGTH:
         return (DWORD)self->max_password_length;
     case NCSP_PASSWORDBOX_CELL_RADIUS:
        return (DWORD)self->cell_backpiece->corner_radius;
     case NCSP_PASSWORDBOX_CELL_WIDTH:
     {
        RECT rc;
        _c(self->cell_backpiece)->getRect(self->cell_backpiece, &rc);
        return (DWORD)RECTW(rc);
     }
     case NCSP_PASSWORDBOX_CELL_HEIGHT:
     {
        RECT rc;
        _c(self->cell_backpiece)->getRect(self->cell_backpiece, &rc);
        return (DWORD)RECTH(rc);
     }
     case NCSP_PASSWORDBOX_CELL_BKCOLOR:
        return (DWORD)self->cell_backpiece->bk_color;
     default:
         break;
    }

    return Class(mWidget).getProperty((mWidget*)self, id);
}

static void mPasswordBox_onChar (mPasswordBox *self, WPARAM wParam, LPARAM lParam)
{
    char ch = (char)wParam;
    int result = 0;

    if (ch == '\b' || ch == 127) { //backspace
        result = mPasswordBox_subtract(self, 1);
    } else if((ch >='A' && ch <='Z') || (ch >='a' && ch <= 'z') ||  (ch >='0' && ch <='9') ) {
        char tmp[2];
        tmp[0] = (char)wParam;
        tmp[1] = 0;
        result = mPasswordBox_append(self, tmp); 
    } else {
        return;
    }
    
    if(result > 0)
        ncsNotifyParent((mWidget*)self, NCSN_PASSWORDBOX_CHANGED);
}

static int mPasswordBox_onKeyDown(mPasswordBox* self, int scancode, DWORD key_flags)
{
    switch(scancode) {
        case SCANCODE_REMOVE:
            mPasswordBox_onChar(self, (WPARAM)'\b', (LPARAM)0);
            return 0;
        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            ncsNotifyParent ((mWidget*)self, NCSN_PASSWORDBOX_ENTERED);
            return 0;

    }
}

static LRESULT mPasswordBox_wndProc (mPasswordBox *self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case MSG_KEYDOWN:
        {
            return mPasswordBox_onKeyDown(self, wParam, lParam);
        }
        case MSG_CHAR:
        {
            mPasswordBox_onChar(self, wParam, lParam);
            return 0;
        }
    }
    return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

static void mPasswordBox_clear(mPasswordBox *self)
{
    mPasswordBox_subtract(self, self->max_password_length);
}

BEGIN_CMPT_CLASS(mPasswordBox, mWidget)
    CLASS_METHOD_MAP(mPasswordBox, construct)
    CLASS_METHOD_MAP(mPasswordBox, destroy)
    CLASS_METHOD_MAP(mPasswordBox, onPaint)
    CLASS_METHOD_MAP(mPasswordBox, onCreate)
    //CLASS_METHOD_MAP(mPasswordBox, onChar)
    CLASS_METHOD_MAP(mPasswordBox, wndProc)
    //CLASS_METHOD_MAP(mPasswordBox, append)
    //CLASS_METHOD_MAP(mPasswordBox, subtract)
    CLASS_METHOD_MAP(mPasswordBox, createBody)
    CLASS_METHOD_MAP(mPasswordBox, getProperty)
    CLASS_METHOD_MAP(mPasswordBox, setProperty)
    CLASS_METHOD_MAP(mPasswordBox, clear)    
END_CMPT_CLASS

