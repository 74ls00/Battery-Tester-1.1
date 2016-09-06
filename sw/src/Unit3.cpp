//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"
#include "Unit1.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall GedData::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall GetData::GetData(bool CreateSuspended)
	: TThread(CreateSuspended)
{
FreeOnTerminate = true;
}
//---------------------------------------------------------------------------

void __fastcall GetData::Execute()
{
for (int i=0; i<4; i++)
	DischCount[i] = 0;

do
	{
	Sleep(3);
	FT_GetQueueStatus(Form1->ftHandle, &bytes);
	if (bytes < 13)
		continue;

	FT_Read(Form1->ftHandle, &buf, 1, &bytes);
	if (buf != 0x55)
		{
//		FT_Purge(Form1->ftHandle, FT_PURGE_RX | FT_PURGE_TX);
		continue;
		}
	Synchronize(&UpdateData);
	}
while (!Terminated);
}
//---------------------------------------------------------------------------

void __fastcall GetData::UpdateData(void)
{
Form1->VTime = (unsigned int *)realloc(Form1->VTime, (Form1->Count+1)*sizeof(int));
FT_Read(Form1->ftHandle, &time, 4, &bytes);
Form1->VTime[Form1->Count] = time;

hour = floor(time/3600000.0);
min = floor(time/60000.0);
min = min%60;
sec = floor(time/1000.0);
sec = sec%60;
if (hour < 10)
	strtime = "0" + IntToStr(hour) + ":";
else
	strtime = IntToStr(hour) + ":";
if (min < 10)
	strtime += "0" + IntToStr(min) + ":";
else
	strtime += IntToStr(min) + ":";
if (sec < 10)
	strtime += "0" + IntToStr(sec);
else
	strtime += IntToStr(sec);

if (Form1->Count > 1000)
	{
	Form1->Chart1->BottomAxis->Automatic = false;
	Form1->Chart1->BottomAxis->Maximum = Form1->Count;
	Form1->Chart1->BottomAxis->Minimum = Form1->Count-1000;
	}
else
	{
	Form1->Chart1->BottomAxis->Automatic = true;
	}

Form1->Series5->Clear();
Form1->Series6->Clear();
Form1->Series5->AddXY(0, 1, "", clBlack);
Form1->Series5->AddXY(Form1->Count, 1, "", clBlack);
Form1->Series6->AddXY(0, Form1->level, "", clBlack);
Form1->Series6->AddXY(Form1->Count, Form1->level, "", clBlack);

Form1->AutoSave << AnsiString(IntToStr((__int64)time)).c_str();
Form1->AutoSave << ";" << strtime.c_str();


for (int i=0; i<4; i++)
	{
	FT_Read(Form1->ftHandle, &data[i], 2, &bytes);

	if ((Form1->discharged[i]) || (!Form1->CheckBox7->Checked))
		Form1->Chart1->Series[i]->AddXY(Form1->Count, data[i]*Form1->CoeffMeas[i], strtime, clRed);

	Form1->V[i] = (float *)realloc(Form1->V[i], (Form1->Count+1)*sizeof(float));
	Form1->V[i][Form1->Count] = data[i]*Form1->CoeffMeas[i];

	Form1->A[i] = (float *)realloc(Form1->A[i], (Form1->Count+1)*sizeof(float));
	Form1->A[i][Form1->Count] = Form1->RealCurrent[i];

	FormatSettings.DecimalSeparator = ',';
	Form1->AutoSave << ";" << AnsiString(FloatToStrF(Form1->A[i][Form1->Count], ffFixed, 4, 0)).c_str() << ";" << AnsiString(FloatToStrF(Form1->V[i][Form1->Count], ffFixed, 4, 3)).c_str();
	FormatSettings.DecimalSeparator = '.';

	if (floor(1000.0*Form1->V[i][Form1->Count] +0.5)/1000.0 < Form1->level)
		{
		if (DischCount[i] == 5)
			{
			Form1->discharged[i] = 0;
			Form1->ChangeCurrent = true;
			}
		else
			DischCount[i]++;
		}
	else
		DischCount[i] = 0;

	if (Form1->discharged[i])
		Form1->Tdis[i] = Form1->Count;
	}

Form1->AutoSave << "\r\n";

if (Form1->ComboBox2->ItemIndex == 1) // constant power discharge
	{
	for (int i=0; i<4; i++)
		if (Form1->V[i][Form1->Count] != 0)
			Form1->mamperes[i] = Form1->Current*(1.5/Form1->V[i][Form1->Count]);
	Form1->ChangeCurrent = true;
	}

if (Form1->ComboBox2->ItemIndex == 0) // constant resistance discharge
	{
	for (int i=0; i < 4; i++)
		if (Form1->V[i][Form1->Count] != 0) {
			Form1->mamperes[i] = Form1->Current*(Form1->V[i][Form1->Count]/1.5);
		}
	Form1->ChangeCurrent = true;
	}

if (Form1->ChangeCurrent)
	{
	Form1->SetCurrent();
	Form1->ChangeCurrent = false;
	}

Form1->Count++;
}
//---------------------------------------------------------------------------
