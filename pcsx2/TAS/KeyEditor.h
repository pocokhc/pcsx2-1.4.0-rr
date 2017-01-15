#pragma once
#ifndef __KEY_EDITOR_H__
#define __KEY_EDITOR_H__

#include <wx/wx.h>


class KeyEditor : public wxFrame
{
public:
	KeyEditor(wxWindow * parent);

public:
	void FrameUpdate();
	
private:
	void DrawHeader();
	void DrawList();

private:
	wxTextCtrl* infoText;
	wxListBox * frameList;
	wxStatusBar* statusbar;
	
	wxTextCtrl* keyText1;
	wxTextCtrl* keyText2;

	wxTextCtrl* frameText;


private:
	void OnClose(wxCloseEvent& evt);
	void OnActivate(wxCloseEvent& evt);

	void OnConsoleAuthor(wxCommandEvent& event);
	void OnUpdate(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnInsert(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnDrawFrame(wxCommandEvent& event);

	void OnListBox(wxCommandEvent& event);
	

	wxDECLARE_EVENT_TABLE();
};

#endif
