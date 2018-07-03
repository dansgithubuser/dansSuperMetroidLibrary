#ifndef __MENU_H
#define __MENU_H
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/textctrl.h>
#include <wx/defs.h>
#include <wx/wx.h>
#include <wx/menu.h>
enum{
	idOpen=5300
};
class clsMenu
{
public:
	clsMenu(wxFrame* parent);

	virtual ~clsMenu();
	wxFrame* theparent;

	wxMenuBar *menubar;
	wxMenu *file;
	void OnQuit(wxCommandEvent&  event);

};

// A small helper class which intercepts all menu events and logs them


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
	Menu_File_Quit = wxID_EXIT,

};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------


#endif
