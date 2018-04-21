//
//
// Demo.fx : �⺻ ���̴� �ҽ�.
//
// 2003.11.4. Kihong Kim / mad_dog@hanmail.net
// 2010.12.01. Update. (DX11.Jun.2010)
// 2016.12.27. Update. (DX11. Windows SDK 8.1)
//


// ��� ����
cbuffer cbDEFAULT : register(b0)
{
    matrix mTM;     //���� ���. 
    matrix mView;   //�� ��ȯ ���. 
    matrix mWV;     //����-�� ��ȯ ���. 
    matrix mProj;   //���� ��ȯ ���. 
};


//���� ������ ������ۡ�
cbuffer cbLIGHT : register(b1)
{
    // ���� ���� ����.
	// ...���� ��꿡 �ʿ��� �ڷḦ �̰��� ����.(����)��
	// ...�� ����, ���� ��..
	float4 Direction;		//���� ����.
	float4 L_Diffuse;		//�� ���� : Ȯ�걤 Diffuse Light.
	float4 L_Ambient;		//���� ���� : �ֺ��� Ambient Light.
	float    Range;			//�� ���� �Ÿ�.
	bool	 LitOn;			//���� ���뿩��.	

};

//���� ������ ������ۡ�
cbuffer cbMATERIAL : register(b2)
{
    //���� ���� ����.
	//...���� ��꿡 �ʿ��� �ڷḦ �̰��� ����.(����)..��
	//...
	float4 M_Diffuse;
	float4 M_Ambient;
};



 
//VS ��� ����ü.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};


// ���� ó�� �Լ�.��
float4 Light(float4 nrm);




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : ���� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
				  float4 pos : POSITION,    //[�Է�] ������ǥ. Vertex Position (Model Space, 3D)
				  float4 nrm : NORMAL       //[�Է�] ��� normal ��
				)
{
	//* �Ʒ��� �׽�Ʈ�� �����մϴٱ� ����  
    //* VS �� ��� ���۰� �����Ǿ� �־�� �մϴ�.    
    pos.w = 1;

	//���� ��ȯ.(World Transform) 
    pos = mul(pos, mTM);        //pos = pos * mTM
	
    //�þ�-�� ��ȯ (View Transform)
    pos = mul(pos, mView);
 
    //���� ���� ��ȯ (Projection Transform)
    pos = mul(pos, mProj);

    //���� ���.(Lighting)��
    float4 diff = 1;  

    if(LitOn) diff = Light(nrm);


    //���� ���.
    VSOutput o = (VSOutput) 0;
    o.pos = pos; 
    o.col = diff;

    return o;
}




////////////////////////////////////////////////////////////////////////////// 
//
// ���� ��� : ����Ʈ ������ �� ����. Lambert Lighting Model
//          : �� ���� View Space ���� ó��.��
//
float4 Light(float4 nrm)
{      
	float4 Color;
	float4 N;
	
	float LightIntensity;
	
	// �븻�� ���庯ȯ �Ͽ��ش�.
	N = mul(nrm, mTM);
	//N = normalize(N);
	
	//Color = (0, 0, 0, 0);
	//Color = L_Ambient * M_Ambient;
	Color = L_Ambient;


	LightIntensity = saturate(dot(N, Direction));

	if (LightIntensity > 0.0f)
	{
		Color += (L_Diffuse * LightIntensity);
	}

	Color = saturate(Color);

	//���� ���� ��� ����. 
	return Color;
}




////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : �ȼ� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
				float4 pos : SV_POSITION,   //[�Է�] (������) �ȼ��� ��ǥ. (Screen, 2D)
				float4 col : COLOR0         //[�Է�] (������) �ȼ��� ����. (Pixel Color : "Diffuse")
				) : SV_TARGET               //[���] ����.(�ʼ�), "����Ÿ��" ���� ����մϴ�.
{
	//Ư���� ��ȯ ���� �Էµ� ������ �״�� ����մϴ�.
    //...
	 
    return col;
}



/**************** end of file "Demo.fx" ***********************/
