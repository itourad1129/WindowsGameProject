//
//	00 D3_SHADER_TYPE_NORMAL : �ʏ��
//

//#include	"def.h"
static		const	int		MAX_MATRICES = 50;


//--- ���W�v�Z�p --------------------------------------------------------------
float4x4	g_mtxWorld;										// World
float4x4	g_mtxViewProjection;							// View	* Projection

//--- ���C�g��F�Ɋւ����� --------------------------------------------------
float4		g_vecLightDirection;							// ���C�g�̌���
float4		g_vecDiffuse;									// diffuse
float4		g_vecAmbient;									// ����
int			g_iLightSw;										// 1 : ���C�g�I��
float		g_fAlp;											// �A���t�@�l

//--- �{�[���Ɋւ����� ------------------------------------------------------
int			g_iNumBones;									// �u�����h�W��
float4x3	g_mtxWorldMatrixArray[MAX_MATRICES];			// �s��p���b�g

//--- �t�H�O�̕ϐ� ------------------------------------------------------------
/*
float3		g_vecCenter;									// �J�����ʒu
float		g_fFogNear =  60.f;								// �t�H�O�̊J�n�ʒu
float		g_fFogFar  = 100.f;								// �t�H�O�̏I���ʒu
float		g_fFogLen  =  40.f;								// g_fFogFar - g_fFogNear �̌���
float4		g_colFog;										// �t�H�O�J���[
*/


//--- �e�N�X�`���[ ------------------------------------------------------------
texture		texDecale;						// �ʏ�̃e�N�X�`���[

//--- �T���v���X�e�[�g --------------------------------------------------------
sampler	smpDecale = sampler_state			// �f�J�[��
{
	Texture	  =	(texDecale);
	MipFilter =	LINEAR;
	MagFilter =	LINEAR;
	MinFilter =	LINEAR;
	ADDRESSU  =	WRAP;
	ADDRESSV  =	WRAP;
};


//-----------------------------------------------------------------------------
//	<<<	�t�H�[�}�b�g >>>
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
//	<<<	�{�[�����蒸�_�V�F�[�_�[ >>>
//-----------------------------------------------------------------------------
VS_OUTPUT	VS(VS_INPUT	In,	uniform	int	NumBones)
{
	VS_OUTPUT	Out	= (VS_OUTPUT)0;
	int4		IndexVector			 = D3DCOLORtoUBYTE4(In.BlendIndices);
	int			IndexArray[		  4] = (int[  4])IndexVector;
	float		BlendWeightsArray[4] = (float[4])In.BlendWeights;
	float		LastWeight = 0.0f;
	float3		nor		   = 0.0f;
	 
	//--- �{�[������ ------------------------------------------------
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

	//---- ���K�� ---------------------------------------------------
	nor = normalize(nor);

	//--- Shade	(Ambient + etc.) ------------------------------------
	Out.Color.xyz =	g_vecAmbient.xyz + max(0.0f, dot(nor, -g_vecLightDirection.xyz)) * g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- �e�N�X�`���[ ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- �R�c���W(�t�H�O�p) ----------------------------------------
	//Out.Fog    = bak;

	return Out;
}


//-----------------------------------------------------------------------------
//	<<<	�{�[�����蒸�_�V�F�[�_�[ >>>	�����C�g�I�t
//-----------------------------------------------------------------------------
VS_OUTPUT	VS_NOLIGHT(VS_INPUT	In,	uniform	int	NumBones)
{
	VS_OUTPUT	Out	= (VS_OUTPUT)0;
	int4		IndexVector			 = D3DCOLORtoUBYTE4(In.BlendIndices);
	int			IndexArray[		  4] = (int[  4])IndexVector;
	float		BlendWeightsArray[4] = (float[4])In.BlendWeights;
	float		LastWeight = 0.0f;
	float3		nor		   = 0.0f;
	 
	//--- �{�[������ ------------------------------------------------
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

	//---- ���K�� ---------------------------------------------------
	nor = normalize(nor);

	//--- Shade	(Ambient + etc.) ------------------------------------
	Out.Color.xyz = g_vecAmbient.xyz + g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- �e�N�X�`���[ ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- �R�c���W(�t�H�O�p) ----------------------------------------
	//Out.Fog    = bak;

	return Out;
}


//-----------------------------------------------------------------------------
//	<<<	�{�[���Ȃ����_�V�F�[�_�[ >>>
//-----------------------------------------------------------------------------
VS_OUTPUT	VS_NOSKIN(VS_NOSKIN_INPUT In)
{
	VS_OUTPUT	Out;
 
	//--- ���W�ϊ� --------------------------------------------------
 	float4		P  = mul(In.Position, g_mtxWorld);
	Out.Position.w = 1;
	Out.Position   = mul(P,	g_mtxViewProjection);

	//--- �F --------------------------------------------------------
	Out.Color.xyz =	g_vecAmbient.xyz + max(0.0f, dot(In.Normal,	-g_vecLightDirection.xyz)) * g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- �e�N�X�`���[ ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- �R�c���W(�t�H�O�p) ----------------------------------------
	//Out.Fog    = P;

	return Out;
}


//-----------------------------------------------------------------------------
//	<<<	�{�[���Ȃ����_�V�F�[�_�[ >>>	�����C�g�I�t
//-----------------------------------------------------------------------------
VS_OUTPUT	VS_NOSKIN_NOLIGHT(VS_NOSKIN_INPUT In)
{
	VS_OUTPUT	Out;
 
	//--- ���W�ϊ� --------------------------------------------------
 	float4		P  = mul(In.Position, g_mtxWorld);
	Out.Position.w = 1;
	Out.Position   = mul(P,	g_mtxViewProjection);

	//--- �F --------------------------------------------------------
	Out.Color.xyz = g_vecAmbient.xyz + g_vecDiffuse.xyz;
	Out.Color.w   = g_vecDiffuse.w;

	//--- �e�N�X�`���[ ----------------------------------------------
	Out.Tex.xy = In.Tex;

	//--- �R�c���W(�t�H�O�p) ----------------------------------------
	//Out.Fog    = P;

	return Out;
}


//-----------------------------------------------------------------------------
//	  <<< �s�N�Z���V�F�[�_�[ >>>
//-----------------------------------------------------------------------------
float4	PS(VS_OUTPUT In) : COLOR0
{

	// ���_�Ǝ��_�Ƃ̋������v�Z����
	// 0.0f�`1.0f�͈͓̔��ɒ�������
/*
	float	d = distance(In.Fog.xyz, g_vecCenter.xyz);
	if(     d < g_fFogNear) d = 0.0f;
	else if(d > g_fFogFar ) d = 1.0f;
	else					d = (d - g_fFogNear) / g_fFogLen;
  
	// �I�u�W�F�N�g�̃����o�[�g�g�U�Ɩ��̌v�Z���ʂƃt�H�O�J���[����`��������
	float4	Out = In.Color * tex2D(smpDecale, In.Tex) * (1.0f - d) + g_colFog * d;
	
	// �A���t�@�l�̓t�H�O�̒l���g��
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
	// ���C�g�I�t
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOLIGHT(g_iNumBones),
	compile	vs_2_0 VS_NOSKIN_NOLIGHT(),

	// ���C�g�I��
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

