#ifdef __WXGTK__

#include <wx/cursor.h>
#include <gtk/gtk.h>
#include <wx/gtk/private/object.h>
#include <wx/gtk/private/gtk2-compat.h>

//-----------------------------------------------------------------------------
// wxFakeCursorRefData
//-----------------------------------------------------------------------------

class wxFakeCursorRefData: public wxGDIRefData
{
public:
    wxFakeCursorRefData();
    virtual ~wxFakeCursorRefData();

    virtual bool IsOk() const { return m_cursor != NULL; }

    GdkCursor *m_cursor;

private:
    // There is no way to copy m_cursor so we can't implement a copy ctor
    // properly.
    wxDECLARE_NO_COPY_CLASS(wxFakeCursorRefData);
};

wxFakeCursorRefData::wxFakeCursorRefData()
{
    m_cursor = NULL;
}

wxFakeCursorRefData::~wxFakeCursorRefData()
{
    if (m_cursor) gdk_cursor_unref( m_cursor );
}

class wxFakeGTKCursor: public wxCursor{
public:
	wxFakeGTKCursor(GdkCursorType gdk_cur){
		wxFakeCursorRefData *d=new wxFakeCursorRefData();
		d->m_cursor=gdk_cursor_new(gdk_cur);
		m_refData=d;
	}
};

#endif
