#ifndef _WXFAKE_WXFAKECOMMANDBARDATA_HPP_
#define _WXFAKE_WXFAKECOMMANDBARDATA_HPP_

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/accel.h>
#include <wx/vector.h>
#include <wx/gdicmn.h>
#include <wx/dc.h>
#include <wx/window.h>
#include <wx/event.h>
#include "wxFake/wxFakeCommandBarArt.hpp"

wxString MyStripMenuCodesEx(const wxString& in,int& out_mnemonics,int& out_mnemonics_pos);

enum wxFakeCommandBarFlags{
	//show "drag to make this menu float" gripper
	//in menu
	fcbfDragToMakeThisMenuFloat=0x1,
	//set menu mode to tool bar
	//(popup menu looks like tool bar)
	fcbfToolBarMode=0x2,
	//(experimental)
	fcbfMultiLineToolBar=0x4,
	//this bar is main menu which has different looking
	fcbfMainMenu=0x8,
};

enum wxFakeButtonType{
	//normal push button
	fbttNormal=0,
	//separator
	fbttSeparator=1,
	//check box
	fbttCheck=2,
	//option button
	fbttOption=3,
	//option button (nullable)
	fbttOptionNullable=4,
	//push button which have a drop down menu,
	//the button itself can be clicked
	fbttSplit=5,
	//column separator (in menu only)
	fbttColumnSeparator=6,
	//3-state check box
	fbttCheck3State=7,
};

enum wxFakeButtonValue{
	//unchecked
	fbtvUnchecked=0,
	//checked
	fbtvChecked=1,
	//3-state
	fbtvUndetermined=2,
};

enum wxFakeButtonFlags{
	//hidden (not visible)
	fbtfHidden=0x1,
	//disabled (not enable)
	fbtfDisabled=0x2,
	//show dropdown sign in tool bar
	//(not needed if button type is fbttSplit)
	fbtfShowDropdown=0x4,
	//obsolete or currently unsupported
	fbtfOwnerMeasure=0x8,
	//obsolete or currently unsupported
	fbtfOwnerDraw=0x10,
	//obsolete or currently unsupported
	fbtfOwnerDrawAfter=0x20,
	//default item in popup menu (currently unsupported)
	fbtfDefaultItem=0x40,
	//hide caption in tool bar
	fbtfHideCationInToolBar=0x80,
	//start new row if menu mode is tool bar
	fbtfStartNewRow=0x100,
	//the button width is full row if menu mode is tool bar
	fbtfFullRow=0x200,
	//show color instead of icon
	//(obsolete)
	fbtfColor=0x400,
	//don't change text color if button is disabled
	fbtfNoDisabledColor=0x800,

	fbtfAlignCenter=0x1000,
	fbtfAlignRight=0x2000,

	fbtfDoubleCheckBox=0x4000,

	//set menu mode of this button to tool bar
	//regradless to command bar's flags
	fbtfForceToolBarMode=0x10000,
	//set menu mode of this button to normal mode
	//regradless to command bar's flags
	fbtfForceMenuMode=0x20000,
};

enum wxFakeButtonAdditionalFlags{
	//the button is in highlight state
	fbtaHighlight=0x1,
	//the button is pressed
	fbtaPressed=0x2,
	//popup menu
	fbtaPopupMenu=0x4,

	//tell the art provider to redraw the background
	fbtaRedrawBackground=0x100,
};

class wxFakeButtonData;
class wxFakeCommandBarIcon;
class wxFakeCommandBarArtBase;

class wxFakeCommandBarIconOwnerDraw{
public:
	virtual wxSize GetSize(const wxFakeCommandBarIcon& icon,wxFakeCommandBarArtBase* artprov) const=0;
	virtual void Draw(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,const wxFakeCommandBarIcon& icon,wxFakeCommandBarArtBase* artprov,int add_flags,const wxRect& rect)=0;
};

enum wxFakeCommandBarIconType{
	fcbiNone=0,
	fcbiBitmap=1,
	fcbiColor=2,
	fcbiOwnerDraw=99,
};

class wxFakeCommandBarIcon{
private:
	wxFakeCommandBarIconType m_type;
	wxBitmap m_bitmap;
	wxColour m_color;
	wxFakeCommandBarIconOwnerDraw *m_ownerdraw;
	void Assign(const wxFakeCommandBarIcon& obj){
		m_type=obj.m_type;
		m_bitmap=obj.m_bitmap;
		m_color=obj.m_color;
		m_ownerdraw=obj.m_ownerdraw;
	}
public:
	wxFakeCommandBarIcon(){
		m_type=fcbiNone;
		m_ownerdraw=NULL;
	}
	wxFakeCommandBarIcon(const wxFakeCommandBarIcon& obj){
		Assign(obj);
	}
	wxFakeCommandBarIcon(const wxBitmap& obj){
		m_type=obj.IsOk()?fcbiBitmap:fcbiNone;
		m_bitmap=obj;
		m_ownerdraw=NULL;
	}
	wxFakeCommandBarIcon(const wxColour& obj){
		m_type=fcbiColor;
		m_color=obj;
		m_ownerdraw=NULL;
	}
	wxFakeCommandBarIcon(wxFakeCommandBarIconOwnerDraw* obj){
		m_type=fcbiOwnerDraw;
		m_ownerdraw=obj;
	}

	wxFakeCommandBarIcon& operator=(const wxFakeCommandBarIcon& obj){
		Assign(obj);

		return *this;
	}
	wxFakeCommandBarIcon& operator=(const wxBitmap& obj){
		m_type=obj.IsOk()?fcbiBitmap:fcbiNone;
		m_bitmap=obj;

		return *this;
	}
	wxFakeCommandBarIcon& operator=(const wxColour& obj){
		m_type=fcbiColor;
		m_color=obj;

		return *this;
	}
	wxFakeCommandBarIcon& operator=(wxFakeCommandBarIconOwnerDraw* obj){
		m_type=fcbiOwnerDraw;
		m_ownerdraw=obj;

		return *this;
	}

	wxFakeCommandBarIconType GetType() const{
		return m_type;
	}
	const wxBitmap& GetBitmap() const{
		return m_bitmap;
	}
	const wxColour& GetColor() const{
		return m_color;
	}
	const wxFakeCommandBarIconOwnerDraw* GetOwnerDraw() const{
		return m_ownerdraw;
	}

	wxSize GetSize(wxFakeCommandBarArtBase* artprov) const;
	void Draw(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,wxFakeCommandBarArtBase* artprov,int add_flags,const wxRect& rect);
};

class wxFakeCommandBarDataBase;

class wxFakeButtonData{
public:
	friend class wxFakeCommandBarDataBase;
private:
	int m_id;
	wxFakeButtonType m_type;
	wxFakeButtonValue m_value;
	int m_flags;
	int m_groupindex;

	bool m_dirty;
	bool m_sizedirty;

	wxFakeCommandBarIcon m_bitmap;

	wxString m_caption;
	wxString m_tooltiptext;
	wxString m_shortcut;
	wxString m_desc;
	wxString m_key;

	wxAcceleratorEntry m_accel;

	wxFakeCommandBarDataBase *m_submenu;
private:
	wxString m_captiontext;
	int m_mnemonics;
	int m_mnemonics_pos;
private:
	void Assign(const wxFakeButtonData& obj){
		m_id=obj.m_id;
		m_type=obj.m_type;
		m_value=obj.m_value;
		m_flags=obj.m_flags;
		m_groupindex=obj.m_groupindex;

		m_dirty=true;
		m_sizedirty=true;

		m_bitmap=obj.m_bitmap;

		m_caption=obj.m_caption;
		m_tooltiptext=obj.m_tooltiptext;
		m_shortcut=obj.m_shortcut;
		m_desc=obj.m_desc;
		m_key=obj.m_key;

		m_accel=obj.m_accel;

		m_submenu=obj.m_submenu;

		m_captiontext=obj.m_captiontext;
		m_mnemonics=obj.m_mnemonics;
		m_mnemonics_pos=obj.m_mnemonics_pos;
	}
public:
	wxFakeButtonData(int id=0){
		m_id=id;
		m_type=fbttNormal;
		m_value=fbtvUnchecked;
		m_flags=0;
		m_dirty=true;
		m_sizedirty=true;
		m_groupindex=0;
		m_submenu=NULL;
		m_mnemonics=0;
		m_mnemonics_pos=-1;
	}
	wxFakeButtonData(const wxFakeButtonData& obj){
		Assign(obj);
	}
	~wxFakeButtonData(){
	}
	wxFakeButtonData& operator=(const wxFakeButtonData& obj){
		Assign(obj);
		return *this;
	}
public:
	int ID() const{
		return m_id;
	}

	wxFakeButtonType Type() const{
		return m_type;
	}
	wxFakeButtonData& Type(wxFakeButtonType type){
		if(m_type!=type){
			m_type=type;
			m_dirty=true;
			m_sizedirty=true;
		}
		return *this;
	}

	wxFakeButtonValue Value() const{
		return m_value;
	}
	wxFakeButtonData& Value(wxFakeButtonValue value){
		if(m_value!=value){
			m_value=value;
			m_dirty=true;
		}
		return *this;
	}

	int Flags() const{
		return m_flags;
	}
	wxFakeButtonData& Flags(int flags){
		if((m_flags^flags)&
			(fbtfHidden|fbtfShowDropdown|fbtfOwnerMeasure|fbtfOwnerDraw|fbtfOwnerDrawAfter
			|fbtfDefaultItem|fbtfHideCationInToolBar|fbtfStartNewRow|fbtfFullRow|fbtfColor
			|fbtfForceToolBarMode|fbtfForceMenuMode)
			){
			m_sizedirty=true;
		}
		if(m_flags!=flags){
			m_flags=flags;
			m_dirty=true;
		}
		return *this;
	}

	int GroupIndex() const{
		return m_groupindex;
	}
	wxFakeButtonData& GroupIndex(int groupindex){
		m_groupindex=groupindex;
		return *this;
	}

	bool Dirty() const{
		return m_dirty;
	}
	wxFakeButtonData& Dirty(bool b){
		m_dirty=b;
		return *this;
	}

	bool SizeDirty() const{
		return m_sizedirty;
	}
	wxFakeButtonData& SizeDirty(bool b){
		m_sizedirty=b;
		return *this;
	}

	void UnDirty(){
		m_dirty=false;
		m_sizedirty=false;
	}

	const wxFakeCommandBarIcon& Bitmap() const{
		return m_bitmap;
	}
	wxFakeButtonData& Bitmap(const wxFakeCommandBarIcon& obj){
		m_bitmap=obj;
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}
	wxFakeButtonData& Bitmap(const wxBitmap& obj){
		m_bitmap=obj;
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}
	wxFakeButtonData& Bitmap(const wxColour& obj){
		m_bitmap=obj;
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}
	wxFakeButtonData& Bitmap(wxFakeCommandBarIconOwnerDraw* obj){
		m_bitmap=obj;
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}

	const wxString& Caption() const{
		return m_caption;
	}
	wxFakeButtonData& Caption(const wxString& caption,bool setShortcut=true,bool setAccelerator=true);

	const wxString& CaptionText() const{
		return m_captiontext;
	}
	wxFakeButtonData& CaptionText(const wxString& captiontext){
		m_captiontext=captiontext;
		m_mnemonics=0;
		m_mnemonics_pos=-1;
		m_dirty=true;
		m_sizedirty=true;

		return *this;
	}

	const int Mnemonics() const{
		return m_mnemonics;
	}
	const int MnemonicsPos() const{
		return m_mnemonics_pos;
	}

	const wxString& ToolTipText() const{
		return m_tooltiptext;
	}
	wxFakeButtonData& ToolTipText(const wxString& tooltiptext){
		m_tooltiptext=tooltiptext;
		return *this;
	}

	const wxString& Shortcut() const{
		return m_shortcut;
	}
	wxFakeButtonData& Shortcut(const wxString& shortcut,bool setAccelerator=true);

	const wxString& Description() const{
		return m_desc;
	}
	wxFakeButtonData& Description(const wxString& desc){
		m_desc=desc;
		return *this;
	}

	const wxString& Key() const{
		return m_key;
	}
	wxFakeButtonData& Key(const wxString& key){
		m_key=key;
		return *this;
	}

	const wxAcceleratorEntry& Accelerator() const{
		return m_accel;
	}
	wxFakeButtonData& Accelerator(const wxAcceleratorEntry& accel){
		m_accel=accel;
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}

	const wxFakeCommandBarDataBase* SubMenu() const{
		return m_submenu;
	}
	wxFakeButtonData& SubMenu(wxFakeCommandBarDataBase* submenu){
		m_submenu=submenu;
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}
public:
	bool Visible() const{
		return (m_flags&fbtfHidden)==0;
	}
	wxFakeButtonData& Visible(bool b){
		m_flags=(m_flags&~fbtfHidden)|(b?0:fbtfHidden);
		m_dirty=true;
		m_sizedirty=true;
		return *this;
	}

	bool Enabled() const{
		return (m_flags&fbtfDisabled)==0;
	}
	wxFakeButtonData& Enabled(bool b){
		m_flags=(m_flags&~fbtfDisabled)|(b?0:fbtfDisabled);
		m_dirty=true;
		return *this;
	}
public:
	void OnClick(wxWindow* wnd,wxEvtHandler* handler,wxFakeCommandBarDataBase* bar,int btnindex);
};

struct wxFakeButtonState{
	wxFakeButtonData *btn;
	wxFakeCommandBarDataBase *bar;
	int index;
	wxFakeButtonSize size;
	wxRect rect;
};

class wxFakeCommandBarDataBase{
protected:
	int m_id;
	int m_flags;
	wxString m_key;
	wxString m_caption;

	wxFakeCommandBarDataBase* m_old_bar;
	int m_index_offset;

	wxVector<wxFakeButtonState> m_btn;
	wxRect m_rcMenu;
	wxRect m_rcGripper;
	wxSize m_szMenu;
	bool m_dirty;
protected:
	void Assign(const wxFakeCommandBarDataBase& obj){
		m_id=obj.m_id;
		m_flags=obj.m_flags;
		m_key=obj.m_key;
		m_caption=obj.m_caption;

		m_old_bar=obj.m_old_bar;
		m_index_offset=obj.m_index_offset;

		m_btn=obj.m_btn;
		m_rcMenu=obj.m_rcMenu;
		m_rcGripper=obj.m_rcGripper;
		m_szMenu=obj.m_szMenu;
		m_dirty=obj.m_dirty;
	}
public:
	wxFakeCommandBarDataBase(){
		m_id=0;
		m_flags=0;
		m_old_bar=NULL;
		m_index_offset=0;
		m_dirty=true;
	}
	virtual ~wxFakeCommandBarDataBase();
	virtual int ButtonCount() const=0;
	virtual wxFakeButtonData* GetButton(int idx)=0;

	int GetID() const{
		return m_id;
	}
	int GetFlags() const{
		return m_flags;
	}
	const wxString& GetKey() const{
		return m_key;
	}
	const wxString& GetCaption() const{
		return m_caption;
	}
	wxVector<wxFakeButtonState>& GetLayoutedButtons(){
		return m_btn;
	}
	wxFakeCommandBarDataBase* GetOldBar() const{
		return m_old_bar;
	}
	void SetOldBar(wxFakeCommandBarDataBase* bar){
		m_old_bar=bar;
	}
	int GetIndexOffset() const{
		return m_index_offset;
	}
	void SetIndexOffset(int i){
		m_index_offset=i;
	}
	bool GetDirty() const{
		return m_dirty;
	}
	void SetDirty(bool b=true){
		m_dirty=b;
	}
	const wxRect& MenuRect() const{
		return m_rcMenu;
	}
	bool HasGripper() const{
		return (m_flags & fcbfDragToMakeThisMenuFloat)!=0;
	}
	const wxRect& GripperRect() const{
		return m_rcGripper;
	}
	const wxSize& MenuSize() const{
		return m_szMenu;
	}

	bool UpdateDirty();
	virtual void LayoutMenuButtons(wxDC& dc,wxWindow* wnd,wxFakeCommandBarArtBase* artprov);
};

class wxFakeCommandBarData:public wxFakeCommandBarDataBase{
protected:
	wxVector<wxFakeButtonData> m_buttons;
protected:
	void Assign(const wxFakeCommandBarData& obj){
		wxFakeCommandBarDataBase::Assign(obj);
		m_buttons=obj.m_buttons;
	}
public:
	int ButtonCount() const;
	wxFakeButtonData* GetButton(int idx);
public:
	wxFakeCommandBarData(int id=0){
		m_id=id;
		m_flags=0;
	}
	wxFakeCommandBarData(const wxFakeCommandBarData& obj){
		Assign(obj);
	}
	virtual ~wxFakeCommandBarData();
	wxFakeCommandBarData& operator=(const wxFakeCommandBarData& obj){
		Assign(obj);
		return *this;
	}
public:
	const wxVector<wxFakeButtonData>& Buttons() const{
		return m_buttons;
	}
	wxVector<wxFakeButtonData>& Buttons(){
		return m_buttons;
	}

	wxFakeCommandBarData& Flags(int flags){
		m_flags=flags;
		return *this;
	}

	wxFakeCommandBarData& Key(const wxString& key){
		m_key=key;
		return *this;
	}

	wxFakeCommandBarData& Caption(const wxString& s){
		m_caption=s;
		return *this;
	}
};

class wxFakeCommandBarRefData:public wxFakeCommandBarDataBase{
protected:
	wxVector<wxFakeButtonData*> m_buttons;
protected:
	void Assign(const wxFakeCommandBarRefData& obj){
		wxFakeCommandBarDataBase::Assign(obj);
		m_buttons=obj.m_buttons;
	}
public:
	int ButtonCount() const;
	wxFakeButtonData* GetButton(int idx);
public:
	wxFakeCommandBarRefData(int id=0){
		m_id=id;
		m_flags=0;
	}
	wxFakeCommandBarRefData(const wxFakeCommandBarRefData& obj){
		Assign(obj);
	}
	virtual ~wxFakeCommandBarRefData();
	wxFakeCommandBarRefData& operator=(const wxFakeCommandBarRefData& obj){
		Assign(obj);
		return *this;
	}
public:
	const wxVector<wxFakeButtonData*>& Buttons() const{
		return m_buttons;
	}
	wxVector<wxFakeButtonData*>& Buttons(){
		return m_buttons;
	}

	wxFakeCommandBarRefData& Flags(int flags){
		m_flags=flags;
		return *this;
	}

	wxFakeCommandBarRefData& Key(const wxString& key){
		m_key=key;
		return *this;
	}

	wxFakeCommandBarRefData& Caption(const wxString& s){
		m_caption=s;
		return *this;
	}
};

class wxFakeCommandBarButtonEvent:public wxCommandEvent{
protected:
	wxFakeButtonData* m_button;
	wxFakeCommandBarDataBase* m_bar;
	int m_buttonindex;
public:
	wxFakeCommandBarButtonEvent(wxEventType commandEventType=wxEVT_NULL, int id=0):wxCommandEvent(commandEventType,id){
		m_button=NULL;
		m_bar=NULL;
		m_buttonindex=-1;
	}
	wxFakeCommandBarButtonEvent(const wxFakeCommandBarButtonEvent& evt):wxCommandEvent(evt){
		m_button=evt.m_button;
		m_bar=evt.m_bar;
		m_buttonindex=evt.m_buttonindex;
	}
	wxFakeButtonData* GetButton() const{
		return m_button;
	}
	void SetButton(wxFakeButtonData* btn){
		m_button=btn;
	}
	wxFakeCommandBarDataBase* GetBar() const{
		return m_bar;
	}
	void SetBar(wxFakeCommandBarDataBase* bar){
		m_bar=bar;
	}
	int GetButtonIndex() const{
		return m_buttonindex;
	}
	void SetButtonIndex(int i){
		m_buttonindex=i;
	}
	virtual wxEvent* Clone() const;
private:
	DECLARE_CLASS(wxFakeCommandBarButtonEvent)
};

#endif

