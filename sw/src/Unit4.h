//---------------------------------------------------------------------------

#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TLabel *Label4;
	TLabel *Label5;
	TButton *Button2;
	TEdit *Edit6;
	TLabel *Label8;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TEdit *Edit7;
	TEdit *Edit8;
	TEdit *Edit9;
	TEdit *Edit10;
	TEdit *Edit11;
	TEdit *Edit12;
	TEdit *Edit13;
	TEdit *Edit14;
	void __fastcall Edit1KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit2KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit3KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit4KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit5KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit7KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit8KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit9KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit10KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Edit11KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit12KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit13KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit14KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	float Meas[4];
	float Set[4];
	float x1[4];
	float y1[4];
	float x2[4];
	float y2[4];
	float Offset[4];
public:		// User declarations
	__fastcall TForm4(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif
