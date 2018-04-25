
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
	float4  Direction;		//���� ����.
	float4  L_Diffuse;		//�� ���� : Ȯ�걤 Diffuse Light.
	float4  L_Ambient;		//���� ���� : �ֺ��� Ambient Light.
	float4  L_Specular;		//�ݻ� ���� : �ݻ籤 Specular Light.
	float    Range;			//�� ���� �Ÿ�.
	bool	 LitOn;			//���� ���뿩��.	
	bool	 SpecOn;		//�ݻ籤 ���뿩��
};

//���� ������ ������ۡ�
cbuffer cbMATERIAL : register(b2)
{
	float4 M_Diffuse;
	float4 M_Ambient;
	float4 M_Specular;
	float  Power;
};

//ī�޶� ������ ������ۡ�
cbuffer cbCAMERA : register(b3)
{
	float4 C_Position;		//ī�޶� ������ǥ
};



 
//VS ��� ����ü.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
	float4 nrm : TEXCOORD0;				//�븻����
	float4 localPos : TEXCOORD1;		//������Ʈ ���� ��ǥ
};


// ���� ó�� �Լ�.��
float4 Light(float4 nrm);
float4 Specular(float4 objPos, float4 Normal);



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
	VSOutput o = (VSOutput)0;

    pos.w = 1;

	o.localPos = pos;

	//���� ��ȯ.(World Transform) 
    pos = mul(pos, mTM);        //pos = pos * mTM
	
    //�þ�-�� ��ȯ (View Transform)
    pos = mul(pos, mView);
 
    //���� ���� ��ȯ (Projection Transform)
    pos = mul(pos, mProj);

	if (LitOn)
		o.col = Light(nrm);
	else
		o.col = 1;

    o.pos = pos; 
	o.nrm = nrm;

    return o;
}




// ���� ��� : ����Ʈ ������ �� ����. Lambert Lighting Model
//           : ���� ���� Local Space ���� ó��.��
float4 Light(float4 Normal)
{      
	float LightIntensity;	
	float4 Color = L_Ambient * M_Ambient;

	//LightIntensity = saturate(dot(N, Direction));
	LightIntensity = saturate(dot(Normal, Direction));

	if (LightIntensity > 0.0f)
	{
		Color += (L_Diffuse * M_Diffuse * LightIntensity);
	}

	Color = saturate(Color);

	//���� ���� ��� ����. 
	return Color;
}

// �ݻ籤 ���
//
float4 Specular(float4 objPos, float4 Normal)
{
	float4 view;
	float4 localCamPos = C_Position;
	float4 Spec = 0;
	float4 Half;

	localCamPos.w = 1;

	view = normalize(localCamPos - objPos);

	Half = normalize(Direction + view);

	Spec += pow(max(dot(Normal, Half), 0), Power) * L_Specular * M_Specular;
	Spec.w = 1;

	return Spec;

}

////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : �ȼ� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
				float4 pos : SV_POSITION,		//[�Է�] (������) �ȼ��� ��ǥ. (Screen, 2D)
				float4 col : COLOR0,			//[�Է�] (������) �ȼ��� ����. (Pixel Color : "Diffuse")
				float4 nrm : TEXCOORD0,			//[�Է�] �븻����
				float4 localPos : TEXCOORD1		//[�Է�] ������Ʈ ���� ��ġ
				) : SV_TARGET					//[���] ����.(�ʼ�), "����Ÿ��" ���� ����մϴ�.
{
	float4 Color = col;

	if (LitOn && SpecOn)
		Color = Light(nrm) + Specular(localPos, nrm);
	else if (LitOn)
		Color = Light(nrm);
	else
		Color = col;

		
	 
    //return col;
	return Color;
}



/**************** end of file "Demo.fx" ***********************/
