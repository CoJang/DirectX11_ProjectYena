#pragma warning(disable:4996)
#include "Windows.h" 
#include "stdio.h"
#include "Device.h"
#include "Render.h"

#include "d3dcompiler.h"				//DX ���̴� �����Ϸ� ���.
#pragma comment(lib, "d3dcompiler")		//DX ���̴� �����Ϸ� ���̺귯��.  D3DCompiler.dll �ʿ�.
#include "Shader.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// ���� �����͵�.


//���̴� �� ��ü ����.
ID3D11VertexShader*			g_pVS = nullptr;		// ���� ���̴� Vertex Shader �������̽�.
ID3D11PixelShader*			g_pPS = nullptr;		// �ȼ� ���̴� Pixel Shader �������̽�.

//���� ���̴� ������ �ڵ� ��ü.(�ӽ�)
ID3DBlob* g_pVSCode = nullptr;

//���̴� ��� ����.
ID3D11Buffer*	g_pCBDef = nullptr;		// �⺻���� ��� �������
ID3D11Buffer*	g_pCBLit = nullptr;		// ����� �������
ID3D11Buffer*	g_pCBMtrl = nullptr;	// ������ �������
ID3D11Buffer*	g_pCBCam = nullptr;		// ī�޶�� �������



//////////////////////////////////////////////////////////////////////////////// 
// Shader Functions
////////////////////////////////////////////////////////////////////////////////

HRESULT ShaderLoad()
{
	WCHAR* filename = L"./fx/Demo.fx";

	// ���� ���̴� ����.
	
	ID3DBlob* pVSCode = nullptr;
	HRESULT hr = ShaderCompile(filename, "VS_Main", "vs_5_0", &pVSCode);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"[Failed!] ShaderLoad() :: Vertex Shader Compile Failed", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	// ���� ���̴� ��ü ���� Creat a VS Object
	hr = g_pDevice->CreateVertexShader( pVSCode->GetBufferPointer(),
										pVSCode->GetBufferSize(),
										nullptr,
										&g_pVS);

	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);	// ��ü �ӽ�����
		return hr;
	}

	// ���� ���� : Input Layout ���۽� �ʿ���
	g_pVSCode = pVSCode;

	// �ȼ� ���̴� ����

	ID3DBlob* pPSCode = nullptr;
	hr = ShaderCompile(filename, "PS_Main", "ps_5_0", &pPSCode);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"[Failed!] ShaderLoad() :: Pixel Shader Compile Failed", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	// ���� ���̴� ��ü ���� Creat a VS Object
	hr = g_pDevice->CreatePixelShader(	pPSCode->GetBufferPointer(),
										pPSCode->GetBufferSize(),
										nullptr,
										&g_pPS);

	if (FAILED(hr))
	{
		SAFE_RELEASE(pPSCode);	// ��ü �ӽ�����
		return hr;
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// ���̴� ����. 
void ShaderRelease()
{
	SAFE_RELEASE(g_pVS);			//���� ���̴� ����.
	SAFE_RELEASE(g_pPS);			//�ȼ� ���̴� ����.
	SAFE_RELEASE(g_pVSCode);		//���� ���̴� ������ �ڵ� (����) ����.
	SAFE_RELEASE(g_pCBDef);			//�⺻ ������� ����.
	SAFE_RELEASE(g_pCBLit);			//���� ������� ����.��
	SAFE_RELEASE(g_pCBMtrl);		//���� ������� ����.��
	SAFE_RELEASE(g_pCBCam);			//ī�޶� ������� ����.��
}

////////////////////////////////////////////////////////////////////////////// 
// ���̴� �ҽ� ������ : ���̴� �ҽ�(*.fx)�� GPU �� ����� ��ȯ�մϴ�. 
HRESULT ShaderCompile(
	WCHAR* FileName,		//�ҽ������̸�.
	char* EntryPoint,		//�����Լ� ������.
	char* ShaderModel,		//���̴� ��.
	ID3DBlob** ppCode		//[���] �����ϵ� ���̴� �ڵ�.
)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//������ �ɼ�1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//���켱 ��� ó��. ���� DX9 ���������� �������� ���. �ӵ��� �䱸�ȴٸ�, "��켱" ���� ó���� ��.
														//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//��켱 ��� ó��. �ӵ��� ����� ������, ����� ��ġ �� GPU �� �����ؾ� �մϴ�.
														//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//����� ���� �ɼ� �߰�.
#endif

														//���̴� �ҽ� ������.
	hr = D3DCompileFromFile(FileName,
							nullptr, nullptr,
							EntryPoint,
							ShaderModel,
							Flags,				//������ �ɼ�1
							0,					//������ �ɼ�2,  Effect ���� �����Ͻ� �����. �̿ܿ��� ���õ�.
							ppCode,				//[���] �����ϵ� ���̴� �ڵ�.
							&pError);				//[���] ������ ���� �ڵ�.

	if (FAILED(hr))
	{
		//������ ����Ȯ���� ���� pError �� ����մϴ�.
		ynErrorW(TRUE, L"���̴� ������ ����", hr, pError, FileName, EntryPoint, ShaderModel);
	}

	SAFE_RELEASE(pError);
	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// (����) ��� ���� ����
HRESULT CreateConstantBuffer(UINT size, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;

	//��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	//��� ���� ����.
	hr = g_pDevice->CreateBuffer(&bd, nullptr, &pCB);
	if (FAILED(hr))
	{
		ynError(hr, L"CreateConstantBuffer : ������� ���� ����");
		return hr;
	}

	//�ܺη� ����.
	*ppCB = pCB;

	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// (����) ��� ���� ����
HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;

	//��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//���� �������� ����.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU ���� ����.

												//���긮�ҽ� ����.
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = pData;				//(�ܺ�) ��� ������ ����.
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;

	//��� ���� ����.
	hr = g_pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		ynError(hr, L"CreateDynamicConstantBuffer : ���� ������� ���� ����");
		return hr;
	}

	//�ܺη� ����.
	*ppCB = pCB;

	return hr;
}

////////////////////////////////////////////////////////////////////////////// 
// ���� ���� ����.
HRESULT UpdateDynamicConstantBuffer(LPDXDC pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	//������� ����
	hr = pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		ynError(hr, L"UpdateDynamicConstantBuffer : Map ����");
		return hr;
	}

	//��� ���� ����.
	memcpy(mr.pData, pData, size);

	//������� �ݱ�.
	pDXDC->Unmap(pBuff, 0);


	return hr;
}




/****************** end of file "Shader.cpp" *********************************/
