//
// Device.h : D3D ����̽� �¾� �ڵ�.
//
// DirectX ���� ���� : DX11
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)
//
#pragma once
#pragma warning(disable:4996)

// Platform Header �� Library
//
#include "Windows.h"
#include "stdio.h"




///////////////////////////////////////////////////////////////////////////////
//
// DX �� ��ġ ���� ���� �����͵�  
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
// ��Ÿ �ý��� ���� �Լ� �� ������.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef YN_OK
#define YN_OK	   0					//���� ����. Ư���� ���� ����.
#define YN_FAIL   -1					//���� �߻�.
#define YN_FAILED(v)   ((v) != YN_OK_)	//���� üũ ��ũ��.
#endif





/**************** end of "Device.h" ***********************************/