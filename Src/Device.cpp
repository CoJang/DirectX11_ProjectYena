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
ID3D11Device*           g_pDevice = NULL;
ID3D11DeviceContext*	g_pDXDC = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRTView = NULL;

//깊이 스텐실 버퍼 관련.
ID3D11Texture2D*		 g_pDS = NULL;			//깊이-스텐실 버퍼.
ID3D11DepthStencilView*  g_pDSView = NULL;		//깊이-스텐실 뷰.



												//장치 설정 기본 정보.
DISPLAYMODE g_Mode = { 960, 600, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM };

//D3D 기능 레벨 (Direct3D feature level) 
//현재 사용할 DX 버전 지정. DX 렌더링 장치의 호환성 향상
D3D_FEATURE_LEVEL g_FeatureLevels = D3D_FEATURE_LEVEL_11_0;		//DX11 대응.
TCHAR* g_strFeatureLevel = L"N/A";

//장치 정보
DXGI_ADAPTER_DESC1 g_Adc;



//전체화면 사용여부.
BOOL g_bWindowMode = TRUE;
//수직동기화 사용여부.
BOOL g_bVSync = FALSE;


// 초기 장치 설정 관련 함수들.
//
HRESULT CreateDeviceSwapChain(HWND hwnd);
HRESULT CreateRenderTarget();
HRESULT CreateDepthStencil();
void	SetViewPort();



///////////////////////////////////////////////////////////////////////////////
//
// 깊이/스텐실 상태 객체 관련  
//
///////////////////////////////////////////////////////////////////////////////

//깊이/스텐실 상태 객체들 : 엔진 전체 공유함.
ID3D11DepthStencilState* g_DSState[DS_MAX_];

int  StateObjectCreate();
void StateObjectRelease();






///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 

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

	// 깊이/스텐실 버퍼 생성.
	CreateDepthStencil();

	// 장치 출력병합기(Output Merger) 에 렌터링 타겟 및 깊이-스텐실 버퍼 등록.
	g_pDXDC->OMSetRenderTargets(
		1,				// 렌더타겟 개수.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&g_pRTView,		// 렌더타겟("백버퍼") 등록.	
		g_pDSView		// 깊이/스텐실 버퍼 등록.
	);

	// 뷰포트 설정.
	SetViewPort();


	//장치 정보 획득
	GetDeviceInfo();


	//----------------------------------------
	// 2단계 : 기타 렌더링 관련 추가 설정.
	//----------------------------------------
	// 렌더링에 필요한 사용자 객체등을 생성/관리 합니다.
	// 카메라, 조명, 폰트, 셰이더 등등...

	//깊이-스텐실 렌더링 상태 객체 생성.
	StateObjectCreate();


	// 폰트 생성.--> Yena 에 포함됨.
	//ynFontCreate(g_pDevice);

	// Yena 셋업 : 시스템 공통 객체 제공 
	// 내장 기본 카메라, 조명, 폰트, 방향축, 그리드, 등등..
	YenaSetup(g_pDevice, g_Mode, 5.0f, 100, 2.5f);


	//작업 완료, 장치 준비 완료.
	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////// 
//
// 디바이스 개체 및 관련 장치 제거 : 종료시 1회 호출.  
// 기본 렌더링 개체들 제거시 생성된 역순으로 처리합니다.(권장)
//
void DXRelease()
{
	//장치 상태 리셋 : 제거 전에 초기화를 해야 합니다. (메모리 누수 방지)
	if (g_pDXDC) g_pDXDC->ClearState();
	//if (g_pSwapChain) g_pSwapChain->SetFullscreenState(false, NULL);

	//예나 제거.
	YenaRelease();
	//ynFontRelease();					//폰트 제거.--> Yena 에 포함됨.

	//상태 객체 제거.
	StateObjectRelease();


	SAFE_RELEASE(g_pDS);				//깊이/스텐실 버퍼 제거.
	SAFE_RELEASE(g_pDSView);
	SAFE_RELEASE(g_pRTView);			//렌더타겟 제거.
	SAFE_RELEASE(g_pSwapChain);			//스왑체인 제거.
	SAFE_RELEASE(g_pDXDC);
	SAFE_RELEASE(g_pDevice);			//디바이스 제거. 맨 나중에 제거합니다.
}





/////////////////////////////////////////////////////////////////////////////
//
// D3D 렌더링 '장치(Device)' 및 스왑체인 생성. 
//
HRESULT CreateDeviceSwapChain(HWND hwnd)
{
	HRESULT hr = S_OK;

	// 장치 (Device) 및 스왑체인(SwapChain) 정보 구성.
	// 스왑체인은 다중버퍼링 시스템을 말하며
	// 고전적인 '플립핑Flipping' 체인과 동일한 의미입니다.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = g_bWindowMode;				//풀스크린 또는 창모드 선택.
	sd.OutputWindow = hwnd;						//출력할 윈도우 핸들.
	sd.BufferCount = 1;							//백버퍼 개수.
	sd.BufferDesc.Width = g_Mode.Width;			//해상도 결정.(백버퍼 크기)
	sd.BufferDesc.Height = g_Mode.Height;
	sd.BufferDesc.Format = g_Mode.Format;		//백버퍼 색상규격 (A8R8G8B8) 창모드에서는 생략 가능 
	sd.BufferDesc.RefreshRate.Numerator = g_bVSync ? 60 : 0;   //버퍼 갱신율.(수직동기화 VSync 활성화시 표준갱신율 적용 : 60hz)
															   //sd.BufferDesc.RefreshRate.Numerator = 0;	//버퍼 갱신율.(수직동기화 VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//용도 설정: '렌더타겟' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = 1;					//AA 설정
	sd.SampleDesc.Quality = 0;



	// D3D 렌더링 '장치(Device)' 및 스왑체인 생성. 
	//
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//비디오 어뎁터 포인터 (기본장치는 NULL)
		D3D_DRIVER_TYPE_HARDWARE,	//HW 가속 
		NULL,						//SW Rasterizer DLL 핸들.  HW 가속시에는 NULL.
		0,							//디바이스 생성 플래그.(기본값)
									//D3D11_CREATE_DEVICE_DEBUG,//디바이스 생성 플래그.(디버그)
		&g_FeatureLevels,			//(생성할) 디바이스 기능 레벨(Feature Level) 배열
		1,							//(생성할) 디바이스 기능 레벨(Feature Level) 배열 크기.
		D3D11_SDK_VERSION,			//DX SDK 버전.
		&sd,						//디바이스 생성 상세 옵션.
		&g_pSwapChain,				//[출력] 스왑체인 인터페이스 얻기.
		&g_pDevice,					//[출력] 디바이스 인터페이스 얻기.
		NULL,						//[출력] (생성된) 디바이스 기능 레벨. 필요없다면 NULL 설정. 
		&g_pDXDC					//[출력] 디바이스 컨텍스트 얻기.
	);
	if (FAILED(hr))
	{
		ynError(hr, L"디바이스 / 스왑체인 생성 실패.");
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

	// 백버퍼 획득.
	ID3D11Texture2D *pBackBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);		// 백버퍼 얻기, 2D 텍스쳐 행태, 화면출력용
	if (FAILED(hr))	return hr;

	//획득한 백버퍼에 렌더타겟 뷰 생성.(렌더타겟'형'으로 설정함)
	hr = g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView);		// '렌더타겟뷰'를 생성.	
	if (FAILED(hr))
	{
		ynError(hr, L"백버퍼-렌더타겟뷰 생성 실패.");
		//return hr;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	SAFE_RELEASE(pBackBuffer);

	return hr;
}





/////////////////////////////////////////////////////////////////////////////
//
// 깊이-스텐실버퍼 생성. : DX11 에서는 렌더링버퍼-렌더타겟뷰- 와 마찬가지로, 
//                       깊이-스텐실버퍼역시 직접 만들어야 합니다.
//                       디바이스에 등록도 역시 수동입니다.
//
HRESULT CreateDepthStencil()
{
	HRESULT hr = S_OK;

	//깊이/스텐실 버퍼용 정보 구성.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = g_Mode.Width;
	td.Height = g_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. 깊이 버퍼.
	td.SampleDesc.Count = 1;					// AA 설정 - RT 과 동일 규격 준수.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 깊이-스텐실 버퍼용으로 설정.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// 깊이 버퍼 생성.
	//ID3D11Texture2D* pDS = NULL;						 
	hr = g_pDevice->CreateTexture2D(&td, NULL, &g_pDS);
	if (FAILED(hr))
	{
		ynError(hr, L"깊이/스텐실 버퍼용 CreateTexture 실패.");
		return hr;
	}


	// 깊이-스텐실버퍼용 리소스 뷰 정보 설정. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //AA 없음.
	dd.Texture2D.MipSlice = 0;

	//깊이-스텐실 버퍼 뷰 생성.
	hr = g_pDevice->CreateDepthStencilView(g_pDS, &dd, &g_pDSView);
	if (FAILED(hr))
	{
		ynError(hr, L"깊이/스텐실뷰 생성 실패.");
		return hr;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	//SAFE_RELEASE(pDS);

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
int ClearBackBuffer(UINT flag, COLOR col, float depth, UINT stencil)
{
	g_pDXDC->ClearRenderTargetView(g_pRTView, (float*)&col);			//렌더타겟 지우기.
	g_pDXDC->ClearDepthStencilView(g_pDSView, flag, depth, stencil);	//깊이/스텐실 지우기.

	return YN_OK;
}



/////////////////////////////////////////////////////////////////////////////
//
// 장치-스왑체인 전환 : 장면 그리기 완료시 꼭 호출해야 함.
//
int Flip()
{
	g_pSwapChain->Present(g_bVSync, 0);			//화면출력 : Flip! (+수직동기화)

	return YN_OK;
}




////////////////////////////////////////////////////////////////////////////// 
//
// 장치 렌더링 상태 객체 생성.
//
//----------------------------------------------------------------------------
// 상태 객체 State Objects (DX10/11)
// DX10 부터 구형 TnL 의 RenderState 가 제거되었습니다.
// 이를 대신하는 것이 상태객체 State Objects 인터페이스로, 렌더링 상태를 하나의 그룹으로 
// 묶고 렌더링시 디바이스에 설정합니다.  이를 통해 장치의 어려 상태 변화를 한번에 설정하여 
// 불필요한 연산부하(Overhead) 를 줄이고 보다 효과적인 렌더링을 가능케 합니다.
//
// 상태객체는 엔진 초기시 제작후 사용하기를 권장하며 종료시 제거(Release) 해야 합니다.
// 상태객체는 수정불가능(Immutable, 읽기전용) 개체 입니다.
// DX9 에서는 State-Block 이 이와 유사한 기능을 담당했었습니다.
//
// 상태 객체 인터페이스 중 '레스터라이즈 스테이지 Rasterize Stage' 상태 조절은 
// ID3D11RasterizerState 인터페이스를 통해 처리합니다.  
// 간단하게 렌더링 설정/기능 모듬 정도로 생각합시다.  자세한 것은 다음을 참조하십시오. 
// 링크1 : 상태 객체 https://msdn.microsoft.com/en-us/library/windows/desktop/bb205071(v=vs.85).aspx
// 링크2 : 깊이버퍼 상태 구성하기 https://msdn.microsoft.com/ko-kr/library/windows/desktop/bb205074(v=vs.85).aspx#Create_Depth_Stencil_State 
//----------------------------------------------------------------------------
//
int StateObjectCreate()
{

	//----------------------------
	// 레스터 상태 개체 생성 : "레스터라이즈 스테이지 Rasterize Stage" 상태 조절.
	//----------------------------
	//...


	//----------------------------
	// 깊이/스텐실 상태 개체 생성.: "출력병합기 Output Merger" 상태 조절. 
	//----------------------------
	//...	 
	D3D11_DEPTH_STENCIL_DESC  ds;
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = FALSE;
	//ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	//ds.StnecilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//...이하 기본값, 생략... 
	//...
	//첫번째 상태 객체 : Z-Test ON! (기본값)
	g_pDevice->CreateDepthStencilState(&ds, &g_DSState[DS_DEPTH_ON]);

	//두번째 상태 객체 : Z-Test OFF 상태.
	ds.DepthEnable = FALSE;
	g_pDevice->CreateDepthStencilState(&ds, &g_DSState[DS_DEPTH_OFF]);

	//세번째 상태 객체 : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) 이 꺼지면, Z-Write 역시 비활성화 됩니다.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이값 쓰기 끔.
	g_pDevice->CreateDepthStencilState(&ds, &g_DSState[DS_DEPTH_WRITE_OFF]);



	//----------------------------
	// 알파블렌딩 상태 개체 생성 : "출력병합기 Output Merger" 상태 조절. 
	//----------------------------
	//...


	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////////
//
//  상태객체 제거하기 : 엔진 종료시 1회 호출.
//
void StateObjectRelease()
{
	for (int i = 0; i < DS_MAX_; i++)
		SAFE_RELEASE(g_DSState[i]);
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
	if (time >= 999)
	{
		oldtime = nowtime;

		fps = (float)frmcnt * 1000 / (float)time;
		frmcnt = 0;
	}

	//ynTextDraw(x, y, RGB(0, 0, 255), L"FPS:%.1f/%d", fps, frmcnt );
	ynTextDraw(x, y, COLOR(0, 1, 0, 1), L"FPS:%.1f/%d", fps, frmcnt);
}




/////////////////////////////////////////////////////////////////////////////
//
// 엔진 동기화용 경과시간 획득.
//
float GetEngineTime()
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();
	float dTime = (nowtime - oldtime) * 0.001f;
	oldtime = nowtime;

	return dTime;
}





////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
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



////////////////////////////////////////////////////////////////////////////// 
//
// 에러 메세지 처리 : 셰이더 에러 처리용.
// 
int ynErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob,
	TCHAR* filename, char* EntryPoint, char* ShaderModel)
{

	/*//가변매개변수 처리.
	TCHAR msgva[2048] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);
	*/
	//파라미터, 유니코드로 전환.
	TCHAR func[80] = L"";
	::mbstowcs(func, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs(sm, ShaderModel, strlen(ShaderModel));


	//셰이더 오류 메세지 읽기.
	TCHAR errw[4096] = L"";
	::mbstowcs(errw, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT 에서 에러 메세지 얻기 
	//시스템으로 부터 얻는 셰이더 오류메세지는 부정확하므로 생략.
	TCHAR herr[1024] = L"아래의 오류를 확인하십시오.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	herr, 1024, NULL);
	*/


	//에러 메세지 붙이기.
	TCHAR errmsg[1024];
	_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n에러코드(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);


	//(디버깅 중) VS 출력창으로 출력..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);
	//OutputDebugString(errw);


	//로그파일로 출력.
	//...


	//메세지 창 출력..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Yena::Error", MB_OK | MB_ICONERROR);
		//MessageBox(NULL, errw, L"Yena::Error", MB_OK | MB_ICONERROR);
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
	GetAdapterInfo(&g_Adc);

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
// 장치/GPU 정보 획득 
//
// 기본 장치만 처리함.다중 GPU 구성시에는 별도의 열거 처리가 필요.
// 관련항목1 : DirectX Graphics Infrastructure (DXGI) /  https://msdn.microsoft.com/ko-kr/library/windows/desktop/ee417025(v=vs.85).aspx
// 관련항목2 : How To: Enumerate Adapters  /  https://msdn.microsoft.com/en-us/library/windows/desktop/ff476877(v=vs.85).aspx 
// 관련항목3 : DXGI_ADAPTER_DESC structure /  https://msdn.microsoft.com/ko-kr/library/windows/desktop/bb173058(v=vs.85).aspx
//

// 디바이스/GPU 정보 획득 <방법 #1> 
// DXGI 1.1 / DXGI Factory 사용 / DX11 이상 권장
//
HRESULT GetAdapterInfo(DXGI_ADAPTER_DESC1* pAd)
{
	IDXGIAdapter1* pAdapter;
	IDXGIFactory1* pFactory = NULL;
	//DXGI_ADAPTER_DESC ad;

	//DXGIFactory 개체 생성. (DXGI.lib 필요)
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		return E_FAIL;
	}

	pFactory->EnumAdapters1(0, &pAdapter);		//어뎁터 획득.
	pAdapter->GetDesc1(pAd);					//어뎁터 정보 획득.
												//*pAd = ad;								//외부로 복사.

												//정보 취득후, 접근한 인터페이스를 해제합니다. (메모리 누수 방지)
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	return S_OK;
}

/*
// 디바이스/GPU 정보 획득 <방법 #2>
// DXGI 1.1 / 지정 장치 Interface 사용 / DX11 이상 권장
//
HRESULT GetAdapterInfo(DXGI_ADAPTER_DESC1* pAd)
{
	IDXGIDevice* pDXGIDev;
	IDXGIAdapter1* pAdapter;

	//DXGIFactory 개체 생성. (DXGI.lib 필요)
	if (FAILED(g_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDev)))
	{
	return E_FAIL;
	}

	pDXGIDev->GetAdapter((IDXGIAdapter**)&pAdapter);		//어뎁터 획득.
	pAdapter->GetDesc1(pAd);					//어뎁터 정보 획득.

	//정보 취득후, 접근한 인터페이스를 해제합니다. (메모리 누수 방지)
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pDXGIDev);

	return S_OK;
}
*/




/////////////////////////////////////////////////////////////////////////////
//
// 장치/GPU 정보 출력
//
// 관련항목1 : DXGI_ADAPTER_DESC structure
// 참조링크1 : https://msdn.microsoft.com/ko-kr/library/windows/desktop/bb173058(v=vs.85).aspx
//
void AdapterInfo(int x, int y, COLOR col)
{
#define ToMB(a) (a/1024/1024)

	DXGI_ADAPTER_DESC1& dc = g_Adc;
	ynTextDraw(x, y += 14, COLOR(0, 1, 0, 1), L"[GPU #%d]", 0);
	//GPU 정보.
	ynTextDraw(x, y += 14, col, L"Adapter: %u", 0);		//기본 장치만 처리함. 다중 GPU 구성시에는  열거처리가 필요.
	ynTextDraw(x, y += 14, col, L"Description: %s", dc.Description);
	ynTextDraw(x, y += 14, col, L"VendorID: %u", dc.VendorId);
	ynTextDraw(x, y += 14, col, L"DeviceID: %u", dc.DeviceId);
	ynTextDraw(x, y += 14, col, L"SubSysID: %u", dc.SubSysId);
	ynTextDraw(x, y += 14, col, L"Revision: %u", dc.Revision);
	ynTextDraw(x, y += 14, col, L"VideoMem: %lu MB", ToMB(dc.DedicatedVideoMemory));
	ynTextDraw(x, y += 14, col, L"SystemMem: %lu MB", ToMB(dc.DedicatedSystemMemory));
	ynTextDraw(x, y += 14, col, L"SharedSysMem: %lu MB", ToMB(dc.SharedSystemMemory));
	ynTextDraw(x, y += 14, col, L"AdpaterLuid: %u.%d", dc.AdapterLuid.HighPart, dc.AdapterLuid.LowPart);

}




/////////////////////////////////////////////////////////////////////////////
//
// 장치/GPU 및 시스템 정보 출력
//
void SystemInfo(int x, int y, COLOR col)
{
	COLOR col2 = col * 0.7f;

	ynTextDraw(x, y += 14, col, L"[SYSTEM]");
	ynTextDraw(x, y += 14, col, L"VGA: %s", g_Adc.Description);
	ynTextDraw(x, y += 14, col, L"Feat: %s", g_strFeatureLevel);
	ynTextDraw(x, y += 14, col, L"해상도: %dx%d", g_Mode.Width, g_Mode.Height);

	//GPU 정보 출력.
	//AdapterInfo(x, y += 14, col);

	//Yena 정보 출력.
	YenaSysInfo(x, y, col);
}




/*  Yena Math 에 포함됨.
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
*/






//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////
//
// 폰트 엔진 --> Yena 에 포함됨 
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
/*
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
	g_pFont = new SpriteFont(pDev, filename );
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
	//g_pFontBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, ypStateObj[YNDS_DEPTH_ON_]);		//깊이연산 추가.
	g_pFont->DrawString(g_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col));					//원본 크기.
	//g_pFont->DrawString(ypFontBatch, buff, Vector2((float)x, (float)y), Vector4(col), 0, Vector4::Zero, 0.8f);	//80% 축소 출력.
	g_pFontBatch->End();

}
*/


/***************** End of "Device.cpp" *******************************/



