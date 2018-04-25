
// 상수 버퍼
cbuffer cbDEFAULT : register(b0)
{
    matrix mTM;     //월드 행렬. 
    matrix mView;   //뷰 변환 행렬. 
    matrix mWV;     //월드-뷰 변환 행렬. 
    matrix mProj;   //투영 변환 행렬. 
};


//조명 정보용 상수버퍼★
cbuffer cbLIGHT : register(b1)
{
	float4  Direction;		//빛의 방향.
	float4  L_Diffuse;		//주 광량 : 확산광 Diffuse Light.
	float4  L_Ambient;		//보조 광량 : 주변광 Ambient Light.
	float4  L_Specular;		//반사 광량 : 반사광 Specular Light.
	float    Range;			//빛 도달 거리.
	bool	 LitOn;			//조명 적용여부.	
	bool	 SpecOn;		//반사광 적용여부
};

//재질 정보용 상수버퍼★
cbuffer cbMATERIAL : register(b2)
{
	float4 M_Diffuse;
	float4 M_Ambient;
	float4 M_Specular;
	float  Power;
};

//카메라 정보용 상수버퍼★
cbuffer cbCAMERA : register(b3)
{
	float4 C_Position;		//카메라 로컬좌표
};



 
//VS 출력 구조체.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
	float4 nrm : TEXCOORD0;				//노말벡터
	float4 localPos : TEXCOORD1;		//오브젝트 로컬 좌표
};


// 조명 처리 함수.★
float4 Light(float4 nrm);
float4 Specular(float4 objPos, float4 Normal);



////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : 정점 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
				  float4 pos : POSITION,    //[입력] 정점좌표. Vertex Position (Model Space, 3D)
				  float4 nrm : NORMAL       //[입력] 노멀 normal ★
				)
{
	VSOutput o = (VSOutput)0;

    pos.w = 1;

	o.localPos = pos;

	//월드 변환.(World Transform) 
    pos = mul(pos, mTM);        //pos = pos * mTM
	
    //시야-뷰 변환 (View Transform)
    pos = mul(pos, mView);
 
    //원근 투영 변환 (Projection Transform)
    pos = mul(pos, mProj);

	if (LitOn)
		o.col = Light(nrm);
	else
		o.col = 1;

    o.pos = pos; 
	o.nrm = nrm;

    return o;
}




// 조명 계산 : 램버트 라이팅 모델 적용. Lambert Lighting Model
//           : 로컬 공간 Local Space 기준 처리.★
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

	//최종 조명 결과 리턴. 
	return Color;
}

// 반사광 계산
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
// Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
				float4 pos : SV_POSITION,		//[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
				float4 col : COLOR0,			//[입력] (보간된) 픽셀별 색상. (Pixel Color : "Diffuse")
				float4 nrm : TEXCOORD0,			//[입력] 노말벡터
				float4 localPos : TEXCOORD1		//[입력] 오브젝트 로컬 위치
				) : SV_TARGET					//[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
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
