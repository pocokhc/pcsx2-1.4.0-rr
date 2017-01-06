#include "PrecompiledHeader.h"

#include "MainFrame.h"

#include "TAS.h"


void MainEmuFrame::Menu_KeyMovie_Record(wxCommandEvent &event)
{
	Menu_KeyMovie_Open(false);
}
void MainEmuFrame::Menu_KeyMovie_Play(wxCommandEvent &event)
{
	Menu_KeyMovie_Open(true);
}
void MainEmuFrame::Menu_KeyMovie_Stop(wxCommandEvent &event)
{
	TAS_KeyMovieStop();
}
void MainEmuFrame::Menu_KeyMovie_Open(bool fReadOnly)
{
	TAS_KeyMovieStop();

	// wxFileDialog
	wxFileDialog openFileDialog(this, _("Select P2M2 record file."), L"", L"",
		L"p2m2 file(*.p2m2)|*.p2m2",
		wxFD_OPEN);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		// cancel
		return;
	}
	wxString path = openFileDialog.GetPath();
	
	// start
	TAS_KeyMovieStart(path.c_str(), true, fReadOnly);
}



