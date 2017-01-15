#include "PrecompiledHeader.h"

#include "app.h"	//Counters.hをincludeするのに必要
#include "Counters.h"	// use"g_FrameCount"

#include "KeyEditor.h"
#include "KeyMovie.h"
#include "MovieControle.h"

#include <string>


enum {
	ID_ConsoleAuthor = 1,

	ID_Update,
	ID_Insert,
	ID_Delete,
	ID_Copy,
	ID_ListBox,
	ID_DrawFrame,
};

wxBEGIN_EVENT_TABLE(KeyEditor, wxFrame)
	//EVT_ACTIVATE(KeyEditor::OnActivate)
	EVT_CLOSE(KeyEditor::OnClose)
wxEND_EVENT_TABLE()

KeyEditor::KeyEditor(wxWindow * parent)
	: wxFrame(parent, wxID_ANY, L"KeyEditor", wxPoint(437+680,52), wxSize(680,560))
{
	// menu bar
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_ConsoleAuthor, _("&set author"));
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, _("&menu"));
	SetMenuBar(menuBar);

	// panel
	int x = 2;
	int y = 2;
	wxPanel *panel = new wxPanel(this, wxID_ANY);

	// listbox
	frameList = new wxListBox(panel, ID_ListBox, wxPoint(x, y),wxSize(230,460));

	// text
	x += 230 + 5;
	infoText = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(x, y), wxSize(200, 250) , wxTE_MULTILINE);

	// key
	y += 250;
	keyText1 = new wxTextCtrl(panel, wxID_ANY, "1", wxPoint(x, y), wxSize(400, 50), wxTE_MULTILINE);
	keyText1->Disable();
	y += 50;
	new wxButton(panel, ID_Copy, _("copy"), wxPoint(x, y));
	y += 30;
	keyText2 = new wxTextCtrl(panel, wxID_ANY, "2", wxPoint(x, y), wxSize(400, 50), wxTE_MULTILINE);

	// button
	y += 50;
	int w = 90;
	new wxButton(panel, ID_Update, _("update"), wxPoint(x,y));
	new wxButton(panel, ID_Insert, _("insert"), wxPoint(x+w, y));
	new wxButton(panel, ID_Delete, _("delete"), wxPoint(x + w * 2, y));
	
	y += 30;
	new wxButton(panel, ID_DrawFrame, _("draw frame:"), wxPoint(x, y));
	frameText = new wxTextCtrl(panel, wxID_ANY, "100", wxPoint(x+w, y), wxSize(100, 25));

	// status bar
	statusbar = CreateStatusBar();
	statusbar->SetStatusText(_("key editor open"));


	// event
	Bind(wxEVT_COMMAND_MENU_SELECTED, &KeyEditor::OnConsoleAuthor, this, ID_ConsoleAuthor);
	// button
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &KeyEditor::OnUpdate, this, ID_Update);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &KeyEditor::OnInsert, this, ID_Insert);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &KeyEditor::OnDelete, this, ID_Delete);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &KeyEditor::OnCopy, this, ID_Copy);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &KeyEditor::OnDrawFrame, this, ID_DrawFrame);
	
	//list box
	Bind(wxEVT_COMMAND_LISTBOX_SELECTED, &KeyEditor::OnListBox, this, ID_ListBox);

	
}
void KeyEditor::OnClose(wxCloseEvent& evt)
{
	Hide();
}
void KeyEditor::OnActivate(wxCloseEvent& evt)
{
	frameText->SetValue(wxString::Format("%d", g_FrameCount));
}
//-------------------------------
// every frame
//-------------------------------
void KeyEditor::FrameUpdate()
{
	if (g_FrameCount == 0)return;

	wxString pauseMessage = g_MovieControle.getStopFlag() ? "[pause]" : "[run]";
	wxString recordMessage = "";
	if (g_KeyMovie.getModeState() == KeyMovie::RECORD) {
		recordMessage = "[record]";
	}
	else if (g_KeyMovie.getModeState() == KeyMovie::REPLAY) {
		recordMessage = "[replay]";
	}
	SetTitle(wxString::Format("frame:%d ", g_FrameCount)+ pauseMessage+ recordMessage);
	
}
void KeyEditor::DrawList()
{
	long selectFrame;
	if( !frameText->GetValue().ToLong(&selectFrame) )return;
	
	// 表示するフレーム数をきめる
	int start = selectFrame - 200;
	int end = selectFrame +200;
	if (start < 0) {
		start = 0;
	}
	if (end > g_KeyMovie.getMaxFrame()) {
		end = g_KeyMovie.getMaxFrame();
	}

	frameList->Clear();
	for (uint i = start; i < end; i++)
	{
		PadData key;
		g_KeyMovie.getPadData(key, i);
		wxString s = "";
		if (i > 0){
			for (int slot = 0; slot < 10; slot++) {
				if (g_KeyMovie.getSaveFrame(slot) == i) {
					s += wxString::Format("[save%d]", slot);
				}
			}
		}
		frameList->Append( s+key.getString());
	}
	// select
	uint selectIndex = selectFrame -start;
	if (0 <= selectIndex && selectIndex < frameList->GetCount()) {
		frameList->SetSelection(selectIndex);
		keyText1->SetValue(frameList->GetString(selectIndex));
	}
}
void KeyEditor::DrawHeader()
{
	KeyMovieHeader & header = g_KeyMovie.getHeader();
	infoText->SetLabelText("");
	std::ostream stream(infoText);
	stream << "MaxFrame:" << g_KeyMovie.getMaxFrame() << std::endl;
	stream << "UndoCount:" << g_KeyMovie.getUndoCount() << std::endl;
	stream << "KeyFile version:" << (int)header.version << std::endl;
	stream << "Auther:" << header.auther << std::endl;
	for (int slot = 0; slot < 10; slot++) {
		long frame = g_KeyMovie.getSaveFrame(slot);
		stream << "save" << slot << ":" << frame << " frame" << std::endl;
	}
	stream.flush();
}

//----------------------------------------------
// event
//----------------------------------------------
void KeyEditor::OnConsoleAuthor(wxCommandEvent& event)
{
	wxTextEntryDialog* dlg = new wxTextEntryDialog(NULL,"input author.");
	if (dlg->ShowModal() != wxID_OK) {
		return;
	}
	wxString val = dlg->GetValue();
	int max = sizeof(g_KeyMovie.getHeader().auther) - 1;
	strncpy(g_KeyMovie.getHeader().auther, val.c_str(),max);
	g_KeyMovie.getHeader().auther[max] = 0;

	DrawHeader();
}
void KeyEditor::OnUpdate(wxCommandEvent& event)
{
	if (frameList->GetSelection() == wxNOT_FOUND)return;
	PadData key1;
	key1.setString(frameList->GetStringSelection());
	PadData key2;
	key2.setString(keyText2->GetValue());
	key2.frame = key1.frame;
	if (g_KeyMovie.UpdateKeyFrame(key2)) {
		frameText->SetValue(wxString::Format("%d", key2.frame));
		DrawHeader();
		DrawList();
	}
}
void KeyEditor::OnInsert(wxCommandEvent& event)
{
	if (frameList->GetSelection() == wxNOT_FOUND)return;
	PadData key1;
	key1.setString(frameList->GetStringSelection());
	PadData key2;
	key2.setString(keyText2->GetValue());
	key2.frame = key1.frame;
	if (g_KeyMovie.InsertKeyFrame(key2)) {
		frameText->SetValue(wxString::Format("%d", key2.frame));
		DrawHeader();
		DrawList();
	}
}
void KeyEditor::OnDelete(wxCommandEvent& event)
{
	if (frameList->GetSelection() == wxNOT_FOUND)return;
	PadData key;
	key.setString(frameList->GetStringSelection());
	if (g_KeyMovie.DeleteKeyFrame(key.frame)) {
		frameText->SetValue(wxString::Format("%d",key.frame));
		DrawHeader();
		DrawList();
	}
}
void KeyEditor::OnCopy(wxCommandEvent& event)
{
	keyText2->SetValue(keyText1->GetValue());
}
void KeyEditor::OnListBox(wxCommandEvent& event)
{
	if (frameList->GetSelection() == wxNOT_FOUND)return;
	wxString s = frameList->GetStringSelection();
	keyText1->SetValue(s);
}

void KeyEditor::OnDrawFrame(wxCommandEvent& event)
{
	DrawHeader();
	DrawList();
}


