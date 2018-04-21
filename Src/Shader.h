
#pragma once

//���̴� ����.
extern ID3D11VertexShader*     g_pVS;
extern ID3D11PixelShader*      g_pPS;

//���� ���̴� ������ �ڵ� ��ü.(�ӽ�)
extern ID3DBlob* g_pVSCode;


HRESULT ShaderLoad();
HRESULT ShaderCompile(
						WCHAR* FileName,		//�ҽ������̸�.
						char* EntryPoint,		//�����Լ� ������.
						char* ShaderModel,		//���̴� ��.
						ID3DBlob** ppCode		//[���] �����ϵ� ���̴� �ڵ�.
					 );

void	ShaderRelease();

//�⺻ ��� ���� ����ü.��
//�� ������ ��ü�� �⺻ ���� - ��ȯ ��.. - �� ���.
struct cbDEFAULT
{
	XMMATRIX mTM;		//"World" ��ȯ ��� : DirectXMath, 16����Ʈ ���� ����. 
	XMMATRIX mView;		//"View" ��ȯ ���
	XMMATRIX mWV;		//"World * View" ��ȯ ���
	XMMATRIX mProj;		//"Projection" ��ȯ ���

};

//���� ������ ������� ����ü��
struct cbLIGHT
{
	XMVECTOR Direction;		//���� ����.
	XMVECTOR Diffuse;		//�� ���� : Ȯ�걤 Diffuse Light.
	XMVECTOR Ambient;		//���� ���� : �ֺ��� Ambient Light.
	FLOAT    Range;			//�� ���� �Ÿ�.
	BOOL	 LitOn;			//���� ���뿩��.
};

//���� ������ ������� ����ü��
struct cbMATERIAL
{
	XMVECTOR Diffuse;		//�� ����(Ȯ�걤) �� �ݻ���(%) 
	XMVECTOR Ambient;		//���� ����(�ֺ���) �� �ݻ���(%) 
};



//���̴� ��� ����.�������̽�.
extern ID3D11Buffer*	g_pCBDef;		//��
extern ID3D11Buffer*	g_pCBLit;		//��
extern ID3D11Buffer*	g_pCBMtrl;		//��

										//(����) ��� ���� ����.
HRESULT CreateConstantBuffer(UINT size, ID3D11Buffer** ppCB);
//���� ������� ����.
HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB);
//���� ������� ����.
HRESULT UpdateDynamicConstantBuffer(LPDXDC pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size);