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
// DirectX 표준 라이브러리 
//
///////////////////////////////////////////////////////////////////////////////
// < DirectX 버전별 개발 라이브러리 >
// DX9         : DirectX SDK 2002 Nov. 
// DX10.x/DX11 : DirectX SDK 2010 June (DX 독립 배포 마지막 버전)
// DX11.X/DX12 : Windows SDK 8.x  (DX 가 포함되어 배포됨) 

// DX 표준 헤더 
#include "D3D11.h"					//DX 표준 헤더. (DX 11.0)
//#include "D3D11_1.h"				//DX 표준 헤더. (DX 11.1)
#pragma comment(lib, "D3D11")		//DX 라이브러리 로딩.  D3D11.dll 필요.

//DirectX Math : Dx 수학 라이브러리 
//#include "XNAMath.h"				//XNA Math 라이브러리.(ver.2.0) DirectX SDK 2010 June. 대응.
#include "DirectXMath.h"			//XNA Math 라이브러리.(ver.3.0) VS2012 (Windows SDK 8) 이상.
using namespace DirectX;

//DXGI : DirectX Graphics Infrastructure 
//DX 의 버전에 독립적인 하드웨어 저수준 접근 방식을 제공합니다.
//#include "DXGI.h"					//DXGI 헤더. (d3d11.h 에 포함됨)
#pragma comment(lib, "dxgi")		//DXGI 라이브러리. DXGI.dll 필요.


///////////////////////////////////////////////////////////////////////////////
//
// DirectX Toolkit : DX 확장 라이브러리 ★
//
///////////////////////////////////////////////////////////////////////////////
// DX 확장 라이브러리 및 유틸리티 모음집.
// DX 사용시 필요한 여러 기능 클래스 및 부가 서비스를 제공합니다.
// DirectXTK 를 보다 편하게 사용하기 위해서 Project 속성 추가가 필요합니다.
// DirectXTK.h 또는  Yena/DXTK/Readme.txt 을 참조하세요.  
#include "DirectXTK.h"



///////////////////////////////////////////////////////////////////////////////
//
// 타입 과 상수 재정의 : 버전별 호환성 향상 증대가 목적.
//
///////////////////////////////////////////////////////////////////////////////
//DX11 장치관련 재정의.											
typedef ID3D11Device*			LPDEVICE;
typedef ID3D11DeviceContext*	LPDXDC;
typedef IDXGISwapChain*			LPSWAPCHAIN;
typedef ID3D11RenderTargetView*	LPRTVIEW;

/*
// DirectX Math 타입 재정의 : 호환성 향상
// DX9/DX10 : DirectX SDK June.2010 이하
typedef D3DXMATRIXA16	MATRIXA;	//행렬 : 16바이트 정렬 버전.
typedef D3DXMATRIX		MATRIX;		//행렬 : 일반 버전.
typedef D3DXVECTOR4		VECTOR4;
typedef D3DXVECTOR3		VECTOR3;
typedef D3DXVECTOR2		VECTOR2;
*/
// DirectX Math 타입 재정의 : 구형 소스와의 호환성 향상이 목적.
// DX10/11 "XNAMath ver.2": DirectX SDK june.2010
// DX11/12 "XNAMath ver.3": DirectXMath, Windows SDK 8.x 에 포함  
// 링크 : https://msdn.microsoft.com/ko-kr/library/windows/desktop/ee418728(v=vs.85).aspx
//
typedef XMMATRIX		MATRIXA;	//행렬 : 16바이트 정렬, SIMD 버전. 전역/지역 변수용. "Register Type"
typedef XMFLOAT4X4		MATRIX;		//행렬 : 일반 버전. SIMD 미지원, Class 데이터 저장용. "Storage Type"
typedef XMVECTOR		VECTOR;		//4성분 벡터 : 16바이트 정렬, SIMD 버전. 전역/지역 변수용. "Register Type"
typedef XMFLOAT4		VECTOR4;	//4성분 벡터 : 일반 버전, SIMD 미지원, Class 데이터 저장용. "Storage Type"
typedef XMFLOAT3		VECTOR3;
typedef XMFLOAT2		VECTOR2;

//색상 타입: 2가지.
//typedef XMCOLOR		COLOR;		// r, g, b, a.  [정수형 0~255]
typedef XMFLOAT4		COLOR;		// r, g, b, a.  [실수형 0~1.0]


									///////////////////////////////////////////////////////////////////////////////
									//
									// DX 및 장치 관련 전역 데이터들  
									//
									///////////////////////////////////////////////////////////////////////////////

int		DXSetup(HWND hwnd);
void	DXRelease();

int		ClearBackBuffer(COLOR col);
int     Flip();

float	GetEngineTime();
void	PutFPS(int x, int y);
//void  ynTextDraw( int x, int y, COLOR col, char* msg, ...);

void	GetDeviceInfo();
void	SystemUpdate(float dTime);
void	SystemInfo(int x, int y, COLOR col);

// 엔진 DX 기능 레벨 구하기. 
void	GetFeatureLevel();


extern ID3D11Device*           g_pDevice;
extern ID3D11DeviceContext*	   g_pDXDC;
extern IDXGISwapChain*         g_pSwapChain;
extern ID3D11RenderTargetView* g_pRTView;


// 장치 설정 정보 구조체. (DX9/11 구형 호환성 유지용)
typedef DXGI_MODE_DESC	  DISPLAYMODE;	//DX11 대응
										//typedef DXGI_MODE_DESC1 DISPLAYMODE;	//DX11.1 대응
										//typedef D3DDISPLAYMODE DISPLAYMODE;   //DX9 용.

extern DISPLAYMODE g_Mode;
extern HWND g_hWnd;
extern BOOL g_bShowFrame;

//전체화면 사용여부.
extern BOOL g_bWindowMode;
//수직동기화 사용여부.
extern BOOL g_bVSync;

//현재 디바이스 DX 기능 레벨.
extern TCHAR* g_strFeatureLevel;




///////////////////////////////////////////////////////////////////////////////
//
// 기타 시스템 관련 함수 및 재정의.
//
///////////////////////////////////////////////////////////////////////////////

//비동기 키보드 입력 처리.
#ifndef IsKeyDown
#define IsKeyDown(key)	((GetAsyncKeyState(key)&0x8000) == 0x8000)
#define IsKeyUp(key)	((GetAsyncKeyState(key)&0x8001) == 0x8001)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(pBuff) if((pBuff)){ (pBuff)->Release(); (pBuff) = NULL; }
#define SAFE_DELETE(pBuff)	if((pBuff)){ delete (pBuff); (pBuff) = NULL; }
#define SAFE_DELARRY(pBuff) if((pBuff)){ delete [] (pBuff); (pBuff) = NULL; }
#endif

#ifndef YES_
#define YES_ TRUE
#define NO_  FALSE
#endif

#ifndef YN_OK
#define YN_OK	   0					//에러 없음. 특별한 문제 없음.
#define YN_FAIL   -1					//에러 발생.
#define YN_FAILED(v)   ((v) != YN_OK_)	//에러 체크 매크로.
#endif

// 색상 * 스칼라 곱  (ex)  col = col * 0.5f
COLOR operator * (COLOR& lhs, float s);



///////////////////////////////////////////////////////////////////////////////
//
// 에러 출력 함수
//
///////////////////////////////////////////////////////////////////////////////

//int ynError(BOOL bBox, TCHAR* msg, ...);
int ynErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...);

#define ynError(hr, msg, ...)  \
ynErrorW( __FILEW__, __LINE__, __FUNCTIONW__, TRUE, hr, msg, __VA_ARGS__ )




///////////////////////////////////////////////////////////////////////////////
//
// 폰트 엔진 : 일반 함수 버전.★
//
// DirectXTK : SpriteFont 기반 폰트 출력 클래스
// 2D Texture / Sprite 기반의 폰트 출력 라이브러리.
// 빠르고 경쾌하지만, CJK 계열의 폰트의 경우 완성형만 지원됩니다.
// 참조 : https://directxtk.codeplex.com/wikipage?title=SpriteFont
// #include "SpriteFont.h"	
//
// 자세한 것은 DirectXTK.h 또는  Yena/DXTK/Readme.txt 을 참조. 
//
///////////////////////////////////////////////////////////////////////////////

int  ynFontCreate(LPDEVICE pDev);
void ynFontRelease();
void ynTextDraw(int x, int y, COLOR col, TCHAR* msg, ...);



/**************** end of "Device.h" ***********************************/