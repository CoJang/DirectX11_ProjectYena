//
// Device.h : D3D 디바이스 셋업 코드.
//
// DirectX 기준 버전 : DX11
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)
//
#pragma once
#pragma warning(disable:4996)

// Platform Header 및 Library
//
#include "Windows.h"
#include "stdio.h"




///////////////////////////////////////////////////////////////////////////////
//
// DX 및 장치 관련 전역 데이터들  
//
///////////////////////////////////////////////////////////////////////////////

int		DXSetup(HWND hwnd);
void	DXRelease(); 

void	PutFPS(int x, int y);

void	ynTextDraw(int x, int y, COLORREF col, TCHAR* msg, ...);
#define ynTextOut ynTextDraw

extern HWND g_hWnd;
 


///////////////////////////////////////////////////////////////////////////////
//
// 기타 시스템 관련 함수 및 재정의.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef YN_OK
#define YN_OK	   0					//에러 없음. 특별한 문제 없음.
#define YN_FAIL   -1					//에러 발생.
#define YN_FAILED(v)   ((v) != YN_OK_)	//에러 체크 매크로.
#endif





/**************** end of "Device.h" ***********************************/