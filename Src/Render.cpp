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
COLOR g_clearColor(0, 0.25f, 0.3f, 1); //����
  

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
	PutFPS(0, 0);

	static bool bShow = true;
	if (IsKeyUp(VK_F1)) bShow ^= true;

	if (!bShow)
	{
		//ynTextDraw(1,20, COLOR(1, 1, 0, 1), "[Help] F1"); 
		return;
	}


	// Today's Topic.
	{
		int x = 300, y = 50;
		COLOR col(1, 1, 1, 1);
		COLOR col2(1, 1, 0, 1);
		ynTextDraw(x, y, col, L"�� %s", g_WindowName);

		y += 24;
		WCHAR* msg =
			L"1.�⺻�����ӿ�ũ ����.\n"
			L"2.HW ������ ����̽�(D3D Device) �� ����.\n"
			L"3.Idle �ð� ������.\n"
			L"4.����ü�� Swap(Flipping) chain �� ���� \n";
		ynTextDraw(x, y, col, msg);
	}


	int x = 300, y = 300;
	static int cnt = 0;
	COLOR col(1, 1, 0, 1);
	ynTextDraw(x, y, col, L"Hello, Device!!    cnt=%08d", ++cnt);

	y += 14;
	y += 14;
	ynTextDraw(x, y, col, L"* HW Rendering Device (%s) ���� *", g_strFeatureLevel);

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
	//����Ÿ�� (�����) �����
	ClearBackBuffer(g_clearColor);

	//��ü ������ : ���ΰ�, ����, ����.. 
	//...
	

	//���� �� ��Ÿ ���� ���.
	ShowInfo();

	//-------------------------------
	// ��� �׸��� ����.
	//------------------------------- 
	//...
	Flip();


}//end of void SceneRender()






/****************** end of file "Render.cpp" *********************************/