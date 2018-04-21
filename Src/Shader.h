
#pragma once

//셰이더 관련.
extern ID3D11VertexShader*     g_pVS;
extern ID3D11PixelShader*      g_pPS;

//정점 셰이더 컴파일 코드 개체.(임시)
extern ID3DBlob* g_pVSCode;


HRESULT ShaderLoad();
HRESULT ShaderCompile(
						WCHAR* FileName,		//소스파일이름.
						char* EntryPoint,		//메인함수 진입점.
						char* ShaderModel,		//셰이더 모델.
						ID3DBlob** ppCode		//[출력] 컴파일된 셰이더 코드.
					 );

void	ShaderRelease();

//기본 상수 버퍼 구조체.★
//각 렌더링 개체의 기본 정보 - 변환 등.. - 를 운용.
struct cbDEFAULT
{
	XMMATRIX mTM;		//"World" 변환 행렬 : DirectXMath, 16바이트 정렬 버전. 
	XMMATRIX mView;		//"View" 변환 행렬
	XMMATRIX mWV;		//"World * View" 변환 행렬
	XMMATRIX mProj;		//"Projection" 변환 행렬

};

//조명 정보용 상수버퍼 구조체★
struct cbLIGHT
{
	XMVECTOR Direction;		//빛의 방향.
	XMVECTOR Diffuse;		//주 광량 : 확산광 Diffuse Light.
	XMVECTOR Ambient;		//보조 광량 : 주변광 Ambient Light.
	FLOAT    Range;			//빛 도달 거리.
	BOOL	 LitOn;			//조명 적용여부.
};

//재질 정보용 상수버퍼 구조체★
struct cbMATERIAL
{
	XMVECTOR Diffuse;		//주 광량(확산광) 의 반사율(%) 
	XMVECTOR Ambient;		//보조 광량(주변광) 의 반사율(%) 
};



//셰이더 상수 버퍼.인터페이스.
extern ID3D11Buffer*	g_pCBDef;		//★
extern ID3D11Buffer*	g_pCBLit;		//★
extern ID3D11Buffer*	g_pCBMtrl;		//★

										//(정적) 상수 버퍼 생성.
HRESULT CreateConstantBuffer(UINT size, ID3D11Buffer** ppCB);
//동적 상수버퍼 생성.
HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB);
//동적 상수버퍼 갱신.
HRESULT UpdateDynamicConstantBuffer(LPDXDC pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size);