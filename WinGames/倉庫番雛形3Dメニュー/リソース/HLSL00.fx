//
//	00 D3_SHADER_TYPE_NORMAL : 通常版
//

//#include	"def.h"
static		const	int		MAX_MATRICES = 50;


//--- 座標計算用 --------------------------------------------------------------
float4x4	g_mtxWorld;										// World
float4x4	g_mtxViewProjection;							// View	* Projection

//--- ライトや色に関わるもの --------------------------------------------------
float4		g_vecLightDirection;							// ライトの向き
float4		g_vecDiffuse;									// diffuse
float4		g_vecAmbient;									// 環境光
int			g_iLightSw;										// 1 : ライトオン
float		g_fAlp;											// アルファ値

//--- ボーンに関わるもの ------------------------------------------------------
int			g_iNumBones;									// ブレンド係数
float4x3	g_mtxWorldMatrixArray[MAX_MATRICES];			// 行列パレット

//--- フォグの変数 ------------------------------------------------------------
/*
float3		g_vecCenter;									// カメラ位置
float		g_fFogNear =  60.f;								// フォグの開始位置
float		g_fFogFar  = 100.f;								// フォグの終了位置
float		g_fFogLen  =  40.f;								// g_fFogFar - g_fFogNear の結果
float4		g_colFog;										// フォグカラー
*/


//--- テクスチャー ------------------------------------------------------------
texture		texDecale;						// 通常のテクスチャー

//--- サンプラステート --------------------------------------------------------
sampler	smpDecale = sampler_state			// デカール
{
	Texture	  =	(texDecale);
	MipFilter =	LINEAR;
	MagFilter =	LINEAR;
	MinFilter =	LINEAR;
	ADDRESSU  =	WRAP;
	ADDRESSV  =	WRAP;
};


//-----------------------------------------------------------------------------
//	<<<	フォーマット >>>
//-----------------------------------------------------------------------------
struct	VS_NOSKIN_INPUT
{
	float4	Position		: POSITION;
	float3	Normal			: NORMAL;
	float2	Tex				: TEXCOORD0;
};

struct	VS_INPUT
{
	float4	Position		: POSITION;
	float4	BlendWeights	: BLENDWEIGHT;
	float4	BlendIndices	: BLENDINDICES;
	float3	Normal			: NORMAL;
	float2	Tex				: TEXCOORD0;
};

struct	VS_OUTPUT
{
	float4	Position		: POSITION;
	float4	Color			: COLOR;
	float2	Tex				: TEXCOORD0;
	//float3	Fog			: TEXCOORD1;
};


//-----------------------------------------------------------------------------
//	<<<	ボーンあり頂点シェーダー >>>
//-----------------------------------------------------------------------------
VS_OUTPUT	VS(VS_INPUT	In,	uniform	int	NumBones)
{
	VS_OUTPUT	Out	= (VS_OUTPUT)0;
	int4		IndexVector			 = D3DCOLORtoUBYTE4(In.BlendIndices);
	int			IndexArray[		  4] = (int[  4])IndexVector;
	float		BlendWeightsArray[4] = (float[4])In.BlendWeights;
	float		LastWeight = 0.0f;
	float3		nor		   = 0.0f;
	 
	//--- ボーン部分 ------------------------------------------------
	for(int	iBone =	0; iBone < NumBones	- 1; iBone++)
	{
		Out.Position.xyz +=	mul(In.Position, g_mtxWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
		nor				 +=	mul(In.Normal,	 g_mtxWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
		LastWeight		 +=	BlendWeightsArray[iBone];
	}
	LastWeight		  =	1.0f - LastWeight; 
	Out.Position.xyz +=	(mul(In.Position, g_mtxWorldMatrixArray[IndexArray[NumBones	- 1]]) * LastWeight);
	nor			     += (mul(In.Normal,	  g_mtxWorldMatrixArray[IndexArray[NumBones - 1]]) * LastWeight);
	Out.Position.w	  =	1;
	float3	bak       = Out.Position.xyz;
	Out.Position	  =	mul(Out.Position, g_mtxViewProjection);

	//---- 正規化 ---------------------------------------------------
	nor = normalize(nor);

	//--- Shade	(Ambient + etc.) ------------------------------------
	Out.Color.xyz =	g_vecAmbient.xyz + max(0.0f, dot(nor, -g_vecLightDirection.xyz)) * g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- テクスチャー ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- ３Ｄ座標(フォグ用) ----------------------------------------
	//Out.Fog    = bak;

	return Out;
}


//-----------------------------------------------------------------------------
//	<<<	ボーンあり頂点シェーダー >>>	※ライトオフ
//-----------------------------------------------------------------------------
VS_OUTPUT	VS_NOLIGHT(VS_INPUT	In,	uniform	int	NumBones)
{
	VS_OUTPUT	Out	= (VS_OUTPUT)0;
	int4		IndexVector			 = D3DCOLORtoUBYTE4(In.BlendIndices);
	int			IndexArray[		  4] = (int[  4])IndexVector;
	float		BlendWeightsArray[4] = (float[4])In.BlendWeights;
	float		LastWeight = 0.0f;
	float3		nor		   = 0.0f;
	 
	//--- ボーン部分 ------------------------------------------------
	for(int	iBone =	0; iBone < NumBones	- 1; iBone++)
	{
		Out.Position.xyz +=	mul(In.Position, g_mtxWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
		nor				 +=	mul(In.Normal,	 g_mtxWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
		LastWeight		 +=	BlendWeightsArray[iBone];
	}
	LastWeight		  =	1.0f - LastWeight; 
	Out.Position.xyz +=	(mul(In.Position, g_mtxWorldMatrixArray[IndexArray[NumBones	- 1]]) * LastWeight);
	nor			     += (mul(In.Normal,	  g_mtxWorldMatrixArray[IndexArray[NumBones - 1]]) * LastWeight);
	Out.Position.w	  =	1;
	float3	bak       = Out.Position.xyz;
	Out.Position	  =	mul(Out.Position, g_mtxViewProjection);

	//---- 正規化 ---------------------------------------------------
	nor = normalize(nor);

	//--- Shade	(Ambient + etc.) ------------------------------------
	Out.Color.xyz = g_vecAmbient.xyz + g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- テクスチャー ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- ３Ｄ座標(フォグ用) ----------------------------------------
	//Out.Fog    = bak;

	return Out;
}


//-----------------------------------------------------------------------------
//	<<<	ボーンなし頂点シェーダー >>>
//-----------------------------------------------------------------------------
VS_OUTPUT	VS_NOSKIN(VS_NOSKIN_INPUT In)
{
	VS_OUTPUT	Out;
 
	//--- 座標変換 --------------------------------------------------
 	float4		P  = mul(In.Position, g_mtxWorld);
	Out.Position.w = 1;
	Out.Position   = mul(P,	g_mtxViewProjection);

	//--- 色 --------------------------------------------------------
	Out.Color.xyz =	g_vecAmbient.xyz + max(0.0f, dot(In.Normal,	-g_vecLightDirection.xyz)) * g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- テクスチャー ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- ３Ｄ座標(フォグ用) ----------------------------------------
	//Out.Fog    = P;

	return Out;
}


//-----------------------------------------------------------------------------
//	<<<	ボーンなし頂点シェーダー >>>	※ライトオフ
//-----------------------------------------------------------------------------
VS_OUTPUT	VS_NOSKIN_NOLIGHT(VS_NOSKIN_INPUT In)
{
	VS_OUTPUT	Out;
 
	//--- 座標変換 --------------------------------------------------
 	float4		P  = mul(In.Position, g_mtxWorld);
	Out.Position.w = 1;
	Out.Position   = mul(P,	g_mtxViewProjection);

	//--- 色 --------------------------------------------------------
	Out.Color.xyz = g_vecAmbient.xyz + g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- テクスチャー ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- ３Ｄ座標(フォグ用) ----------------------------------------
	//Out.Fog    = P;

	return Out;
}


//-----------------------------------------------------------------------------
//	  <<< ピクセルシェーダー >>>
//-----------------------------------------------------------------------------
float4	PS(VS_OUTPUT In) : COLOR0
{

	// 頂点と視点との距離を計算する
	// 0.0f～1.0fの範囲内に調整する
/*
	float	d = distance(In.Fog.xyz, g_vecCenter.xyz);
	if(     d < g_fFogNear) d = 0.0f;
	else if(d > g_fFogFar ) d = 1.0f;
	else					d = (d - g_fFogNear) / g_fFogLen;
  
	// オブジェクトのランバート拡散照明の計算結果とフォグカラーを線形合成する
	float4	Out = In.Color * tex2D(smpDecale, In.Tex) * (1.0f - d) + g_colFog * d;
	
	// アルファ値はフォグの値を使う
	Out.a *= (g_fAlp - d);

	return Out;
*/

	float4	decale = tex2D(smpDecale, In.Tex) * In.Color;
	decale.a *= g_fAlp;
	return	decale;
}



//-----------------------------------------------------------------------------
//	<<<	 Techniques	>>>
//-----------------------------------------------------------------------------
VertexShader	vsArray[10] =
{	 
	// ライトオフ
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOSKIN_NOLIGHT(),

	// ライトオン
	compile	vs_2_0 VS(g_iNumBones),
	compile	vs_2_0 VS(g_iNumBones),
	compile	vs_2_0 VS(g_iNumBones),
	compile	vs_2_0 VS(g_iNumBones),
	compile	vs_2_0 VS_NOSKIN()
};

technique	Normal
{
	pass p0
	{
		VertexShader = (vsArray[g_iNumBones + g_iLightSw * 5]);
 		PixelShader	 = compile ps_2_0 PS();
	}
}

