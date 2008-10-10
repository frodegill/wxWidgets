/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: private.h 53819 2008-05-29 14:11:45Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_CORE_H_
#define _WX_PRIVATE_CORE_H_

#include "wx/defs.h"

#include <CoreFoundation/CoreFoundation.h>

#include "wx/osx/core/cfstring.h"
#include "wx/osx/core/cfdataref.h"

#if wxOSX_USE_COCOA_OR_CARBON

WXDLLIMPEXP_BASE long UMAGetSystemVersion() ;

void WXDLLIMPEXP_CORE wxMacStringToPascal( const wxString&from , unsigned char * to );
wxString WXDLLIMPEXP_CORE wxMacMakeStringFromPascal( const unsigned char * from );

#endif

#if wxUSE_GUI

#if wxOSX_USE_IPHONE
#include <CoreGraphics/CoreGraphics.h>
#else
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "wx/bitmap.h"
#include "wx/window.h"

class WXDLLIMPEXP_CORE wxMacCGContextStateSaver
{
    DECLARE_NO_COPY_CLASS(wxMacCGContextStateSaver)

public:
    wxMacCGContextStateSaver( CGContextRef cg )
    {
        m_cg = cg;
        CGContextSaveGState( cg );
    }
    ~wxMacCGContextStateSaver()
    {
        CGContextRestoreGState( m_cg );
    }
private:
    CGContextRef m_cg;
};

class WXDLLIMPEXP_CORE wxDeferredObjectDeleter : public wxObject
{
public :
    wxDeferredObjectDeleter( wxObject* obj ) : m_obj(obj) 
    {
    }
    virtual ~wxDeferredObjectDeleter()
    {
        delete m_obj;
    }
protected :
    wxObject* m_obj ;
} ;

// Quartz

WXDLLIMPEXP_CORE CGImageRef wxMacCreateCGImageFromBitmap( const wxBitmap& bitmap );

WXDLLIMPEXP_CORE CGDataProviderRef wxMacCGDataProviderCreateWithCFData( CFDataRef data );
WXDLLIMPEXP_CORE CGDataConsumerRef wxMacCGDataConsumerCreateWithCFData( CFMutableDataRef data );
WXDLLIMPEXP_CORE CGDataProviderRef wxMacCGDataProviderCreateWithMemoryBuffer( const wxMemoryBuffer& buf );

CGColorSpaceRef WXDLLIMPEXP_CORE wxMacGetGenericRGBColorSpace(void);

class wxWindowMac;
// to
extern wxWindow* g_MacLastWindow;
class wxNonOwnedWindow;

// temporary typedef so that no additional casts are necessary within carbon code at the moment

class wxMacControl;
class wxWidgetImpl;
class wxNotebook;
class wxTextCtrl;

#if wxOSX_USE_CARBON
typedef wxMacControl wxWidgetImplType;
#else
typedef wxWidgetImpl wxWidgetImplType;
#endif

class wxMenuItemImpl : public wxObject 
{
public :
    wxMenuItemImpl( wxMenuItem* peer ) : m_peer(peer)
    {
    }
    
    virtual ~wxMenuItemImpl() ;
    virtual void SetBitmap( const wxBitmap& bitmap ) = 0;
    virtual void Enable( bool enable ) = 0;
    virtual void Check( bool check ) = 0;
    virtual void SetLabel( const wxString& text, wxAcceleratorEntry *entry ) = 0;
    virtual void Hide( bool hide = true ) = 0;
    
    virtual void * GetHMenuItem() = 0;

    wxMenuItem* GetWXPeer() { return m_peer ; }

    static wxMenuItemImpl* Create( wxMenuItem* peer, wxMenu *pParentMenu,
                       int id,
                       const wxString& text,
                       wxAcceleratorEntry *entry,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu );

protected :
    wxMenuItem* m_peer;
    
    DECLARE_ABSTRACT_CLASS(wxMenuItemImpl);
} ;

class wxMenuImpl : public wxObject 
{
public :
    wxMenuImpl( wxMenu* peer ) : m_peer(peer)
    {
    }
    
    virtual ~wxMenuImpl() ;    
    virtual void InsertOrAppend(wxMenuItem *pItem, size_t pos) = 0;    
    virtual void Remove( wxMenuItem *pItem ) = 0;
    
    virtual void MakeRoot() = 0;

    virtual void SetTitle( const wxString& text ) = 0;

    virtual WXHMENU GetHMenu() = 0;
    
    wxMenu* GetWXPeer() { return m_peer ; }

    static wxMenuImpl* Create( wxMenu* peer, const wxString& title );
    static wxMenuImpl* CreateRootMenu( wxMenu* peer );
protected :
    wxMenu* m_peer;
    
    DECLARE_ABSTRACT_CLASS(wxMenuItemImpl);
} ;



class WXDLLIMPEXP_CORE wxWidgetImpl : public wxObject
{
public :
    wxWidgetImpl( wxWindowMac* peer , bool isRootControl = false );
    wxWidgetImpl();
    virtual ~wxWidgetImpl();

    void Init();

    bool                IsRootControl() const { return m_isRootControl; }

    wxWindowMac*        GetWXPeer() const { return m_wxPeer; }
    
    bool IsOk() const { return GetWXWidget() != NULL; }

    // not only the control itself, but also all its parents must be visible
    // in order for this function to return true
    virtual bool        IsVisible() const = 0;
    // set the visibility of this widget (maybe latent)
    virtual void        SetVisibility( bool visible ) = 0;

    virtual void        Raise() = 0;
    
    virtual void        Lower() = 0;

    virtual void        ScrollRect( const wxRect *rect, int dx, int dy ) = 0;

    virtual WXWidget    GetWXWidget() const = 0;
    
    virtual void        SetBackgroundColour( const wxColour& col ) = 0;

    // all coordinates in native parent widget relative coordinates
    virtual void        GetContentArea( int &left , int &top , int &width , int &height ) const = 0;
    virtual void        Move(int x, int y, int width, int height) = 0;
    virtual void        GetPosition( int &x, int &y ) const = 0;
    virtual void        GetSize( int &width, int &height ) const = 0;
    virtual void        SetControlSize( wxWindowVariant variant ) = 0;

    virtual void        SetNeedsDisplay( const wxRect* where = NULL ) = 0;
    virtual bool        GetNeedsDisplay() const = 0;

    virtual bool        NeedsFocusRect() const;
    virtual void        SetNeedsFocusRect( bool needs );

    virtual bool        CanFocus() const = 0;
    // return true if successful
    virtual bool        SetFocus() = 0;
    virtual bool        HasFocus() const = 0;
    
    virtual void        RemoveFromParent() = 0;
    virtual void        Embed( wxWidgetImpl *parent ) = 0;
    
    virtual void        SetDefaultButton( bool isDefault ) = 0;
    virtual void        PerformClick() = 0;
    virtual void        SetLabel( const wxString& title, wxFontEncoding encoding ) = 0;

    virtual wxInt32     GetValue() const = 0;
    virtual void        SetValue( wxInt32 v ) = 0;
    virtual void        SetBitmap( const wxBitmap& bitmap ) = 0;
    virtual void        SetupTabs( const wxNotebook &notebook ) =0;
    virtual void        GetBestRect( wxRect *r ) const = 0;
    virtual bool        IsEnabled() const = 0;
    virtual void        Enable( bool enable ) = 0;
    virtual void        SetMinimum( wxInt32 v ) = 0;
    virtual void        SetMaximum( wxInt32 v ) = 0;
    virtual void        PulseGauge() = 0;
    virtual void        SetScrollThumb( wxInt32 value, wxInt32 thumbSize ) = 0;

    virtual void        SetFont( const wxFont & font , const wxColour& foreground , long windowStyle, bool ignoreBlack = true ) = 0;

    // is the clicked event sent AFTER the state already changed, so no additional
    // state changing logic is required from the outside
    virtual bool        ButtonClickDidStateChange() = 0;
    
    virtual void        InstallEventHandler( WXWidget control = NULL ) = 0;

    // static methods for associating native controls and their implementations

    static wxWidgetImpl* 
                        FindFromWXWidget(WXWidget control);
    
    static void         RemoveAssociations( wxWidgetImpl* impl);
    
    static void         Associate( WXWidget control, wxWidgetImpl *impl );
    
    // static creation methods, must be implemented by all toolkits
    
    static wxWidgetImplType*    CreateUserPane( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;
    static wxWidgetImplType*    CreateContentView( wxNonOwnedWindow* now ) ;

    static wxWidgetImplType*    CreateButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;

    static wxWidgetImplType*    CreateDisclosureTriangle( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;
                                    
    static wxWidgetImplType*    CreateStaticLine( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;

    static wxWidgetImplType*    CreateGroupBox( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;

    static wxWidgetImplType*    CreateStaticText( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;

    static wxWidgetImplType*    CreateTextControl( wxTextCtrl* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& content,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;

    static wxWidgetImplType*    CreateSearchControl( wxTextCtrl* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& content,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) ;

    static wxWidgetImplType*    CreateCheckBox( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);
                                    
    static wxWidgetImplType*    CreateRadioButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateToggleButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateBitmapToggleButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxBitmap& bitmap,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateBitmapButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxBitmap& bitmap,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateTabView( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateGauge( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateSlider( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateSpinButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateScrollBar( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateChoice( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxMenu* menu,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    static wxWidgetImplType*    CreateListBox( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle);

    // converts from Toplevel-Content relative to local
    static void Convert( wxPoint *pt , wxWidgetImpl *from , wxWidgetImpl *to );
protected :
    bool                m_isRootControl;
    wxWindowMac*        m_wxPeer;
    bool                m_needsFocusRect;

    DECLARE_ABSTRACT_CLASS(wxWidgetImpl)
};

//
// the interface to be implemented eg by a listbox
//

class WXDLLIMPEXP_CORE wxListWidgetColumn 
{
public :
    virtual ~wxListWidgetColumn() {}
} ;

class WXDLLIMPEXP_CORE wxListWidgetCellValue
{
public :
    wxListWidgetCellValue() {}
    virtual ~wxListWidgetCellValue() {}
    
   virtual void Set( CFStringRef value ) = 0;
    virtual void Set( const wxString& value ) = 0;
    virtual void Set( int value ) = 0;
    
    virtual int GetIntValue() const = 0;
    virtual wxString GetStringValue() const = 0;
} ;

class WXDLLIMPEXP_CORE wxListWidgetImpl
{
public:
    wxListWidgetImpl() {}
    virtual ~wxListWidgetImpl() { }
    
    virtual wxListWidgetColumn*     InsertTextColumn( unsigned pos, const wxString& title, bool editable = false, 
                                wxAlignment just = wxALIGN_LEFT , int defaultWidth = -1) = 0 ;
    virtual wxListWidgetColumn*     InsertCheckColumn( unsigned pos , const wxString& title, bool editable = false, 
                                wxAlignment just = wxALIGN_LEFT , int defaultWidth =  -1) = 0 ;
    
    // add and remove

    // TODO will be replaced
    virtual void            ListDelete( unsigned int n ) = 0;
    virtual void            ListInsert( unsigned int n ) = 0;
    virtual void            ListClear() = 0;

    // selecting

    virtual void            ListDeselectAll() = 0;
    virtual void            ListSetSelection( unsigned int n, bool select, bool multi ) = 0;
    virtual int             ListGetSelection() const = 0;
    virtual int             ListGetSelections( wxArrayInt& aSelections ) const = 0;
    virtual bool            ListIsSelected( unsigned int n ) const = 0;
    
    // display

    virtual void            ListScrollTo( unsigned int n ) = 0;
    virtual void            UpdateLine( unsigned int n, wxListWidgetColumn* col = NULL ) = 0;
    virtual void            UpdateLineToEnd( unsigned int n) = 0;

    // accessing content

    virtual unsigned int    ListGetCount() const = 0;
};

//
// interface to be implemented by a textcontrol
//

class WXDLLIMPEXP_FWD_CORE wxTextAttr;

// common interface for all implementations
class WXDLLIMPEXP_CORE wxTextWidgetImpl

{
public :
    wxTextWidgetImpl() {}

    virtual ~wxTextWidgetImpl() {}

    virtual bool CanFocus() const { return true; }

    virtual wxString GetStringValue() const = 0 ;
    virtual void SetStringValue( const wxString &val ) = 0 ;
    virtual void SetSelection( long from, long to ) = 0 ;
    virtual void GetSelection( long* from, long* to ) const = 0 ;
    virtual void WriteText( const wxString& str ) = 0 ;

    virtual void SetStyle( long start, long end, const wxTextAttr& style ) ;
    virtual void Copy() ;
    virtual void Cut() ;
    virtual void Paste() ;
    virtual bool CanPaste() const ;
    virtual void SetEditable( bool editable ) ;
    virtual long GetLastPosition() const ;
    virtual void Replace( long from, long to, const wxString &str ) ;
    virtual void Remove( long from, long to ) ;


    virtual bool HasOwnContextMenu() const
        { return false ; }

    virtual bool SetupCursor( const wxPoint& WXUNUSED(pt) )
        { return false ; }

    virtual void Clear() ;
    virtual bool CanUndo() const;
    virtual void Undo() ;
    virtual bool CanRedo() const;
    virtual void Redo() ;
    virtual int GetNumberOfLines() const ;
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const ;
    virtual void ShowPosition(long WXUNUSED(pos)) ;
    virtual int GetLineLength(long lineNo) const ;
    virtual wxString GetLineText(long lineNo) const ;
    virtual void CheckSpelling(bool WXUNUSED(check)) { }
};

//
// common interface for search controls
//

class wxSearchWidgetImpl
{
public :
    wxSearchWidgetImpl(){}
    virtual ~wxSearchWidgetImpl(){}

    // search field options
    virtual void ShowSearchButton( bool show ) = 0;
    virtual bool IsSearchButtonVisible() const = 0;

    virtual void ShowCancelButton( bool show ) = 0;
    virtual bool IsCancelButtonVisible() const = 0;

    virtual void SetSearchMenu( wxMenu* menu ) = 0;

    virtual void SetDescriptiveText(const wxString& text) = 0;
} ;

//
// toplevel window implementation class
//

class wxNonOwnedWindowImpl : public wxObject
{
public :
    wxNonOwnedWindowImpl( wxNonOwnedWindow* nonownedwnd) : m_wxPeer(nonownedwnd)
    {
    }
    wxNonOwnedWindowImpl()
    {
    }
    virtual ~wxNonOwnedWindowImpl()
    {
    }
            
    virtual void Destroy() 
    {
    }

    virtual void Create( wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name ) = 0;
    
    
    virtual WXWindow GetWXWindow() const = 0;
        
    virtual void Raise()
    {
    }
    
    virtual void Lower()
    {
    }

    virtual bool Show(bool WXUNUSED(show))
    {
        return false;
    }
    
    virtual bool ShowWithEffect(bool show, wxShowEffect WXUNUSED(effect), unsigned WXUNUSED(timeout))
    {
        return Show(show);
    }
        
    virtual void Update()
    {
    }

    virtual bool SetTransparent(wxByte WXUNUSED(alpha))
    {
        return false;
    }

    virtual bool SetBackgroundColour(const wxColour& WXUNUSED(col) )
    {
        return false;
    }

    virtual void SetExtraStyle( long WXUNUSED(exStyle) )
    {
    }
    
    virtual bool SetBackgroundStyle(wxBackgroundStyle WXUNUSED(style))
    {            
        return false ;
    }
    
    bool CanSetTransparent()
    {
        return false;
    }

    virtual void GetContentArea( int &left , int &top , int &width , int &height ) const = 0;    
    virtual void MoveWindow(int x, int y, int width, int height) = 0;
    virtual void GetPosition( int &x, int &y ) const = 0;
    virtual void GetSize( int &width, int &height ) const = 0;

    virtual bool SetShape(const wxRegion& WXUNUSED(region))
    {
        return false;
    }
        
    virtual void SetTitle( const wxString& title, wxFontEncoding encoding ) = 0;
    
    virtual bool IsMaximized() const = 0;
    
    virtual bool IsIconized() const= 0;
    
    virtual void Iconize( bool iconize )= 0;
    
    virtual void Maximize(bool maximize) = 0;
    
    virtual bool IsFullScreen() const= 0;
    
    virtual bool ShowFullScreen(bool show, long style)= 0;

    virtual void RequestUserAttention(int flags) = 0;
    
    virtual void ScreenToWindow( int *x, int *y ) = 0; 
    
    virtual void WindowToScreen( int *x, int *y ) = 0;
    
    wxNonOwnedWindow*   GetWXPeer() { return m_wxPeer; }
    
    // static creation methods, must be implemented by all toolkits
    
    static wxNonOwnedWindowImpl* CreateNonOwnedWindow( wxNonOwnedWindow* wxpeer, wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name  ) ;

protected :
    wxNonOwnedWindow*   m_wxPeer;
    DECLARE_ABSTRACT_CLASS(wxNonOwnedWindowImpl)
};

#endif // wxUSE_GUI

//---------------------------------------------------------------------------
// cocoa bridging utilities
//---------------------------------------------------------------------------

bool wxMacInitCocoa();

class WXDLLIMPEXP_CORE wxMacAutoreleasePool
{
public :
    wxMacAutoreleasePool();
    ~wxMacAutoreleasePool();
private :
    void* m_pool;
};

// NSObject

void wxMacCocoaRelease( void* obj );
void wxMacCocoaAutorelease( void* obj );
void wxMacCocoaRetain( void* obj );


#endif
    // _WX_PRIVATE_CORE_H_
