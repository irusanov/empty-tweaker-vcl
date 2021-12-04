// ---------------------------------------------------------------------------

#ifndef MainAppFormH
#define MainAppFormH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Types.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IniFiles.hpp>

#include "Libs\OlsApi.h"
#include "Constants.h"
#include "Utils\Utils.h"
#include "Utils\QueryPerformance.h"
#include "Utils\AppSettings.h"

#include "Windows\AboutWindow.h"
#include "Windows\SettingsWindow.h"
#include "Components\TAdvancedEdit\TAdvancedEdit.h"
#include "Components\TTimingComboBox\TTimingComboBox.h"
#include <Vcl.ExtDlgs.hpp>

// ---------------------------------------------------------------------------
class TMainForm : public TForm {
__published: // IDE-managed Components
    TButton *ButtonApply;
    TButton *ButtonRefresh;
    TTabControl *TabControl1;
    TPanel *PanelDram;
    TPanel *PanelChipset;
    TPanel *PanelInfo;
    TMainMenu *MainMenu1;
    TMenuItem *File1;
    TMenuItem *Help1;
    TMenuItem *Exit1;
    TMenuItem *About1;
    TTrayIcon *TrayIcon;
    TMenuItem *ools1;
    TMenuItem *SettingsMenuItem;
    TMenuItem *N1;
    TPopupMenu *TrayPopupMenu;
    TMenuItem *TrayMenuExitApp;
    TMenuItem *TrayMenuShowApp;
    TMenuItem *N2;
    TMenuItem *Refresh1;
    TPanel *Panel1;

    void __fastcall TabControl1Change(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall ButtonRefreshClick(TObject *Sender);
    void __fastcall ButtonApplyClick(TObject *Sender);
    void __fastcall TabControl1DrawTab(TCustomTabControl *Control, int TabIndex,
        const TRect &Rect, bool Active);
    void __fastcall TrayIconDblClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall SettingsMenuItemClick(TObject *Sender);

protected:
    // virtual void __fastcall CreateParams(TCreateParams &Params);

private: // User declarations
    void __fastcall AppExit();
    void __fastcall OnMinimize(TObject *Sender);
    void __fastcall OnRestore(TObject *Sender);

public: // User declarations
    // MANID Register, MSR C001_001Eh
    typedef struct {
        unsigned char reticleSite; // [9-8]
        unsigned char majorRev; // [7-4]
        unsigned char minorRev; // [3-0]
    } man_id_t;

    typedef struct {
        unsigned int cpuid;
        string codeName;
        string cpuName;
        unsigned char family;
        unsigned char model;
        unsigned char extFamily;
        unsigned char extModel;
        unsigned char stepping;
        unsigned char patchLevel;
        double frequency;
        double fsbFromPll;
        double fsb;
        double multi;
        double dram;
        unsigned int pciMul;
        unsigned int pciDiv;
        unsigned char fsbDiv;
        unsigned char dramDiv;
        unsigned int maxVid;
        unsigned int startVid;
        unsigned int currVid;
        unsigned int maxFid;
        unsigned int startFid;
        unsigned int currFid;
        unsigned int fid;
        bool MP;
        int l1DataCache;
        int l1InstCache;
        int l1Cache;
        int l2Cache;
        man_id_t manID;
    } cpu_info_t;

    __fastcall TMainForm(TComponent* Owner);
    void __fastcall RefreshCpuSpeed();
    void __fastcall RefreshPciFrequency();
    bool __fastcall InitSystemInfo();

    cpu_info_t cpu_info;
    QueryPerformance qpc;
    AppSettings settings;
};

// ---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
// ---------------------------------------------------------------------------
#endif
