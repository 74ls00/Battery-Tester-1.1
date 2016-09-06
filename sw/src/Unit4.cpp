//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include "Unit1.h"
#include "FTD2XX.H"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;
//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TForm4::Edit1KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	__int16 data = StrToInt(Edit1->Text);
	data = data << 2; // two LS bits are insignificant with AD5317 10-bit DAC

	DWORD bytes;

	Form1->SendCommand(0x31);
	for (int i=0; i<4; i++)
	{
		FT_Write(Form1->ftHandle, &data, 2, &bytes);
	}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit2KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	unsigned __int32 time;
	__int16 data[4];
	DWORD bytes;
	char command = 0x55;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0xAA;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0x30;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	Sleep(100);

	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	if (bytes < 13)
		return;

	FT_Read(Form1->ftHandle, &time, 1, &bytes);
	FT_Read(Form1->ftHandle, &time, 4, &bytes);
	for (int i=0; i<4; i++)
		{
		FT_Read(Form1->ftHandle, &data[i], 2, &bytes);
		}

	Meas[0] = StrToFloat(Edit2->Text)/(float)data[0];
	Label4->Caption = FloatToStrF(Meas[0], ffFixed, 5, 5);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm4::Edit3KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	unsigned __int32 time;
	__int16 data[4];
	DWORD bytes;
	char command = 0x55;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0xAA;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0x30;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	Sleep(100);

	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	if (bytes < 13)
		return;

	FT_Read(Form1->ftHandle, &time, 1, &bytes);
	FT_Read(Form1->ftHandle, &time, 4, &bytes);
	for (int i=0; i<4; i++)
		{
		FT_Read(Form1->ftHandle, &data[i], 2, &bytes);
		}

	Meas[1] = StrToFloat(Edit3->Text)/(float)data[1];
	Label5->Caption = FloatToStrF(Meas[1], ffFixed, 5, 5);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm4::Edit4KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	unsigned __int32 time;
	__int16 data[4];
	DWORD bytes;
	char command = 0x55;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0xAA;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0x30;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	Sleep(100);

	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	if (bytes < 13)
		return;

	FT_Read(Form1->ftHandle, &time, 1, &bytes);
	FT_Read(Form1->ftHandle, &time, 4, &bytes);
	for (int i=0; i<4; i++)
		{
		FT_Read(Form1->ftHandle, &data[i], 2, &bytes);
		}

	Meas[2] = StrToFloat(Edit4->Text)/(float)data[2];
	Label6->Caption = FloatToStrF(Meas[2], ffFixed, 5, 5);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit5KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
if (Key == VK_RETURN)
	{
	unsigned __int32 time;
	__int16 data[4];
	DWORD bytes;
	char command = 0x55;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0xAA;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	command = 0x30;
	FT_Write(Form1->ftHandle, &command, 1, &bytes);
	Sleep(100);

	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	if (bytes < 13)
		return;

	FT_Read(Form1->ftHandle, &time, 1, &bytes);
	FT_Read(Form1->ftHandle, &time, 4, &bytes);
	for (int i=0; i<4; i++)
		{
		FT_Read(Form1->ftHandle, &data[i], 2, &bytes);
		}

	Meas[3] = StrToFloat(Edit5->Text)/(float)data[3];
	Label7->Caption = FloatToStrF(Meas[3], ffFixed, 5, 5);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm4::Edit7KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x1[0] = StrToFloat(Edit7->Text);
	y1[0] = StrToFloat(Edit1->Text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit8KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x1[1] = StrToFloat(Edit8->Text);
	y1[1] = StrToFloat(Edit1->Text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit9KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x1[2] = StrToFloat(Edit9->Text);
	y1[2] = StrToFloat(Edit1->Text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit10KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x1[3] = StrToFloat(Edit10->Text);
	y1[3] = StrToFloat(Edit1->Text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Button2Click(TObject *Sender)
{
unsigned long bytes;
__int16 version = StrToInt(Edit6->Text);

char command = 0x55;
FT_Write(Form1->ftHandle, &command, 1, &bytes);
command = 0xAA;
FT_Write(Form1->ftHandle, &command, 1, &bytes);
command = 0x41;
FT_Write(Form1->ftHandle, &command, 1, &bytes);

FT_Write(Form1->ftHandle, &version, 2, &bytes);
for (int i=0; i<4; i++)
	{
	FT_Write(Form1->ftHandle, &Meas[i], 4, &bytes);
	FT_Write(Form1->ftHandle, &Set[i], 4, &bytes);
	FT_Write(Form1->ftHandle, &Offset[i], 4, &bytes);
	}

Sleep(250);
// verification
command = 0x55;
FT_Write(Form1->ftHandle, &command, 1, &bytes);
command = 0xAA;
FT_Write(Form1->ftHandle, &command, 1, &bytes);
command = 0x40;
FT_Write(Form1->ftHandle, &command, 1, &bytes);

char readbuf;
int count = 0;
do
	{
	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	if (bytes > 0)
		FT_Read(Form1->ftHandle, &readbuf, 1, &bytes);
	Sleep(1);
	count ++;
	if (count > 250)
		{
		MessageBox(Form4->Handle, L"Timeout reading EEPROM (1)", L"Error", MB_OK|MB_ICONERROR);
		return;
		}
	}
while (readbuf != 0x55);

count = 0;
do
	{
	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	count++;
	if (count > 250)
		{
		MessageBox(Form4->Handle, L"Timeout reading EEPROM (2)", L"Error", MB_OK|MB_ICONERROR);
		return;
		}
	}
while (readbuf < 50);

char buf[50];
FT_Read(Form1->ftHandle, buf, 50, &bytes);
if (bytes != 50)
	{
	MessageBox(Form4->Handle, L"Error reading EEPROM", L"Error", MB_OK|MB_ICONERROR);
	return;
	}

char bad = 0;
//bad += memcmp(&FwVer, buf, 2);
for (int i=0; i<4; i++)
	{
	bad += memcmp(&Meas[i], buf+2+12*i, 4);
	bad += memcmp(&Set[i], buf+6+12*i, 4);
	bad += memcmp(&Offset[i], buf+10+12*i, 4);
	}

if (bad)
	MessageBox(Form4->Handle, L"Write failed", L"Error", MB_OK|MB_ICONERROR);
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit11KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x2[0] = StrToFloat(Edit11->Text);
	y2[0] = StrToFloat(Edit1->Text);
	Set[0] = (y2[0]-y1[0])/(x2[0]-x1[0]);
	Offset[0] = y1[0] - Set[0]*x1[0];
	Label9->Caption = FloatToStrF(Set[0], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[0], ffFixed, 5, 4);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit12KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x2[1] = StrToFloat(Edit12->Text);
	y2[1] = StrToFloat(Edit1->Text);
	Set[1] = (y2[1]-y1[1])/(x2[1]-x1[1]);
	Offset[1] = y1[1] - Set[1]*x1[1];
	Label10->Caption = FloatToStrF(Set[1], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[1], ffFixed, 5, 4);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit13KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x2[2] = StrToFloat(Edit13->Text);
	y2[2] = StrToFloat(Edit1->Text);
	Set[2] = (y2[2]-y1[2])/(x2[2]-x1[2]);
	Offset[2] = y1[2] - Set[2]*x1[2];
	Label11->Caption = FloatToStrF(Set[2], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[2], ffFixed, 5, 4);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Edit14KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	x2[3] = StrToFloat(Edit14->Text);
	y2[3] = StrToFloat(Edit1->Text);
	Set[3] = (y2[3]-y1[3])/(x2[3]-x1[3]);
	Offset[3] = y1[3] - Set[3]*x1[3];
	Label12->Caption = FloatToStrF(Set[3], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[3], ffFixed, 5, 4);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::FormShow(TObject *Sender)
{
for (int i=0; i<4; i++)
	{
	Meas[i] = Form1->CoeffMeas[i];
	Offset[i] = Form1->CoeffSetOffset[i];
	Set[i] = Form1->CoeffSet[i];
	}
Label4->Caption = FloatToStrF(Meas[0], ffFixed, 5, 5);
Label5->Caption = FloatToStrF(Meas[1], ffFixed, 5, 5);
Label6->Caption = FloatToStrF(Meas[2], ffFixed, 5, 5);
Label7->Caption = FloatToStrF(Meas[3], ffFixed, 5, 5);
Label9->Caption = FloatToStrF(Set[0], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[0], ffFixed, 5, 4);
Label10->Caption = FloatToStrF(Set[1], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[1], ffFixed, 5, 4);
Label11->Caption = FloatToStrF(Set[2], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[2], ffFixed, 5, 4);
Label12->Caption = FloatToStrF(Set[3], ffFixed, 5, 4) + "x + " + FloatToStrF(Offset[3], ffFixed, 5, 4);
}
//---------------------------------------------------------------------------

