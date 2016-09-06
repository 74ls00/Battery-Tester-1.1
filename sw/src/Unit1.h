//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <VCLTee.Chart.hpp>
#include <ExtCtrls.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Buttons.hpp>
#include <IniFiles.hpp>
#include <Dialogs.hpp>
#include <math.h>
#include <pngimage.hpp>
#include <fstream.h>
#include <strsafe.h>
#include <windows.h>

#include "FTD2XX.H"
#include "Unit3.h"
#include <ComCtrls.hpp>
#include <Mask.hpp>
#include <VclTee.TeeGDIPlus.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TChart *Chart1;
	TFastLineSeries *Series1;
	TFastLineSeries *Series2;
	TFastLineSeries *Series3;
	TFastLineSeries *Series4;
	TGroupBox *GroupBox1;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TLabel *Label4;
	TLabel *Label3;
	TLabel *Label2;
	TLabel *Label1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *CheckBox4;
	TCheckBox *CheckBox5;
	TSpeedButton *SpeedButton1;
	TTimer *Timer1;
	TButton *Button1;
	TButton *Button2;
	TGroupBox *GroupBox3;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TSaveDialog *SaveDialog1;
	TButton *Button3;
	TOpenDialog *OpenDialog1;
	TFastLineSeries *Series5;
	TFastLineSeries *Series6;
	TSpeedButton *SpeedButton2;
	TGroupBox *GroupBox2;
	TEdit *Edit5;
	TComboBox *ComboBox1;
	TLabel *Label5;
	TLabel *Label7;
	TComboBox *ComboBox2;
	TLabel *Label8;
	TCheckBox *CheckBox6;
	TEdit *Edit6;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TEdit *Edit7;
	TLabel *Label12;
	TEdit *Edit8;
	TLabel *Label18;
	TLabel *Label19;
	TStatusBar *StatusBar1;
	TCheckBox *CheckBox7;
	TShape *Shape1;
	TShape *Shape2;
	TShape *Shape3;
	TShape *Shape4;
	TButton *Button4;
	TLabel *Label6;
	TComboBox *ComboBox3;
	TLabel *Label20;
	TComboBox *ComboBox4;
	TLabel *Label21;
	TComboBox *ComboBox5;
	void __fastcall Edit1KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall CheckBox3Click(TObject *Sender);
	void __fastcall CheckBox4Click(TObject *Sender);
	void __fastcall CheckBox5Click(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Edit10KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall CheckBox6Click(TObject *Sender);
	void __fastcall Edit5KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit5Change(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Edit6KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ComboBox3Change(TObject *Sender);
	void __fastcall Edit5KeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Edit6KeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Edit7KeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Edit8KeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Edit7MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Edit8MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Edit5MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Edit6MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Edit5KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit7KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit8KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit6KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ComboBox4Change(TObject *Sender);
private:	// User declarations
	void __fastcall BatteryLabels(void);
	void __fastcall Stop(void);
	int InitDevice(void);
	GetData *DataThread;

	TTime StartTime;
	float Discharge[4];
	int res;
	bool DataUnchanged;
	int ZeroCount;
	bool PulseActive;
	int PulseCount;
	int totaldischarge;
	float mOhms[4];
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	int __fastcall SetCurrent(void);
	int __fastcall SetLEDs(void);
	int __fastcall SendCommand(char code);
	bool LED[8];
	float mamperes[4];
	float Current;
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	float *V[4];
	float *A[4];
	float RealCurrent[4];
	unsigned __int32 *VTime;
	float CoeffMeas[4];
	float CoeffSet[4];
	float CoeffSetOffset[4];
	__int16 FwVer;
	int Count;
	float level;
	ofstream AutoSave;
	int discharged[4];
	int Tdis[4];
	bool ChangeCurrent;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
