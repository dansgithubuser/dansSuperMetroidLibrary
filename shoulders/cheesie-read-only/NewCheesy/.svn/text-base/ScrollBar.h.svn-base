#include <wx/scrolbar.h>
#ifndef SCROLLBAR_H
#define SCROLLBAR_H
enum{
	HORIZ             = 0x0004,
	VERT                = 0x0008,

	BOTH                    = VERT  | HORIZ,

};

class ScrollBar:wxScrollBar{
public:
	ScrollBar(wxWindow* Parent, int ID, int Type,  wxPoint pos, wxSize size);
	void OnScroll();
private:
	wxScrollBar* me;
	int type;
	int position;

private:
	DECLARE_EVENT_TABLE();
};
#endif 