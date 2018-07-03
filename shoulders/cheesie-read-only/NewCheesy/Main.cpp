#include "ProjectHeader.h"

IMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
	if ( !wxApp::OnInit() )
	return false;

	// create the main application window
	MyFrame *frame = new MyFrame(wxT("wxComboCtrl and wxOwnerDrawnComboBox Sample"));

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frame->Show(true);

	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned false here, the
	// application would exit immediately.
	return true;
}
