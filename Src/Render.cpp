//
// Render.cpp : 기본 렌더링/프레임웍 소스
//
// DirectX 기준 버전 : DX11
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
#include "Device.h"		//★
#include "Render.h"

TCHAR* g_WindowName = L"CoJang & Yena Collaboration";



/////////////////////////////////////////////////////////////////////////////
//
//  전역 데이터들.
//  
COLOR g_clearColor(0, 0.25f, 0.3f, 1); //배경색
  

/////////////////////////////////////////////////////////////////////////////
//
int DataLoading()
{ 
	// 데이터 로딩/생성 코드는 여기에...
	// ...
	 

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//
void DataRelease()
{
	// 데이터 해제 코드는 여기에..
	// ... 

}



/////////////////////////////////////////////////////////////////////////////// 
//
//  도움말. 
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
		ynTextDraw(x, y, col, L"■ %s", g_WindowName);

		y += 24;
		WCHAR* msg =
			L"1.기본프레임워크 구축.\n"
			L"2.HW 렌더링 디바이스(D3D Device) 를 생성.\n"
			L"3.Idle 시간 렌더링.\n"
			L"4.스왑체인 Swap(Flipping) chain 의 이해 \n";
		ynTextDraw(x, y, col, msg);
	}


	int x = 300, y = 300;
	static int cnt = 0;
	COLOR col(1, 1, 0, 1);
	ynTextDraw(x, y, col, L"Hello, Device!!    cnt=%08d", ++cnt);

	y += 14;
	y += 14;
	ynTextDraw(x, y, col, L"* HW Rendering Device (%s) 버전 *", g_strFeatureLevel);

}

 


////////////////////////////////////////////////////////////////////////////// 
//
// 전체 장면 그리기
//
void SceneRender()
{
	//-------------------------------
	// 엔진/시스템 갱신.
	//------------------------------- 
	//...

	// 장면 갱신 
	//
	// 주인공, 몬스터, 오브제 이동, 충돌검사 등등..
	// ...

	// 게임 로직, 이벤트, 층돌, 점수계산..
	// ...	 
	

	//-------------------------------
	// 장면 그리기 시작.. 
	//-------------------------------
	//렌더타겟 (백버퍼) 지우기
	ClearBackBuffer(g_clearColor);

	//개체 렌더링 : 주인공, 몬스터, 지형.. 
	//...
	

	//도움말 및 기타 정보 출력.
	ShowInfo();

	//-------------------------------
	// 장면 그리기 종료.
	//------------------------------- 
	//...
	Flip();


}//end of void SceneRender()






/****************** end of file "Render.cpp" *********************************/