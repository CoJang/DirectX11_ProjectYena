//
// Device.cpp : D3D ��ü �� ����̽� ����/���� �ҽ�.
//
// DirectX ���� ���� : DX11
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
	//DirectX �¾� �ڵ�� ���⿡...
	 
	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////// 
//
void DXRelease()
{	 
	//DirectX ���� �ڵ�� ���⿡...
}
 



///////////////////////////////////////////////////////////////////////////////
//
// �ʴ� �����Ӽ� ���.
//
// ���� : int x, y : ����� ȭ�� ��ǥ.
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
// ���ڿ� ��� : GDI, �����ڵ� ����.
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


 
