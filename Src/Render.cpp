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
#include "Shader.h"

TCHAR* g_WindowName = L"CoJang & Yena Collaboration Blinn-Phong (Local)";



/////////////////////////////////////////////////////////////////////////////
//
//  ���� �����͵�.
//  
COLOR g_clearColor(0, 0.25f, 0.3f, 1);  //����
  
struct COLVTX
{
	float x, y, z;			// ��ǥ
	float nx, ny, nz;		// ��� : Normal
};

// �𵨿� ���Ϲ���.
ID3D11Buffer*		g_pVB = nullptr;		// ���� ���� ������.
ID3D11InputLayout*	g_pVBLayout = nullptr;  // ���� �Է±���

// �⺻ ������� ������ : ������ �⺻ ������ ���.
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
// ť�� ������.
//
// - �� ť��� �ﰢ�� ����Ʈ ���·� ����.
// - �� '��' ���� face 2�� ����.
// - ����(Normal) ������ ������ �� ���� '��'�� �ǹ�.
// - ��Ȯ�� ����(���� ó��)�� ���ؼ��� ������ �ߺ��� �Ұ���.
//
// [�׸�] �� ���� ���� ����.
//  v0 --- v1
//   |    / | 
//   |   /  |
//   |  /   |
//   | /    |
//  v2 --- v3
//
COLVTX	g_vtxCube[] = {

	//����. ( Face#0) :   
	{ -1.0, 1.0,-1.0,  0.0, 0.0,-1.0 } ,		//v0 : Position + Normal.��
	{ 1.0, 1.0,-1.0,  0.0, 0.0,-1.0 } ,		//v1
	{ -1.0,-1.0,-1.0,  0.0, 0.0,-1.0 } ,		//v2
												//       ( Face#1)
	{ -1.0,-1.0,-1.0,  0.0, 0.0,-1.0 } ,
	{ 1.0, 1.0,-1.0,  0.0, 0.0,-1.0 } ,
	{ 1.0,-1.0,-1.0,  0.0, 0.0,-1.0 } ,

	//�޸�.  (Face#2) :  
	{ 1.0, 1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ -1.0, 1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ -1.0,-1.0, 1.0,  0.0, 0.0, 1.0 } ,
	//		  (Face#3)
	{ 1.0, 1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ -1.0,-1.0, 1.0,  0.0, 0.0, 1.0 } ,
	{ 1.0,-1.0, 1.0,  0.0, 0.0, 1.0 } ,


	// ������. (Face#4)
	{ 1.0, 1.0,-1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0,-1.0,-1.0,  1.0, 0.0, 0.0 } ,
	//			(Face#5)
	{ 1.0,-1.0,-1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  1.0, 0.0, 0.0 } ,
	{ 1.0,-1.0, 1.0,  1.0, 0.0, 0.0 } ,


	// ������. (Face#6)
	{ -1.0, 1.0, 1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0, 1.0,-1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  -1.0, 0.0, 0.0 } ,
	//			(Face#7)
	{ -1.0, 1.0, 1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  -1.0, 0.0, 0.0 } ,
	{ -1.0,-1.0, 1.0,  -1.0, 0.0, 0.0 } ,


	//���.  ( Face#8)
	{ -1.0, 1.0, 1.0,  0.0, 1.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  0.0, 1.0, 0.0 } ,
	{ -1.0, 1.0,-1.0,  0.0, 1.0, 0.0 } ,
	//		 ( Face#9)
	{ -1.0, 1.0,-1.0,  0.0, 1.0, 0.0 } ,
	{ 1.0, 1.0, 1.0,  0.0, 1.0, 0.0 } ,
	{ 1.0, 1.0,-1.0,  0.0, 1.0, 0.0 } ,


	//�Ϻ�.  ( Face#10)
	{ 1.0,-1.0, 1.0,  0.0,-1.0, 0.0 } ,
	{ -1.0,-1.0, 1.0,  0.0,-1.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  0.0,-1.0, 0.0 } ,
	//		 ( Face#11)
	{ 1.0,-1.0, 1.0,  0.0,-1.0, 0.0 } ,
	{ -1.0,-1.0,-1.0,  0.0,-1.0, 0.0 } ,
	{ 1.0,-1.0,-1.0,  0.0,-1.0, 0.0 } ,

};

////////////////////////////////////////////////////////////////////////////// 
// ������ ���� ���� ������/�Լ���.

BOOL g_bCullBack = TRUE;		//�޸� ����...On/Off.
BOOL g_bWireFrame = FALSE;		//���̾� ������ On/Off.
BOOL g_bZEnable = TRUE;			//���� ���� ���� On/Off.
BOOL g_bZWrite = TRUE;			//���̹��� ���� On/Off.	
BOOL g_bSpecular = TRUE;		//�ݻ籤 ���� On/Off.	


// 
// �����Ͷ����� ���� ��ü Rasterizer State Objects
// 
enum 
{
	RS_SOLID,				//�ﰢ�� ä��� : Fill Mode - Soild.
	RS_WIREFRM,				//�ﰢ�� ä��� : Fill Mode - Wireframe.
	RS_CULLBACK,			//�޸� �ø� (ON) : BackFaceCulling - "CCW" 
	RS_WIRECULLBACK,		//���̾� ������ + �޸� �ø� (ON) 

	RS_MAX_
};

//�����Ͷ����� ���� ��ü �迭
ID3D11RasterizerState*	g_RState[RS_MAX_] = { NULL, };

void RasterStateCreate();
void RasterStateRelease();
void RenderModeUpdate();

//������ ��� : �ټ��� ������ ��� ���� �� ����� ���� ����.
enum 
{
	RM_SOLID = 0x0000,			// �ﰢ��ä��� : ON, Solid
	RM_WIREFRAME = 0x0001,		// �ﰢ��ä��� : OFF, Wire-frame
	RM_CULLBACK = 0x0002,		// �޸� �ø� : ON, "CCW"

	//������ �⺻��� : Solid + Cull-On.
	RM_DEFAULT = RM_SOLID | RM_CULLBACK,

};
DWORD g_RMode = RM_DEFAULT;		//'����' ������ ���.

/////////////////////////////////////////////////////////////////////////////
// ���� ���� Lighting ��
//
// DX9 �� ���뱸��ü(D3DLIGHT9) �� ���� HW ������ �����Ͽ����ϴ�. 
// DX10 ���ʹ� ����ڰ� �ʿ��� ������ �����Ͽ� ����ؾ� �մϴ�.
// ���� ��� ���� ���̴��� ���� �������� ���� ó���ؾ� �մϴ�.

//���� ����ü : DX9 �� ���������, �ʿ��� ������ �����մϴ�.��
struct LIGHT
{
	XMFLOAT3 Direction;		//���� ����.
	COLOR    Diffuse;		//�� ���� : Ȯ�걤 Diffuse Light.
	COLOR    Ambient;		//���� ���� : �ֺ��� Ambient Light.
	COLOR	 Specular;		//�ݻ� ���� : �ݻ籤 Specular Light.
	FLOAT    Range;			//�� ���� �Ÿ�.
};

LIGHT g_Light;					//���� ����.
BOOL  g_bLightEnable = TRUE;	//���� �÷���. On/Off. ��
BOOL  g_bSpecEnable = TRUE;	//���� �÷���. On/Off. ��

cbLIGHT g_cbLit;				//���� ������� ���ſ�. 

void LightsUpdate(float dTime);

/////////////////////////////////////////////////////////////////////////////
// ���� ���� Material ��
// ���� Material �� ������ ��ü�� ���� �����ϴ� ���� �����Դϴ�. 
// ���� �� ��ü���� �и�, ����ϴ� ���� ���ڽ��ϴ�.�� 
// DX9 �� ���뱸��ü(D3DMATERIAL9) �� ���� HW ����� ���� ������ ����Ͽ����� 
// DX10 ���ʹ� ����ڰ� �ʿ��� ������ �����Ͽ� ����ؾ� �մϴ�.
// ���� ��� ���� ���̴��� ���� �������� ���� ó���ؾ� �մϴ�.
//

struct MATERIAL
{
	COLOR    Diffuse;		//�� ����(Ȯ�걤) �� �ݻ���(%) 
	COLOR    Ambient;		//���� ����(�ֺ���) �� �ݻ���(%) 
	COLOR	 Specular;		//�ݻ� ����(�ݻ籤) �� �ݻ���(%)
	FLOAT	 Power;			//�ݻ��� ���� ����
};

MATERIAL g_Mtrl;			//���� ����.
cbMATERIAL g_cbMtrl;		//���� ������ۿ� ���ſ�.

struct CAMERA
{
	XMFLOAT3 Position;		//ī�޶��� ��ġ
};

CAMERA	g_Cam;
cbCAMERA g_cbCam;			//ī�޶� ������� ���ſ�


/////////////////////////////////////////////////////////////////////////////
//	������ �ε� / ����
int DataLoading()
{ 
	//���̴� �ε�.
	ShaderLoad();

	// �⺻ ��� ���� ����.
	ZeroMemory(&g_cbDef, sizeof(cbDEFAULT));
	CreateDynamicConstantBuffer(sizeof(cbDEFAULT), &g_cbDef, &g_pCBDef);

	// ����� ������� ����.��
	// ���������� �ǽð����� ������ �ʴ´ٸ�, �Ϲ�(����) ������۷ε� ����մϴ�.
	ZeroMemory(&g_cbLit, sizeof(cbLIGHT));
	CreateDynamicConstantBuffer(sizeof(cbLIGHT), &g_cbLit, &g_pCBLit);

	// ������ ������� ����.��
	// ���������� �ǽð����� ������ �ʴ´ٸ�, �Ϲ�(����) ������۷ε� ����մϴ�.
	ZeroMemory(&g_cbMtrl, sizeof(cbMATERIAL));
	CreateDynamicConstantBuffer(sizeof(cbMATERIAL), &g_cbMtrl, &g_pCBMtrl);

	// ī�޶�� ������� ����.��
	// ī�޶������� �ǽð����� ������ �ʴ´ٸ�, �Ϲ�(����) ������۷ε� ����մϴ�.
	ZeroMemory(&g_cbCam, sizeof(cbCAMERA));
	CreateDynamicConstantBuffer(sizeof(cbCAMERA), &g_cbCam, &g_pCBCam);



	//������Ʈ �ε�.
	ObjLoad();

	//������ ���� ��ü ����.
	RasterStateCreate();

	//�׸��� ON, ������ OFF.
	//ynGridAxisState(FALSE, FALSE);

	return TRUE;
}

void DataRelease()
{
	// ������ ���� �ڵ�� ���⿡..
	// ... 

	ObjRelease();			//������Ʈ ���� 
	ShaderRelease();		//���̴� ����.
	RasterStateRelease();	//������ ���� ��ü ����.

}

/////////////////////////////////////////////////////////////////////////////
//
//  ������Ʈ : ���� �� �ʿ� ���ҽ� �ε�.
//
int ObjLoad()
{
	HRESULT hr = S_OK;

	/*// ���� ������ ���� ���� �迭
	// �Ϲ������δ� �ܺ�(HDD) ���� �ε��մϴ�.
	COLVTX	Vertices[] = {
	// ������ ��ǥ���� �� ����(Model Space, Local Space)��ǥ.
	// ��ǥ (x, y, z)    ����(R G B A), Alpha �� �⺻�� 1.
	//Face 0 : ���ﰢ��.(CW)
	{ -1.0f,  0.0f, 0.0f,   1, 0, 0, 1 },	//����. ���� ũ�� ����.
	{  0.0f,  2.0f, 0.0f,   0, 1, 0, 1 },	//�ʷ�.
	{  1.0f,  0.0f, 0.0f,   0, 0, 1, 1 },	//�Ķ�.
	//Face 1 : ���ﰢ��.(CCW)
	{ -0.5f,  0.0f, 0.0f,   1, 0, 0, 1 },	//����.
	{  0.0f, -1.0f, 0.0f,   0, 1, 0, 1 },	//�ʷ�.
	{  0.5f,  0.0f, 0.0f,   0, 0, 1, 1 },	//�Ķ�.
	};*/


	// ���� ���� Vertex Buffer ����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//���� �����
	bd.ByteWidth = sizeof(g_vtxCube);				//���� ũ�� sizeof( VTX_MESH ) *3 ;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = g_vtxCube;					//���ۿ� �� ������ ���� : "������"..

											//���� ���� ����.
	hr = g_pDevice->CreateBuffer(&bd, &rd, &g_pVB);
	if (FAILED(hr))	return hr;




	// ���� �Է±��� Input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	//��
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���� �Է±��� ��ü ���� Create the input layout
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
// ������Ʈ : ���� 
//
void ObjRelease()
{
	SAFE_RELEASE(g_pVB);
	SAFE_RELEASE(g_pVBLayout);
}



////////////////////////////////////////////////////////////////////////////// 
//
// ������Ʈ ����
//
void ObjUpdate(float dTime)
{
	if (IsKeyDown(VK_F9))		//�� ���� ����.
	{
		g_vPos = g_vRot = XMFLOAT3(0, 0, 0);
		g_vScale = XMFLOAT3(1, 1, 1);
	}

	//�ڵ�ȸ��ó��..
	//static bool bAni = true;
	static bool bAutoRot = true;
	if (IsKeyUp('R')) { bAutoRot ^= true; }


	//������ ó��.
	XMMATRIX mScale = XMMatrixScaling(g_vScale.x, g_vScale.y, g_vScale.z);

	//�̵� ó��.
	float mov = 5.0f * dTime;		// 5.0 / sec �̵���.
	if (IsKeyDown(VK_LEFT))  g_vPos.x -= mov;
	if (IsKeyDown(VK_RIGHT)) g_vPos.x += mov;
	if (IsKeyDown(VK_UP))	 g_vPos.z += mov;
	if (IsKeyDown(VK_DOWN))  g_vPos.z -= mov;
	XMMATRIX mTrans = XMMatrixTranslation(g_vPos.x, g_vPos.y, g_vPos.z);

	//ȸ�� ó��.  
	float rot = XM_PI * 0.5f * dTime;		//90��/sec �� ȸ��.(����)
	if (IsKeyDown('A'))  g_vRot.y += rot;
	if (IsKeyDown('D'))  g_vRot.y -= rot;
	if (IsKeyDown('W'))  g_vRot.x += rot;
	if (IsKeyDown('S'))  g_vRot.x -= rot;
	if (bAutoRot) g_vRot.y += rot * 0.5f;
	//g_vRot.y += XMConvertToRadians(90) * dTime;	//90��/sec �� ȸ��.(DirectXMath ���)	
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(g_vRot.x, g_vRot.y, g_vRot.z);
	//XMMATRIX mRot = XMMatrixRotationY(g_vRot.y);


	//-----------------------
	// ��ȯ ��� ���� 
	//-----------------------
	XMMATRIX mTM = mScale * mRot * mTrans;



	//-----------------------
	// ������� ����.
	//-----------------------
	g_cbDef.mTM = mTM;
	g_cbDef.mWV = mTM * g_cbDef.mView;		//��
	//g_cbDef.mTM = XMMatrixTranspose(mTM);	//���̴����� '�� �켱 Column major' �������� ó���ϸ� �ӵ� ����� ����� �� �ֽ��ϴ�. �̸� ���� ��� ��ġ ó��.	


	//���̴� ��� ���� ����.
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBDef, &g_cbDef, sizeof(cbDEFAULT));


	//-----------------------
	// ���� ���� ����.��
	//-----------------------
	g_Mtrl.Diffuse = COLOR(1, 0, 0, 1);
	g_Mtrl.Ambient = COLOR(1, 0, 0, 1); 
	g_Mtrl.Specular = COLOR(1, 1, 1, 1);
	g_Mtrl.Power = FLOAT(30.0f);

	//���� ���� ������� ����.
	g_cbMtrl.Diffuse = XMLoadFloat4(&g_Mtrl.Diffuse);
	g_cbMtrl.Ambient = XMLoadFloat4(&g_Mtrl.Ambient);
	g_cbMtrl.Specular = XMLoadFloat4(&g_Mtrl.Specular);
	g_cbMtrl.Power = g_Mtrl.Power;

	//������� ���� : ���������� �ǽð����� ������ �ʴ´ٸ�, �Ϲ�(����) ������۷ε� ����մϴ�.
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBMtrl, &g_cbMtrl, sizeof(cbMATERIAL));


}




////////////////////////////////////////////////////////////////////////////// 
//
// ������Ʈ �׸���.
//
void ObjDraw()
{

	//���� ���� ����
	UINT stride = sizeof(COLVTX);
	UINT offset = 0;
	g_pDXDC->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);

	//�Է� ���̾ƿ� ����. Set the input layout
	g_pDXDC->IASetInputLayout(g_pVBLayout);

	// ���� ���� ���� ���� Set primitive topology
	g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���̴� ����.
	g_pDXDC->VSSetShader(g_pVS, nullptr, 0);
	g_pDXDC->PSSetShader(g_pPS, nullptr, 0);
	g_pDXDC->VSSetConstantBuffers(0, 1, &g_pCBDef);		//VS �� ��� ���� ����.
	g_pDXDC->PSSetConstantBuffers(1, 1, &g_pCBLit);		//VS �� ���� ���� ����.�� 
	g_pDXDC->PSSetConstantBuffers(2, 1, &g_pCBMtrl);	//VS �� ���� ���� ����.�� 
	g_pDXDC->PSSetConstantBuffers(3, 1, &g_pCBCam);		//VS �� ī�޶� ���� ����.�� 


	//�׸���! Render a triangle 
	g_pDXDC->Draw(36, 0);


	//��ġ ���� ���� 
	//g_pDXDC->ClearState();
}

////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// [DirectX ���ж��̺귯�� ��� �ȳ�] 
//	
// ������ ����/���� ������ CPU �� ���� �ε��Ҽ� ���갡��(SSE/SSE2)�� ���� 
// SIMD ���� �ڷ��� (XMMATRIX, XMVECTOR) �� ����ϰ� �ֽ��ϴ�.
// ��� ���� ����ó�� -  ������ ����, �б�, ���� - �� �ڷ��� ��ȯ ���� ������ 
// �߰� �۾��� �䱸�Ǹ� �̷� ���� �ڵ��� �������̳� �������� �������ϴ�.
//
// �� SIMD �ڷ��� (XMFLAOT4X4, XMFLOAT3 ...) ���� ����Ѵٸ�, ���� ���� ó���� ������������
// SIMD ������ ���� �� ����, ��κ��� DirectXMath �Լ����� SIMD �ڷ����� ���߾� ����Ǿ� �־� 
// ���� ������ �����ε��� ����ڰ� ���� �߰��ؾ� �մϴ�.
//
// DirectXTK / SimpleMath �� ����ϸ� ���� 2���� ������� ������ ����, ���� �������̰� ������ ���� 
// �ڵ带 ���� �� �� �ֽ��ϴ�. �� DirectXMath ���� ���� �ణ�� ���� ���ϰ� �߻��մϴ�.
//
// ���� �ڼ��� ���� �Ʒ��� �����Ͻʽÿ�.
//
// ��ũ1 : DirectXMath ���̵� https://msdn.microsoft.com/ko-kr/library/windows/desktop/ee415571(v=vs.85).aspx
// ��ũ2 : DirectX�� SSE ���� https://blogs.msdn.microsoft.com/chuckw/2012/09/11/directxmath-sse-sse2-and-arm-neon/
// ��ũ3 : DirectXTK / SimeMath https://github.com/Microsoft/DirectXTK/wiki/SimpleMath
//	
////////////////////////////////////////////////////////////////////////////// 
//
// ��� ���� ��ü : ī�޶�, ���� ���..
//
////////////////////////////////////////////////////////////////////////////// 
// �Ʒ��� ������ �����ϸ� ����� �׽�Ʈ �� ���ô�.
XMFLOAT3		g_vEye(3.0f, 2.5f, -6.0f);		//ī�޶� ��ġ1.(Position)
XMFLOAT3		g_vLookAt(0.0f, 0.0f, 0.0f);	//�ٶ󺸴� ��1.(Position)
XMFLOAT3		g_vUp(0.0f, 1.0f, 0.0f);		//ī�޶� ��� ����1.(Direction)

// ���� ��ȯ ����. 
float g_fFov = XMConvertToRadians(45);	//�⺻ FOV �ޱ�. Field of View (Y) 
float g_fAspect = 1.333f;				//����:���� ����. 800:600 = 1.33:1 (4:3)   960:600 = 1.6:1 (16:10 HD)
float g_fZnear = 1.0f;					//�þ� �ּ� �Ÿ� (1.0m) 
float g_fZfar = 100.0f;					//�þ� �ִ� �Ÿ� (100m) 


/////////////////////////////////////////////////////////////////////////////// 
//
// ī�޶� ����.
//
void CameraUpdate(float dTime)
{
	// ī�޶� ��ü�� "��ġ" �� "����" ���� 
	XMVECTOR eye = XMLoadFloat3(&g_vEye);		//ī�޶� ��ġ 
	XMVECTOR lookat = XMLoadFloat3(&g_vLookAt);	//�ٶ󺸴� ��.��ġ.
	XMVECTOR up = XMLoadFloat3(&g_vUp);			//ī�޶� ��� ����.	

	// �� ��ȯ ��� ���� :  View Transform 
	XMMATRIX mView = XMMatrixLookAtLH(eye, lookat, up);


	// ī�޶� "����" �� "�Կ� ����" ����.   
	g_fAspect = g_Mode.Width / (float)g_Mode.Height;	 //���� �ػ� ���� FOV ����.

	// ���� ���� ��ȯ ��� ���� : Projection Transform.
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(g_fFov, g_fAspect, g_fZnear, g_fZfar);


	//��� ���ۿ� ��� ����.
	g_cbDef.mView = mView;
	g_cbDef.mProj = mProj;

	XMMATRIX mInverse = g_cbDef.mTM;
	mInverse = XMMatrixInverse(nullptr, mInverse);

	// ���� ��ȯ�� ī�޶� ��ǥ
	g_cbCam.Position = XMVector4Transform(eye, mInverse);
	//g_cbCam.Position = XMVector3Normalize(g_cbCam.Position);


	//��� ���� ����
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBCam, &g_cbCam, sizeof(cbCAMERA));

	//��ǥ ���� : HUD��
	XMStoreFloat3(&g_Cam.Position, g_cbCam.Position);

	//Yena �� ī�޶� ������ ���� 
	//Yena ���� ��ü���� ���Ž� ī�޶� ������ �ʿ��մϴ�. 
	//����� ī�޶� ����Ѵٸ�, �ݵ�� Yena �� �����ؾ� �մϴ�.
	yn_View = mView;
	yn_Proj = mProj;


}





////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// ������ ���� ��ü ���� �Լ���.
//
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
// 
// ������ ���� ��ü ����.
//
void RasterStateCreate()
{
	//[���°�ü 1] �⺻ ������ ���� ��ü.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//�ﰢ�� ���� ä���.(�⺻��)
	rd.CullMode = D3D11_CULL_NONE;		//�ø� ����. (�⺻���� �ø� Back)		
	rd.FrontCounterClockwise = false;   //���� �⺻��...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	//�����Ͷ����� ��ü ����.
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_SOLID]);


	//[���°�ü2] ���̾� ������ �׸���. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_WIREFRM]);

	//[���°�ü3] �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_CULLBACK]);

	//[���°�ü4] ���̾� ������ + �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	g_pDevice->CreateRasterizerState(&rd, &g_RState[RS_WIRECULLBACK]);

}




////////////////////////////////////////////////////////////////////////////// 
//
// ������ ���� ��ü ���� : ���ø����̼� ����� ȣ��.
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
// ���� ó�� : ���� ����� ���̴����� ó���մϴ�.
//
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////// 
//
// ���� ó���� ���� GPU / ������ ���� ����.
//
void LightsUpdate(float dTime)
{
	//if(!g_bLightEnable) return;


	//---------------- 
	// �ֱ��� ���� ����  Diffuse Light
	//---------------- 	 
	g_Light.Direction = XMFLOAT3(1.0f, -3.0f, 2.0f);			//�� ����.(Normalize �ϴ� ���� ��õ. �̹� ���������� �̷��� ���)
	g_Light.Range = 1000.0f;									//������ �ִ� ���� �Ÿ� : Directional Light ���Ѵ�.(��������)
	g_Light.Diffuse = COLOR(1, 1, 1, 1);						//���� ( 100% ���).


	//---------------- 
	// �ֺ��� ���� ����. Ambient Light
	//---------------- 
	g_Light.Ambient =
						//COLOR(0.0f, 0.0f, 0.5f, 1.0f);		//50% �Ķ���....
						//COLOR(0.5f, 0.5f, 0.5f, 1.0f);		//50% ȸ��.
						//COLOR(0.4f, 0.4f, 0.4f, 1.0f);		//40% ȸ��.
						//COLOR(0.3f, 0.3f, 0.3f, 1.0f);		//30% ȸ��.
						COLOR(0.2f, 0.2f, 0.2f, 1.0f);			//20% ȸ�� : ǥ�� �ֺ���������
																//COLOR(0.1f, 0.1f, 0.1f, 1.0f);		//10% ȸ�� : ���� ������....
																//COLOR(0.0f, 0.0f, 0.0f, 1.0f);		//100% ������....

	g_Light.Specular = COLOR(1.0f, 1.0f, 1.0f, 1.0f);


	//---------------- 
	// ������� ����.
	//---------------- 
	XMVECTOR dir = XMLoadFloat3(&g_Light.Direction);	//������ normalize �Ǿ�� �մϴ�.
	g_cbLit.Diffuse = XMLoadFloat4(&g_Light.Diffuse);
	g_cbLit.Ambient = XMLoadFloat4(&g_Light.Ambient);
	g_cbLit.Specular = XMLoadFloat4(&g_Light.Specular);
	g_cbLit.Range = g_Light.Range;
	g_cbLit.LitOn = g_bLightEnable;
	g_cbLit.SpecOn = g_bSpecEnable;

	XMMATRIX mInverseTranspose = g_cbDef.mTM;
	mInverseTranspose = XMMatrixInverse(nullptr, mInverseTranspose);
	//mInverseTranspose = XMMatrixTranspose(mInverseTranspose);

	dir = XMVector4Transform(dir, mInverseTranspose);
	g_cbLit.Direction = XMVector3Normalize(-dir);

	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBLit, &g_cbLit, sizeof(cbLIGHT));

	//---------------- 
	// ���� ���� Material
	//---------------- 
	//... ������ ������ ��ü�� ���� �����ϴ� ���� �����̹Ƿ� 
	//... �� ��ü���� �и�, ����ϴ� ���� ���ڽ��ϴ�.�� 

}

// ����-�ý��� ���� ������  ����.. : �׷����� �սô�. ���� ������ ���Դϴ�.
// 
float EngineUpdate()
{
	//���� �ð� / Ÿ�̸� ���.
	float dTime = GetEngineTime();

	//Yena ���� : ���ʿ��� ����� �ּ�ó���Ͽ� ����� ��Ȱ��ȭ �Ͻʽÿ�.
	//ynCameraUpdate(dTime);		// ī�޶� ����	
	//ynLightsUpdate(dTime);		// ���� ����						
	ynGridAxisUpdate(dTime);		// ������, �׸��� ����.
	//YenaUpdate(dTime);			// ���� �ý��� ��ü �ϰ� ����.


	// ������ �ɼ� ���� 	 
	if (IsKeyUp(VK_SPACE))	g_bWireFrame ^= TRUE;
	if (IsKeyUp(VK_F4))		g_bCullBack ^= TRUE;
	if (IsKeyUp(VK_F5))		g_bLightEnable ^= TRUE;
	if (IsKeyUp(VK_F6))		g_bSpecEnable ^= TRUE;
	//if (IsKeyUp(VK_F5))		g_bZEnable ^= TRUE;		
	//if (IsKeyUp(VK_F6))		g_bZWrite ^= TRUE;		


	// ���� ����.
	if (g_bWireFrame) g_clearColor = COLOR(0.15f, 0.15f, 0.15f, 1.0f);
	else			  g_clearColor = COLOR(0, 0.125f, 0.3f, 1.0f);

	// ������ ��� ��ȯ.	  
	RenderModeUpdate();

	// ���� ���� ��� ��ȯ.	 
	if (g_bZEnable)
	{
		if (g_bZWrite)
			g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_ON], 0);				//���� ���� ���� (�⺻��)
		else  g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_WRITE_OFF], 0);	//���� ����  : Z-Test On + Z-Write Off.
	}
	else  g_pDXDC->OMSetDepthStencilState(g_DSState[DS_DEPTH_OFF], 0);	//���� ���� ��Ȱ��ȭ : Z-Test Off + Z-Write Off.


	return dTime;
}


/////////////////////////////////////////////////////////////////////////////// 
//
// ������ ��� üũ : �� ������ ��� ���º� On/Off ó��.
//
#define CheckRMode(k, v) if((k)) g_RMode |= (v); else g_RMode &= ~(v);

/////////////////////////////////////////////////////////////////////////////// 
//
// ������ ��� ��ȯ
//
void RenderModeUpdate()
{
	// ������ ��� üũ : ����ڰ� ������ ������ ���� ����.
	CheckRMode(g_bCullBack, RM_CULLBACK);
	CheckRMode(g_bWireFrame, RM_WIREFRAME);


	// ������ ��� ��ȯ : ������ ����� ������ ���� ������ ���¸� ����.
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
		int x = g_Mode.Width / 2 - 150, y = 1;
		COLOR col(1, 1, 1, 1);
		COLOR col2(1, 1, 0, 1);
		COLOR col3(0.4f, 0.4f, 0.4f, 1);
		ynTextDraw(x, y, col, L"�� %s", g_WindowName);

		ynTextDraw(x, y += 14, col, L"> ���� ���� ����");
		ynTextDraw(x, y += 14, col3, L"> 1.ǥ�� Ȯ�걤 ��(����) : Lambert Model (Local)");
		ynTextDraw(x, y += 14, col3, L"> 2.ǥ�� ���ݻ籤 �� : Blinn-Phong Model");
		ynTextDraw(x, y += 14, col2, L"> 3.ǥ�� ���ݻ籤 ��(����) : Blinn-Phong Model (Local)");
		//y = g_Mode.Height - 200;
		//ynTextDraw(x, y += 14, col2, L" * ����� �Է� ó�� �� �� ��ȯ ���� * ");
	}

	//�ý��� ����.
	{
		int x = 1, y = 14;
		COLOR col(0, 1, 0, 1);
		SystemInfo(x, y, col);
	}

	//�߰� ����..
	int x = 1, y = 180;
	COLOR col(1, 1, 1, 1);
	COLOR col2(1, 1, 0, 1);
	y += 14;
	ynTextDraw(x, y += 14, col, L"Fill: Space (%s)", g_bWireFrame ? L"WIRE" : L"SOLID");
	ynTextDraw(x, y += 14, col, L"Cull: F4 (%s)", g_bCullBack ? L"ON" : L"OFF");
	ynTextDraw(x, y += 14, col2, L"Light: F5 (%s)", g_bLightEnable ? L"ON" : L"OFF");
	ynTextDraw(x, y += 14, col2, L"Specular: F6 (%s)", g_bSpecEnable? L"ON":L"OFF");
	//ynTextDraw(x, y += 14, g_bZEnable?col:col*0.5f, L"���̾���: F6 (%s)", g_bZWrite?L"ON":L"OFF");

	//�� ���� ���.
	y += 14;
	ynTextDraw(x, y += 14, col, L"[Model]");
	ynTextDraw(x, y += 14, col, L"���� : F9");
	ynTextDraw(x, y += 14, col, L"�̵� : ����Ű");
	ynTextDraw(x, y += 14, col, L"ȸ�� : ADWS");
	ynTextDraw(x, y += 14, col, L"�ڵ� : R");

	//���� ���� ���.
	y += 25;
	col = g_bLightEnable ? col2 : col*0.4f;
	ynTextDraw(x, y += 14, col, L"[Light]");
	ynTextDraw(x, y += 14, col, L"Dir = { %.2f, %.2f, %.2f }", g_Light.Direction.x, g_Light.Direction.y, g_Light.Direction.z);
	ynTextDraw(x, y += 14, col, L"Diff = { %.2f, %.2f, %.2f, %.2f }", g_Light.Diffuse.x, g_Light.Diffuse.y, g_Light.Diffuse.z, g_Light.Diffuse.w);
	ynTextDraw(x, y += 14, col, L"Amb = { %.2f, %.2f, %.2f, %.2f }", g_Light.Ambient.x, g_Light.Ambient.y, g_Light.Ambient.z, g_Light.Ambient.w);
	ynTextDraw(x, y += 14, col, L"Spec = { %.2f, %.2f, %.2f, %.2f }", g_Light.Specular.x, g_Light.Specular.y, g_Light.Specular.z, g_Light.Specular.w);
	//SIMD Ÿ���� ����ϸ� �Ʒ�ó�� �����ؾ� ��.��
	//ynTextDraw(x, y += 14, col, L"Amb = { %.2f, %.2f, %.2f, %.2f }", XMVectorGetX(g_cbLit.Ambient), XMVectorGetY(g_cbLit.Ambient), XMVectorGetZ(g_cbLit.Ambient), XMVectorGetW(g_cbLit.Ambient));

	//���� ���� ���.
	y += 14;
	ynTextDraw(x, y += 14, col, L"[Material]");
	ynTextDraw(x, y += 14, col, L"Diff = { %.2f, %.2f, %.2f, %.2f }", g_Mtrl.Diffuse.x, g_Mtrl.Diffuse.y, g_Mtrl.Diffuse.z, g_Mtrl.Diffuse.w);
	ynTextDraw(x, y += 14, col, L"Amb = { %.2f, %.2f, %.2f, %.2f }", g_Mtrl.Ambient.x, g_Mtrl.Ambient.y, g_Mtrl.Ambient.z, g_Mtrl.Ambient.w);
	ynTextDraw(x, y += 14, col, L"Spec = { %.2f, %.2f, %.2f, %.2f }", g_Mtrl.Specular.x, g_Mtrl.Specular.y, g_Mtrl.Specular.z, g_Mtrl.Specular.w);
	ynTextDraw(x, y += 14, col, L"Power = { %.2f }", g_Mtrl.Power);



	col = COLOR(0, 1, 1, 1);
	y = g_Mode.Height - 14 * 5;
	ynTextDraw(x, y += 14, col, L"[Model]");
	ynTextDraw(x, y += 14, col, L"Pos = { %.2f, %.2f, %.2f }", g_vPos.x, g_vPos.y, g_vPos.z);
	ynTextDraw(x, y += 14, col, L"Rot = { %.2f, %.2f, %.2f }", g_vRot.x, g_vRot.y, g_vRot.z);
	ynTextDraw(x, y += 14, col, L"Scl = { %.2f, %.2f, %.2f }", g_vScale.x, g_vScale.y, g_vScale.z);


	//ī�޶� ���� ���.
	x = 200;  y = g_Mode.Height - 14 * 6;
	//col = COLOR(1, 1, 0, 1)*0.8f;
	ynTextDraw(x, y += 14, col, L"[ī�޶�]");
	ynTextDraw(x, y += 14, col, L"Eye={%.2f, %.2f, %.2f}", g_vEye.x, g_vEye.y, g_vEye.z);
	ynTextDraw(x, y += 14, col, L"Look={%.2f, %.2f, %.2f}", g_vLookAt.x, g_vLookAt.y, g_vLookAt.z);
	ynTextDraw(x, y += 14, col, L"Up={%.2f, %.2f, %.2f}", g_vUp.x, g_vUp.y, g_vUp.z);
	ynTextDraw(x, y += 14, col, L"LocalPos={%.2f, %.2f, %.2f}", g_Cam.Position.x, g_Cam.Position.y, g_Cam.Position.z);

	//ī�޶� - "����" : ������ȯ ���� ���.
	x = 400;  y = g_Mode.Height - 14 * 5;
	//col = COLOR(1, 1, 0, 1);
	ynTextDraw(x, y += 14, col, L"[����]");
	ynTextDraw(x, y += 14, col, L"FOV=%.2f (%.2f��)", g_fFov, XMConvertToDegrees(g_fFov));
	ynTextDraw(x, y += 14, col, L"Aspect=%.2f", g_fAspect);
	ynTextDraw(x, y += 14, col, L"Zn=%.1f  Zf = %.1f", g_fZnear, g_fZfar);

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
	float dTime = EngineUpdate();

	// ��� ���� 
	//
	LightsUpdate(dTime);	  //���� ���š�
	CameraUpdate(dTime);	  //ī�޶󰻽� : ��-���� ��ȯ��� ��� 

	// ���ΰ�, ����, ������ �̵�, �浹�˻� ���..
	// ...
	ObjUpdate(dTime);

	// ���� ����, �̺�Ʈ, ����, �������..
	// ...	 


	//-------------------------------
	// ��� �׸��� ����.. 
	//-------------------------------
	//����Ÿ��(�����)/ ���̹��� �����..
	ClearBackBuffer(D3D11_CLEAR_DEPTH, g_clearColor);

	//�ý��� ��ü ���.
	//ynGridAxisDraw();

	//��ü ������ : ���ΰ�, ����, ����.. 
	ObjDraw();

	//�ý��� ��ü & ���� ���..
	ynGridAxisDraw();
	ShowInfo();

	//-------------------------------
	// ��� �׸��� ����.
	//------------------------------- 
	Flip();


}//end of void SceneRender()



/****************** end of file "Render.cpp" *********************************/