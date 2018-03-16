//
// Device.cpp : D3D ��ü �� ����̽� ����/���� �ҽ�.
//
// DirectX ���� ���� : DX11
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
// DX �� ��ġ ���� ���� �����͵�  
//
///////////////////////////////////////////////////////////////////////////////

// D3D ���� ��ü �������̽� ������.
ID3D11Device*				g_pDevice = NULL;
ID3D11DeviceContext*		g_pDXDC = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pRTView = NULL;

// ��ġ ���� �⺻ ����
DISPLAYMODE g_Mode = { 800, 600, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM };

//D3D ��� ���� (Direct3D feature level) 
//���� ����� DX ���� ����. DX ������ ��ġ�� ȣȯ�� ���
D3D_FEATURE_LEVEL g_FeatureLevels = D3D_FEATURE_LEVEL_11_0;		//DX11 ����.
//D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_9_3;		//DX9.0c ����.
TCHAR* g_strFeatureLevel = L"N/A";

// �ʱ� ��ġ ���� ���� �Լ�
HRESULT CreateDeviceSwapChain(HWND hwnd);
HRESULT CreateRenderTarget();
void	SetViewPort();


///////////////////////////////////////////////////////////////////////////// 
//
// DX �� ���� ��ü �ʱ�ȭ : ���ø����̼� ���۽� 1ȸ ȣ��.
//
// 1.����̽� ���� 
// 2.����ü�� �� ����Ÿ�� ����.
// 3.����/���ٽ� ���� ����.
// 4.��Ÿ ������ ���� ��ġ ����.
//
int DXSetup(HWND hwnd)
{
	//----------------------------------------
	// 1�ܰ� : ������ ��ġ ���� �ʼ� ����
	//----------------------------------------
	// D3D ������ ��ġ Device �� ����ü�� Swap Chain ����. 
	CreateDeviceSwapChain(hwnd);

	// ��ġ-����ü���� ����Ÿ��(�����) ȹ��
	CreateRenderTarget();

	// ��ġ ��º��ձ�(Output Merger) �� ���͸� Ÿ�� �� ����-���ٽ� ���� ���.
	g_pDXDC->OMSetRenderTargets(
		1,			// ����Ÿ�� ����.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&g_pRTView, // ����Ÿ��("�����") ���.	
		nullptr
		);

	SetViewPort(); // ����Ʈ ����

	GetDeviceInfo(); // ����̽�(��ġ) ���� ȹ��

	//----------------------------------------
	// 2�ܰ� : ��Ÿ ������ ���� �߰� ����.
	//----------------------------------------
	// �������� �ʿ��� ����� ��ü���� ����/���� �մϴ�.
	// ī�޶�, ����, ��Ʈ, ���̴� ���...

	// ��Ʈ ����.��
	ynFontCreate(g_pDevice);

	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////// 
//
// ����̽� ��ü �� ���� ��ġ ���� : ����� 1ȸ ȣ��.  
// �⺻ ������ ��ü�� ���Ž� ������ �������� ó���մϴ�.(����)
//
void DXRelease()
{	 
	// ��ġ ���� ���� : ���� ���� �ʱ�ȭ�� �ؾ� �մϴ�. (�޸� ���� ����)
	if (g_pDXDC) g_pDXDC->ClearState();

	ynFontRelease();
	SAFE_RELEASE(g_pRTView);		// ����Ÿ�� ����
	SAFE_RELEASE(g_pSwapChain);		
	SAFE_RELEASE(g_pDXDC);
	SAFE_RELEASE(g_pDevice);		// ����̽� ����. ���� ���߿� ����
}
 
HRESULT CreateDeviceSwapChain(HWND hwnd)
{
	HRESULT hr = S_OK;

	// ��ġ (Device) �� ����ü��(SwapChain) ���� ����.
	// ����ü���� ���߹��۸� �ý����� ���ϸ�
	// �������� '�ø���Flipping' ü�ΰ� ������ �ǹ��Դϴ�.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = TRUE;							//Ǯ��ũ�� �Ǵ� â��� ����.
	sd.OutputWindow = hwnd;						//����� ������ �ڵ�.
	sd.BufferCount = 1;							//����� ����.
	sd.BufferDesc.Width = g_Mode.Width;			//�ػ� ����.(����� ũ��)
	sd.BufferDesc.Height = g_Mode.Height;
	sd.BufferDesc.Format = g_Mode.Format;		//����� ����԰� (A8R8G8B8) â��忡���� ���� ���� 
	sd.BufferDesc.RefreshRate.Numerator = 0;	//���� ������.(��������ȭ VSync Ȱ��ȭ�� ǥ�ذ����� ���� : 60hz)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //�뵵 ���� '���� Ÿ��'
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;					// Anti-Aliasing ����
	sd.SampleDesc.Quality = 0;

	// D3D ������ ��ġ(Device) �� ����ü�� ����
	hr = D3D11CreateDeviceAndSwapChain(
			NULL,						// ���� ����� ������(�׷��� �����ġ ��ȣ) �⺻��ġ�� NULL
			D3D_DRIVER_TYPE_HARDWARE,	// �ϵ���� ���� (�׷��� ī�带 ���)
			NULL,						// SW Rasterizer DLL �ڵ�. �ϵ���� ���ӽÿ��� NULL
			0,							// ����̽� ���� �÷���(�⺻��). [����� ��] D3D11_CREATE_DEVICE_DEBUG
			&g_FeatureLevels,			// ������ ����̽� ��� ���� �迭
			1,							// ������ ����̽� ��� ���� �迭 ũ��
			D3D11_SDK_VERSION,			// DX SDK ����
			&sd,						// ����̽� ���� �� �ɼ�
			&g_pSwapChain,				// [���] ����ü�� ���ͺ��̽� ���
			&g_pDevice,					// [���] ����̽� ���ͺ��̽� ���
			NULL,						// [���] ������ ����̽� ��� ����. �ʿ���ٸ� NULL ����
			&g_pDXDC					// [���] ����̽� ���ؽ�Ʈ ���
	);

	if (FAILED(hr))
	{
		ynError(hr, L"D3D11CreateDeviceAndSwapChain Failed!");
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//
// ��ġ-����ü���� ����Ÿ��(�����) ȹ��
//
// ���ҽ���(Resource View)  
// DX11 �� ���ҽ��� "����" �������� �����ϰ� ����Ǿ� �ֽ��ϴ�. 
// (�޸� ������ ȿ���� ���� �� �ٿ뵵 Ȱ���� ����..)
// ���� ����� �� �뵵�� �´� ���ٹ��(�������̽�)�� ȹ��(����)�ؾ� �մϴ�.
// �� �۾��� "���ҽ� �� Resource View" �� ���� ó���մϴ�.
//                     
// ����ü�ο� ����(Bind)�� ������ ��¹��� - "����� BackBuffer" �� 2D ��� ���ҽ�(�ؽ�ó) �̸�
// "����Ÿ�� RenderTarget" ������ ���ҽ��並 ����, �ش� ���۸� ����/��� �ϰڽ��ϴ�.
//
HRESULT CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// ����� ȹ��
	ID3D11Texture2D* pBackBuffer;
	// ����� ���, 2D �ؽ��� �����̸� ȭ�� ��¿��̴�.
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
	{
		ynError(hr, L"GetBuffer Failed!");
		return hr;
	}

	// ȹ���� ����ۿ� ����Ÿ�� �� ����. (����Ÿ�� '��'���� ������)
	hr = g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView); //����Ÿ�� �並 ����.
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
// ����Ʈ ���� :  DX11 ������ �⺻ó�� ���� ������ ����ڰ� ���� �����ؾ��մϴ� 
//			     (OpenGL ������ �������� �̷��� �� �Խ��ϴ�.)
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
// ��ġ-����ü��/����Ÿ��/����/���ٽ� ���� Ŭ���� 
//
int ClearBackBuffer(COLOR col)
{
	g_pDXDC->ClearRenderTargetView(g_pRTView, (float*)&col);			//����Ÿ�� �����.

	return YN_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// ��ġ-����ü�� ��ȯ : ��� �׸��� �Ϸ�� �� ȣ���ؾ� ��.
//
int Flip()
{
	g_pSwapChain->Present(0, 0);			//ȭ����� : Flip!

	return YN_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// �ʴ� �����Ӽ� ���.
//
// ���� : int x, y : ����� ȭ�� ��ǥ.
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
// ���� ó�� �Լ���.
//
////////////////////////////////////////////////////////////////////////////// 

////////////////////////////////////////////////////////////////////////////// 
//
// ���� �޼��� ó��
//
int ynErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...)
{
	TCHAR msgva[1024] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);


	//HRESULT ���� ���� �޼��� ���.
	TCHAR herr[1024] = L"";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);

	//���� �޼��� ���̱�.
	TCHAR errmsg[2048] = L"";
	_stprintf(errmsg, L"%s \n�����ڵ�(0x%X) : %s \nFile = %s (%d)\nFunc = %s\n\n",
		msgva, hr, herr,
		file, line, func);


	//(����� ��) VS ���â���� ���..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);

	//�α����Ϸ� ���.
	//...

	//�޼��� â ���..
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
// ��ġ/GPU ���� ȹ�� �Լ���
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// ��ġ/GPU ���� ȹ��
//
void GetDeviceInfo()
{
	//��ġ ��ɷ��� Ȯ��.
	GetFeatureLevel();

	//GPU ���� ���.
	//...

	//����� ���� ���.
	//...

}




/////////////////////////////////////////////////////////////////////////////
//
//  DX ��� ���� ���ϱ�.
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

	ver = ((feat & 0xf000) >> 12) - OFFSET;	//���� ���� ����.   	
	sub = ((feat & 0x0f00) >> 8);			//���� ���� ����.

	g_strFeatureLevel = strFeature[ver][sub];
}




/////////////////////////////////////////////////////////////////////////////
//
// ���� ������ �����ε�.
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
// ��Ʈ ���� ��
//
// DirectXTK : SpriteFont ��� ��Ʈ ��� Ŭ����
// 2D Texture / Sprite ����� ��Ʈ ��� ���̺귯��.
// ������ ����������, CJK �迭�� ��Ʈ�� ��� �ϼ����� �����˴ϴ�.
// ���� : https://directxtk.codeplex.com/wikipage?title=SpriteFont
// #include "SpriteFont.h"	
//
// �ڼ��� ���� DirectXTK.h �Ǵ�  Yena/DXTK/Readme.txt �� ����. 
//
///////////////////////////////////////////////////////////////////////////////

SpriteBatch* g_pFontBatch = nullptr;
SpriteFont*  g_pFont = nullptr;


///////////////////////////////////////////////////////////////////////////////
// 
int ynFontCreate(LPDEVICE pDev)
{

	//��ġ ��� ȹ��.
	LPDXDC pDXDC = nullptr;
	pDev->GetImmediateContext(&pDXDC);


	//Sprite Batch ��ü ����.
	g_pFontBatch = new SpriteBatch(pDXDC);

	//DirectX Toolkit : Sprite Font ��ü ����.
	TCHAR* filename = L"../Yena/Font/����9k.sfont";	//ASCII 0 ~ 255 + Ư������'��' + Unicode �ѱ� �ϼ��� �� 11,440 ����, ũ��:9		
	try {
		g_pFont = new SpriteFont(pDev, filename);
		g_pFont->SetLineSpacing(14.0f);				//��Ʈ9 ����, �ٰ��� ����. '���߶��� ��½� ����� ������'
		g_pFont->SetDefaultCharacter('_');			//��� ���ڰ� �̰˻��� ��� ����� Ű��.
	}
	catch (std::exception& e)
	{
		//ynError(0, L"��Ʈ ���� ���� : File=%s", filename);
		TCHAR msg[1024] = L"";
		::mbstowcs(msg, e.what(), strlen(e.what()));
		ynError(0, L"��Ʈ ���� ���� : %s \n File=%s", msg, filename);
	}

	//�����, ��ġ��� ����.
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
// ���ڿ� ��� : DX11 (�����ڵ�) ����.
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
	//g_pFontBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, ypStateObj[YNSO_DEPTH_ON_]);		//���̿��� �߰�.
	g_pFont->DrawString(g_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col));					//���� ũ��.
																										//g_pFont->DrawString(ypFontBatch, buff, Vector2((float)x, (float)y), Vector4(col), 0, Vector4::Zero, 0.8f);	//80% ��� ���.
	g_pFontBatch->End();

}
 

/////////////////////////////////////////////////////////////////////////////// 
//
// ���ڿ� ��� : GDI, �����ڵ� ����. ���� Dx9 ����
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


 
