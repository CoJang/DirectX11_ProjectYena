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
	//프레임수 표시.
	PutFPS(1, 1); 


	//도움말 출력.
	int x = 100;
	int y = 100;
	HDC hdc = GetDC(g_hWnd);
	TCHAR* msg = L"";
	COLORREF col = RGB(0, 0, 0);	
	COLORREF col2 = RGB(255, 0, 255);	
	
	ynTextDraw(x, y += 20, col, g_WindowName);	
	ynTextDraw(x, y += 20, col2, L"DirectX 설정 및 기본프레임워크 구성 실습.");
	ynTextDraw(x, y += 20, col2, L"1. 기반프레임웍크 구축");
	ynTextDraw(x, y += 20, col2, L"2. DirectX Setup");
	ynTextDraw(x, y += 20, col2, L"3. DX SwapChain (Double Buffering)");
	ynTextDraw(x, y += 20, col2, L"4. DX Font System (DXTK)");
	y += 20;
	ynTextDraw(x, y += 20, col2, L"배포된 예제를 참고하여 DirectX 설정을 완료하십시오.");
	y += 20;
	//ynTextDraw(x, y += 20, col, L"DX 를 사용하기 위한 3D 렌더링 기본 프레임워크 입니다.");
	ynTextDraw(x, y += 20, col, L"기본 프레임워크를 기반으로 단계별 튜토리얼을 진행할 것입니다.");
	ynTextDraw(x, y += 20, col, L"Idle Time 에 맞추어 출력중으로 메세지가 깜빡거립니다.");
	ynTextDraw(x, y += 20, col, L"더블버퍼링(Double Buffering)이으로 깜빡거림(Flickering)을 해결 할 수 있습니다.");
	ynTextDraw(x, y += 20, col, L"초당 렌더링 프레임수(FPS) 출력 중. (좌측상단)");

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

	//개체 렌더링 : 주인공, 몬스터, 지형.. 
	//...
	

	//도움말 및 기타 정보 출력.
	ShowInfo();

	//-------------------------------
	// 장면 그리기 종료.
	//------------------------------- 
	//...



}//end of void SceneRender()






/****************** end of file "Render.cpp" *********************************/