#pragma warning(disable:4996)
#include "Windows.h" 
#include "stdio.h"
#include "Device.h"
#include "Render.h"

#include "d3dcompiler.h"				//DX 셰이더 컴파일러 헤더.
#pragma comment(lib, "d3dcompiler")		//DX 셰이더 컴파일러 라이브러리.  D3DCompiler.dll 필요.
#include "Shader.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// 전역 데이터들.


//쉐이더 별 객체 선언.
ID3D11VertexShader*			g_pVS = nullptr;		// 정점 쉐이더 Vertex Shader 인터페이스.
ID3D11PixelShader*			g_pPS = nullptr;		// 픽셀 쉐이더 Pixel Shader 인터페이스.

//정점 쉐이더 컴파일 코드 개체.(임시)
ID3DBlob* g_pVSCode = nullptr;

//쉐이더 상수 버퍼.
ID3D11Buffer*	g_pCBDef = nullptr;		// 기본정보 운용 상수버퍼
ID3D11Buffer*	g_pCBLit = nullptr;		// 조명용 상수버퍼
ID3D11Buffer*	g_pCBMtrl = nullptr;	// 재질용 상수버퍼



//////////////////////////////////////////////////////////////////////////////// 
// Shader Functions
////////////////////////////////////////////////////////////////////////////////

HRESULT ShaderLoad()
{
	WCHAR* filename = L"./fx/Demo.fx";

	// 정점 쉐이더 생성.
	
	ID3DBlob* pVSCode = nullptr;
	HRESULT hr = ShaderCompile(filename, "VS_Main", "vs_5_0", &pVSCode);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"[Failed!] ShaderLoad() :: Vertex Shader Compile Failed", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	// 정점 쉐이더 객체 생성 Creat a VS Object
	hr = g_pDevice->CreateVertexShader( pVSCode->GetBufferPointer(),
										pVSCode->GetBufferSize(),
										nullptr,
										&g_pVS);

	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);	// 객체 임시제거
		return hr;
	}

	// 전역 참조 : Input Layout 제작시 필요함
	g_pVSCode = pVSCode;

	// 픽셀 쉐이더 생성

	ID3DBlob* pPSCode = nullptr;
	hr = ShaderCompile(filename, "PS_Main", "ps_5_0", &pPSCode);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"[Failed!] ShaderLoad() :: Pixel Shader Compile Failed", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	// 정점 쉐이더 객체 생성 Creat a VS Object
	hr = g_pDevice->CreatePixelShader(	pPSCode->GetBufferPointer(),
										pPSCode->GetBufferSize(),
										nullptr,
										&g_pPS);

	if (FAILED(hr))
	{
		SAFE_RELEASE(pPSCode);	// 객체 임시제거
		return hr;
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// 셰이더 제거. 
void ShaderRelease()
{
	SAFE_RELEASE(g_pVS);			//정점 셰이더 제거.
	SAFE_RELEASE(g_pPS);			//픽셀 셰이더 제거.
	SAFE_RELEASE(g_pVSCode);		//정점 셰이더 컴파일 코드 (버퍼) 제거.
	SAFE_RELEASE(g_pCBDef);			//기본 상수버퍼 제거.
	SAFE_RELEASE(g_pCBLit);			//조명 상수버퍼 제거.★
	SAFE_RELEASE(g_pCBMtrl);		//재질 상수버퍼 제거.★
}

////////////////////////////////////////////////////////////////////////////// 
// 셰이더 소스 컴파일 : 셰이더 소스(*.fx)를 GPU 용 기계어로 변환합니다. 
HRESULT ShaderCompile(
	WCHAR* FileName,		//소스파일이름.
	char* EntryPoint,		//메인함수 진입점.
	char* ShaderModel,		//셰이더 모델.
	ID3DBlob** ppCode		//[출력] 컴파일된 셰이더 코드.
)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//컴파일 옵션1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//열우선 행렬 처리. 구형 DX9 이전까지의 전통적인 방식. 속도가 요구된다면, "행우선" 으로 처리할 것.
														//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//행우선 행렬 처리. 속도의 향상이 있지만, 행렬을 전치 후 GPU 에 공급해야 합니다.
														//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//디버깅 모드시 옵션 추가.
#endif

														//셰이더 소스 컴파일.
	hr = D3DCompileFromFile(FileName,
							nullptr, nullptr,
							EntryPoint,
							ShaderModel,
							Flags,				//컴파일 옵션1
							0,					//컴파일 옵션2,  Effect 파일 컴파일시 적용됨. 이외에는 무시됨.
							ppCode,				//[출력] 컴파일된 셰이더 코드.
							&pError);				//[출력] 컴파일 에러 코드.

	if (FAILED(hr))
	{
		//컴파일 에러확인을 위해 pError 를 출력합니다.
		ynErrorW(TRUE, L"셰이더 컴파일 실패", hr, pError, FileName, EntryPoint, ShaderModel);
	}

	SAFE_RELEASE(pError);
	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// (정적) 상수 버퍼 생성
HRESULT CreateConstantBuffer(UINT size, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;

	//상수 버퍼 정보 설정.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	//상수 버퍼 생성.
	hr = g_pDevice->CreateBuffer(&bd, nullptr, &pCB);
	if (FAILED(hr))
	{
		ynError(hr, L"CreateConstantBuffer : 상수버퍼 생성 실패");
		return hr;
	}

	//외부로 전달.
	*ppCB = pCB;

	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// (동적) 상수 버퍼 생성
HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;

	//상수 버퍼 정보 설정.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//동적 정점버퍼 설정.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU 접근 설정.

												//서브리소스 설정.
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = pData;				//(외부) 상수 데이터 설정.
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;

	//상수 버퍼 생성.
	hr = g_pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		ynError(hr, L"CreateDynamicConstantBuffer : 동적 상수버퍼 생성 실패");
		return hr;
	}

	//외부로 전달.
	*ppCB = pCB;

	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// 동적 버퍼 갱신.
HRESULT UpdateDynamicConstantBuffer(LPDXDC pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	//상수버퍼 접근
	hr = pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		ynError(hr, L"UpdateDynamicConstantBuffer : Map 실패");
		return hr;
	}

	//상수 버퍼 갱신.
	memcpy(mr.pData, pData, size);

	//상수버퍼 닫기.
	pDXDC->Unmap(pBuff, 0);


	return hr;
}




/****************** end of file "Shader.cpp" *********************************/
