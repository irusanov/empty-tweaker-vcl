// ---------------------------------------------------------------------------

#ifndef TTimingComboBoxH
#define TTimingComboBoxH
// ---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>

// ---------------------------------------------------------------------------
class PACKAGE TTimingComboBox : public TComboBox {
private:
    bool initItems = true;
    bool tChanged;
    bool tCustomItems;
    bool tCustomValue;

    int tMin = 0;
    int tMax = 7;
    int tIndex = -1;
    int tValue = -1;
    int tItemValue = -1;
    void __fastcall OptionChange(TObject *Sender);
    void __fastcall DropDown(TObject *Sender);
    void __fastcall CloseUp(TObject *Sender);

    // void __fastcall DrawItem(TWinControl *Control, int Index, TRect &Rect,
    // TOwnerDrawState State);
protected:
    virtual void __fastcall CreateWnd();

public:
    __fastcall void setMin(int value) {
        if (value >= 0) {
            tMin = value;

            if (!tCustomItems) {
                Items->Clear();

                for (int i = tMin; i <= tMax; i++) {
                    Items->Add(IntToStr(i));
                }
            }
        }
    }

    __fastcall void setMax(int value) {
        if (value > 0 && value > tMin) {
            tMax = value;

            if (!tCustomItems) {
                Items->Clear();

                for (int i = tMin; i <= tMax; i++) {
                    Items->Add(IntToStr(i));
                }
            }
        }
    }

    __fastcall int getValue() {
        return this->ItemIndex + tMin;
    }

    __fastcall int getItemValue() {
        if (!tCustomValue) {
            return -1;
        }

        int index = this->ItemIndex;
        if (index > -1) {
            return StrToInt(this->Items->Strings[index]);
        }

        return -1;
    }

    __fastcall void setValue(int value);
    __fastcall void setItemValue(int value);

    __fastcall TTimingComboBox(TComponent* Owner);

__published:
    __property bool Changed = {read = tChanged};
    __property bool CustomItems = {read = tCustomItems, write = tCustomItems};
    __property bool CustomValue = {read = tCustomValue, write = tCustomValue};
    __property int Min = {read = tMin, write = setMin};
    __property int Max = {read = tMax, write = setMax};
    __property int Value = {read = getValue, write = setValue};
    __property int ItemValue = {read = getItemValue, write = setItemValue};
    __fastcall void setChanged();
};
// ---------------------------------------------------------------------------
#endif
