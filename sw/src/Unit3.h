//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "FTD2XX.H"
//---------------------------------------------------------------------------
class GetData : public TThread
{
private:
	unsigned __int32 time;
	__int16 data[4];
	DWORD bytes;
	char buf;
	int sec;
	int min;
	int hour;
	AnsiString strtime;
	float current[4];
	int DischCount[4];
protected:
	void __fastcall Execute();
public:
	__fastcall GetData(bool CreateSuspended);
	void __fastcall UpdateData(void);
};
//---------------------------------------------------------------------------
#endif
