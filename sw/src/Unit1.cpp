//---------------------------------------------------------------------------

#include <vcl.h>
#include <fstream.h>
#include <string>

#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#include "fft.h"

#include "Unit4.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
FormatSettings.DecimalSeparator = '.';
DataUnchanged = false;

TIniFile *ini = new TIniFile(ExtractFilePath(Application->ExeName) + "btest.ini");
Form1->Left = ini->ReadInteger("Settings", "Left", 0);
Form1->Top = ini->ReadInteger("Settings", "Top", 0);
Button4->Visible = ini->ReadBool("Settings", "Dev", 0);

ComboBox1->ItemIndex = ini->ReadInteger("Discharge", "Depth", 0);
ComboBox2->ItemIndex = ini->ReadInteger("Discharge", "Mode", 2);
Edit5->Text = ini->ReadString("Discharge", "Current1", 0);
Edit6->Text = ini->ReadString("Discharge", "Current2", 0);
CheckBox6->Checked = ini->ReadBool("Discharge", "Pulse", false);
Edit7->Text = ini->ReadString("Discharge", "Duration1", 1);
Edit8->Text = ini->ReadString("Discharge", "Duration2", 1);
CheckBox7->Checked = ini->ReadBool("Discharge", "Autodisable", true);
CheckBox5->Checked = ini->ReadBool("Graph", "ShowLegend", true);

delete ini;

Chart1->LeftAxis->Maximum = 1.7;
Chart1->LeftAxis->Minimum = 0.5;
Chart1->LeftAxis->Increment = 0.1;

Count = 0;

for (int i=0; i<4; i++)
	V[i] = NULL;

ComboBox4->ItemIndex = ini->ReadInteger("Discharge", "Chemistry", 0);
ComboBox4Change(NULL);
}
//---------------------------------------------------------------------------

int TForm1::InitDevice(void)
{
AnsiString serial = "BATTEST1";

ftStatus = FT_OpenEx(serial.c_str(), FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
if (ftStatus != FT_OK)
	{
	MessageBox(Form1->Handle, L"Device not found", L"Error", MB_OK|MB_ICONERROR);
	return 1;
	}

FT_SetLatencyTimer(ftHandle, 5);
ftStatus = FT_SetUSBParameters(ftHandle, 64, 0);
if (ftStatus)
	MessageBox(Form1->Handle, L"Error setting USB buffer size", L"Error", MB_OK|MB_ICONERROR);

Sleep(100);

FT_SetBaudRate(ftHandle, 115200);
FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);

DWORD bytes;

SendCommand(0x40);
Sleep(100);

int count = 0;
do
	{
	FT_GetQueueStatus(ftHandle, &bytes);
	Sleep(1);
	count++;
	if (count == 100)
		{
		MessageBox(Form1->Handle, L"Timeout reading EEPROM", L"Error", MB_OK|MB_ICONERROR);
		FT_Close(ftHandle);
		return 1;
		}
	}
while (bytes != 51);

char control;
FT_Read(ftHandle, &control, 1, &bytes);
if ((bytes != 1) || (control != 0x55))
	{
	MessageBox(Form1->Handle, L"Error reading EEPROM", L"Error", MB_OK|MB_ICONERROR);
	FT_Close(ftHandle);
	return 1;
	}

char buf[50];
FT_Read(ftHandle, buf, 50, &bytes);
if (bytes != 50)
	{
	MessageBox(Form1->Handle, L"Error reading calibration data", L"Error", MB_OK|MB_ICONERROR);
	FT_Close(ftHandle);
	delete [] buf;
	return 1;
	}
// first 2 bytes — firmware revision
// 4 bytes — voltage (float)
// 4 bytes - current (float)
// 4 bytes - current offset (float)

memcpy(&FwVer, buf, 2);
for (int i=0; i<4; i++)
	{
	memcpy(&CoeffMeas[i], buf+2+12*i, 4);
	memcpy(&CoeffSet[i], buf+6+12*i, 4);
	memcpy(&CoeffSetOffset[i], buf+10+12*i, 4);
	}

return 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1KeyUp(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	Series1->Title=Edit1->Text;
	Series2->Title=Edit2->Text;
	Series3->Title=Edit3->Text;
	Series4->Title=Edit4->Text;
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
Series1->Active = CheckBox1->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
Series2->Active = CheckBox2->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox3Click(TObject *Sender)
{
Series3->Active = CheckBox3->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox4Click(TObject *Sender)
{
Series4->Active = CheckBox4->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox5Click(TObject *Sender)
{
Chart1->Legend->Visible = CheckBox5->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
if (SpeedButton1->Down)
	{
	ZeroCount = 0;
	PulseCount = 0;
	PulseActive = true;
	Sleep(100);
	StartTime = Time();
	Series1->Clear();
	Series2->Clear();
	Series3->Clear();
	Series4->Clear();
	Series5->Clear();
	Series6->Clear();
	Button3->Enabled = false;
	ComboBox3->Enabled = false;

	if (CheckBox6->Checked)
		Chart1->Title->Caption =  "Импульсный разряд, " + Edit5->Text + "/" + Edit6->Text + " мА";
	else
		Chart1->Title->Caption =  "Постоянный разряд, " + Edit5->Text + " мА";

	AnsiString fn = ExtractFilePath(Application->ExeName) + "autosave.csv";
	AutoSave.open(fn.c_str(), ios::out | ios::trunc);
	FormatSettings.DecimalSeparator = ',';
	AutoSave << "Battery test data ver. 2.0" << "\r\n";
	AutoSave << AnsiString(Chart1->Title->Caption).c_str() << "\r\n";
	AutoSave << AnsiString(BoolToStr(CheckBox5->Checked, true)).c_str() << "\r\n";
	AutoSave << AnsiString(BoolToStr(CheckBox1->Checked, true)).c_str() << ";" << AnsiString(Edit1->Text).c_str() << "\r\n";
	AutoSave << AnsiString(BoolToStr(CheckBox2->Checked, true)).c_str() << ";" << AnsiString(Edit2->Text).c_str() << "\r\n";
	AutoSave << AnsiString(BoolToStr(CheckBox3->Checked, true)).c_str() << ";" << AnsiString(Edit3->Text).c_str() << "\r\n";
	AutoSave << AnsiString(BoolToStr(CheckBox4->Checked, true)).c_str() << ";" << AnsiString(Edit4->Text).c_str() << "\r\n";
	AutoSave << AnsiString(FloatToStrF(level, ffFixed, 2, 1)).c_str() << "\r\n";
	AutoSave << "Time, ms;Time, h:m:s;Ch1, mA;Ch1, V;Ch2, mA;Ch2, V;Ch3, mA;Ch3, V;Ch4, mA;Ch4, V" << "\r\n";
	FormatSettings.DecimalSeparator = '.';

	for (int i=0; i<4; i++)
		{
		mOhms[i] = 0;
		discharged[i] = 1;
		}
	totaldischarge = 4;

	for (int i=0; i<4; i++)
		mamperes[i] = StrToInt(Edit5->Text);
	SetCurrent();
	ChangeCurrent = false;
	Current = StrToInt(Edit5->Text);

	SendCommand(0x33);  // start auto-measurements

	Count = 0;
	DataThread = new GetData(false);
	Timer1->Enabled = true;
	StartTime = Time();
	Button3->Enabled = false;
	SpeedButton2->Enabled = false;
	}
else
	{
	Stop();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Stop(void)
{
Timer1->Enabled = false;
StatusBar1->SimpleText = "";

SendCommand(0x34); // stop auto-measurements

Sleep(200);
DataThread->Terminate();
Sleep(200);

Chart1->BottomAxis->Minimum = 0;
Chart1->BottomAxis->Maximum = Count;
Chart1->BottomAxis->Automatic = true;

for (int i=0; i<4; i++)
	mamperes[i] = 0;
SetCurrent();

for (int i=0; i<7; i++)
	LED[i] = false;
SetLEDs();

AutoSave.close();
Button3->Enabled = true;
SpeedButton2->Enabled = true;
SpeedButton1->Down = false;
ComboBox3->Enabled = true;
ComboBox3Change(NULL);
// BatteryLabels();

Label13->Caption = "00:00:00";
}
//---------------------------------------------------------------------------

int __fastcall TForm1::SendCommand(char code)
{
DWORD bytes;
__int16 command = 0xAA55;
FT_Write(ftHandle, &command, 2, &bytes);
FT_Write(ftHandle, &code, 1, &bytes);
return 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
FormatSettings.DecimalSeparator = '.';

Label13->Caption = TimeToStr(Time() - StartTime);

PulseCount++;
if (CheckBox6->Checked)
	{
	if (PulseActive)
		{
		if (PulseCount >= 2*StrToInt(Edit7->Text))
			{
			PulseActive = false;
			PulseCount = 0;
			for (int i=0; i<4; i++)
				mamperes[i] = StrToInt(Edit6->Text);
			Current = StrToInt(Edit6->Text);
			ChangeCurrent = true;
			}
		}
	else
		{
		if (PulseCount >= 2*StrToInt(Edit8->Text))
			{
			PulseActive = true;
			PulseCount = 0;
			for (int i=0; i<4; i++)
				mamperes[i] = StrToInt(Edit5->Text);
			Current = StrToInt(Edit5->Text);
			ChangeCurrent = true;
			}
		}
	}

// расчёт внутреннего сопротивления
int time = 5*10*(StrToInt(Edit7->Text) + StrToInt(Edit8->Text));  // за последние 5 циклов
if ((CheckBox6->Checked) && (Count > time))
	{
	float MinU;
	float MaxU;
	float MinI;
	float MaxI;
	for (int j=0; j<4; j++)
		{
		MaxU = 0;
		MinU = 3;
		MinI = 5000;
		MaxI = 0;

		for (int i=(Count-time); i<Count-5; i++)
			{
			if (V[j][i] > MaxU)
				MaxU = V[j][i];
			if (V[j][i] < MinU)
				MinU = V[j][i];

			if (A[j][i] > MaxI)
				MaxI = A[j][i];
			if (A[j][i] < MinI)
				MinI = A[j][i];
			}
		if ((MaxI-MinI) > 10)
			mOhms[j] = 1E6*(MaxU-MinU)/(MaxI-MinI);
		else
			mOhms[j] = 0;
		}
	}


if ((totaldischarge == 0) && (CheckBox7->Checked))
	{
	Stop();
	MessageBox(Form1->Handle, L"Все элементы разрядились\nдо указанного предела", L"Остановка", MB_OK|MB_ICONINFORMATION);
	}

LED[0] = true;

totaldischarge = 0;
for (int i=0; i<4; i++)
	{
	if (discharged[i] == 0)
		LED[i+1] = !LED[i+1];
	else
		LED[i+1] = true;

	if (V[i][Count-1] < 0.05)
		LED[i+1] = false;

	totaldischarge += discharged[i];
	}
SetLEDs();

bool IsZero = true;
for (int j=0; j<4; j++)
	{
	if (V[j][Count-1] > 0.4)
		IsZero = false;
	}

if (IsZero)
	ZeroCount++;
else
	ZeroCount = 0;

if (ZeroCount > 5)
	{
	Stop();
	MessageBox(Form1->Handle, L"Все элементы разрядились до нуля", L"Остановка", MB_OK|MB_ICONINFORMATION);
	}

StatusBar1->SimpleText = "Разрядный ток: 1 — " + FloatToStrF(A[0][Count-1], ffFixed, 4, 0)
						+ " мА; 2 — " + FloatToStrF(A[1][Count-1], ffFixed, 4, 0)
						+ " мА; 3 — " + FloatToStrF(A[2][Count-1], ffFixed, 4, 0)
						+ " мА; 4  — " + FloatToStrF(A[3][Count-1], ffFixed, 4, 0) + "мА";

BatteryLabels();

DataUnchanged = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
if (Timer1->Enabled)
	{
	Timer1->Enabled = false;
	AutoSave.close();
	}

if (SpeedButton2->Down)
	{
	SendCommand(0x34); // stop auto-measurements
	for (int i=0; i<4; i++)
		mamperes[i] = 0;
	SetCurrent();
	Sleep(200);
	FT_Close(ftHandle);
	}

TIniFile *ini = new TIniFile(ExtractFilePath(Application->ExeName) + "btest.ini");
ini->WriteInteger("Discharge", "Chemistry", ComboBox4->ItemIndex);
ini->WriteInteger("Settings", "Left", Form1->Left);
ini->WriteInteger("Settings", "Top", Form1->Top);

ini->WriteInteger("Discharge", "Depth", ComboBox1->ItemIndex);
ini->WriteInteger("Discharge", "Mode", ComboBox2->ItemIndex);
ini->WriteString("Discharge", "Current1", Edit5->Text);
ini->WriteString("Discharge", "Current2", Edit6->Text);
ini->WriteBool("Discharge", "Pulse", CheckBox6->Checked);
ini->WriteString("Discharge", "Duration1", Edit7->Text);
ini->WriteString("Discharge", "Duration2", Edit8->Text);
ini->WriteBool("Discharge", "Autodisable", CheckBox7->Checked);

ini->WriteBool("Graph", "ShowLegend", CheckBox5->Checked);

delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
if (!Count)
	return;

SaveDialog1->Filter = "CSV files (*.csv)|*.csv|*.CSV|Text files (*.txt)|*.txt|*.TXT|All files (*.*)|*.*";
SaveDialog1->Title = "Save data";
SaveDialog1->DefaultExt = "csv";

if (SaveDialog1->Execute())
	{
	FormatSettings.DecimalSeparator = ',';

	if (FileExists(SaveDialog1->FileName))
		if (MessageBox(Form1->Handle, L"File exists. Overwrite?", L"File exists", MB_ICONWARNING|MB_YESNO) != IDYES)
			return;

	std::ofstream OutFile(AnsiString(SaveDialog1->FileName).c_str(), std::ios::out | std::ios::binary);
		if (!OutFile)
			{
			MessageBox(Form1->Handle, L"Couldn't create file", L"Error", MB_ICONERROR|MB_OK);
			return;
			}

	OutFile << "Battery test data ver. 2.0" << "\r\n";
	OutFile << AnsiString(Chart1->Title->Caption).c_str() << "\r\n";
	OutFile << AnsiString(BoolToStr(CheckBox5->Checked, true)).c_str() << "\r\n";
	OutFile << AnsiString(BoolToStr(CheckBox1->Checked, true)).c_str() << ";" << AnsiString(Edit1->Text).c_str() << "\r\n";
	OutFile << AnsiString(BoolToStr(CheckBox2->Checked, true)).c_str() << ";" << AnsiString(Edit2->Text).c_str() << "\r\n";
	OutFile << AnsiString(BoolToStr(CheckBox3->Checked, true)).c_str() << ";" << AnsiString(Edit3->Text).c_str() << "\r\n";
	OutFile << AnsiString(BoolToStr(CheckBox4->Checked, true)).c_str() << ";" << AnsiString(Edit4->Text).c_str() << "\r\n";
	OutFile << AnsiString(FloatToStrF(level, ffFixed, 2, 1)).c_str() << "\r\n";
	OutFile << "Time, ms;Time, h:m:s;Ch1, mA;Ch1, V;Ch2, mA;Ch2, V;Ch3, mA;Ch3, V;Ch4, mA;Ch4, V" << "\r\n";
	int hour, min, sec, msec;
	AnsiString strtime;

	int StepSize;
	switch (ComboBox5->ItemIndex)
	{
		case 0: StepSize = 1; break;
		case 1: StepSize = 10; break;
		case 2: StepSize = 100; break;
		default: StepSize = 1; break;
	}

	for (int i=0; i<Count-1; i += StepSize)
		{
		hour = floor(VTime[i]/3600000.0);
		min = floor(VTime[i]/60000.0);
		min = min%60;
		sec = floor(VTime[i]/1000.0);
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

		OutFile << AnsiString(IntToStr((__int64)VTime[i])).c_str();
		OutFile << ";" << strtime.c_str();
		for (int j=0; j<4; j++)
			if ((i <= Tdis[j]) || (Tdis[j] == 0) || (!CheckBox7->Checked))
				OutFile << ";" << AnsiString(FloatToStrF(A[j][i], ffFixed, 4, 0)).c_str() << ";" << AnsiString(FloatToStrF(V[j][i], ffFixed, 4, 3)).c_str();
			else
				OutFile << ";0,0;0,0";
		OutFile << "\r\n";
		}

	OutFile.close();
	FormatSettings.DecimalSeparator = '.';
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
OpenDialog1->Filter = "CSV files (*.csv)|*.csv|*.CSV|Text files (*.txt)|*.txt|*.TXT|All files (*.*)|*.*";
OpenDialog1->DefaultExt = "csv";
if (OpenDialog1->Execute())
	{
	DataUnchanged = false;
	Form2->Caption = "Progress :: Loading file";
	Form2->Visible = true;
	Form2->CGauge1->Progress = 0;
	FormatSettings.DecimalSeparator = ',';
	ifstream InFile(AnsiString(OpenDialog1->FileName).c_str(), ios::in);
		if (!InFile)
			{
			MessageBox(Form1->Handle, L"Couldn't open file", L"Error", MB_ICONERROR|MB_OK);
			return;
			}

	Series1->Clear();
	Series2->Clear();
	Series3->Clear();
	Series4->Clear();
	Series5->Clear();
	Series6->Clear();
	Chart1->BottomAxis->Automatic = true;

	int lines = 0;
	char test[256];
	AnsiString tmp;
	while (InFile.ignore(1000, '\n'))
		{
		lines++;
		}
	InFile.clear();
	InFile.seekg(0, ios::beg);
	Form2->CGauge1->MaxValue = lines;

	InFile.ignore(1000, '\n');
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	Chart1->Title->Caption = tmp;
	Chart1->Title->Caption.Trim();
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	CheckBox5->Checked = StrToBool(tmp);

	InFile.getline(test, 256, ';');
	CheckBox1->Checked = StrToBool(test);
	Series1->Active = CheckBox1->Checked;
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	Edit1->Text = tmp;
	Edit1->Text.Trim();
	Series1->Title = Edit1->Text;

	InFile.getline(test, 256, ';');
	CheckBox2->Checked = StrToBool(test);
	Series2->Active = CheckBox2->Checked;
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	Edit2->Text = tmp;
	Edit2->Text.Trim();
	Series2->Title = Edit2->Text;

	InFile.getline(test, 256, ';');
	CheckBox3->Checked = StrToBool(test);
	Series3->Active = CheckBox3->Checked;
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	Edit3->Text = tmp;
	Edit3->Text.Trim();
	Series3->Title = Edit3->Text;

	InFile.getline(test, 256, ';');
	CheckBox4->Checked = StrToBool(test);
	Series4->Active = CheckBox4->Checked;
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	Edit4->Text = tmp;
	Edit4->Text.Trim();
	Series4->Title = Edit4->Text;

	float lev;
	InFile.getline(test, 256, '\n');
	tmp = test;
	if (tmp.Pos(";"))
		tmp = tmp.SubString(1, tmp.Pos(";")-1);
	lev = StrToFloat(tmp);

	// ComboBox1:
	// 0 = 0,7 В
	// 1 = 0,9 В
	// 2 = 1,0 В
	// 3 = 2,0 В
	// 4 = 2,5 В
	// 5 = 3,0 В

	if (floor(lev*10.0 + 0.5) == 7)
		{
		level = 0.7;
		ComboBox1->ItemIndex = 0;
		}
	if (floor(lev*10.0 + 0.5) == 9)
		{
		level = 0.9;
		ComboBox1->ItemIndex = 1;
		}
	if (floor(lev*10.0 + 0.5) == 10)
		{
		level = 1.0;
		ComboBox1->ItemIndex = 2;
		}
	if (floor(lev*10.0 + 0.5) == 20)
		{
		level = 2.0;
		ComboBox1->ItemIndex = 3;
		}
	if (floor(lev*10.0 + 0.5) == 25)
		{
		level = 2.5;
		ComboBox1->ItemIndex = 4;
		}
	if (floor(lev*10.0 + 0.5) == 30)
		{
		level = 3.0;
		ComboBox1->ItemIndex = 5;
		}

	InFile.ignore(1000, '\n');

	float v1, v2, v3, v4;
	float a1, a2, a3, a4;
	float t;
	Count = 0;
	int DischCount[4];
	for (int i=0; i<4; i++)
		{
		DischCount[i] = 0;
		discharged[i] = 1;
		Tdis[i] = 0;
		mOhms[i] = 0;
		}

	do
		{
		InFile.getline(test, 256, ';');
		if (InFile.fail() || InFile.eof())
			{
			Count++;
			break;
			}
		t = StrToFloat(test);

		InFile.getline(test, 256, ';');

		InFile.getline(test, 256, ';');
		a1 = StrToFloat(test);
		InFile.getline(test, 256, ';');
		v1 = StrToFloat(test);

		InFile.getline(test, 256, ';');
		a2 = StrToFloat(test);
		InFile.getline(test, 256, ';');
		v2 = StrToFloat(test);

		InFile.getline(test, 256, ';');
		a3 = StrToFloat(test);
		InFile.getline(test, 256, ';');
		v3 = StrToFloat(test);

		InFile.getline(test, 256, ';');
		a4 = StrToFloat(test);
		InFile.getline(test, 256, '\n');
		v4 = StrToFloat(test);

		for (int i=0; i<4; i++)
			{
			V[i] = (float *)realloc(V[i], (Count+1)*sizeof(float));
			A[i] = (float *)realloc(A[i], (Count+1)*sizeof(float));
			VTime = (unsigned int *)realloc(VTime, (Count+1)*sizeof(int));
			}
		VTime[Count] = t;
		A[0][Count] = a1;
		V[0][Count] = v1;
		A[1][Count] = a2;
		V[1][Count] = v2;
		A[2][Count] = a3;
		V[2][Count] = v3;
		A[3][Count] = a4;
		V[3][Count] = v4;

		for (int i=0; i<4; i++)
			{
			if (V[i][Count] <= level)
				{
				if (DischCount[i] < 5)
					DischCount[i]++;

				if (DischCount[i] == 5)
					discharged[i] = 0;
				}
			else
				DischCount[i] = 0;

			if (discharged[i])
				Tdis[i] = Count;
			}

		Count++;
		Form2->CGauge1->Progress = Count;
		}
	while (true);

	FormatSettings.DecimalSeparator = '.';

	/*
	for (int i=0; i<4; i++)
		{
		if (V[i][0] < 0.05)
			Tdis[i] = 0;
		}
	*/
	Form2->CGauge1->Progress = lines;

	int cnt = Count-1;
	Form2->Visible = false;
	Series1->Clear();
	Series2->Clear();
	Series3->Clear();
	Series4->Clear();

	Form2->CGauge1->Progress = 0;
	Form2->CGauge1->MaxValue = cnt-1;
	Form2->Caption = "Progress :: Plotting";

	AnsiString strtime;
	for (int i=0; i<cnt; i++)
		{
		int hour = floor(VTime[i]/3600000.0);
		int min = floor(VTime[i]/60000.0);
		min = min%60;
		int sec = floor(VTime[i]/1000.0);
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

		for (int j=0; j<4; j++)
			if ((i < Tdis[j]) || (!CheckBox7->Checked))
				{
				Chart1->Series[j]->AddXY(i, V[j][i], strtime, clRed);
				}

		Series5->AddXY(i, 1, strtime, clBlack);
		Series6->AddXY(i, level, strtime, clBlack);

		Form2->CGauge1->Progress = i;
		}

	// ------
	/*
	unsigned short h, m, s, ms;
	try
		{
		DecodeTime(StrToTime(Edit10->Text), h, m, s, ms);
		}
	catch (EConvertError &Error)
		{
		MessageBeep(-1);
		h = 0;
		m = 0;
		s = 0;
		}

	if ((h == 0) && (m == 0) && (s == 0))
		{
		Series5->AddXY(0, 1, "", clBlack);
		Series5->AddXY(cnt, 1, "", clBlack);
		Chart1->BottomAxis->Automatic = true;
		Series6->AddXY(0, level, "", clBlack);
		Series6->AddXY(cnt, level, "", clBlack);
		}
	else
		{
		Series5->AddXY(0, 1, "", clBlack);
		Series5->AddXY(4*(h*3600 + m*60 + s), 1, "", clBlack);
		Chart1->BottomAxis->Automatic = false;
		Chart1->BottomAxis->Minimum = 0;
		Chart1->BottomAxis->Maximum = 4*(h*3600 + m*60 + s);
		Series6->AddXY(0, level, "", clBlack);
		Series6->AddXY(4*(h*3600 + m*60 + s), level, "", clBlack);
		}
    */
	Form2->Visible = false;
	ComboBox3Change(NULL);
//	BatteryLabels();
	}

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
SaveDialog1->Filter = "PNG files (*.png)|*.png|*.PNG|All files (*.*)|*.*";
SaveDialog1->Title = "Save image";
SaveDialog1->DefaultExt = "png";

if (SaveDialog1->Execute())
	{
	FormatSettings.DecimalSeparator = '.';
	if (FileExists(SaveDialog1->FileName))
		if (MessageBox(Form1->Handle, L"File exists. Overwrite?", L"File exists", MB_ICONWARNING|MB_YESNO) != IDYES)
			return;

	TPngImage *png = new TPngImage(COLOR_PALETTE, 8, Chart1->Width, Chart1->Height);
	Graphics::TBitmap *bmp = new Graphics::TBitmap();
	bmp->SetSize(Chart1->Width, Chart1->Height);
	bmp->PixelFormat = pf8bit;
	Chart1->PaintTo(bmp->Canvas, 0, 0);
	png->CompressionLevel = 9;
	png->Assign(bmp);
	png->SaveToFile(SaveDialog1->FileName);
	delete png;
	delete bmp;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BatteryLabels(void)
{
if (!Count)
	return;

TLabel *labels[4];
labels[0] = Label14;
labels[1] = Label15;
labels[2] = Label16;
labels[3] = Label17;

float avgI[4];
float avgU[4];
float avgW[4];
for (int i=0; i<4; i++)
	{
	avgI[i] = 0;
	avgU[i] = 0;
	avgW[i] = 0;
	for (int j=0; j<Tdis[i]; j++)
		{
		avgI[i] += A[i][j];
		avgU[i] += V[i][j];
		avgW[i] += A[i][j]*V[i][j];
		}
	avgI[i] /= (float)Tdis[i];
	avgU[i] /= (float)Tdis[i];
	avgW[i] /= (float)Tdis[i];
	avgI[i] /= 1000.0;
	avgW[i] /= 1000.0;

	labels[i]->Caption = IntToStr(i+1) + ": ";

	if (SpeedButton1->Down)
		{
		if (discharged[i])
			{
			labels[i]->Font->Color = clGreen;
			}
		else
			{
			labels[i]->Font->Color = clRed;
			}
		labels[i]->Caption = labels[i]->Caption + FloatToStrF(V[i][Count-1], ffFixed, 4, 3) + " В ("
							+ FloatToStrF(avgI[i]*VTime[Tdis[i]]/1000.0/3600.0, ffFixed, 4, 2) + " А*ч, "
							+ FloatToStrF(avgW[i]*VTime[Tdis[i]]/1000.0/3600.0, ffFixed, 4, 2) + " Вт*ч";

		if (mOhms[i])
			labels[i]->Caption = labels[i]->Caption + ", " + FloatToStrF(mOhms[i], ffFixed, 5, 0) + " мОм";

		labels[i]->Caption = labels[i]->Caption + ")";
		}
	else
		{
		if (!discharged[i])
			{
			labels[i]->Caption = labels[i]->Caption + "Разрядилась (";
			labels[i]->Font->Color = clRed;
			}
		else
			{
			labels[i]->Caption = labels[i]->Caption + "Не разрядилась (";
			labels[i]->Font->Color = clGreen;
			}

		labels[i]->Caption = labels[i]->Caption + FloatToStrF(VTime[Tdis[i]]/1000.0, ffFixed, 6, 0) + " с, "
							+ FloatToStrF(avgI[i]*VTime[Tdis[i]]/1000.0/3600.0, ffFixed, 4, 2) + " А*ч, "
							+ FloatToStrF(avgW[i]*VTime[Tdis[i]]/1000.0/3600.0, ffFixed, 4, 2) + " Вт*ч";

		if (mOhms[i])
			labels[i]->Caption = labels[i]->Caption + ", " + FloatToStrF(mOhms[i], ffFixed, 5, 0) + " мОм";

		labels[i]->Caption = labels[i]->Caption + ")";
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit10KeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	BatteryLabels();
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ComboBox1Change(TObject *Sender)
{
// 0,7 В
// 0,9 В
// 1,0 В
// 2,0 В
// 2,5 В
// 3,0 В
switch (ComboBox1->ItemIndex)
	{
	case 0:
		level = 0.7;
		break;
	case 1:
		level = 0.9;
		break;
	case 2:
		level = 1.0;
		break;
	case 3:
		level = 2.0;
		break;
	case 4:
		level = 2.5;
		break;
	case 5:
		level = 3.0;
		break;
	default:
		level = 0.9;
		break;
	}

if ((!SpeedButton1->Down) && (Count))
	{
	int DischCount[4];
	for (int i=0; i<4; i++)
		{
		DischCount[i] = 0;
		discharged[i] = 1;
		}

	for (int j=0; j<Count-1; j++)
		{
		for (int i=0; i<4; i++)
			{
			if (V[i][j] <= level)
				{
				if (DischCount[i] == 5)
					{
					if (discharged[i])
						Tdis[i] = j;
					discharged[i] = 0;
					}
				else
					DischCount[i]++;
				}
			else
				DischCount[i] = 0;

			if (discharged[i])
				Tdis[i] = j;
			}
		}
    /*
	for (int i=0; i<4; i++)
		{
		if (V[i][0] < 0.05)
			Tdis[i] = 0;
		}
	*/
	BatteryLabels();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton2Click(TObject *Sender)
{
if (SpeedButton2->Down)
    {
	if (InitDevice())
		{
		SpeedButton2->Down = false;
		return;
		}
	SpeedButton1->Enabled = true;
	}
else
	{
	FT_Close(ftHandle);
	SpeedButton1->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox6Click(TObject *Sender)
{
if (CheckBox6->Checked)
	{
	Label12->Enabled = true;
	Edit8->Enabled = true;
	Label19->Enabled = true;
	Edit6->Enabled = true;
	Label10->Enabled = true;
	Label11->Enabled = true;
	Edit7->Enabled = true;
	Label18->Enabled = true;
	Chart1->Title->Caption =  "Импульсный разряд, " + Edit5->Text + "/" + Edit6->Text + " мА";
	}
else
	{
	for (int i=0; i<4; i++)
		mamperes[i] = StrToInt(Edit5->Text);
	ChangeCurrent = true;
	Label12->Enabled = false;
	Edit8->Enabled = false;
	Label19->Enabled = false;
	Edit6->Enabled = false;
	Label10->Enabled = false;
	Label11->Enabled = false;
	Edit7->Enabled = false;
	Label18->Enabled = false;

	Chart1->Title->Caption =  "Постоянный разряд, " + Edit5->Text + " мА";
	}
}
//---------------------------------------------------------------------------

int __fastcall TForm1::SetCurrent(void)
{
__int16 data[4];
__int16 tmp;

for (int i=0; i<4; i++)
	{
	tmp = floor(0.5 + CoeffSet[i]*mamperes[i] + CoeffSetOffset[i]);
	if (CheckBox7->Checked)
		tmp *= discharged[i];

	if (tmp < 0)
		tmp = 0;
	if (tmp > 1023)
		tmp = 1023;
	data[i] = tmp << 2; // приводим к пригодному для ЦАП виду: 2 бита в LSB у него свободны

	RealCurrent[i] = discharged[i]*((float)tmp-CoeffSetOffset[i])/CoeffSet[i];
	if (RealCurrent[i] < 0)
		RealCurrent[i] = 0;
	}

DWORD bytes;
SendCommand(0x31); // set current
FT_Write(ftHandle, &data[0], 8, &bytes);

Sleep(25);
SendCommand(0x31); // set current
FT_Write(ftHandle, &data[0], 8, &bytes);

return 0;
}
//---------------------------------------------------------------------------

int __fastcall TForm1::SetLEDs(void)
{
char leds = 0;
char position;
for (int i=0; i<7; i++)
	{
	if (LED[i])
		leds = leds | (1<<i);
	}

DWORD bytes;
SendCommand(0x32); // enable/disbale LEDs
FT_Write(ftHandle, &leds, 1, &bytes);
Sleep(10);
SendCommand(0x32);
FT_Write(ftHandle, &leds, 1, &bytes);
return 0;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Edit5KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	Edit5->Color = clWindow;
	for (int i=0; i<4; i++)
		mamperes[i] = StrToInt(Edit5->Text);
	Current = StrToInt(Edit5->Text);
	ChangeCurrent = true;

	if (CheckBox6->Checked)
		Chart1->Title->Caption =  "Импульсный разряд, " + Edit5->Text + "/" + Edit6->Text + " мА";
	else
		Chart1->Title->Caption =  "Постоянный разряд, " + Edit5->Text + " мА";
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit5Change(TObject *Sender)
{
if ((SpeedButton1->Down) && (!CheckBox6->Checked))
	Edit5->Color = clYellow;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
Form4->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Edit6KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
	if (CheckBox6->Checked)
		Chart1->Title->Caption =  "Импульсный разряд, " + Edit5->Text + "/" + Edit6->Text + " мА";
	else
		Chart1->Title->Caption =  "Постоянный разряд, " + Edit5->Text + " мА";
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ComboBox3Change(TObject *Sender)
{
// подсчёт внутреннего сопротивления
int time = (1 + ComboBox3->ItemIndex)*15*60*10; // время в сотнях миллисекунд
if (time > Count-1)
	{
	for (int i=0; i<4; i++)
		mOhms[i] = 0;
	BatteryLabels();
	return;
	}

int interval = 5*10*(StrToInt(Edit7->Text) + StrToInt(Edit8->Text));
float MinU;
float MaxU;
float MinI;
float MaxI;

for (int j=0; j<4; j++)
	{
	if (time > Tdis[j])
		{
		mOhms[j] = 0;
		continue;
		}

	if (interval > time)
		interval = time;

	MaxU = 0;
	MinU = 3;
	MinI = 5000;
	MaxI = 0;

	for (int i=(time-interval); i<time; i++)
		{
		if (V[j][i] > MaxU)
			MaxU = V[j][i];
		if (V[j][i] < MinU)
			MinU = V[j][i];

		if (A[j][i] > MaxI)
			MaxI = A[j][i];
		if (A[j][i] < MinI)
			MinI = A[j][i];
		}

	if ((MaxI-MinI) > 10)
		mOhms[j] = 1E6*(MaxU-MinU)/(MaxI-MinI);
	else
		mOhms[j] = 0;
	}
BatteryLabels();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit5KeyPress(TObject *Sender, wchar_t &Key)
{
int n = Edit5->Text.Length();
if ((n<2) && (Key == VK_BACK))
	{
	Key = 0;
	Edit5->SelectAll();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit5KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_DELETE)
	Key = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit6KeyPress(TObject *Sender, wchar_t &Key)
{
int n = Edit6->Text.Length();
if ((n<2) && (Key == VK_BACK))
	{
	Key = 0;
	Edit6->SelectAll();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit6KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_DELETE)
	Key = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit7KeyPress(TObject *Sender, wchar_t &Key)
{
int n = Edit7->Text.Length();
if ((n<2) && (Key == VK_BACK))
	{
	Key = 0;
    Edit7->SelectAll();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit7KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_DELETE)
	Key = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit8KeyPress(TObject *Sender, wchar_t &Key)
{
int n = Edit8->Text.Length();
if ((n<2) && (Key == VK_BACK))
	{
	Key = 0;
    Edit8->SelectAll();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit8KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_DELETE)
	Key = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit7MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
Edit7->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit8MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
Edit8->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit5MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
Edit5->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit6MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
Edit6->SelectAll();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ComboBox4Change(TObject *Sender)
{

switch (ComboBox4->ItemIndex)
	{
	case 0:  // Nickel 1.3V
		CheckBox7->Checked = true;
		CheckBox7->Enabled = false;
		ComboBox1->ItemIndex = 1;
		Chart1->LeftAxis->Maximum = 2;
		Chart1->LeftAxis->Minimum = 0.5;
		break;
	case 1: // Lithium 4.3V
		CheckBox7->Checked = true;
		CheckBox7->Enabled = false;
		ComboBox1->ItemIndex = 5;
		Chart1->LeftAxis->Maximum = 5;
		Chart1->LeftAxis->Minimum = 2;
		break;
	case 2:	// 1.5V battery
		CheckBox7->Checked = false;
		CheckBox7->Enabled = true;
		ComboBox1->ItemIndex = 0;
		Chart1->LeftAxis->Maximum = 2;
		Chart1->LeftAxis->Minimum = 0.5;
		break;
	case 3:	// 3.0V battery
		CheckBox7->Checked = false;
		CheckBox7->Enabled = true;
		ComboBox1->ItemIndex = 3;
		Chart1->LeftAxis->Maximum = 3.6;
		Chart1->LeftAxis->Minimum = 1.5;
		break;
	case 4:	// 4.5V battery
		CheckBox7->Checked = false;
		CheckBox7->Enabled = true;
		ComboBox1->ItemIndex = 5;
		Chart1->LeftAxis->Maximum = 5;
		Chart1->LeftAxis->Minimum = 2;
		break;
	default:
		CheckBox7->Checked = false;
		CheckBox7->Enabled = true;
		ComboBox1->ItemIndex = 1;
		Chart1->LeftAxis->Maximum = 5;
		Chart1->LeftAxis->Minimum = 0.5;
		break;
	}
ComboBox1Change(Form1);

}
//---------------------------------------------------------------------------


