#include "ScrollBar.h"
BEGIN_EVENT_TABLE(ScrollBar,wxScrollBar)
//EVT_LEFT_DOWN(PicWindow::LeftClick) 

//EVT_SCROLL(ScrollBar::OnScroll)
END_EVENT_TABLE()

ScrollBar::ScrollBar(wxWindow* Parent, int ID, int Type,  wxPoint pos, wxSize size){

	me= new wxScrollBar(Parent, ID,pos,size,Type);
	type=Type;

}

void  ScrollBar::OnScroll()
{
	//m_changed = true;

	//event.Skip();
}