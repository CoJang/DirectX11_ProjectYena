//
// Render.cpp : �⺻ ������/�����ӿ� �ҽ�
//
// DirectX ���� ���� : DX11
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2004.05.07. Update
// 2005.09.23. Update (.Net)
// 2009.12.01. Update.
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)

// 2018.03.15. Start. CoJang's Own Code
//
#pragma warning(disable:4996)
#include "Windows.h" 
#include "Device.h"		//��
#include "Render.h"

TCHAR* g_WindowName = L"CoJang & Yena Collaboration";



/////////////////////////////////////////////////////////////////////////////
//
//  ���� �����͵�.
//  

  

/////////////////////////////////////////////////////////////////////////////
//
int DataLoading()
{ 
	// ������ �ε�/���� �ڵ�� ���⿡...
	// ...
	 

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//
void DataRelease()
{
	// ������ ���� �ڵ�� ���⿡..
	// ... 

}



/////////////////////////////////////////////////////////////////////////////// 
//
//  ����. 
//
void ShowInfo()
{ 
	//�����Ӽ� ǥ��.
	PutFPS(1, 1); 


	//���� ���.
	int x = 100;
	int y = 100;
	HDC hdc = GetDC(g_hWnd);
	TCHAR* msg = L"";
	COLORREF col = RGB(0, 0, 0);	
	COLORREF col2 = RGB(255, 0, 255);	
	
	ynTextDraw(x, y += 20, col, g_WindowName);	
	ynTextDraw(x, y += 20, col2, L"DirectX ���� �� �⺻�����ӿ�ũ ���� �ǽ�.");
	ynTextDraw(x, y += 20, col2, L"1. ��������ӿ�ũ ����");
	ynTextDraw(x, y += 20, col2, L"2. DirectX Setup");
	ynTextDraw(x, y += 20, col2, L"3. DX SwapChain (Double Buffering)");
	ynTextDraw(x, y += 20, col2, L"4. DX Font System (DXTK)");
	y += 20;
	ynTextDraw(x, y += 20, col2, L"������ ������ �����Ͽ� DirectX ������ �Ϸ��Ͻʽÿ�.");
	y += 20;
	//ynTextDraw(x, y += 20, col, L"DX �� ����ϱ� ���� 3D ������ �⺻ �����ӿ�ũ �Դϴ�.");
	ynTextDraw(x, y += 20, col, L"�⺻ �����ӿ�ũ�� ������� �ܰ躰 Ʃ�丮���� ������ ���Դϴ�.");
	ynTextDraw(x, y += 20, col, L"Idle Time �� ���߾� ��������� �޼����� �����Ÿ��ϴ�.");
	ynTextDraw(x, y += 20, col, L"�������۸�(Double Buffering)������ �����Ÿ�(Flickering)�� �ذ� �� �� �ֽ��ϴ�.");
	ynTextDraw(x, y += 20, col, L"�ʴ� ������ �����Ӽ�(FPS) ��� ��. (�������)");

}

 


////////////////////////////////////////////////////////////////////////////// 
//
// ��ü ��� �׸���
//
void SceneRender()
{
	//-------------------------------
	// ����/�ý��� ����.
	//------------------------------- 
	//...

	// ��� ���� 
	//
	// ���ΰ�, ����, ������ �̵�, �浹�˻� ���..
	// ...

	// ���� ����, �̺�Ʈ, ����, �������..
	// ...	 
	

	//-------------------------------
	// ��� �׸��� ����.. 
	//-------------------------------

	//��ü ������ : ���ΰ�, ����, ����.. 
	//...
	

	//���� �� ��Ÿ ���� ���.
	ShowInfo();

	//-------------------------------
	// ��� �׸��� ����.
	//------------------------------- 
	//...



}//end of void SceneRender()






/****************** end of file "Render.cpp" *********************************/