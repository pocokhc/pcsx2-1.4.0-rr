#include "PrecompiledHeader.h"

#include "LuaFrame.h"
#include "LuaEngine.h"


enum {
	ID_ConsoleClear = 1,
	ID_OpenFile,
	ID_Stop,
	ID_Start,
	ID_Restart,
};

wxBEGIN_EVENT_TABLE(LuaFrame, wxFrame)
	EVT_CLOSE(LuaFrame::OnClose)
wxEND_EVENT_TABLE()

LuaFrame::LuaFrame(wxWindow * parent)
	: wxFrame(parent, wxID_ANY, L"Lua", wxPoint(437+680-300,52+560), wxSize(300,120)
	//, wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX | wxRESIZE_BORDER)
	)
{
	// menu bar
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_ConsoleClear, _("&Clear"),
		_("Help string shown in status bar for this menu item"));
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, _("&Console"));
	SetMenuBar(menuBar);

	// status bar
	statusbar = CreateStatusBar();
	statusbar->SetStatusText(_("lua no open"));

	// button
	int x = 2;
	int w = 90;
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	new wxButton(panel, ID_OpenFile, _("Browse..."), wxPoint(x,2));
	stopButton = new wxButton(panel, ID_Stop, _("Stop"), wxPoint(x+w*1, 2));
	startButton = new wxButton(panel, ID_Start, _("Start"), wxPoint(x + w * 2, 2));
	restartButton = new wxButton(panel, ID_Restart, _("Restart"), wxPoint(x + w * 2, 2));
	stopButton->Disable();
	restartButton->Hide();

	// event
	Bind(wxEVT_COMMAND_MENU_SELECTED, &LuaFrame::OnConsoleClear, this, ID_ConsoleClear);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LuaFrame::OnOpenFile, this, ID_OpenFile);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LuaFrame::OnStop, this, ID_Stop);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LuaFrame::OnRun, this, ID_Start);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LuaFrame::OnRestart, this, ID_Restart);

}
void LuaFrame::drawState(wxString str) {
	statusbar->SetStatusText(str);
}
void LuaFrame::pushRunState() {
	// stop button enable
	// run button -> restart button
	stopButton->Enable();
	startButton->Hide();
	restartButton->Show();
}
void LuaFrame::pushStopState() {
	// stop button disable
	// restart button -> run button
	stopButton->Disable();
	startButton->Show();
	restartButton->Hide();
}
void LuaFrame::OnClose(wxCloseEvent& evt)
{
	Hide();
}
void LuaFrame::OnConsoleClear(wxCommandEvent& event)
{
	//no use
	//メニューバー用においてあるだけ、消してもいいかも
}
void LuaFrame::OnOpenFile(wxCommandEvent& event)
{
	// wxFileDialog
	wxFileDialog openFileDialog(this, _("Select lua."), L"", L"",
		L"lua file(*.lua)|*.lua",
		wxFD_OPEN);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		// cancel
		return;
	}
	wxString path = openFileDialog.GetPath();
	LoadLuaCode(path);
	//	LoadLuaCode("C:\\data\\GitHub\\pcsx2-1.4.0-rr\\bin\\lua\\test.lua");

}
void LuaFrame::OnStop(wxCommandEvent& event)
{
	LuaCodeStop();
}
void LuaFrame::OnRun(wxCommandEvent& event)
{
	LuaCodeRun();
}
void LuaFrame::OnRestart(wxCommandEvent& event)
{
	LuaCodeRestart();
}
