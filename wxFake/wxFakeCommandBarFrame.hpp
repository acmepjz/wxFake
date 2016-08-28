#ifndef _WXFAKE_WXFAKECOMMANDBARFRAME_HPP_
#define _WXFAKE_WXFAKECOMMANDBARFRAME_HPP_

#include <wx/frame.h>
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include <wx/brush.h>
#include <wx/vector.h>
#include <wx/eventfilter.h>
#include <wx/timer.h>
#include <map>

class wxFakeCommandBarArtBase;

class wxFakeMenu;
class wxFakeButtonData;
class wxFakeCommandBarDataBase;
class wxFakeCommandBarData;
class wxFakeCommandBarRefData;
class wxFakeCommandBar;
class wxFakeMiniFrame;

struct wxFakeCommandBarRow{
	int m_size;
	wxVector<wxFakeCommandBar*> m_bars;

	wxFakeCommandBarRow(){
		m_size=0;
	}

	void Destroy();
};

struct wxFakeCommandBarDockArea{
	int m_size;
	wxVector<wxFakeCommandBarRow*> m_rows;

	wxFakeCommandBarDockArea(){
		m_size=0;
	}

	void Destroy(){
		for(unsigned int i=0;i<m_rows.size();i++){
			m_rows[i]->Destroy();
			delete m_rows[i];
		}

		m_rows.clear();
		m_size=0;
	}
};

enum wxFakeDockableCommandBarFlags{
	fdcbMainMenu=0x1,
	fdcbShowChevron=0x2,
	fdcbClosable=0x4,
	fdcbHideFromCustomMenu=0x8,
	fdcbAlwaysFloat=0x10,
	fdcbAlwaysDocked=0x20,

	fdcbUndocked=0x10000,
};

struct wxFakeDockableCommandBar{
	int m_dir;
	int m_row;
	int m_idx;
	int m_flags;
	wxPoint m_pos;
	wxSize m_size;
	wxFakeCommandBarDataBase* m_bardata;
	wxFakeMiniFrame* m_frame;
	wxFakeCommandBar* m_bar;
};

struct wxFakeDockablePane{
	float m_scale;
	int m_layer;
	int m_dir;
	int m_row;
	wxVector<int> m_idxs;
	int m_flags;
	wxRect m_rect; //??
	wxWindow *m_window;
	wxFakeMiniFrame* m_frame;
	wxString m_caption;
};

struct wxFakeDockablePaneRow;

struct wxFakeDockablePaneSplitterHitTestResult{
	int m_layer;
	int m_dir;
	int m_row;
	int m_idx;
	wxFakeDockablePaneRow* m_subrow;
};

struct wxFakeDockablePaneItem{
	wxFakeDockablePaneRow* m_subrow;
	wxFakeDockablePane* m_pane;
};

struct wxFakeDockablePaneRow{
	wxRect m_rect; //??
	float m_scale; //only used when nested
	int m_dir;
	int m_size; //only used when not nested, not including resize gripper
	wxVector<wxFakeDockablePaneItem> m_pane;

	wxFakeDockablePaneRow(){
		m_scale=1.0f;
		m_dir=0;
		m_size=0;
	}

	bool DeleteEmptySubRow(){
		for(int i=0;i<(int)m_pane.size();i++){
			if(m_pane[i].m_subrow!=NULL){
				if(m_pane[i].m_subrow->DeleteEmptySubRow()){
					delete m_pane[i].m_subrow;
					m_pane.erase(m_pane.begin()+i);
					i--;
				}
			}
		}
		return m_pane.empty();
	}

	//internal function
	wxFakeDockablePaneSplitterHitTestResult SplitterHitTest(int layer,int dir,int row,const wxPoint& pos);

	void Destroy(){
		for(unsigned int i=0;i<m_pane.size();i++){
			if(m_pane[i].m_subrow!=NULL){
				m_pane[i].m_subrow->Destroy();
				delete m_pane[i].m_subrow;
			}
		}

		m_pane.clear();
		m_scale=1.0f;
		m_dir=0;
		m_size=0;
	}
};

struct wxFakeDockablePaneDockArea{
	int m_size; //including resize gripper
	wxVector<wxFakeDockablePaneRow*> m_rows;

	wxFakeDockablePaneDockArea(){
		m_size=0;
	}

	//internal function
	wxFakeDockablePaneSplitterHitTestResult SplitterHitTest(int layer,int dir,const wxRect& rect,const wxPoint& pos);

	void Destroy(){
		for(unsigned int i=0;i<m_rows.size();i++){
			m_rows[i]->Destroy();
			delete m_rows[i];
		}

		m_rows.clear();
		m_size=0;
	}
};

struct wxFakeDockablePaneDockLayer{
	wxFakeDockablePaneDockArea m_dockedpanes[4];
	wxRect m_rect;

	void Destroy(){
		for(int i=0;i<4;i++){
			m_dockedpanes[i].Destroy();
		}
	}
};

class wxFakeCommandBarFrame:public wxFrame,public wxEventFilter{
protected:
	std::multimap<int,wxFakeButtonData*> m_btns;
	std::multimap<int,wxFakeCommandBarRefData*> m_bars;

	wxVector<wxFakeButtonData*> m_accel_btns;
	wxVector<wxFakeCommandBarDataBase*> m_accel_bars;
	wxVector<int> m_accel_btnidx;

	wxFakeCommandBarDockArea m_dockedbars[4];
	wxVector<wxFakeDockableCommandBar> m_dockablebars;

	wxPoint m_drag_offset;
	int m_drag_index;

	bool m_dockablebars_dirty;
	bool m_temporary_disable_dock;
	bool m_update_on_next_idle;

	wxFakeCommandBarData* m_dockablebars_menu;

	wxFakeCommandBarArtBase* m_artprov;

	wxWindow* m_clientwnd;

	wxVector<wxFakeDockablePaneDockLayer*> m_panelayer;
	wxVector<wxFakeDockablePane*> m_dockablepanes;

	wxFakeDockablePaneSplitterHitTestResult m_pane_drag;
	wxRect m_pane_drag_old_rect;

	wxTimer* m_timer;

	wxBrush m_stipple_brush;

	void Init();
public:
	wxFakeCommandBarFrame(){
		Init();
	}
	wxFakeCommandBarFrame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr){
		Init();
		Create(parent,id,title,pos,size,style,name);
	}
	~wxFakeCommandBarFrame();

	bool Create(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr);

	//add button with specified id.
	wxFakeButtonData* AddButton(int id=0);
	//add button from existing button (will create a copy).
	wxFakeButtonData* AddButton(const wxFakeButtonData& btn);
	//add button from pointer. The ownership of this button will be
	//transferd to button manager
	wxFakeButtonData* AddButton(wxFakeButtonData* btn);

	//add command bar with specified id.
	wxFakeCommandBarRefData* AddBarData(int id=0);
	//add command bar from existing one (will create a copy).
	wxFakeCommandBarRefData* AddBarData(const wxFakeCommandBarRefData& bar);
	//add command bar from pointer. The ownership will be
	//transferd to button manager
	wxFakeCommandBarRefData* AddBarData(wxFakeCommandBarRefData* bar);

	wxFakeButtonData* FindButton(int id);
	wxFakeButtonData* FindButton(const wxString& key);

	wxFakeCommandBarRefData* FindBarData(int id);
	wxFakeCommandBarRefData* FindBarData(const wxString& key);

	wxFakeCommandBarArtBase* GetArtProvider() const{
		return m_artprov;
	}
	void SetArtProvider(wxFakeCommandBarArtBase* artprov);

	wxWindow* GetClientWindow() const{
		return m_clientwnd;
	}
	void SetClientWindow(wxWindow* wnd);
	void LayoutClientWindow();

	bool PopupMenuEx(wxFakeCommandBarDataBase* bar,const wxPoint& ptOrigin=wxDefaultPosition,const wxSize& size=wxDefaultSize,int dir=0,int leftMargin=0,int topMargin=0,int rightMargin=0,int bottomMargin=0);
	bool PopupMenuEx(int id){
		wxFakeCommandBarRefData *bar=FindBarData(id);
		if(bar==NULL) return false;
		return PopupMenuEx((wxFakeCommandBarDataBase*)bar);
	}
	bool PopupMenuEx(const wxString& key){
		wxFakeCommandBarRefData *bar=FindBarData(key);
		if(bar==NULL) return false;
		return PopupMenuEx((wxFakeCommandBarDataBase*)bar);
	}

	void UpdateAcceleratorTable(bool useCommandBar=true);

	wxFakeCommandBar* AddDockableCommandBar(wxFakeCommandBarDataBase* bar,int dir=0,int row=-1,int pos=-1,int flags=fdcbShowChevron|fdcbClosable);
	wxFakeCommandBar* AddDockableCommandBar(int id,int dir=0,int row=-1,int pos=-1,int flags=fdcbShowChevron|fdcbClosable){
		wxFakeCommandBarRefData *bar=FindBarData(id);
		if(bar==NULL) return NULL;
		return AddDockableCommandBar((wxFakeCommandBarDataBase*)bar,dir,row,pos,flags);
	}
	wxFakeCommandBar* AddDockableCommandBar(const wxString& key,int dir=0,int row=-1,int pos=-1,int flags=fdcbShowChevron|fdcbClosable){
		wxFakeCommandBarRefData *bar=FindBarData(key);
		if(bar==NULL) return NULL;
		return AddDockableCommandBar((wxFakeCommandBarDataBase*)bar,dir,row,pos,flags);
	}

	wxFakeDockablePane* AddDockablePane(wxWindow *window,const wxString& caption=wxEmptyString,wxFakeDockablePane* parent=NULL,int layer=0,int dir=0,int row=-1,int pos=-1,float scale=1.0f,int height=96,int flags=0);

	bool ShowCommandBarByIndex(int index,bool show=true);

	bool ShowCommandBar(wxFakeMiniFrame* frame,bool show=true){
		for(int i=0;i<(int)m_dockablebars.size();i++){
			if(m_dockablebars[i].m_frame==frame){
				return ShowCommandBarByIndex(i,show);
			}
		}
		return false;
	}
	bool ShowCommandBar(wxFakeCommandBar* bar,bool show=true){
		for(int i=0;i<(int)m_dockablebars.size();i++){
			if(m_dockablebars[i].m_bar==bar){
				return ShowCommandBarByIndex(i,show);
			}
		}
		return false;
	}
	bool ShowCommandBar(wxFakeCommandBarDataBase* bar,bool show=true){
		for(int i=0;i<(int)m_dockablebars.size();i++){
			if(m_dockablebars[i].m_bardata==bar){
				return ShowCommandBarByIndex(i,show);
			}
		}
		return false;
	}
	bool ShowCommandBar(int id,bool show=true){
		wxFakeCommandBarRefData *bar=FindBarData(id);
		if(bar==NULL) return false;
		return ShowCommandBar((wxFakeCommandBarDataBase*)bar,show);
	}
	bool ShowCommandBar(const wxString& key,bool show=true){
		wxFakeCommandBarRefData *bar=FindBarData(key);
		if(bar==NULL) return false;
		return ShowCommandBar((wxFakeCommandBarDataBase*)bar,show);
	}

	bool MakeMenuFloatByIndex(int index,const wxPoint& pos,const wxPoint& drag_offset);

	bool MakeMenuFloat(wxFakeCommandBarDataBase* bar,const wxPoint& pos,const wxPoint& drag_offset){
		for(int i=0;i<(int)m_dockablebars.size();i++){
			if(m_dockablebars[i].m_bardata==bar){
				return MakeMenuFloatByIndex(i,pos,drag_offset);
			}
		}
		return false;
	}

	virtual bool Layout();
	bool LayoutEx(wxFakeCommandBar* selected_bar,const wxPoint& pos);

	bool BeginDragCommandBarByIndex(int index,const wxPoint& drag_offset);
	bool BeginDragCommandBar(wxFakeCommandBar* bar,const wxPoint& drag_offset){
		for(int i=0;i<(int)m_dockablebars.size();i++){
			if(m_dockablebars[i].m_bar==bar){
				return BeginDragCommandBarByIndex(i,drag_offset);
			}
		}
		return false;
	}

	virtual bool Destroy();

	virtual int FilterEvent(wxEvent &event);

	void PopupCustomCommandBarMenu();

	void UpdateOnNextIdle(bool update=true){
		m_update_on_next_idle=update;
	}
	void UpdateDirty(bool unDirty=true);
	void UnDirty();
	void UnDirtyVisible();

	bool Redraw(wxDC& dc);

	wxFakeDockablePaneSplitterHitTestResult SplitterHitTest(const wxPoint& pos);

#ifdef __WXMSW__
	// dirty hack
	virtual WXLRESULT MSWWindowProc(WXUINT message,WXWPARAM wParam,WXLPARAM lParam);
#endif
protected:
	void OnPaint(wxPaintEvent& event);
	void OnActivate(wxActivateEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnTimer(wxTimerEvent& event);

	int LayoutOneRow(int dir,int row,int offset,int offset2,int max_size,wxFakeCommandBar* selected_bar,const wxPoint& pos);
	void LayoutDockedBars(int dir,int offset,int offset2,int max_size,wxFakeCommandBar* selected_bar,const wxPoint& pos);

	void LayoutDockedPanes(wxFakeDockablePaneDockLayer* docklayer,int dir,const wxPoint& offset,int max_size);
	void LayoutDockedPaneRow(wxFakeDockablePaneRow* dockrow,const wxRect& rect);
	void LayoutSingleDockedPane(wxFakeDockablePane* pane,const wxRect& rect);

	//return value: -1: can't dock  >=0: distance
	int GetNewDockedRowFromPos(int dir,int old_row,int offset,int width,const wxPoint& pos,int& new_row,int& new_idx);

	void DrawResizeHint(wxDC& dc, const wxRect& rect);
private:
	DECLARE_CLASS(wxFakeCommandBarFrame)
	DECLARE_EVENT_TABLE()
};

#endif
