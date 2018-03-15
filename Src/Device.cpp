//
// Device.cpp : D3D 객체 및 디바이스 설정/제거 소스.
//
// DirectX 기준 버전 : DX11
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)
//
#pragma warning(disable:4996)
#include "Device.h"
#include "Tchar.h"  


///////////////////////////////////////////////////////////////////////////////
//


///////////////////////////////////////////////////////////////////////////// 
//
int DXSetup(HWND hwnd)
{
	//DirectX 셋업 코드는 여기에...
	 
	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////// 
//
void DXRelease()
{	 
	//DirectX 해제 코드는 여기에...
}
 



///////////////////////////////////////////////////////////////////////////////
//
// 초당 프레임수 출력.
//
// 인자 : int x, y : 출력할 화면 좌표.
//
void PutFPS(int x, int y)
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();

	static int frmcnt = 0;
	static float fps = 0.0f;

	++frmcnt;
	
	int time = nowtime - oldtime;
	if( time >= 999)
	{
		oldtime = nowtime;

		fps = (float)frmcnt*1000/(float)time;
		frmcnt = 0;
	}
	
	ynTextDraw(x, y, RGB(0, 0, 255), L"FPS:%.1f/%d", fps, frmcnt );
	///ynTextDraw(x, y, COLOR(0, 1, 0, 1), L"FPS:%.1f/%d", fps, frmcnt);
}


 

/////////////////////////////////////////////////////////////////////////////// 
//
// 문자열 출력 : GDI, 유니코드 대응.
//
void ynTextDraw(int x, int y, COLORREF col, TCHAR* msg, ...)
{
	const DWORD _size = 2048;

	TCHAR buff[_size] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(buff, msg, vl);
	va_end(vl);

	HDC hdc = GetDC(g_hWnd);
	SetTextColor(hdc, col);
	TextOut(hdc, x, y, buff, (int)_tcslen(buff));
	SetTextColor(hdc, RGB(0, 0, 0));
	ReleaseDC(g_hWnd, hdc);

}


 
/***************** End of "Device.cpp" *******************************/


 
