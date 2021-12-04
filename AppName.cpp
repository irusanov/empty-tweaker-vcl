// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
// ---------------------------------------------------------------------------
USEFORM("MainAppForm.cpp", MainForm);
USEFORM("Windows\SettingsWindow.cpp", SettingsForm);
USEFORM("Windows\AboutWindow.cpp", AboutDialog);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
    try {
        Application->Initialize();
        Application->MainFormOnTaskBar = true;
        Application->Title = "AppName";
         Application->CreateForm(__classid(TMainForm), &MainForm);
         Application->CreateForm(__classid(TAboutDialog), &AboutDialog);
         Application->CreateForm(__classid(TSettingsForm), &SettingsForm);
         Application->Run();
    }
    catch (Exception &exception) {
        Application->ShowException(&exception);
    }
    catch (...) {
        try {
            throw Exception("");
        }
        catch (Exception &exception) {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
// ---------------------------------------------------------------------------
