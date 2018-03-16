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
// DX 및 장치 관련 전역 데이터들  
//
///////////////////////////////////////////////////////////////////////////////

// D3D 관련 개체 인터페이스 포인터.
ID3D11Device*				g_pDevice = NULL;
ID3D11DeviceContext*		g_pDXDC = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pRTView = NULL;

// 장치 설정 기본 정보
DISPLAYMODE g_Mode = { 800, 600, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM };

//D3D 기능 레벨 (Direct3D feature level) 
//현재 사용할 DX 버전 지정. DX 렌더링 장치의 호환성 향상
D3D_FEATURE_LEVEL g_FeatureLevels = D3D_FEATURE_LEVEL_11_0;		//DX11 대응.
//D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_9_3;		//DX9.0c 대응.
TCHAR* g_strFeatureLevel = L"N/A";

// 초기 장치 설정 관련 함수
HRESULT CreateDeviceSwapChain(HWND hwnd);
HRESULT CreateRenderTarget();
void	SetViewPort();


///////////////////////////////////////////////////////////////////////////// 
//
// DX 및 관련 객체 초기화 : 어플리케이션 시작시 1회 호출.
//
// 1.디바이스 생성 
// 2.스왑체인 및 렌더타겟 생성.
// 3.깊이/스텐실 버퍼 생성.
// 4.기타 렌더링 관련 장치 생성.
//
int DXSetup(HWND hwnd)
{
	//----------------------------------------
	// 1단계 : 렌더링 장치 관련 필수 설정
	//----------------------------------------
	// D3D 렌더링 장치 Device 및 스왑체인 Swap Chain 생성. 
	CreateDeviceSwapChain(hwnd);

	// 장치-스왑체인의 렌더타겟(백버퍼) 획득
	CreateRenderTarget();

	// 장치 출력병합기(Output Merger) 에 렌터링 타겟 및 깊이-스텐실 버퍼 등록.
	g_pDXDC->OMSetRenderTargets(
		1,			// 렌더타겟 개수.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&g_pRTView, // 렌더타겟("백버퍼") 등록.	
		nullptr
		);

	SetViewPort(); // 뷰포트 설정

	GetDeviceInfo(); // 디바이스(장치) 정보 획득

	//----------------------------------------
	// 2단계 : 기타 렌더링 관련 추가 설정.
	//----------------------------------------
	// 렌더링에 필요한 사용자 객체등을 생성/관리 합니다.
	// 카메라, 조명, 폰트, 셰이더 등등...

	// 폰트 생성.★
	ynFontCreate(g_pDevice);

	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////// 
//
// 디바이스 개체 및 관련 장치 제거 : 종료시 1회 호출.  
// 기본 렌더링 개체들 제거시 생성된 역순으로 처리합니다.(권장)
//
void DXRelease()
{	 
	// 장치 상태 리셋 : 제거 전에 초기화를 해야 합니다. (메모리 누수 방지)
	if (g_pDXDC) g_pDXDC->ClearState();

	ynFontRelease();
	SAFE_RELEASE(g_pRTView);		// 렌더타겟 제거
	SAFE_RELEASE(g_pSwapChain);		
	SAFE_RELEASE(g_pDXDC);
	SAFE_RELEASE(g_pDevice);		// 디바이스 제거. 가장 나중에 제거
}
 
HRESULT CreateDeviceSwapChain(HWND hwnd)
{
	HRESULT hr = S_OK;

	// 장치 (Device) 및 스왑체인(SwapChain) 정보 구성.
	// 스왑체인은 다중버퍼링 시스템을 말하며
	// 고전적인 '플립핑Flipping' 체인과 동일한 의미입니다.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = TRUE;							//풀스크린 또는 창모드 선택.
	sd.OutputWindow = hwnd;						//출력할 윈도우 핸들.
	sd.BufferCount = 1;							//백버퍼 개수.
	sd.BufferDesc.Width = g_Mode.Width;			//해상도 결정.(백버퍼 크기)
	sd.BufferDesc.Height = g_Mode.Height;
	sd.BufferDesc.Format = g_Mode.Format;		//백버퍼 색상규격 (A8R8G8B8) 창모드에서는 생략 가능 
	sd.BufferDesc.RefreshRate.Numerator = 0;	//버퍼 갱신율.(수직동기화 VSync 활성화시 표준갱신율 적용 : 60hz)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //용도 설정 '렌더 타겟'
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;					// Anti-Aliasing 설정
	sd.SampleDesc.Quality = 0;

	// D3D 렌더링 장치(Device) 및 스왑체인 생성
	hr = D3D11CreateDeviceAndSwapChain(
			NULL,						// 비디오 어댑터 포인터(그래픽 출력장치 번호) 기본장치는 NULL
			D3D_DRIVER_TYPE_HARDWARE,	// 하드웨어 가속 (그래픽 카드를 사용)
			NULL,						// SW Rasterizer DLL 핸들. 하드웨어 가속시에는 NULL
			0,							// 디바이스 생성 플래그(기본값). [디버그 시] D3D11_CREATE_DEVICE_DEBUG
			&g_FeatureLevels,			// 생성할 디바이스 기능 레벨 배열
			1,							// 생성할 디바이스 기능 레벨 배열 크기
			D3D11_SDK_VERSION,			// DX SDK 버젼
			&sd,						// 디바이스 생성 상세 옵션
			&g_pSwapChain,				// [출력] 스왑체인 인터베이스 얻기
			&g_pDevice,					// [출력] 디바이스 인터베이스 얻기
			NULL,						// [출력] 생성된 디바이스 기능 레벨. 필요없다면 NULL 설정
			&g_pDXDC					// [출력] 디바이스 컨텍스트 얻기
	);

	if (FAILED(hr))
	{
		ynError(hr, L"D3D11CreateDeviceAndSwapChain Failed!");
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// 장치-스왑체인의 렌더타겟(백버퍼) 획득
//
// 리소스뷰(Resource View)  
// DX11 의 리소스는 "공유" 목적으로 유연하게 설계되어 있습니다. 
// (메모리 관리의 효율성 증대 및 다용도 활용이 가능..)
// 따라서 사용전 각 용도에 맞는 접근방식(인터페이스)를 획득(생성)해야 합니다.
// 이 작업을 "리소스 뷰 Resource View" 를 통해 처리합니다.
//                     
// 스왑체인에 연결(Bind)된 렌더링 출력버퍼 - "백버퍼 BackBuffer" 는 2D 기반 리소스(텍스처) 이며
// "렌더타겟 RenderTarget" 용으로 리소스뷰를 생성, 해당 버퍼를 접근/운용 하겠습니다.
//
HRESULT CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// 백버퍼 획득
	ID3D11Texture2D* pBackBuffer;
	// 백버퍼 얻기, 2D 텍스쳐 형태이며 화면 출력용이다.
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
	{
		ynError(hr, L"GetBuffer Failed!");
		return hr;
	}

	// 획득한 백버퍼에 렌더타겟 뷰 생성. (렌더타겟 '형'으로 설정함)
	hr = g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView); //렌더타겟 뷰를 생성.
	if (FAILED(hr))
	{
		ynError(hr, L"Create RenderTargetView Failed!");
		//return hr;
	}

	SAFE_RELEASE(pBackBuffer);

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// 뷰포트 설정 :  DX11 에서는 기본처리 되지 않으며 사용자가 직접 설정해야합니다 
//			     (OpenGL 에서는 예전부터 이렇게 해 왔습니다.)
//
void SetViewPort()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)g_Mode.Width;
	vp.Height = (FLOAT)g_Mode.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_pDXDC->RSSetViewports(1, &vp);
}

/////////////////////////////////////////////////////////////////////////////
//
// 장치-스왑체인/렌더타겟/깊이/스텐실 버퍼 클리어 
//
int ClearBackBuffer(COLOR col)
{
	g_pDXDC->ClearRenderTargetView(g_pRTView, (float*)&col);			//렌더타겟 지우기.

	return YN_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// 장치-스왑체인 전환 : 장면 그리기 완료시 꼭 호출해야 함.
//
int Flip()
{
	g_pSwapChain->Present(0, 0);			//화면출력 : Flip!

	return YN_OK;
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
	
	ynTextDraw(x, y, COLOR(0,1,0,1), L"FPS:%.1f/%d", fps, frmcnt );
}

////////////////////////////////////////////////////////////////////////////// 
//
// 에러 처리 함수들.
//
////////////////////////////////////////////////////////////////////////////// 

////////////////////////////////////////////////////////////////////////////// 
//
// 에러 메세지 처리
//
int ynErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...)
{
	TCHAR msgva[1024] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);


	//HRESULT 에서 에러 메세지 얻기.
	TCHAR herr[1024] = L"";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);

	//에러 메세지 붙이기.
	TCHAR errmsg[2048] = L"";
	_stprintf(errmsg, L"%s \n에러코드(0x%X) : %s \nFile = %s (%d)\nFunc = %s\n\n",
		msgva, hr, herr,
		file, line, func);


	//(디버깅 중) VS 출력창으로 출력..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);

	//로그파일로 출력.
	//...

	//메세지 창 출력..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Yena::Error", MB_OK | MB_ICONERROR);
		//GetLastError(hr);
	}

	return YN_OK;
}






/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// 장치/GPU 정보 획득 함수들
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// 장치/GPU 정보 획득
//
void GetDeviceInfo()
{
	//장치 기능레벨 확인.
	GetFeatureLevel();

	//GPU 정보 얻기.
	//...

	//모니터 정보 얻기.
	//...

}




/////////////////////////////////////////////////////////////////////////////
//
//  DX 기능 레벨 구하기.
//
static TCHAR* strFeature[4][4] =
{
	{ L"DX9",   L"DX9.1",  L"DX9.2", L"DX9.3" },
	{ L"DX10",  L"DX10.1", L"N/A",   L"N/A" },
	{ L"DX11",  L"DX11.1", L"N/A",   L"N/A" },
	{ L"DX12",  L"DX12.1"  L"N/A",   L"N/A" }
};

void GetFeatureLevel()
{
	UINT feat = g_FeatureLevels;
	UINT ver = 0;
	UINT sub = 0;

#define OFFSET 0x9;

	ver = ((feat & 0xf000) >> 12) - OFFSET;	//메인 버전 산출.   	
	sub = ((feat & 0x0f00) >> 8);			//하위 버전 산출.

	g_strFeatureLevel = strFeature[ver][sub];
}




/////////////////////////////////////////////////////////////////////////////
//
// 색상 연산자 오버로딩.
//
COLOR operator * (COLOR& lhs, float rhs)
{
	COLOR col;
	col.x = lhs.x * rhs;
	col.y = lhs.y * rhs;
	col.z = lhs.z * rhs;
	col.w = lhs.w * rhs;

	return col;
}







//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////
//
// 폰트 엔진 ★
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

SpriteBatch* g_pFontBatch = nullptr;
SpriteFont*  g_pFont = nullptr;


///////////////////////////////////////////////////////////////////////////////
// 
int ynFontCreate(LPDEVICE pDev)
{

	//장치 목록 획득.
	LPDXDC pDXDC = nullptr;
	pDev->GetImmediateContext(&pDXDC);


	//Sprite Batch 개체 생성.
	g_pFontBatch = new SpriteBatch(pDXDC);

	//DirectX Toolkit : Sprite Font 객체 생성.
	TCHAR* filename = L"../Yena/Font/굴림9k.sfont";	//ASCII 0 ~ 255 + 특수문자'■' + Unicode 한글 완성형 총 11,440 글자, 크기:9		
	try {
		g_pFont = new SpriteFont(pDev, filename);
		g_pFont->SetLineSpacing(14.0f);				//폰트9 기준, 줄간격 설정. '다중라인 출력시 흐려짐 방지용'
		g_pFont->SetDefaultCharacter('_');			//출력 글자값 미검색시 대신 출력할 키값.
	}
	catch (std::exception& e)
	{
		//ynError(0, L"폰트 생성 실패 : File=%s", filename);
		TCHAR msg[1024] = L"";
		::mbstowcs(msg, e.what(), strlen(e.what()));
		ynError(0, L"폰트 생성 실패 : %s \n File=%s", msg, filename);
	}

	//사용후, 장치목록 해제.
	SAFE_RELEASE(pDXDC);

	return YN_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
void ynFontRelease()
{
	SAFE_DELETE(g_pFontBatch);
	SAFE_DELETE(g_pFont);
}




/////////////////////////////////////////////////////////////////////////////// 
//
// 문자열 출력 : DX11 (유니코드) 대응.
//
void ynTextDraw(int x, int y, COLOR col, TCHAR* msg, ...)
{
	const DWORD _size = 2048;

	TCHAR buff[_size] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(buff, msg, vl);
	va_end(vl);

	g_pFontBatch->Begin();
	//g_pFontBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, ypStateObj[YNSO_DEPTH_ON_]);		//깊이연산 추가.
	g_pFont->DrawString(g_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col));					//원본 크기.
																										//g_pFont->DrawString(ypFontBatch, buff, Vector2((float)x, (float)y), Vector4(col), 0, Vector4::Zero, 0.8f);	//80% 축소 출력.
	g_pFontBatch->End();

}
 

/////////////////////////////////////////////////////////////////////////////// 
//
// 문자열 출력 : GDI, 유니코드 대응. 구형 Dx9 대응
//
//void ynTextDraw(int x, int y, COLORREF col, TCHAR* msg, ...)
//{
//	const DWORD _size = 2048;
//
//	TCHAR buff[_size] = L"";
//	va_list vl;
//	va_start(vl, msg);
//	_vstprintf(buff, msg, vl);
//	va_end(vl);
//
//	HDC hdc = GetDC(g_hWnd);
//	SetTextColor(hdc, col);
//	TextOut(hdc, x, y, buff, (int)_tcslen(buff));
//	SetTextColor(hdc, RGB(0, 0, 0));
//	ReleaseDC(g_hWnd, hdc);
//
//}


 
/***************** End of "Device.cpp" *******************************/


 
