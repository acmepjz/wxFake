#include <wx/defs.h>
#include "wxFake/wxFakeCommandBarData.hpp"
#include "wxFake/wxFakeCommandBarArt.hpp"
#include "wxFake/wxFakeCommandBar.hpp"
#include "wxFake/wxFakeMenu.hpp"
#include "wxFake/wxFakeMiniFrame.hpp"
#include <stdio.h>

#include "wxFake/wxFakeCommandBarArt.inl"

wxFakeCommandBarArtBase* wxFakeCommandBarDefaultArtProvider = NULL;

wxFakeCommandBarArtBase::~wxFakeCommandBarArtBase(){
}

wxFakeOffice2003CommandBarArt::wxFakeOffice2003CommandBarArt(){
	m_bg=wxColour(0xF6F6F6UL);
	m_border=wxColour(0x800000UL);
	m_title1=wxColour(0xD68759UL);
	m_title2=wxColour(0x9A400CUL);
	m_bar1=wxColour(0xFAE2D0UL);
	m_bar2=wxColour(0xE2A981UL);
	m_hl1=wxColour(0xD0FCFDUL);
	m_hl2=wxColour(0x9DDFFDUL);
	m_checked1=wxColour(0x7DDDFAUL);
	m_checked2=wxColour(0x4EBCF5UL);
	m_pressed1=m_hlchecked1=wxColour(0x5586F8UL);
	m_pressed2=m_hlchecked2=wxColour(0x0A37D2UL);
	m_sprt1=wxColour(0xCB8C6AUL);
	m_sprt2=wxColour(0xFFFFFFUL);
	m_text=wxColour(0x000000UL);
	m_texthl=wxColour(0x000000UL);
	m_chevron1=wxColour(0xF1A675UL);
	m_chevron2=wxColour(0x913500UL);

	m_gripper=wxBitmap(gripper_xpm);
	m_checksign=wxBitmap(checksign_xpm);
	m_optionsign=wxBitmap(optionsign_xpm);
	m_toolbar_up=wxBitmap(toolbar_up_xpm);
	m_toolbar_down=wxBitmap(toolbar_down_xpm);
	m_toolbar_left=wxBitmap(toolbar_left_xpm);
	m_toolbar_right=wxBitmap(toolbar_right_xpm);
	m_menu_left=wxBitmap(menu_left_xpm);
	m_menu_right=wxBitmap(menu_right_xpm);
	m_overflow_right=wxBitmap(overflow_right_xpm);
	m_overflow_down=wxBitmap(overflow_down_xpm);
	m_chevron_right=wxBitmap(chevron_right_xpm);
	m_chevron_down=wxBitmap(chevron_down_xpm);

	m_pane_outerborder=wxColour(0xC9662AUL);
	m_pane_innerborder=wxColour(0xF9D4B9UL);
	m_pane_title1=wxColour(0xFCE8D8UL);
	m_pane_title2=wxColour(0xE0A47BUL);
	m_pane_bg1=wxColour(0xFEECDDUL);
	m_pane_bg2=wxColour(0xCA7D4EUL);

	m_pane_close=wxBitmap(pane_close_xpm);
	m_pane_dropdown=wxBitmap(pane_dropdown_xpm);
	m_pane_minimize=wxBitmap(pane_minimize_xpm);
	m_pane_restore=wxBitmap(pane_restore_xpm);
	m_pane_moveleft=wxBitmap(pane_moveleft_xpm);
	m_pane_moveright=wxBitmap(pane_moveright_xpm);
	m_pane_pin=wxBitmap(pane_pin_xpm);
	m_pane_unpin=wxBitmap(pane_unpin_xpm);
	m_pane_topleftcorner=wxBitmap(pane_topleftcorner_xpm);
	m_pane_toprightcorner=wxBitmap(pane_toprightcorner_xpm);

	UpdateDisabledTextColor(wxColour(0xCB8C6AUL));
}

void wxFakeOffice2003CommandBarArt::UpdateDisabledTextColor(wxColour clr){
	m_textdis=clr;

	char s[32];
	sprintf(s,".\tc #%02X%02X%02X",clr.Red(),clr.Green(),clr.Blue());

	const char *s_old=checksign_xpm[2];

	checksign_xpm[2]=s;
	optionsign_xpm[2]=s;
	toolbar_up_xpm[2]=s;
	toolbar_down_xpm[2]=s;
	toolbar_left_xpm[2]=s;
	toolbar_right_xpm[2]=s;
	menu_left_xpm[2]=s;
	menu_right_xpm[2]=s;

	m_checksign_disabled=wxBitmap(checksign_xpm);
	m_optionsign_disabled=wxBitmap(optionsign_xpm);
	m_toolbar_up_disabled=wxBitmap(toolbar_up_xpm);
	m_toolbar_down_disabled=wxBitmap(toolbar_down_xpm);
	m_toolbar_left_disabled=wxBitmap(toolbar_left_xpm);
	m_toolbar_right_disabled=wxBitmap(toolbar_right_xpm);
	m_menu_left_disabled=wxBitmap(menu_left_xpm);
	m_menu_right_disabled=wxBitmap(menu_right_xpm);

	checksign_xpm[2]=s_old;
	optionsign_xpm[2]=s_old;
	toolbar_up_xpm[2]=s_old;
	toolbar_down_xpm[2]=s_old;
	toolbar_left_xpm[2]=s_old;
	toolbar_right_xpm[2]=s_old;
	menu_left_xpm[2]=s_old;
	menu_right_xpm[2]=s_old;
}

wxSize wxFakeOffice2003CommandBarArt::GetColorSize(){
	return wxSize(16,16);
}

void wxFakeOffice2003CommandBarArt::GetButtonPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	leftMargin=-1;
	topMargin=-1;
	rightMargin=-1;
	bottomMargin=-1;
}

void wxFakeOffice2003CommandBarArt::GetMenuPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	leftMargin=1;
	topMargin=-1;
	rightMargin=1;
	bottomMargin=-1;
}

void wxFakeOffice2003CommandBarArt::GetChevronMenuPopupMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	leftMargin=0;
	topMargin=0;
	rightMargin=0;
	bottomMargin=0;
}

void wxFakeOffice2003CommandBarArt::GetBarClientSizeMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	leftMargin=0;
	topMargin=0;
	rightMargin=0;
	bottomMargin=0;
}

void wxFakeOffice2003CommandBarArt::GetMenuClientSizeMargin(int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	leftMargin=2;
	topMargin=2;
	rightMargin=2;
	bottomMargin=2;
}

wxSize wxFakeOffice2003CommandBarArt::GetEmptyMenuSize(){
	return wxSize(48,16);
}

void wxFakeOffice2003CommandBarArt::GetMenuGripperSize(int& height,int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	height=7;
	leftMargin=0;
	topMargin=0;
	rightMargin=0;
	bottomMargin=1;
}

wxFakeButtonSize wxFakeOffice2003CommandBarArt::MeasureButton(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,int bar_flags){
	wxFakeButtonSize sz={};

	wxFakeButtonType nType=btn.Type();
	int nFlags=btn.Flags();
	bool isToolBarMode=((bar_flags & fcbfToolBarMode)!=0)?((nFlags & fbtfForceMenuMode)==0):((nFlags & fbtfForceToolBarMode)!=0);

	switch(nType){
	case fbttSeparator:
		sz.btnWidth=4;
		sz.btnHeight=-1;

		if(isToolBarMode && (nFlags & fbtfStartNewRow)==0){
			//separator in tool bar mode
			sz.sizeMode=fbsmToolBar;
			sz.width=3;
			sz.height=-1;
		}else{
			//separator in menu mode
			sz.width=-1;
			sz.height=3;
		}

		break;
	case fbttColumnSeparator:
		sz.width=3;
		sz.height=-1;

		break;
	default:
		//---toolbar item size---
		//horizontal margin size
		sz.btnWidth=2;
		sz.btnHeight=16;

		//check icon size
		wxSize iconSize=btn.Bitmap().GetSize(this);
		if(iconSize.x>0){
			sz.btnWidth+=iconSize.x+2;
			if(iconSize.y>sz.btnHeight) sz.btnHeight=iconSize.y;
		}

		//check label size
		wxSize textSize=dc.GetTextExtent(btn.CaptionText());
		if((nFlags & fbtfHideCationInToolBar)==0){
			if(textSize.x>0){
				sz.btnWidth+=textSize.x+2;
				if(textSize.y-4>sz.btnHeight) sz.btnHeight=textSize.y-4;
			}
		}

		//check dropdown sign
		if(nType==fbttSplit){
			sz.btnWidth+=9;
		}else if(nFlags & fbtfShowDropdown){
			sz.btnWidth+=7;
		}

		//vertical margin size
		sz.btnHeight+=4;

		//---menu item size---
		if(isToolBarMode){
			sz.sizeMode=(nFlags & fbtfStartNewRow)?fbsmToolBarStartNewRow:fbsmToolBar;
			sz.width=(nFlags & fbtfFullRow)?-1:sz.btnWidth;
			sz.height=sz.btnHeight;
		}else{
			sz.width0=20;
			sz.width=8;
			sz.height=16;

			//check icon size
			if(iconSize.x+4>sz.width0) sz.width0=iconSize.x+4;
			if(iconSize.y>sz.height) sz.height=iconSize.y;

			if(nFlags & fbtfDoubleCheckBox) sz.width0+=18;

			//check caption size
			if(textSize.x>0){
				sz.width+=textSize.x+4; //?
				if(textSize.y-4>sz.height) sz.height=textSize.y-4;
			}

			//check shortcut text size
			wxSize textSize2=dc.GetTextExtent(btn.Shortcut());
			if(textSize2.x>0){
				sz.width2=textSize2.x+4; //?
				if(textSize2.y-4>sz.height) sz.height=textSize2.y-4;
			}

			//check dropdown sign
			if(nType==fbttSplit || (nFlags & fbtfShowDropdown)!=0 || btn.SubMenu()!=NULL){
				sz.width2+=8; //12; //?
			}

			//vertical margin size
			sz.height+=4;
		}

		break;
	}

	return sz;
}

wxFakeButtonHitTestState wxFakeOffice2003CommandBarArt::HitTest(const wxFakeButtonData& btn,int bar_flags,bool is_bar,int dir,const wxPoint& pt,const wxFakeButtonSize& btn_size,const wxRect& rect){
	wxFakeButtonType nType=btn.Type();

	if(is_bar && nType==fbttSplit){
		if(dir==fcbdLeft || dir==fcbdRight){
			return (pt.y>=rect.y+rect.height-9)?fbhtSplit:fbhtNormal;
		}else{
			return (pt.x>=rect.x+rect.width-9)?fbhtSplit:fbhtNormal;
		}
	}else{
		return fbhtNormal;
	}
}

/*void wxFakeOffice2003CommandBarArt::DrawBorder(wxDC& dc,wxWindow* wnd,int flags,const wxRect& rect){
}*/

void wxFakeOffice2003CommandBarArt::DrawColor(wxDC& dc,wxWindow* wnd,wxColour clr,int flags,int add_flags,const wxRect& rect){
	dc.SetBrush(wxBrush(clr));
	dc.SetPen(wxPen(m_border));
	dc.DrawRectangle(wxRect(rect.x+2,rect.y+2,rect.width-4,rect.height-4));
}

void wxFakeOffice2003CommandBarArt::DrawEmptyMenuItem(wxDC& dc,wxWindow* wnd,const wxRect& rect){
	dc.SetBackgroundMode(wxTRANSPARENT);
	dc.SetTextForeground(m_textdis);
	MyDrawLabel(dc,"(None)",rect,0,wxALIGN_CENTER_VERTICAL);
}

void wxFakeOffice2003CommandBarArt::DrawOneGripper(wxDC& dc,wxWindow* wnd,int x,int y){
	//stupid code
	dc.DrawBitmap(m_gripper,x,y,true);
}

void wxFakeOffice2003CommandBarArt::DrawMenuGripper(wxDC& dc,wxWindow* wnd,int add_flags,const wxRect& rect){
	//draw background and border
	if(add_flags & fbtaHighlight){
		dc.GradientFillLinear(rect,m_hl1,m_hl2,wxDOWN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxPen(m_border));
		dc.DrawRectangle(rect);
	}else{
		dc.GradientFillLinear(rect,m_bar1,m_bar2,wxDOWN);
	}

	//draw gripper
	int x,y,r;
	r=((rect.width/2) & 0xFFFFFFFC)-1;
	if(r<15) r=15;
	x=rect.x+(rect.width-r)/2-1;
	y=rect.y+2;

	do{
		DrawOneGripper(dc,wnd,x,y);
		x+=4;
		r-=4;
	}while(r>0);
}

void wxFakeOffice2003CommandBarArt::DrawMenuBorderAndBackground(wxDC& dc,wxWindow* wnd,const wxFakeMenuPopupConnection& conn,const wxRect& rect){
	dc.SetBrush(wxBrush(m_bg));
	dc.SetPen(wxPen(m_border));
	dc.DrawRectangle(rect);

	dc.SetPen(*wxTRANSPARENT_PEN);
	switch(conn.dir){
	case wxTOP:
		dc.DrawRectangle(conn.start,rect.y,conn.end-conn.start,1);
		break;
	case wxBOTTOM:
		dc.DrawRectangle(conn.start,rect.y+rect.height-1,conn.end-conn.start,1);
		break;
	case wxLEFT:
		dc.DrawRectangle(rect.x,conn.start,1,conn.end-conn.start);
		break;
	case wxRIGHT:
		dc.DrawRectangle(rect.x+rect.width-1,conn.start,1,conn.end-conn.start);
		break;
	}
}

void wxFakeOffice2003CommandBarArt::DrawMenuEmptyPart(wxDC& dc,wxWindow* wnd,int bar_flags,const wxRect& rect){
	if((bar_flags & fcbfToolBarMode)==0){
		dc.GradientFillLinear(wxRect(rect.x,rect.y,20,rect.height),m_bar1,m_bar2,wxRIGHT);
	}
}

void wxFakeOffice2003CommandBarArt::DrawToolbarBackground(wxDC& dc,wxWindow* wnd,int bar_flags,int dir,const wxRect& rect,const wxRect& dock_area){
	if(bar_flags & fcbfMainMenu){
		if(dock_area.width>0){
			int x1=rect.x-dock_area.x;
			if(x1<0) x1=0;
			else if(x1>dock_area.width) x1=dock_area.width;

			int x2=rect.x+rect.width-dock_area.x;
			if(x2<0) x2=0;
			else if(x2>dock_area.width) x2=dock_area.width;

			wxColour clr1(
				int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x1/dock_area.width,
				int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x1/dock_area.width,
				int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x1/dock_area.width
				);

			wxColour clr2(
				int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x2/dock_area.width,
				int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x2/dock_area.width,
				int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x2/dock_area.width
				);

			dc.GradientFillLinear(rect,clr1,clr2,wxRIGHT);
		}else if(dir==fcbdLeft || dir==fcbdRight){
			dc.GradientFillLinear(rect,m_bar2,m_bar1,wxDOWN);
		}else{
			dc.GradientFillLinear(rect,m_bar2,m_bar1,wxRIGHT);
		}
	}else{
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.GradientFillLinear(rect,m_bar1,m_bar2,wxRIGHT);
		}else{
			dc.GradientFillLinear(rect,m_bar1,m_bar2,wxDOWN);
		}
	}
}

void wxFakeOffice2003CommandBarArt::DrawButton(wxDC& dc,wxWindow* wnd,const wxFakeButtonData& btn,int bar_flags,bool is_bar,int dir,int add_flags,const wxFakeButtonSize& btn_size,const wxRect& rect,const wxSize& client_size,const wxRect& dock_area){
	wxFakeButtonType nType=btn.Type();
	wxFakeButtonValue nValue=btn.Value();
	int nFlags=btn.Flags();
	bool isToolBarMode=((bar_flags & fcbfToolBarMode)!=0)?((nFlags & fbtfForceMenuMode)==0):((nFlags & fbtfForceToolBarMode)!=0);

	if(is_bar || isToolBarMode){
		//---tool bar mode---

		//background and border
		if((is_bar && nValue==fbtvUnchecked && (add_flags & (fbtaHighlight | fbtaPressed | fbtaRedrawBackground))==fbtaRedrawBackground)
			|| (add_flags & fbtaPopupMenu)!=0)
		{
			//new: calculate background color according to the client area
			wxColour clr1,clr2;
			wxDirection nDirection;

			if(add_flags & fbtaPopupMenu){
				clr1=m_bar1;
				clr2=m_bar2;
				nDirection=(dir==fcbdLeft || dir==fcbdRight)?wxRIGHT:wxDOWN;
			}else if(bar_flags & fcbfMainMenu){
				if(dock_area.width>0 || client_size.x>0){
					int x1,x2;
					wxRect r=(dock_area.width>0)?dock_area:wxSize(client_size);

					x1=rect.x-r.x;
					x2=x1+rect.width;

					if(x1<0) x1=0;
					else if(x1>r.width) x1=r.width;
					if(x2<0) x2=0;
					else if(x2>r.width) x2=r.width;

					clr1=wxColour(
						int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x1/r.width,
						int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x1/r.width,
						int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x1/r.width
						);

					clr2=wxColour(
						int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x2/r.width,
						int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x2/r.width,
						int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x2/r.width
						);
				}else{
					//error
					clr1=m_bar2;
					clr2=m_bar1;
				}
				nDirection=wxRIGHT;
			}else{
				int w=(dir==fcbdLeft || dir==fcbdRight)?client_size.x:client_size.y;
				if(w>0){
					int x1=(dir==fcbdLeft || dir==fcbdRight)?rect.x:rect.y;
					int x2=x1+((dir==fcbdLeft || dir==fcbdRight)?rect.width:rect.height);

					if(x1<0) x1=0;
					else if(x1>w) x1=w;
					if(x2<0) x2=0;
					else if(x2>w) x2=w;

					clr1=wxColour(
						int(m_bar1.Red())+(int(m_bar2.Red())-int(m_bar1.Red()))*x1/w,
						int(m_bar1.Green())+(int(m_bar2.Green())-int(m_bar1.Green()))*x1/w,
						int(m_bar1.Blue())+(int(m_bar2.Blue())-int(m_bar1.Blue()))*x1/w
						);

					clr2=wxColour(
						int(m_bar1.Red())+(int(m_bar2.Red())-int(m_bar1.Red()))*x2/w,
						int(m_bar1.Green())+(int(m_bar2.Green())-int(m_bar1.Green()))*x2/w,
						int(m_bar1.Blue())+(int(m_bar2.Blue())-int(m_bar1.Blue()))*x2/w
						);
				}else{
					clr1=m_bar1;
					clr2=m_bar2;
				}
				nDirection=(dir==fcbdLeft || dir==fcbdRight)?wxRIGHT:wxDOWN;
			}

			dc.GradientFillLinear(rect,clr1,clr2,nDirection);
			if(add_flags & fbtaPopupMenu){
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(rect);
			}
		}else if(nType!=fbttSeparator && nType!=fbttColumnSeparator && (nValue!=fbtvUnchecked
			|| ((nFlags & fbtfDisabled)==0 && (add_flags & (fbtaHighlight | fbtaPressed))!=0)))
		{
			if((nFlags & fbtfDisabled)==0 && (add_flags & fbtaPressed)!=0){
				dc.GradientFillLinear(rect,m_pressed1,m_pressed2,(dir==fcbdLeft || dir==fcbdRight)?wxRIGHT:wxDOWN);
			}else if((nFlags & fbtfDisabled)==0 && (add_flags & fbtaHighlight)!=0){
				if(nValue!=fbtvUnchecked){
					dc.GradientFillLinear(rect,m_hlchecked1,m_hlchecked2,(dir==fcbdLeft || dir==fcbdRight)?wxRIGHT:wxDOWN);
				}else{
					dc.GradientFillLinear(rect,m_hl1,m_hl2,(dir==fcbdLeft || dir==fcbdRight)?wxRIGHT:wxDOWN);
				}
			}else{
				dc.GradientFillLinear(rect,m_checked1,m_checked2,(dir==fcbdLeft || dir==fcbdRight)?wxRIGHT:wxDOWN);
			}

			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen(wxPen(m_border));
			dc.DrawRectangle(rect);

			if(nType==fbttSplit){
				dc.SetBrush(wxBrush(m_border));
				dc.SetPen(*wxTRANSPARENT_PEN);
				if(dir==fcbdLeft || dir==fcbdRight){
					dc.DrawRectangle(rect.x,rect.y+rect.height-9,rect.width,1);
				}else{
					dc.DrawRectangle(rect.x+rect.width-9,rect.y,1,rect.height);
				}
			}
		}else if(add_flags & fbtaRedrawBackground){
			dc.SetBrush(wxBrush(m_bg));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(rect);
		}

		switch(nType){
		case fbttSeparator:
			dc.SetBrush(wxBrush(m_sprt1));
			dc.SetPen(*wxTRANSPARENT_PEN);
			if(is_bar && (bar_flags & fcbfMultiLineToolBar)==0){
				if(dir==fcbdLeft || dir==fcbdRight){
					dc.DrawRectangle(rect.x+1,rect.y+rect.height/2-1,rect.width-3,1);
				}else{
					dc.DrawRectangle(rect.x+rect.width/2-1,rect.y+1,1,rect.height-3);
				}
				dc.SetBrush(wxBrush(m_sprt2));
				if(dir==fcbdLeft || dir==fcbdRight){
					dc.DrawRectangle(rect.x+2,rect.y+rect.height/2,rect.width-3,1);
				}else{
					dc.DrawRectangle(rect.x+rect.width/2,rect.y+2,1,rect.height-3);
				}
			}else if(((nFlags & fbtfStartNewRow)!=0) ^
				((bar_flags & fcbfMultiLineToolBar)!=0 && (dir==fcbdLeft || dir==fcbdRight)))
			{
				dc.DrawRectangle(rect.x+1,rect.y+rect.height/2,rect.width-2,1);
			}else{
				dc.DrawRectangle(rect.x+rect.width/2,rect.y+1,1,rect.height-2);
			}
		case fbttColumnSeparator:
			if(bar_flags & fcbfMultiLineToolBar){
				if(dir==fcbdLeft || dir==fcbdRight){
					dc.DrawRectangle(rect.x+1,rect.y+rect.height/2,rect.width-2,1);
				}else{
					dc.DrawRectangle(rect.x+rect.width/2,rect.y+1,1,rect.height-2);
				}
			}
			break;
		default:
			wxSize sz=btn.Bitmap().GetSize(this);

			//bitmap
			if(sz.x>0 && sz.y>0){
				if(dir==fcbdLeft || dir==fcbdRight){
					((wxFakeCommandBarIcon&)btn.Bitmap()).Draw(dc,wnd,btn,this,add_flags,wxRect(rect.x+(rect.width-sz.x)/2,rect.y+2,sz.x,sz.y));
					sz.y+=2;
				}else{
					((wxFakeCommandBarIcon&)btn.Bitmap()).Draw(dc,wnd,btn,this,add_flags,wxRect(rect.x+2,rect.y+(rect.height-sz.y)/2,sz.x,sz.y));
					sz.x+=2;
				}
			}

			//caption
			if((nFlags & fbtfHideCationInToolBar)==0 ||
				(!isToolBarMode && (bar_flags & fcbfMultiLineToolBar)!=0))
			{
				dc.SetTextForeground(((nFlags & (fbtfDisabled | fbtfNoDisabledColor))==fbtfDisabled)?m_textdis:
					(((add_flags & fbtaHighlight)!=0 && (nFlags & fbtfDisabled)==0)?m_texthl:m_text));
				if(nFlags & fbtfAlignCenter){
					MyDrawLabel(dc,btn.CaptionText(),rect,
						dir==fcbdBottom?fcbdTop:dir,
						wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,btn.MnemonicsPos());
				}else{
					if(dir==fcbdLeft || dir==fcbdRight){
						sz.y+=2;
					}else{
						sz.x+=2;
					}

					int ww=0;
					if(nType==fbttSplit){
						ww=9;
					}else if(nFlags & fbtfShowDropdown){
						ww=7;
					}

					MyDrawLabel(dc,btn.CaptionText(),
						(dir==fcbdLeft || dir==fcbdRight)
						?wxRect(rect.x,rect.y+sz.y,rect.width,rect.height-sz.y-ww)
						:wxRect(rect.x+sz.x,rect.y,rect.width-sz.x-ww,rect.height),
						dir==fcbdBottom?fcbdTop:dir,
						wxALIGN_CENTER_VERTICAL|((nFlags & fbtfAlignRight)!=0?wxALIGN_RIGHT:0),
						(nFlags & fbtfHideCationInToolBar)==0?btn.MnemonicsPos():-1);

					//do some weird thing: draw shortcut key
					if(!isToolBarMode && (bar_flags & fcbfMultiLineToolBar)!=0
						&& (nFlags & fbtfAlignRight)==0)
					{
						MyDrawLabel(dc,btn.Shortcut(),
							(dir==fcbdRight)
							?wxRect(rect.x,rect.y+rect.height-btn_size.width2,rect.width,btn_size.width2)
							:(dir==fcbdLeft)
							?wxRect(rect.x,rect.y+sz.y,rect.width,btn_size.width2)
							:wxRect(rect.x+rect.width-btn_size.width2,rect.y,btn_size.width2,rect.height),
							dir==fcbdBottom?fcbdTop:dir,
							wxALIGN_CENTER_VERTICAL);
					}
				}
			}

			//drop-down sign
			if(nType==fbttSplit || (nFlags & fbtfShowDropdown)!=0){
				if(dir==fcbdLeft || dir==fcbdRight){
					sz=m_toolbar_right.GetSize();
					dc.DrawBitmap((nFlags & fbtfDisabled)!=0
						?(dir==fcbdRight?m_toolbar_left_disabled:m_toolbar_right_disabled)
						:(dir==fcbdRight?m_toolbar_left:m_toolbar_right),
						rect.x+(rect.width-sz.x+1)/2,rect.y+rect.height-sz.y-2,true);
				}else{
					sz=m_toolbar_down.GetSize();
					dc.DrawBitmap((nFlags & fbtfDisabled)!=0
						?(dir==fcbdBottom?m_toolbar_up_disabled:m_toolbar_down_disabled)
						:(dir==fcbdBottom?m_toolbar_up:m_toolbar_down),
						rect.x+rect.width-sz.x-2,rect.y+(rect.height-sz.y+1)/2,true);
				}
			}

			break;
		}
	}else{
		//---menu mode---
		switch(nType){
		case fbttSeparator:
			if(add_flags & fbtaRedrawBackground){
				dc.SetBrush(wxBrush(m_bg));
				dc.SetPen(*wxTRANSPARENT_PEN);
				dc.DrawRectangle(rect);
			}
			dc.GradientFillLinear(wxRect(rect.x,rect.y,btn_size.width0,rect.height),m_bar1,m_bar2,wxRIGHT);
			dc.SetBrush(wxBrush(m_sprt1));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(rect.x+btn_size.width0+2,rect.y+rect.height/2,rect.width-btn_size.width0-3,1);
			break;
		case fbttColumnSeparator:
			if(add_flags & fbtaRedrawBackground){
				dc.SetBrush(wxBrush(m_bg));
				dc.SetPen(*wxTRANSPARENT_PEN);
				dc.DrawRectangle(rect);
			}
			dc.SetBrush(wxBrush(m_sprt1));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(rect.x+rect.width/2,rect.y+1,1,rect.height-2);
			break;
		default:
			//side bar and something else
			if((add_flags & fbtaHighlight)!=0 && (nFlags & fbtfDisabled)==0){
				dc.GradientFillLinear(rect,m_hl1,m_hl2,wxDOWN);
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(rect);
				if(nType==fbttSplit){
					dc.DrawRectangle(rect.x+rect.width-12,rect.y+1,1,rect.height-2);
				}
			}else{
				if(add_flags & fbtaRedrawBackground){
					dc.SetBrush(wxBrush(m_bg));
					dc.SetPen(*wxTRANSPARENT_PEN);
					dc.DrawRectangle(rect);
				}
				dc.GradientFillLinear(wxRect(rect.x,rect.y,btn_size.width0,rect.height),m_bar1,m_bar2,wxRIGHT);
				if(nType==fbttSplit){
					dc.SetBrush(wxBrush(m_sprt1));
					dc.SetPen(*wxTRANSPARENT_PEN);
					dc.DrawRectangle(rect.x+rect.width-12,rect.y+1,1,rect.height-2);
				}
			}

			//checked border
			wxSize sz0=btn.Bitmap().GetSize(this),sz=sz0;
			if(sz.x<16) sz.x=16;
			if(sz.y<16) sz.y=16;
			wxRect r(rect.x+(btn_size.width0-sz.x)/2-1,rect.y+1,sz.x+2,rect.height-2);
			wxRect r1;

			if(nFlags & fbtfDoubleCheckBox){
				r1=wxRect(r.x-9,rect.y+rect.height/2-9,18,18);
				r.x+=9;
			}else{
				r1=r;
			}

			if(nValue!=fbtvUnchecked){
				if((add_flags & fbtaHighlight)!=0 && (nFlags & fbtfDisabled)==0){
					dc.GradientFillLinear(r1,m_hlchecked1,m_hlchecked2,wxDOWN);
				}else{
					dc.GradientFillLinear(r1,m_checked1,m_checked2,wxDOWN);
				}
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(r1);
			}

			//checked sign and bitmap
			if(sz0.x>0 && sz0.y>0){
				((wxFakeCommandBarIcon&)btn.Bitmap()).Draw(dc,wnd,btn,this,add_flags,wxRect(r.x+(r.width-sz0.x)/2,r.y+(r.height-sz0.y)/2,sz0.x,sz0.y));
			}
			if((sz0.x<=0 || sz0.y<=0 || (nFlags & fbtfDoubleCheckBox)!=0) && nValue!=fbtvUnchecked){
				if(nType==fbttOption || nType==fbttOptionNullable || nValue==fbtvUndetermined){
					sz0=m_optionsign.GetSize();
					dc.DrawBitmap((nFlags & fbtfDisabled)!=0?m_optionsign_disabled:m_optionsign,r1.x+(r1.width-sz0.x)/2,r1.y+(r1.height-sz0.y)/2,true);
				}else{
					sz0=m_checksign.GetSize();
					dc.DrawBitmap((nFlags & fbtfDisabled)!=0?m_checksign_disabled:m_checksign,r1.x+(r1.width-sz0.x)/2,r1.y+(r1.height-sz0.y)/2,true);
				}
			}

			//caption
			dc.SetTextForeground(((nFlags & (fbtfDisabled | fbtfNoDisabledColor))==fbtfDisabled)?m_textdis:
				(((add_flags & fbtaHighlight)!=0 && (nFlags & fbtfDisabled)==0)?m_texthl:m_text));
			MyDrawLabel(dc,btn.CaptionText(),
				wxRect(rect.x+btn_size.width0+4,rect.y,btn_size.width-btn_size.width0-4,rect.height),
				0,wxALIGN_CENTER_VERTICAL,btn.MnemonicsPos());
			MyDrawLabel(dc,btn.Shortcut(),
				wxRect(rect.x+btn_size.width0+btn_size.width,rect.y,btn_size.width2,rect.height),
				0,wxALIGN_CENTER_VERTICAL);

			//drop-down sign
			if(nType==fbttSplit || (nFlags & fbtfShowDropdown)!=0 || btn.SubMenu()!=NULL){
				sz0=m_menu_right.GetSize();
				dc.DrawBitmap((nFlags & fbtfDisabled)!=0?m_menu_right_disabled:m_menu_right,rect.x+rect.width-8,rect.y+(rect.height-sz0.y+1)/2,true);
			}
		}
	}
}

int wxFakeOffice2003CommandBarArt::GetToolbarGripperWidth() const{
	return 8;
}

int wxFakeOffice2003CommandBarArt::GetToolbarChevronWidth() const{
	return 12;
}

void wxFakeOffice2003CommandBarArt::DrawToolbarGripper(wxDC& dc,wxWindow* wnd,int bar_flags,int dir,const wxRect& rect,const wxRect& dock_area){
	//draw background
	if((bar_flags & fcbfMainMenu)==0){
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.GradientFillLinear(rect,m_bar1,m_bar2,wxRIGHT);
			if(dock_area.width>0){
				int x=rect.x-dock_area.x;
				if(x<0) x=0;
				else if(x>dock_area.width) x=dock_area.width;

				wxColour clr01(
					int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x/dock_area.width,
					int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x/dock_area.width,
					int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x/dock_area.width
					);

				x=rect.x+rect.width-1-dock_area.x;
				if(x<0) x=0;
				else if(x>dock_area.width) x=dock_area.width;

				wxColour clr02(
					int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x/dock_area.width,
					int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x/dock_area.width,
					int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x/dock_area.width
					);

				wxColour clr1(
					(int(m_bar1.Red())+int(clr01.Red()))/2,
					(int(m_bar1.Green())+int(clr01.Green()))/2,
					(int(m_bar1.Blue())+int(clr01.Blue()))/2
					);
				wxColour clr2(
					(int(m_bar2.Red())+int(clr02.Red()))/2,
					(int(m_bar2.Green())+int(clr02.Green()))/2,
					(int(m_bar2.Blue())+int(clr02.Blue()))/2
					);

				dc.SetPen(wxPen(clr01));
				dc.DrawPoint(rect.x,rect.y);
				dc.SetPen(wxPen(clr02));
				dc.DrawPoint(rect.x+rect.width-1,rect.y);

				dc.SetPen(wxPen(clr1));
				dc.DrawPoint(rect.x+1,rect.y);
				dc.DrawPoint(rect.x,rect.y+1);

				dc.SetPen(wxPen(clr2));
				dc.DrawPoint(rect.x+rect.width-2,rect.y);
				dc.DrawPoint(rect.x+rect.width-1,rect.y+1);
			}
		}else{
			dc.GradientFillLinear(rect,m_bar1,m_bar2,wxDOWN);
			if(dock_area.width>0){
				int x=rect.x-dock_area.x;
				if(x<0) x=0;
				else if(x>dock_area.width) x=dock_area.width;

				wxColour clr(
					int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x/dock_area.width,
					int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x/dock_area.width,
					int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x/dock_area.width
					);
				wxColour clr1(
					(int(m_bar1.Red())+int(clr.Red()))/2,
					(int(m_bar1.Green())+int(clr.Green()))/2,
					(int(m_bar1.Blue())+int(clr.Blue()))/2
					);
				wxColour clr2(
					(int(m_bar2.Red())+int(clr.Red()))/2,
					(int(m_bar2.Green())+int(clr.Green()))/2,
					(int(m_bar2.Blue())+int(clr.Blue()))/2
					);

				dc.SetPen(wxPen(clr));
				dc.DrawPoint(rect.x,rect.y);
				dc.DrawPoint(rect.x,rect.y+rect.height-1);

				dc.SetPen(wxPen(clr1));
				dc.DrawPoint(rect.x+1,rect.y);
				dc.DrawPoint(rect.x,rect.y+1);

				dc.SetPen(wxPen(clr2));
				dc.DrawPoint(rect.x+1,rect.y+rect.height-1);
				dc.DrawPoint(rect.x,rect.y+rect.height-2);
			}
		}
	}

	//draw gripper
	if(dir==fcbdLeft || dir==fcbdRight){
		for(int x=rect.x+4;x<rect.x+rect.width-4;x+=4){
			DrawOneGripper(dc,wnd,x,rect.y+3);
		}
	}else{
		for(int y=rect.y+4;y<rect.y+rect.height-4;y+=4){
			DrawOneGripper(dc,wnd,rect.x+3,y);
		}
	}
}

void wxFakeOffice2003CommandBarArt::DrawToolbarChevron(wxDC& dc,wxWindow* wnd,bool overflow,bool chevron,int bar_flags,int dir,int add_flags,const wxRect& rect,const wxRect& dock_area){
	//draw background
	if((bar_flags & fcbfMainMenu)==0){
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.GradientFillLinear(wxRect(rect.x,rect.y,rect.width,rect.height-9),m_bar1,m_bar2,wxRIGHT);
		}else{
			dc.GradientFillLinear(wxRect(rect.x,rect.y,rect.width-9,rect.height),m_bar1,m_bar2,wxDOWN);
		}
	}
	wxColour clr01,clr02;
	if(add_flags & (fbtaPressed | fbtaPopupMenu)){
		clr01=m_pressed1;
		clr02=m_pressed2;
	}else if(add_flags & fbtaHighlight){
		clr01=m_hl1;
		clr02=m_hl2;
	}else{
		clr01=m_chevron1;
		clr02=m_chevron2;
	}
	if(dir==fcbdLeft || dir==fcbdRight){
		dc.GradientFillLinear(wxRect(rect.x,rect.y+rect.height-9,rect.width,9),clr01,clr02,wxRIGHT);
	}else{
		dc.GradientFillLinear(wxRect(rect.x+rect.width-9,rect.y,9,rect.height),clr01,clr02,wxDOWN);
	}

	wxColour clr1(
		(int(m_bar1.Red())+int(clr01.Red()))/2,
		(int(m_bar1.Green())+int(clr01.Green()))/2,
		(int(m_bar1.Blue())+int(clr01.Blue()))/2
		);
	wxColour clr2=(bar_flags & fcbfMainMenu)==0?wxColour(
		(int(m_bar2.Red())+int(clr02.Red()))/2,
		(int(m_bar2.Green())+int(clr02.Green()))/2,
		(int(m_bar2.Blue())+int(clr02.Blue()))/2
		):clr1;

	dc.SetPen(wxPen(clr01));
	if(dir==fcbdLeft || dir==fcbdRight){
		dc.DrawPoint(rect.x,rect.y+rect.height-10);
	}else{
		dc.DrawPoint(rect.x+rect.width-10,rect.y);
	}
	dc.SetPen(wxPen(clr1));
	if(dir==fcbdLeft || dir==fcbdRight){
		dc.DrawPoint(rect.x,rect.y+rect.height-11);
		dc.DrawPoint(rect.x+1,rect.y+rect.height-10);
	}else{
		dc.DrawPoint(rect.x+rect.width-11,rect.y);
		dc.DrawPoint(rect.x+rect.width-10,rect.y+1);
	}
	if(dock_area.width<=0){
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.DrawPoint(rect.x,rect.y+rect.height-2);
			dc.DrawPoint(rect.x+1,rect.y+rect.height-1);
		}else{
			dc.DrawPoint(rect.x+rect.width-2,rect.y);
			dc.DrawPoint(rect.x+rect.width-1,rect.y+1);
		}
		dc.SetPen(wxPen(m_bar1));
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.DrawPoint(rect.x,rect.y+rect.height-1);
		}else{
			dc.DrawPoint(rect.x+rect.width-1,rect.y);
		}
	}

	dc.SetPen(wxPen(clr02));
	if(dir==fcbdLeft || dir==fcbdRight){
		dc.DrawPoint(rect.x+rect.width-1,rect.y+rect.height-10);
	}else{
		dc.DrawPoint(rect.x+rect.width-10,rect.y+rect.height-1);
	}
	dc.SetPen(wxPen(clr2));
	if(dir==fcbdLeft || dir==fcbdRight){
		dc.DrawPoint(rect.x+rect.width-1,rect.y+rect.height-11);
		dc.DrawPoint(rect.x+rect.width-2,rect.y+rect.height-10);
	}else{
		dc.DrawPoint(rect.x+rect.width-11,rect.y+rect.height-1);
		dc.DrawPoint(rect.x+rect.width-10,rect.y+rect.height-2);
	}
	if(dock_area.width<=0){
		dc.DrawPoint(rect.x+rect.width-2,rect.y+rect.height-1);
		dc.DrawPoint(rect.x+rect.width-1,rect.y+rect.height-2);
		dc.SetPen(wxPen((bar_flags & fcbfMainMenu)==0?m_bar2:m_bar1));
		dc.DrawPoint(rect.x+rect.width-1,rect.y+rect.height-1);
	}

	if(dock_area.width>0){
		int x=rect.x+rect.width-1-dock_area.x;
		if(x<0) x=0;
		else if(x>dock_area.width) x=dock_area.width;

		wxColour clr(
			int(m_bar2.Red())+(int(m_bar1.Red())-int(m_bar2.Red()))*x/dock_area.width,
			int(m_bar2.Green())+(int(m_bar1.Green())-int(m_bar2.Green()))*x/dock_area.width,
			int(m_bar2.Blue())+(int(m_bar1.Blue())-int(m_bar2.Blue()))*x/dock_area.width
			);
		clr1=wxColour(
			(int(clr01.Red())+int(clr.Red()))/2,
			(int(clr01.Green())+int(clr.Green()))/2,
			(int(clr01.Blue())+int(clr.Blue()))/2
			);
		clr2=wxColour(
			(int(clr02.Red())+int(clr.Red()))/2,
			(int(clr02.Green())+int(clr.Green()))/2,
			(int(clr02.Blue())+int(clr.Blue()))/2
			);

		dc.SetPen(wxPen(clr));
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.DrawPoint(rect.x,rect.y+rect.height-1);
		}else{
			dc.DrawPoint(rect.x+rect.width-1,rect.y);
		}
		dc.DrawPoint(rect.x+rect.width-1,rect.y+rect.height-1);

		dc.SetPen(wxPen(clr1));
		if(dir==fcbdLeft || dir==fcbdRight){
			dc.DrawPoint(rect.x,rect.y+rect.height-2);
			dc.DrawPoint(rect.x+1,rect.y+rect.height-1);
		}else{
			dc.DrawPoint(rect.x+rect.width-2,rect.y);
			dc.DrawPoint(rect.x+rect.width-1,rect.y+1);
		}

		dc.SetPen(wxPen(clr2));
		dc.DrawPoint(rect.x+rect.width-2,rect.y+rect.height-1);
		dc.DrawPoint(rect.x+rect.width-1,rect.y+rect.height-2);
	}

	//draw sign
	if(dir==fcbdLeft || dir==fcbdRight){
		if(overflow){
			dc.DrawBitmap(m_overflow_down,rect.x+4,rect.y+rect.height-8,true);
		}
		if(chevron){
			dc.DrawBitmap(m_chevron_right,rect.x+rect.width-9,rect.y+rect.height-7,true);
		}
	}else{
		if(overflow){
			dc.DrawBitmap(m_overflow_right,rect.x+rect.width-8,rect.y+4,true);
		}
		if(chevron){
			dc.DrawBitmap(m_chevron_down,rect.x+rect.width-7,rect.y+rect.height-9,true);
		}
	}
}

void MyDrawLabel(wxDC& dc, const wxString& text, wxRect rect, int dir, int alignment, int indexAccel){
	// swap it
	if(dir==fcbdLeft || dir==fcbdRight){
		int tmp=rect.x;
		rect.x=rect.y;
		rect.y=tmp;

		tmp=rect.width;
		rect.width=rect.height;
		rect.height=tmp;
	}

	// find the text position
	wxCoord widthText, heightText, heightLine;
	dc.GetMultiLineTextExtent(text, &widthText, &heightText, &heightLine);

	wxCoord width, height;
	// no bitmap
	width = widthText;
	height = heightText;

	wxCoord x, y;
	if ( alignment & wxALIGN_RIGHT )
	{
		x = rect.GetRight() - width;
	}
	else if ( alignment & wxALIGN_CENTRE_HORIZONTAL )
	{
		x = (rect.GetLeft() + rect.GetRight() + 1 - width) / 2;
	}
	else // alignment & wxALIGN_LEFT
	{
		x = rect.GetLeft();
	}

	if ( alignment & wxALIGN_BOTTOM )
	{
		y = rect.GetBottom() - height;
	}
	else if ( alignment & wxALIGN_CENTRE_VERTICAL )
	{
		y = (rect.GetTop() + rect.GetBottom() + 1 - height) / 2;
	}
	else // alignment & wxALIGN_TOP
	{
		y = rect.GetTop();
	}

	// we will draw the underscore under the accel char later
	wxCoord startUnderscore = 0,
		endUnderscore = 0,
		yUnderscore = 0;

	// split the string into lines and draw each of them separately
	//
	// NB: while wxDC::DrawText() on some platforms supports drawing multi-line
	//     strings natively, this is not the case for all of them, notably not
	//     wxMSW which uses this function for multi-line texts, so we may only
	//     call DrawText() for single-line strings from here to avoid infinite
	//     recursion.
	wxString curLine;
	for ( wxString::const_iterator pc = text.begin(); ; ++pc )
	{
		if ( pc == text.end() || *pc == '\n' )
		{
			int xRealStart = x; // init it here to avoid compielr warnings

			if ( !curLine.empty() )
			{
				// NB: can't test for !(alignment & wxALIGN_LEFT) because
				//     wxALIGN_LEFT is 0
				if ( alignment & (wxALIGN_RIGHT | wxALIGN_CENTRE_HORIZONTAL) )
				{
					wxCoord widthLine;
					dc.GetTextExtent(curLine, &widthLine, NULL);

					if ( alignment & wxALIGN_RIGHT )
					{
						xRealStart += width - widthLine;
					}
					else // if ( alignment & wxALIGN_CENTRE_HORIZONTAL )
					{
						xRealStart += (width - widthLine) / 2;
					}
				}
				//else: left aligned, nothing to do

				switch(dir){
				case fcbdBottom:
					dc.DrawRotatedText(curLine, rect.x * 2 + rect.width - xRealStart - 2,
						rect.y * 2 + rect.height - y, 180.0);
					break;
				case fcbdLeft:
					dc.DrawRotatedText(curLine, y, rect.x * 2 + rect.width - xRealStart - 2, 90.0);
					break;
				case fcbdRight:
					dc.DrawRotatedText(curLine, rect.y * 2 + rect.height - y, xRealStart, 270.0);
					break;
				default:
					dc.DrawText(curLine, xRealStart, y);
					break;
				}
			}

			y += heightLine;

			// do we have underscore in this line? we can check yUnderscore
			// because it is set below to just y + heightLine if we do
			if ( y == yUnderscore )
			{
				// adjust the horz positions to account for the shift
				startUnderscore += xRealStart;
				endUnderscore += xRealStart;
			}

			if ( pc == text.end() )
				break;

			curLine.clear();
		}
		else // not end of line
		{
			if ( pc - text.begin() == indexAccel )
			{
				// remeber to draw underscore here
				dc.GetTextExtent(curLine, &startUnderscore, NULL);
				curLine += *pc;
				dc.GetTextExtent(curLine, &endUnderscore, NULL);

				yUnderscore = y + heightLine;
			}
			else
			{
				curLine += *pc;
			}
		}
	}

	// draw the underscore if found
	if ( startUnderscore != endUnderscore )
	{
		// backup the old pen
		wxPen oldPen = dc.GetPen();

		// it should be of the same colour as text
		dc.SetPen(wxPen(dc.GetTextForeground(), 0, wxPENSTYLE_SOLID));

		// This adjustment is relatively arbitrary: we need to draw the
		// underline slightly higher to avoid overflowing the character cell
		// but whether we should do it 1, 2 or 3 pixels higher is not clear.
		//
		// XXX The currently used value seems to be compatible with native MSW
		// behaviour, i.e. it results in the same appearance of the owner-drawn
		// and normal labels. XXX

		switch(dir){
		case fcbdBottom:
			dc.DrawLine(rect.x * 2 + rect.width - startUnderscore - 3,
				rect.y * 2 + rect.height - yUnderscore,
				rect.x * 2 + rect.width - endUnderscore - 3,
				rect.y * 2 + rect.height - yUnderscore);
			break;
		case fcbdLeft:
			dc.DrawLine(yUnderscore,
				rect.x * 2 + rect.width - startUnderscore - 3,
				yUnderscore,
				rect.x * 2 + rect.width - endUnderscore - 3);
			break;
		case fcbdRight:
			dc.DrawLine(rect.y * 2 + rect.height - yUnderscore - 1,
				startUnderscore,
				rect.y * 2 + rect.height - yUnderscore - 1,
				endUnderscore);
			break;
		default:
			dc.DrawLine(startUnderscore, yUnderscore - 1, endUnderscore, yUnderscore - 1);
			break;
		}

		// reset the pen
		dc.SetPen(oldPen);
	}
}

int wxFakeOffice2003CommandBarArt::GetMinimizedMiniFrameHeight(int style) const{
	return 22;
}

//TODO: docked mode
int wxFakeOffice2003CommandBarArt::MiniFrameHitTest(int style,const wxPoint& pt,const wxRect& rect){
	if(rect.Contains(pt)){
		//check title bar
		wxRect r(rect.x+3,rect.y+3,rect.width-6,(style & fpstMinimized)?rect.height-6:16);
		if(r.Contains(pt)){
			int x=r.x+r.width-1;
			int y=r.y+r.height/2-7;

			bool b=(pt.x<x && pt.y>=y && pt.y<y+14);

			//close button
			if(style & fpstCloseBox){
				x-=14;
				if(b && pt.x>=x) return fphtClose;
			}

			//minimize/restore button
			if(style & fpstMinimizeBox){
				x-=14;
				if(b && pt.x>=x) return fphtMinimize;
			}

			//pin/unpin button
			if(style & fpstPinBox){
				x-=14;
				if(b && pt.x>=x) return fphtPin;
			}

			//customize button
			if(style & fpstCustomBox){
				x-=14;
				if(b && pt.x>=x) return fphtCustom;
			}

			//dropdown button
			if(style & fpstDropdownBox){
				if(pt.x>=r.x+8 && pt.x<x && pt.y>r.y+1 && pt.y<r.y+r.height-1){
					return fphtDropdown;
				}
			}

			return fphtGripper;
		}

		//check resize border
		if(style & fpstSizable){
			if(style & fpstMinimized){
				if(pt.x<rect.x+3) return fphtSizeW;
				else if(pt.x>=rect.x+rect.width-3) return fphtSizeE;
			}else{
				if(pt.x<rect.x+3){
					if(pt.y<rect.y+16) return fphtSizeNW;
					else if(pt.y>=rect.y+rect.height-16) return fphtSizeSW;
					else return fphtSizeW;
				}else if(pt.x>=rect.x+rect.width-3){
					if(pt.y<rect.y+16) return fphtSizeNE;
					else if(pt.y>=rect.y+rect.height-16) return fphtSizeSE;
					else return fphtSizeE;
				}else if(pt.y<rect.y+3){
					if(pt.x<rect.x+16) return fphtSizeNW;
					else if(pt.x>=rect.x+rect.width-16) return fphtSizeNE;
					else return fphtSizeN;
				}else if(pt.y>=rect.y+rect.height-3){
					if(pt.x<rect.x+16) return fphtSizeSW;
					else if(pt.x>=rect.x+rect.width-16) return fphtSizeSE;
					else return fphtSizeS;
				}
			}
		}
	}

	return -1;
}

bool wxFakeOffice2003CommandBarArt::GetMiniFrameButtonRect(int idx,int style,const wxRect& rect,wxRect& ret){
	if(idx==fphtCustom || idx==fphtDropdown){
		wxRect r(rect.x+3,rect.y+3,rect.width-6,(style & fpstMinimized)?rect.height-6:16);

		int x=r.x+r.width-1;
		int y=r.y+r.height/2-7;

		//close button
		if(style & fpstCloseBox) x-=14;

		//minimize/restore button
		if(style & fpstMinimizeBox) x-=14;

		//pin/unpin button
		if(style & fpstPinBox) x-=14;

		//customize button
		if(style & fpstCustomBox){
			x-=14;
			if(idx==fphtCustom){
				ret=wxRect(x,y,14,14);
				return true;
			}
		}

		//dropdown button
		if(style & fpstDropdownBox){
			if(idx==fphtDropdown){
				ret=wxRect(r.x+8,r.y+1,x-r.x-8,r.height-2);
				return true;
			}
		}
	}

	return false;
}

void wxFakeOffice2003CommandBarArt::GetMiniFrameClientSizeMargin(int style,int& leftMargin,int& topMargin,int& rightMargin,int& bottomMargin){
	leftMargin=3;
	topMargin=(style & fpstToolbarMode)?20:19;
	rightMargin=3;
	bottomMargin=3;
}

//TODO: docked mode
void wxFakeOffice2003CommandBarArt::DrawMiniFrame(wxDC& dc,wxWindow* wnd,const wxString& title,int style,int highlight_idx,int pressed_idx,int submenu_idx,const wxRect& rect){
	//background
	dc.SetBrush(wxBrush(m_pane_outerborder));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(rect);

	dc.SetBrush(wxBrush(m_pane_innerborder));
	dc.DrawRectangle(rect.x+3,rect.y+2,rect.width-6,1);
	dc.DrawRectangle(rect.x+3,rect.y+rect.height-3,rect.width-6,1);
	dc.DrawRectangle(rect.x+2,rect.y+3,1,rect.height-5);
	dc.DrawRectangle(rect.x+rect.width-3,rect.y+3,1,rect.height-5);

	//title bar
	{
		wxRect r(rect.x+3,rect.y+3,rect.width-6,(style & fpstMinimized)?rect.height-6:16);
		dc.GradientFillLinear(r,m_pane_title1,m_pane_title2,wxDOWN);

		//gripper
		for(int y=r.y+3;y<r.y+r.height-4;y+=4){
			DrawOneGripper(dc,wnd,r.x+2,y);
		}

		r.x+=8;
		r.y+=1;
		r.width-=9;
		r.height-=2;

		//close button
		if(style & fpstCloseBox){
			r.width-=14;
			wxRect r1(r.x+r.width,r.y+r.height/2-7,14,14);

			if(highlight_idx==fphtClose){
				if(pressed_idx==fphtClose){
					dc.GradientFillLinear(r1,m_pressed1,m_pressed2,wxDOWN);
				}else{
					dc.GradientFillLinear(r1,m_hl1,m_hl2,wxDOWN);
				}
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(r1);
			}

			dc.DrawBitmap(m_pane_close,r1.x+2,r1.y+2,true);
		}

		//minimize/restore button
		if(style & fpstMinimizeBox){
			r.width-=14;
			wxRect r1(r.x+r.width,r.y+r.height/2-7,14,14);

			if(highlight_idx==fphtMinimize){
				if(pressed_idx==fphtMinimize){
					dc.GradientFillLinear(r1,m_pressed1,m_pressed2,wxDOWN);
				}else{
					dc.GradientFillLinear(r1,m_hl1,m_hl2,wxDOWN);
				}
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(r1);
			}

			dc.DrawBitmap((style & fpstMinimized)?m_pane_restore:m_pane_minimize,r1.x+2,r1.y+2,true);
		}

		//pin/unpin button
		if(style & fpstPinBox){
			r.width-=14;
			wxRect r1(r.x+r.width,r.y+r.height/2-7,14,14);

			if(highlight_idx==fphtPin){
				if(pressed_idx==fphtPin){
					dc.GradientFillLinear(r1,m_pressed1,m_pressed2,wxDOWN);
				}else{
					dc.GradientFillLinear(r1,m_hl1,m_hl2,wxDOWN);
				}
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(r1);
			}

			dc.DrawBitmap((style & fpstAutoHide)?m_pane_unpin:m_pane_pin,r1.x+2,r1.y+2,true);
		}

		//customize button
		if(style & fpstCustomBox){
			r.width-=14;
			wxRect r1(r.x+r.width,r.y+r.height/2-7,14,14);

			if(highlight_idx==fphtCustom || submenu_idx==fphtCustom){
				if(submenu_idx==fphtCustom){
					dc.GradientFillLinear(r1,m_bar1,m_bar2,wxDOWN);
				}else if(pressed_idx==fphtCustom){
					dc.GradientFillLinear(r1,m_pressed1,m_pressed2,wxDOWN);
				}else{
					dc.GradientFillLinear(r1,m_hl1,m_hl2,wxDOWN);
				}
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(r1);
			}

			dc.DrawBitmap(m_pane_dropdown,r1.x+2,r1.y+2,true);
		}

		//dropdown button
		if(style & fpstDropdownBox){
			if(highlight_idx==fphtDropdown || submenu_idx==fphtDropdown){
				if(submenu_idx==fphtDropdown){
					dc.GradientFillLinear(r,m_bar1,m_bar2,wxDOWN);
				}else if(pressed_idx==fphtDropdown){
					dc.GradientFillLinear(r,m_pressed1,m_pressed2,wxDOWN);
				}else{
					dc.GradientFillLinear(r,m_hl1,m_hl2,wxDOWN);
				}
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(m_border));
				dc.DrawRectangle(r);
			}

			dc.DrawBitmap(m_pane_dropdown,r.x+r.width-12,r.y+r.height/2-5,true);

			r.x+=2;
		}

		//title
		MyDrawLabel(dc,title,r,0,wxALIGN_CENTRE_VERTICAL);
	}

	//client area
	if((style & fpstMinimized)==0){
		dc.SetBrush(wxBrush(m_pane_bg1));
		dc.SetPen(*wxTRANSPARENT_PEN);
		if(style & fpstGradientBackground){
			dc.DrawRectangle(rect.x+3,rect.y+19,rect.width-6,8);
			dc.DrawBitmap(m_pane_topleftcorner,rect.x+3,rect.y+19,true);
			dc.DrawBitmap(m_pane_toprightcorner,rect.x+rect.width-8,rect.y+19,true);
			dc.GradientFillLinear(wxRect(rect.x+3,rect.y+27,rect.width-6,rect.height-30),m_pane_bg1,m_pane_bg2,wxDOWN);
		}else{
			dc.DrawRectangle(rect.x+3,rect.y+19,rect.width-6,rect.height-22);
		}
	}
}
