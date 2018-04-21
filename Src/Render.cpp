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
#include "Shader.h"

TCHAR* g_WindowName = L"CoJang & Yena Collaboration Lambert Model";



/////////////////////////////////////////////////////////////////////////////
//
//  전역 데이터들.
//  
COLOR g_clearColor(0, 0.25f, 0.3f, 1);  //배경색
  
struct COLVTX
{
	float x, y, z;			// 좌표
	float nx, ny, nz;		// 노멀 : Normal
};

// 모델용 기하버퍼.
ID3D11Buffer*		g_pVB = nullptr;		// 정점 버퍼 포인터.
ID3D11InputLayout*	g_pVBLayout = nullptr;  // 정점 입력구조

// 기본 상수버퍼 데이터 : 렌더링 기본 정보를 운용.
cbDEFAULT g_cbDef;

XMFLOAT3 g_vPos(0, 0, 0);
XMFLOAT3 g_vRot(0, 0, 0);
XMFLOAT3 g_vScale(1, 1, 1);

int		ObjLoad();
void	ObjRelease();
void	ObjUpdate(float dTime);
void	ObjDraw(float dTime);


///////////////////////////////////////////////////////////////////////////// 
//
// 큐브 데이터.
//
// - 각 큐브는 삼각형 리스트 형태로 제작.
// - 각 '면' 별로 face 2개 설정.
// - 법선(Normal) 벡터의 방향은 각 면의 '밖'을 의미.
// - 정확한 법선(음영 처리)을 위해서는 정점의 중복은 불가피.
//
// [그림] 각 면의 정점 구성.
//  v0 --- v1
//   |    / | 
//   |   /  |
//   |  /   |
//   | /    |
//  v2 --- v3
//
COLVTX	g_vtxCube[] = {

	//정면. ( Face#0) :   
	{ -1.0, 1.0,-1.0,  0.0, 0.0,-1.0 } ,		//v0 : Position + Normal.★
	{ 1.0, 1.0,-1.0,  0.0, 0.0,-1.0 } ,		//v1
	{ -1.0,-1.0,-1.0,  0.0, 0.0,-1.0 } ,		//v2
												//       ( Face#1)
	{ -1.0,-1.0,-1.0,  0.0, 0.0,-1.0 } ,
	{ 1.0, 1.0,-1.0,  0.0, 0.0,-1.0 } ,
	{ 1.0,-1.0,-1.0,  0.0, 0.0,-1.0 } ,

	//뒷면.  (Face#2) :  
	{ 1.0, 1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ -1.0, 1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ -1.0,-1.0, 1.0,  0.0, 0.0, 1.0 } ,
	//		  (Face#3)
	{ 1.0, 1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ -1.0,-1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ 1.0,-1.0, 1.0,  0.0, 0.0, 1.0 } ,


	// 우측면. (Face#4)
	{ 1.0, 1.0,-1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0,-1.0,-1.0,  1.0, 0.0, 0.0 } ,
	//			(Face#5)
	{ 1.0,-1.0,-1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0,-1.0, 1.0,  1.0, 0.0, 0.0 } ,


	// 좌측면. (Face#6)
	{ -1.0, 1.0, 1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0, 1.0,-1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  -1.0, 0.0, 0.0 } ,
	//			(Face#7)
	{ -1.0, 1.0, 1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0,-1.0, 1.0,  -1.0, 0.0, 0.0 } ,


	//상부.  ( Face#8)
	{ -1.0, 1.0, 1.0,  0.0, 1.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  0.0, 1.0, 0.0 } ,
	{ -1.0, 1.0,-1.0,  0.0, 1.0, 0.0 } ,
	//		 ( Face#9)
	{ -1.0, 1.0,-1.0,  0.0, 1.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  0.0, 1.0, 0.0 } ,
	{ 1.0, 1.0,-1.0,  0.0, 1.0, 0.0 } ,


	//하부.  ( Face#10)
	{ 1.0,-1.0, 1.0,  0.0,-1.0, 0.0 } ,
	{ -1.0,-1.0, 1.0,  0.0,-1.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  0.0,-1.0, 0.0 } ,
	//		 ( Face#11)
	{ 1.0,-1.0, 1.0,  0.0,-1.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  0.0,-1.0, 0.0 } ,
	{ 1.0,-1.0,-1.0,  0.0,-1.0, 0.0 } ,

};

////////////////////////////////////////////////////////////////////////////// 
// 렌더링 상태 조절 데이터/함수들.

BOOL g_bCullBack = TRUE;		//뒷면 제거...On/Off.
BOOL g_bWireFrame = FALSE;		//와이어 프레임 On/Off.
BOOL g_bZEnable = TRUE;			//깊이 버퍼 연산 On/Off.
BOOL g_bZWrite = TRUE;			//깊이버퍼 쓰기 On/Off.	


// 
// 레스터라이져 상태 객체 Rasterizer State Objects
// 
enum 
{
	RS_SOLID,				//삼각형 채우기 : Fill Mode - Soild.
	RS_WIREFRM,				//삼각형 채우기 : Fill Mode - Wireframe.
	RS_CULLBACK,			//뒷면 컬링 (ON) : BackFaceCulling - "CCW" 
	RS_WIRECULLBACK,		//와이어 프레임 + 뒷면 컬링 (ON) 

	RS_MAX_
};

//레스터라이져 상태 객체 배열
ID3D11RasterizerState*	g_RState[RS_MAX_] = { NULL, };

void RasterStateCreate();
void RasterStateRelease();
void RenderModeUpdate();

//렌더링 모드 : 다수의 렌더링 모드 조합 및 운용을 위한 정의.
enum 
{
	RM_SOLID = 0x0000,			// 삼각형채우기 : ON, Solid
	RM_WIREFRAME = 0x0001,		// 삼각형채우기 : OFF, Wire-frame
	RM_CULLBACK = 0x0002,		// 뒷면 컬링 : ON, "CCW"

	//렌더링 기본모드 : Solid + Cull-On.
	RM_DEFAULT = RM_SOLID | RM_CULLBACK,

};
DWORD g_RMode = RM_DEFAULT;		//'현재' 렌더링 모드.

/////////////////////////////////////////////////////////////////////////////
// 조명 정보 Lighting ★
//
// DX9 은 전용구조체(D3DLIGHT9) 를 통해 HW 조명을 수행하였습니다. 
// DX10 부터는 사용자가 필요한 정보를 구성하여 운용해야 합니다.
// 실제 계산 역시 셰이더를 통해 여러분이 직접 처리해야 합니다.

//조명 구조체 : DX9 과 비슷하지만, 필요한 정보만 구성합니다.★
struct LIGHT
{
	XMFLOAT3 Direction;		//빛의 방향.
	COLOR    Diffuse;		//주 광량 : 확산광 Diffuse Light.
	COLOR    Ambient;		//보조 광량 : 주변광 Ambient Light.
	FLOAT    Range;			//빛 도달 거리.
};

LIGHT g_Light;					//조명 정보.
BOOL  g_bLightEnable = TRUE;	//조명 플래그. On/Off. ★

cbLIGHT g_cbLit;				//조명 상수버퍼 갱신용. 

void LightsUpdate(float dTime);

/////////////////////////////////////////////////////////////////////////////
// 재질 정보 Material ★
// 재질 Material 은 렌더링 개체별 빛에 반응하는 고유 성질입니다. 
// 따라서 각 개체별로 분리, 운용하는 것이 좋겠습니다.★ 
// DX9 은 전용구조체(D3DMATERIAL9) 를 통해 HW 조명시 재질 정보를 사용하였으나 
// DX10 부터는 사용자가 필요한 정보를 구성하여 운용해야 합니다.
// 실제 계산 역시 셰이더를 통해 여러분이 직접 처리해야 합니다.
//

//재질 구조체 : DX9 과 비슷하지만, 필요한 정보만 구성합니다.★
struct MATERIAL
{
	COLOR    Diffuse;		//주 광량(확산광) 의 반사율(%) 
	COLOR    Ambient;		//보조 광량(주변광) 의 반사율(%) 
};

MATERIAL g_Mtrl;				//재질 정보.

cbMATERIAL g_cbMtrl;			//재질 상수버퍼용 갱신용.


/////////////////////////////////////////////////////////////////////////////
//
int DataLoading()
{ 
	// 데이터 로딩/생성 코드는 여기에...
	// ...

	//셰이더 로드.
	ShaderLoad();

	// 기본 상수 버퍼 생성.
	ZeroMemory(&g_cbDef, sizeof(cbDEFAULT));
	CreateDynamicConstantBuffer(sizeof(cbDEFAULT), &g_cbDef, &g_pCBDef);

	// 조명용 상수버퍼 생성.★
	// 조명정보가 실시간으로 변하지 않는다면, 일반(정적) 상수버퍼로도 충분합니다.
	ZeroMemory(&g_cbLit, sizeof(cbLIGHT));
	CreateDynamicConstantBuffer(sizeof(cbLIGHT), &g_cbLit, &g_pCBLit);

	// 재질용 상수버퍼 생성.★
	// 재질정보가 실시간으로 변하지 않는다면, 일반(정적) 상수버퍼로도 충분합니다.
	ZeroMemory(&g_cbMtrl, sizeof(cbMATERIAL));
	CreateDynamicConstantBuffer(sizeof(cbMATERIAL), &g_cbMtrl, &g_pCBMtrl);



	//오브젝트 로딩.
	ObjLoad();

	//레스터 상태 개체 생성.
	RasterStateCreate();

	//그리드 ON, 방향축 OFF.
	//ynGridAxisState(FALSE, FALSE);

	return TRUE;
}

void DataRelease()
{
	// 데이터 해제 코드는 여기에..
	// ... 

	ObjRelease();			//오브젝트 삭제 
	ShaderRelease();		//셰이더 삭제.
	RasterStateRelease();	//레스터 상태 개체 제거.

}

/////////////////////////////////////////////////////////////////////////////
//
//  오브젝트 : 생성 및 필요 리소스 로딩.
//
int ObjLoad()
{
	HRESULT hr = S_OK;

	/*// 기하 도형을 위한 정점 배열
	// 일반적으로는 외부(HDD) 에서 로딩합니다.
	COLVTX	Vertices[] = {
	// 정점의 좌표값은 모델 공간(Model Space, Local Space)좌표.
	// 좌표 (x, y, z)    색상(R G B A), Alpha 는 기본값 1.
	//Face 0 : 정삼각형.(CW)
	{ -1.0f,  0.0f, 0.0f,   1, 0, 0, 1 },	//빨강. 모델의 크기 변경.
	{  0.0f,  2.0f, 0.0f,   0, 1, 0, 1 },	//초록.
	{  1.0f,  0.0f, 0.0f,   0, 0, 1, 1 },	//파랑.
	//Face 1 : 역삼각형.(CCW)
	{ -0.5f,  0.0f, 0.0f,   1, 0, 0, 1 },	//빨강.
	{  0.0f, -1.0f, 0.0f,   0, 1, 0, 1 },	//초록.
	{  0.5f,  0.0f, 0.0f,   0, 0, 1, 1 },	//파랑.
	};*/


	// 정점 버퍼 Vertex Buffer 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//버퍼 사용방식
	bd.ByteWidth = sizeof(g_vtxCube);				//버퍼 크기 sizeof( VTX_MESH ) *3 ;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버퍼 용도 : "정점 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = g_vtxCube;					//버퍼에 들어갈 데이터 설정 : "정점들"..

											//정점 버퍼 생성.
	hr = g_pDevice->CreateBuffer(&bd, &rd, &g_pVB);
	if (FAILED(hr))	return hr;




	// 정점 입력구조 Input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	//★
	};
	UINT numElements = ARRAYSIZE(layout);

	// 정접 입력구조 객체 생성 Create the input layout
	hr = g_pDevice->CreateInputLayout(layout,
		numElements,
		g_pVSCode->GetBufferPointer(),
		g_pVSCode->GetBufferSize(),
		&g_pVBLayout
	);
	if (FAILED(hr))	return hr;


	return TRUE;
}



////////////////////////////////////////////////////////////////////////////// 
//
// 오브젝트 : 제거 
//
void ObjRelease()
{
	SAFE_RELEASE(g_pVB);
	SAFE_RELEASE(g_pVBLayout);
}



////////////////////////////////////////////////////////////////////////////// 
//
// 오브젝트 갱신
//
void ObjUpdate(float dTime)
{
	if (IsKeyDown(VK_F9))		//모델 정보 리셋.
	{
		g_vPos = g_vRot = XMFLOAT3(0, 0, 0);
		g_vScale = XMFLOAT3(1, 1, 1);
	}

	//자동회전처리..
	//static bool bAni = true;
	static bool bAutoRot = true;
	if (IsKeyUp('R')) { bAutoRot ^= true; }


	//스케일 처리.
	XMMATRIX mScale = XMMatrixScaling(g_vScale.x, g_vScale.y, g_vScale.z);

	//이동 처리.
	float mov = 5.0f * dTime;		// 5.0 / sec 이동량.
	if (IsKeyDown(VK_LEFT))  g_vPos.x -= mov;
	if (IsKeyDown(VK_RIGHT)) g_vPos.x += mov;
	if (IsKeyDown(VK_UP))	 g_vPos.z += mov;
	if (IsKeyDown(VK_DOWN))  g_vPos.z -= mov;
	XMMATRIX mTrans = XMMatrixTranslation(g_vPos.x, g_vPos.y, g_vPos.z);

	//회전 처리.  
	float rot = XM_PI * 0.5f * dTime;		//90º/sec 씩 회전.(라디안)
	if (IsKeyDown('A'))  g_vRot.y += rot;
	if (IsKeyDown('D'))  g_vRot.y -= rot;
	if (IsKeyDown('W'))  g_vRot.x += rot;
	if (IsKeyDown('S'))  g_vRot.x -= rot;
	if (bAutoRot) g_vRot.y += rot * 0.5f;
	//g_vRot.y += XMConvertToRadians(90) * dTime;	//90º/sec 씩 회전.(DirectXMath 사용)	
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(g_vRot.x, g_vRot.y, g_vRot.z);
	//XMMATRIX mRot = XMMatrixRotationY(g_vRot.y);


	//-----------------------
	// 변환 행렬 결합 
	//-----------------------
	XMMATRIX mTM = mScale * mRot * mTrans;



	//-----------------------
	// 상수버퍼 갱신.
	//-----------------------
	g_cbDef.mTM = mTM;
	g_cbDef.mWV = mTM * g_cbDef.mView;		//★
	//g_cbDef.mTM = XMMatrixTranspose(mTM);	//셰이더에서 '열 우선 Column major' 기준으로 처리하면 속도 향상을 기대할 수 있습니다. 이를 위한 행렬 전치 처리.	


	//셰이더 상수 버퍼 갱신.
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBDef, &g_cbDef, sizeof(cbDEFAULT));


	//-----------------------
	// 재질 정보 갱신.★
	//-----------------------
	g_Mtrl.Diffuse = COLOR(1, 1, 1, 1);
	g_Mtrl.Ambient = COLOR(1, 1, 1, 1); //////

										//재질 정보 상수버퍼 갱신.
	g_cbMtrl.Diffuse = XMLoadFloat4(&g_Mtrl.Diffuse);
	g_cbMtrl.Ambient = XMLoadFloat4(&g_Mtrl.Ambient);
	//상수버퍼 갱신 : 재질정보가 실시간으로 변하지 않는다면, 일반(정적) 상수버퍼로도 충분합니다.
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBMtrl, &g_cbMtrl, sizeof(cbMATERIAL));


}




////////////////////////////////////////////////////////////////////////////// 
//
// 오브젝트 그리기.
//
void ObjDraw()
{

	//정점 버퍼 설정
	UINT stride = sizeof(COLVTX);
	UINT offset = 0;
	g_pDXDC->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);

	//입력 레이아웃 설정. Set the input layout
	g_pDXDC->IASetInputLayout(g_pVBLayout);

	// 기하 위상 구조 설정 Set primitive topology
	g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//셰이더 설정.
	g_pDXDC->VSSetShader(g_pVS, nullptr, 0);
	g_pDXDC->PSSetShader(g_pPS, nullptr, 0);
	g_pDXDC->VSSetConstantBuffers(0, 1, &g_pCBDef);		//VS 에 상수 버퍼 설정.
	g_pDXDC->VSSetConstantBuffers(1, 1, &g_pCBLit);		//VS 에 조명 정보 설정.★ 
	g_pDXDC->VSSetConstantBuffers(2, 1, &g_pCBMtrl);	//VS 에 재질 정보 설정.★ 


														//그리기! Render a triangle 
	g_pDXDC->Draw(36, 0);


	//장치 상태 리셋 
	//g_pDXDC->ClearState();
}

////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// [DirectX 수학라이브러리 사용 안내] 
//	
// 예제의 수학/벡터 연산은 CPU 의 빠른 부동소수 연산가속(SSE/SSE2)을 위해 
// SIMD 지원 자료형 (XMMATRIX, XMVECTOR) 을 사용하고 있습니다.
// 대신 개별 정보처리 -  데이터 저장, 읽기, 쓰기 - 와 자료형 변환 등의 불편한 
// 추가 작업이 요구되며 이로 인해 코드의 간결함이나 가독성도 떨어집니다.
//
// 비 SIMD 자료형 (XMFLAOT4X4, XMFLOAT3 ...) 등을 사용한다면, 개별 정보 처리가 용이해지지만
// SIMD 가속을 받을 수 없고, 대부분의 DirectXMath 함수등이 SIMD 자료형에 맞추어 설계되어 있어 
// 여러 연산자 오버로딩을 사용자가 직접 추가해야 합니다.
//
// DirectXTK / SimpleMath 를 사용하면 위의 2가지 장단점을 적절히 차용, 보다 직관적이고 가독성 높은 
// 코드를 유지 할 수 있습니다. 단 DirectXMath 사용시 보다 약간의 성능 저하가 발생합니다.
//
// 좀더 자세한 것은 아래를 참조하십시오.
//
// 링크1 : DirectXMath 가이드 https://msdn.microsoft.com/ko-kr/library/windows/desktop/ee415571(v=vs.85).aspx
// 링크2 : DirectX와 SSE 가속 https://blogs.msdn.microsoft.com/chuckw/2012/09/11/directxmath-sse-sse2-and-arm-neon/
// 링크3 : DirectXTK / SimeMath https://github.com/Microsoft/DirectXTK/wiki/SimpleMath
//	
////////////////////////////////////////////////////////////////////////////// 
//
// 장면 공통 객체 : 카메라, 조명 등등..
//
////////////////////////////////////////////////////////////////////////////// 
// 아래의 정보를 변경하며 장면을 테스트 해 봅시다.
XMFLOAT3		g_vEye(3.0f, 2.5f, -6.0f);		//카메라 위치1.(Position)
XMFLOAT3		g_vLookAt(0.0f, 0.0f, 0.0f);	//바라보는 곳1.(Position)
XMFLOAT3		g_vUp(0.0f, 1.0f, 0.0f);		//카메라 상방 벡터1.(Direction)

// 투영 변환 정보. 
float g_fFov = XMConvertToRadians(45);	//기본 FOV 앵글. Field of View (Y) 
float g_fAspect = 1.333f;				//가로:세로 비율. 800:600 = 1.33:1 (4:3)   960:600 = 1.6:1 (16:10 HD)
float g_fZnear = 1.0f;					//시야 최소 거리 (1.0m) 
float g_fZfar = 100.0f;					//시야 최대 거리 (100m) 


/////////////////////////////////////////////////////////////////////////////// 
//
// 카메라 갱신.
//
void CameraUpdate(float dTime)
{

	//주인공 주시 테스트
	//g_vLookAt = g_vPos;	

	/*//카메라, 주인공과 함께 움직이기.
	XMFLOAT3 cpos(0, 2, -10);
	g_vEye = g_vPos + cpos;			//카메라 위치 : 주인공 '현재 위치로 부터 일정거리 떨어짐.
	g_vLookAt = g_vPos;				//카메라 시점 : 주인공 '현재' 위치.
	*/


	// 카메라 몸체의 "위치" 와 "방향" 정보 
	XMVECTOR eye = XMLoadFloat3(&g_vEye);	//카메라 위치 
	XMVECTOR lookat = XMLoadFloat3(&g_vLookAt);	//바라보는 곳.위치.
	XMVECTOR up = XMLoadFloat3(&g_vUp);		//카메라 상방 벡터.	
											// 뷰 변환 행렬 생성 :  View Transform 
	XMMATRIX mView = XMMatrixLookAtLH(eye, lookat, up);


	// 카메라 "렌즈" 및 "촬영 영역" 정보.   
	g_fAspect = g_Mode.Width / (float)g_Mode.Height;	 //현재 해상도 기준 FOV 설정.

														 // 원근 투영 변환 행렬 생성 : Projection Transform.
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(g_fFov, g_fAspect, g_fZnear, g_fZfar);



	//상수 버퍼에 행렬 설정.
	g_cbDef.mView = mView;
	g_cbDef.mProj = mProj;


	//Yena 로 카메라 정보를 전달 
	//Yena 공통 객체들은 갱신시 카메라 정보가 필요합니다. 
	//사용자 카메라를 사용한다면, 반드시 Yena 로 전달해야 합니다.
	yn_View = mView;
	yn_Proj = mProj;

}





////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// 렌더링 상태 객체 관련 함수들.
//
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
// 
// 레스터 상태 객체 생성.
//
void RasterStateCreate()
{
	//[상태객체 1] 기본 렌더링 상태 개체.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//삼각형 색상 채우기.(기본값)
	rd.CullMode = D3D11_CULL_NONE;		//컬링 없음. (기본값은 컬링 Back)		
	rd.FrontCounterClockwise = false;   //이하 기본값...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	//레스터라이져 객체 생성.
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_SOLID]);


	//[상태객체2] 와이어 프레임 그리기. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_WIREFRM]);

	//[상태객체3] 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_CULLBACK]);

	//[상태객체4] 와이어 프레임 + 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_WIRECULLBACK]);

}




////////////////////////////////////////////////////////////////////////////// 
//
// 레스터 상태 객체 제거 : 어플리케이션 종료시 호출.
//
void RasterStateRelease()
{
	for (int i = 0; i<RS_MAX_; i++)
	{
		SAFE_RELEASE(g_RState[i]);
	}

}




//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//
// 조명 처리 : 실제 계산은 셰이더에서 처리합니다.
//
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////// 
//
// 조명 처리를 위한 GPU / 렌더링 상태 설정.
//
void LightsUpdate(float dTime)
{
	//if(!g_bLightEnable) return;


	//---------------- 
	// 주광원 정보 설정  Diffuse Light
	//---------------- 	 
	g_Light.Direction = XMFLOAT3(1.0f, -3.0f, 2.0f);			//빛 방향.(Normalize 하는 것을 추천. 이번 예제에서만 이렇게 사용)
	g_Light.Range = 1000.0f;									//광선의 최대 도달 거리 : Directional Light 무한대.(생략가능)
	g_Light.Diffuse = COLOR(1, 1, 1, 1);						//광량 ( 100% 흰색).


	//---------------- 
	// 주변광 정보 설정. Ambient Light
	//---------------- 
	g_Light.Ambient =
						//COLOR(0.0f, 0.0f, 0.5f, 1.0f);		//50% 파랑색....
						//COLOR(0.5f, 0.5f, 0.5f, 1.0f);		//50% 회색.
						//COLOR(0.4f, 0.4f, 0.4f, 1.0f);		//40% 회색.
						//COLOR(0.3f, 0.3f, 0.3f, 1.0f);		//30% 회색.
						COLOR(0.2f, 0.2f, 0.2f, 1.0f);			//20% 회색 : 표준 주변광원량★
																//COLOR(0.1f, 0.1f, 0.1f, 1.0f);		//10% 회색 : 거의 검은색....
																//COLOR(0.0f, 0.0f, 0.0f, 1.0f);		//100% 검은색....


	//---------------- 
	// 상수버퍼 갱신.
	//---------------- 
	XMVECTOR dir = XMLoadFloat3(&g_Light.Direction);	//방향은 normalize 되어야 합니다.
	g_cbLit.Direction = XMVector3Normalize(-dir);		//빛방향 주의.★
	g_cbLit.Diffuse = XMLoadFloat4(&g_Light.Diffuse);
	g_cbLit.Ambient = XMLoadFloat4(&g_Light.Ambient);
	g_cbLit.Range = g_Light.Range;
	g_cbLit.LitOn = g_bLightEnable;
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBLit, &g_cbLit, sizeof(cbLIGHT));

	//---------------- 
	// 재질 설정 Material
	//---------------- 
	//... 재질은 렌더링 개체별 빛에 반응하는 고유 성질이므로 
	//... 각 개체별로 분리, 운용하는 것이 좋겠습니다.★ 

}

// 엔진-시스템 공통 데이터  갱신.. : 그려려니 합시다. 차차 공부할 것입니다.
// 
float EngineUpdate()
{
	//엔진 시간 / 타이머 얻기.
	float dTime = GetEngineTime();

	//Yena 갱신 : 불필요한 기능은 주석처리하여 기능을 비활성화 하십시오.
	//ynCameraUpdate(dTime);		// 카메라 갱신	
	//ynLightsUpdate(dTime);		// 조명 갱신						
	ynGridAxisUpdate(dTime);		// 방향축, 그리드 갱신.
	//YenaUpdate(dTime);			// 예나 시스템 전체 일괄 갱신.


									// 렌더링 옵션 조절 	 
	if (IsKeyUp(VK_SPACE))	g_bWireFrame ^= TRUE;
	if (IsKeyUp(VK_F4))		g_bCullBack ^= TRUE;
	if (IsKeyUp(VK_F5))		g_bLightEnable ^= TRUE;
	//if (IsKeyUp(VK_F5))		g_bZEnable ^= TRUE;		
	//if (IsKeyUp(VK_F6))		g_bZWrite ^= TRUE;		


	// 배경색 설정.
	if (g_bWireFrame) g_clearColor = COLOR(0.15f, 0.15f, 0.15f, 1.0f);
	else			  g_clearColor = COLOR(0, 0.125f, 0.3f, 1.0f);

	// 렌더링 모드 전환.	  
	RenderModeUpdate();

	// 깊이 연산 모드 전환.	 
	if (g_bZEnable)
	{
		if (g_bZWrite)
			g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_ON], 0);				//깊이 버퍼 동작 (기본값)
		else  g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_WRITE_OFF], 0);	//깊이 버퍼  : Z-Test On + Z-Write Off.
	}
	else  g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_OFF], 0);	//깊이 버퍼 비활성화 : Z-Test Off + Z-Write Off.


	return dTime;
}


/////////////////////////////////////////////////////////////////////////////// 
//
// 렌더링 모드 체크 : 각 렌더링 모드 상태별 On/Off 처리.
//
#define CheckRMode(k, v) if((k)) g_RMode |= (v); else g_RMode &= ~(v);

/////////////////////////////////////////////////////////////////////////////// 
//
// 렌더링 모드 전환
//
void RenderModeUpdate()
{
	// 렌더링 모드 체크 : 사용자가 지정한 렌더링 상태 조합.
	CheckRMode(g_bCullBack, RM_CULLBACK);
	CheckRMode(g_bWireFrame, RM_WIREFRAME);


	// 레스터 모드 전환 : 지정된 모드의 조합을 통해 렌더링 상태를 조절.
	switch (g_RMode)
	{
	default:

	case RM_SOLID:
		g_pDXDC->RSSetState(g_RState[RS_SOLID]);
		break;

	case RM_WIREFRAME:
		g_pDXDC->RSSetState(g_RState[RS_WIREFRM]);
		break;

	case RM_CULLBACK:
		g_pDXDC->RSSetState(g_RState[RS_CULLBACK]);
		break;

	case RM_WIREFRAME | RM_CULLBACK:
		g_pDXDC->RSSetState(g_RState[RS_WIRECULLBACK]);
		break;
	}

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
		int x = g_Mode.Width / 2 - 150, y = 1;
		COLOR col(1, 1, 1, 1);
		COLOR col2(1, 1, 0, 1);
		COLOR col3(0.4f, 0.4f, 0.4f, 1);
		ynTextDraw(x, y, col, L"■ %s", g_WindowName);

		ynTextDraw(x, y += 14, col, L"> 조명 모델의 이해");
		ynTextDraw(x, y += 14, col2, L"> 1.표준 확산광 모델 : Lambert Model");
		ynTextDraw(x, y += 14, col3, L"> 2.표준 확산광 모델(로컬) : Lambert Model (Local)");
		ynTextDraw(x, y += 14, col3, L"> 3.표준 정반사광 모델 : Blinn-Phong Model");
		//y = g_Mode.Height - 200;
		//ynTextDraw(x, y += 14, col2, L" * 사용자 입력 처리 및 모델 변환 연습 * ");
	}

	//시스템 정보.
	{
		int x = 1, y = 14;
		COLOR col(0, 1, 0, 1);
		SystemInfo(x, y, col);
	}

	//추가 도움말..
	int x = 1, y = 200;
	COLOR col(1, 1, 1, 1);
	COLOR col2(1, 1, 0, 1);
	y += 14;
	ynTextDraw(x, y += 14, col, L"Fill: Space (%s)", g_bWireFrame ? L"WIRE" : L"SOLID");
	ynTextDraw(x, y += 14, col, L"Cull: F4 (%s)", g_bCullBack ? L"ON" : L"OFF");
	ynTextDraw(x, y += 14, col2, L"Light: F5 (%s)", g_bLightEnable ? L"ON" : L"OFF");
	//ynTextDraw(x, y += 14, col, L"깊이테스트: F5 (%s)", g_bZEnable?L"ON":L"OFF");
	//ynTextDraw(x, y += 14, g_bZEnable?col:col*0.5f, L"깊이쓰기: F6 (%s)", g_bZWrite?L"ON":L"OFF");

	//모델 정보 출력.
	y += 14;
	ynTextDraw(x, y += 14, col, L"[Model]");
	ynTextDraw(x, y += 14, col, L"리셋 : F9");
	ynTextDraw(x, y += 14, col, L"이동 : 방향키");
	ynTextDraw(x, y += 14, col, L"회전 : ADWS");
	ynTextDraw(x, y += 14, col, L"자동 : R");

	//조명 정보 출력.
	y += 50;
	col = g_bLightEnable ? col2 : col*0.4f;
	ynTextDraw(x, y += 14, col, L"[Light]");
	ynTextDraw(x, y += 14, col, L"Dir = { %.2f, %.2f, %.2f }", g_Light.Direction.x, g_Light.Direction.y, g_Light.Direction.z);
	ynTextDraw(x, y += 14, col, L"Diff = { %.2f, %.2f, %.2f, %.2f }", g_Light.Diffuse.x, g_Light.Diffuse.y, g_Light.Diffuse.z, g_Light.Diffuse.w);
	ynTextDraw(x, y += 14, col, L"Amb = { %.2f, %.2f, %.2f, %.2f }", g_Light.Ambient.x, g_Light.Ambient.y, g_Light.Ambient.z, g_Light.Ambient.w);
	//SIMD 타입을 사용하면 아래처럼 접근해야 함.★
	//ynTextDraw(x, y += 14, col, L"Amb = { %.2f, %.2f, %.2f, %.2f }", XMVectorGetX(g_cbLit.Ambient), XMVectorGetY(g_cbLit.Ambient), XMVectorGetZ(g_cbLit.Ambient), XMVectorGetW(g_cbLit.Ambient));

	//재질 정보 출력.
	y += 14;
	ynTextDraw(x, y += 14, col, L"[Material]");
	ynTextDraw(x, y += 14, col, L"Diff = { %.2f, %.2f, %.2f, %.2f }", g_Mtrl.Diffuse.x, g_Mtrl.Diffuse.y, g_Mtrl.Diffuse.z, g_Mtrl.Diffuse.w);
	ynTextDraw(x, y += 14, col, L"Amb = { %.2f, %.2f, %.2f, %.2f }", g_Mtrl.Ambient.x, g_Mtrl.Ambient.y, g_Mtrl.Ambient.z, g_Mtrl.Ambient.w);



	col = COLOR(0, 1, 1, 1);
	y = g_Mode.Height - 14 * 5;
	ynTextDraw(x, y += 14, col, L"[Model]");
	ynTextDraw(x, y += 14, col, L"Pos = { %.2f, %.2f, %.2f }", g_vPos.x, g_vPos.y, g_vPos.z);
	ynTextDraw(x, y += 14, col, L"Rot = { %.2f, %.2f, %.2f }", g_vRot.x, g_vRot.y, g_vRot.z);
	ynTextDraw(x, y += 14, col, L"Scl = { %.2f, %.2f, %.2f }", g_vScale.x, g_vScale.y, g_vScale.z);


	//카메라 정보 출력.
	x = 200;  y = g_Mode.Height - 14 * 5;
	//col = COLOR(1, 1, 0, 1)*0.8f;
	ynTextDraw(x, y += 14, col, L"[카메라]");
	ynTextDraw(x, y += 14, col, L"Eye={%.2f, %.2f, %.2f}", g_vEye.x, g_vEye.y, g_vEye.z);
	ynTextDraw(x, y += 14, col, L"Look={%.2f, %.2f, %.2f}", g_vLookAt.x, g_vLookAt.y, g_vLookAt.z);
	ynTextDraw(x, y += 14, col, L"Up={%.2f, %.2f, %.2f}", g_vUp.x, g_vUp.y, g_vUp.z);

	//카메라 - "렌즈" : 투영변환 정보 출력.
	x = 400;  y = g_Mode.Height - 14 * 5;
	//col = COLOR(1, 1, 0, 1);
	ynTextDraw(x, y += 14, col, L"[렌즈]");
	ynTextDraw(x, y += 14, col, L"FOV=%.2f (%.2fº)", g_fFov, XMConvertToDegrees(g_fFov));
	ynTextDraw(x, y += 14, col, L"Aspect=%.2f", g_fAspect);
	ynTextDraw(x, y += 14, col, L"Zn=%.1f  Zf = %.1f", g_fZnear, g_fZfar);

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
	float dTime = EngineUpdate();

	// 장면 갱신 
	//
	LightsUpdate(dTime);	  //조명 갱신★
	CameraUpdate(dTime);	  //카메라갱신 : 뷰-투영 변환행렬 계산 

	// 주인공, 몬스터, 오브제 이동, 충돌검사 등등..
	// ...
	ObjUpdate(dTime);

	// 게임 로직, 이벤트, 층돌, 점수계산..
	// ...	 


	//-------------------------------
	// 장면 그리기 시작.. 
	//-------------------------------
	//렌더타겟(백버퍼)/ 깊이버퍼 지우기..
	ClearBackBuffer(D3D11_CLEAR_DEPTH, g_clearColor);

	//시스템 객체 출력.
	//ynGridAxisDraw();

	//개체 렌더링 : 주인공, 몬스터, 지형.. 
	ObjDraw();

	//시스템 객체 & 도움말 출력..
	ynGridAxisDraw();
	ShowInfo();

	//-------------------------------
	// 장면 그리기 종료.
	//------------------------------- 
	Flip();


}//end of void SceneRender()



/****************** end of file "Render.cpp" *********************************/