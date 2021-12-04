#include <vcl.h>
#pragma hdrstop

#include "MainAppForm.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TAdvancedEdit"
#pragma link "TTimingComboBox"
#pragma resource "*.dfm"
TMainForm *MainForm;

enum CodeName {
    Sample,
};

struct timing_def_t {
    UnicodeString name;
    unsigned int bus;
    unsigned int device;
    unsigned int function;
    unsigned int reg;
    unsigned int offset;
    unsigned int bits;
};

const struct timing_def_t timingDefs[] = {
    // name,                    b,  d,  f, reg, offset, bits
    { "Sample",                 -1,  0,  0,  0,  0,  0 },
};

const int NUMBER_OF_TABS = 3;
bool updateFromButtons = false;

// Get timing definition by component name
static timing_def_t __fastcall GetDefByName(const struct timing_def_t* table,
    int size, UnicodeString name) {
    for (int i = 0; i < size; i++) {
        if (table[i].name == name) {
            return table[i];
        }
    }

    return {
    };
}
// ---------------------------------------------------------------------------

// Read CPU and system info, invoked once
bool __fastcall TMainForm::InitSystemInfo() {
    DWORD eax = 0, ebx = 0, ecx = 0, edx = 0;

    // CPUID information
    if (Cpuid(0x00000001, &eax, &ebx, &ecx, &edx)) {
        cpu_info.cpuid = eax;
        cpu_info.stepping = eax & 0xf;
        cpu_info.family = eax >> 8 & 0xf;
        cpu_info.model = eax >> 4 & 0xf;
    }
    else {
        return false;
    }

    // Extended CPUID information
    if (Cpuid(0x80000001, &eax, &ebx, &ecx, &edx)) {
        cpu_info.extFamily = eax >> 8 & 0xf;
        cpu_info.extModel = eax >> 4 & 0xf;
        cpu_info.MP = edx >> FEATURE_K7_ECC_BIT & 1;
    }
    else {
        return false;
    }

    // L1 Data cache
    if (Cpuid(0x80000005, &eax, &ebx, &ecx, &edx)) {
        cpu_info.l1DataCache = (ecx >> 24 & 0xff);
        cpu_info.l1InstCache = (edx >> 24 & 0xff);
        cpu_info.l1Cache = cpu_info.l1DataCache + cpu_info.l1InstCache;
    }

    // L2 cache
    if (Cpuid(0x80000006, &eax, &ebx, &ecx, &edx)) {
        cpu_info.l2Cache = (ecx >> 16 & 0xffff);
    }

    if (Rdmsr(MSR_K7_MANID, &eax, &edx)) {
        cpu_info.manID.minorRev = GetBits(eax, 0, 4);
        cpu_info.manID.majorRev = GetBits(eax, 4, 4);
        cpu_info.manID.reticleSite = GetBits(eax, 8, 2);
    }

    // Read CPU FID and VID values
    if (Rdmsr(MSR_K7_FID_VID_STATUS, &eax, &edx)) {
        cpu_info.currVid = GetBits(edx, 0, 6);
        cpu_info.startVid = GetBits(edx, 8, 6);
        cpu_info.maxVid = GetBits(edx, 16, 6);

        cpu_info.currFid = GetBits(eax, 0, 6);
        cpu_info.startFid = GetBits(eax, 8, 6);
        cpu_info.maxFid = GetBits(eax, 16, 6);
    }

    cpu_info.cpuName = GetCpuName();

    switch (cpu_info.cpuid) {
    default: ;
    }

    return true;
}
// ---------------------------------------------------------------------------

static void __fastcall ReadTimings(const struct timing_def_t* table, int size) {
    timing_def_t def;
    unsigned int pciAddress, regValue, value;
    TTimingComboBox* combo;
    UnicodeString name;

    for (int i = 0; i < size; i++) {
        name = table[i].name;
        def = GetDefByName(table, size, name);
        combo = static_cast<TTimingComboBox*>
            (Application->FindComponent("MainForm")->FindComponent(name));

        if (combo != NULL && combo != 0 && sizeof(def) > 0) {
            pciAddress = MakePciAddress(def.bus, def.device, def.function,
                def.reg);
            regValue = ReadPciReg(pciAddress);
            value = GetBits(regValue, def.offset, def.bits);

            if (combo->CustomValue) {
                combo->ItemValue = value;
            }
            else {
                combo->Value = value;
            }

            // MessageDlg(def.name, mtConfirmation, mbOKCancel, 0);
        }
    }
}
// ---------------------------------------------------------------------------

static void __fastcall WriteTimings(const struct timing_def_t* table, int size,
    bool doubled) {
    timing_def_t def;
    unsigned int pciAddress, regValue, value, bits;
    TTimingComboBox* combo;
    UnicodeString name;

    for (int i = 0; i < size; i++) {
        name = table[i].name;

        combo = static_cast<TTimingComboBox*>
            (Application->FindComponent("MainForm")->FindComponent(name));

        if (combo != NULL && combo != 0 && combo->Changed) {
            def = GetDefByName(table, size, name);
            bits = def.bits;
            pciAddress = MakePciAddress(def.bus, def.device, def.function, def.reg);
            regValue = ReadPciReg(pciAddress);

            if (combo->CustomValue) {
                value = (unsigned int) combo->ItemValue;
            }
            else {
                value = (unsigned int) combo->Value;
            }

            if (doubled) {
                value = (value << 4) | value;
                bits *= 2;
            }

            regValue = SetBits(regValue, def.offset, bits, value);
            WritePciReg(pciAddress, regValue);
        }
    }
}
// ---------------------------------------------------------------------------

static void __fastcall RefreshTimings() {
    ReadTimings(timingDefs, COUNT_OF(timingDefs));
}
// ---------------------------------------------------------------------------

// Main
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner) {
    if (!InitializeOls()) {
        MessageDlg("Error initializing OpenLibSys", mtError, mbOKCancel, 0);
        Application->Terminate();
    }

    if (GetDllStatus() != 0x0) {
        MessageDlg("Error loading WinRing.dll", mtError, mbOKCancel, 0);
        AppExit();
    }

    Application->OnMinimize = OnMinimize;
    Application->OnRestore = OnRestore;

    InitSystemInfo();
    RefreshTimings();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::TabControl1Change(TObject *Sender) {
    TPanel* TabPanels[NUMBER_OF_TABS] = {PanelDram, PanelChipset, PanelInfo};
    int index = TabControl1->TabIndex;

    TPanel *Panel;
    for (int i = 0; i < NUMBER_OF_TABS; i++) {
        Panel = TabPanels[i];
        Panel->Visible = (i == index);
    }

    switch (index) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    default: ;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::ButtonRefreshClick(TObject *Sender) {
    RefreshTimings();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::ButtonApplyClick(TObject *Sender) {
    switch (TabControl1->TabIndex) {
    case 0:
        WriteTimings(timingDefs, COUNT_OF(timingDefs), false);
        break;
    case 1:
        break;
    default: ;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::AppExit() {
    DeinitializeOls();
    Application->Terminate();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::Exit1Click(TObject *Sender) {
    AppExit();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::About1Click(TObject *Sender) {
    AboutDialog->ShowModal();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::TabControl1DrawTab(TCustomTabControl *Control,
    int TabIndex, const TRect &Rect, bool Active) {
    int left = Active ? 9 : 5;
    int top = Active ? 3 : 2;

    Control->Canvas->Brush->Color = clBtnFace;
    Control->Canvas->Font->Color = Active ? clBlue : clWindowText;
    Control->Canvas->FillRect(Rect);

    Control->Canvas->TextOut(Rect.Left + left, Rect.Top + top,
        (static_cast<TTabControl *>(Control))->Tabs->Strings[TabIndex]);
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::OnMinimize(TObject *Sender) {
    // If minimize to tray enabled
    if (settings.MinimizeToTray) {
        TrayIcon->Visible = true;
        Application->MainFormOnTaskBar = false;
        Application->MainForm->Hide();
        // WindowState = wsMinimized;

        if (!settings.MinimizeHintShown) {
            TrayIcon->Animate = true;
            TrayIcon->ShowBalloonHint();
        }
    }

    TabControl1->Visible = false;
    ButtonApply->Visible = false;
    ButtonRefresh->Visible = false;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::OnRestore(TObject *Sender) {
    // If minimize to tray enabled
    if (settings.MinimizeToTray) {
        TrayIcon->Visible = false;
        Application->MainFormOnTaskBar = true;

        if (!settings.MinimizeHintShown) {
            TrayIcon->BalloonHint = NULL;
            settings.MinimizeHintShown = true;
            settings.save();
        }
    }

    TabControl1->Visible = true;
    ButtonApply->Visible = true;
    ButtonRefresh->Visible = true;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::TrayIconDblClick(TObject *Sender) {
    Application->Restore();
    Application->MainForm->Show();
    // WindowState = wsNormal;
    Application->MainForm->BringToFront();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender) {
    DeinitializeOls();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action) {
    settings.save();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender) {
    settings.load();

    if (settings.SaveWindowPosition) {
        Position = poDefault;
        MainForm->Top = settings.WindowTop;
        MainForm->Left = settings.WindowLeft;
    }
    else {
        Position = poDesktopCenter;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::SettingsMenuItemClick(TObject *Sender) {
    SettingsForm->ShowModal();
}
// ---------------------------------------------------------------------------
