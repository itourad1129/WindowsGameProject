//
// �摜�� printf
//

#include	"main.h"

//---�萔���`-------------------------------------------------------------------------
#define TEX_		TEX_MOJI			//�����Ƀe�N�X�`���[�̔ԍ�������
#define Z_			0			//Z�ʒu
#define MOJI_MAX	1000		//�o�^�ł��镶����

//---�ϐ����`-------------------------------------------------------------------------
static D3_VTX_TLX  vtxBuf[MOJI_MAX * 6];	//�o�^�p�o�b�t�@
static int		   iVtxCount;				//�o�^��

/*----------------------------------------------------------------------------------*
<< ������������ >>
*-----------------------------------------------------------------------*/
void	PrintfInit(void)
{
	iVtxCount = 0;
}

/*---------------------------------------------------------------------------------*
<<< �o�b�t�@�ɗ��܂������������b�V�� >>>
*---------------------------------------------------------------------*/
void	PrintfFlush(void)
{
	//�����ł͓o�^���ꂽ�|���S���f�[�^��`�������Ă��܂��B
	if(iVtxCount == 0){return;}

	d3_shader_set(			D3_SHADER_TYPE_OFF);
	d3_stat_alpha_greate_set(16);
	d3_light_set(			true);
	d3_tex_set(				TEX_);
	D3_POLY_RENDER(TRIANGLELIST, vtxBuf, TLX, iVtxCount / 3);
}

/*-----------------------------------------------------------------------*
 <<<���_��o�^>>>
 ����	*p0, *p1	    = �\���̍���A�E��
		(u0,v0)-(u1,v1) = �����̃T�C�Y
		c				= �F
*-------------------------------------------------------------------*/
inline	void	Set(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1, DWORD c)
{
	//�����ł͈�����蒸�_���쐬���A�o�^���Ă��܂��B
	//���_���O�łЂƂ̃|���S���ƂȂ�܂��B
	//�l�p�`�Ȃ�΂Q�|���S�����K�v�ƂȂ�܂��B

	if((iVtxCount + 6) >= MOJI_MAX){ return;}

	D3_VTX_TLX_ vt[] =
	{
		p0->x, p1->y, Z_, 1, c, u0, v1,
		p0->x, p0->y, Z_, 1, c, u0, v0,
		p1->x, p0->y, Z_, 1, c, u1, v0,
		p0->x, p1->y, Z_, 1, c, u0, v1,
		p1->x, p0->y, Z_, 1, c, u1, v0,
		p1->x, p1->y, Z_, 1, c, u1, v1
	};
	memcpy(&vtxBuf[iVtxCount], vt, sizeof(D3_VTX_TLX) * 6);
	iVtxCount += 6;
}

/*-------------------------------------------------------------------------------*
 <<< �摜�� printf >>>	�����������͂��A�ꕶ�������_��o�^���Ă����܂��B
 ����	fX, fY			= �\���ʒu
		fW, fH			= �����T�C�Y
		fNextX, fNextY  = 
		iLenght			= �\�����镶���̐�(-1 �Ȃ�S����)
		*str			= ������
*--------------------------------------------------------------------------*/
static void	PrintfSub(float fX, float fY, float fW, float fH, float fNextX, float fNextY, int iLenght, char *str)
{
	//--- ��������ꕶ�����\�� ----------------------------
	float	u0, v0, u1, v1;
	VEC2	p0, p1;
	float	fUVSize = 16.f / d3.texInfo[TEX_].fWidth;

	//---��������ꕶ�����\�� ------------------
	p0.x		=fX;
	p0.y		=fY;

	for(int i = 0;; i++)
	{
		if(str[i] == NULL){break;}	     //�I�[���H
		if(str[i] == '\n')
		{
			p0.x  = fX;
			p0.y += fNextY;
			continue;
		}
		else if((uchar)str[i] < ' ')
		{
			continue;
		}

		uchar	a	= str[i];
		v0			= (float)(a % 16) * fUVSize;
		u0			= (float)(a / 16) * fUVSize;
		v1			= v0 + fUVSize;
		u1			= u0 + fUVSize;
		p1.x		= p0.x + fW;
		p1.y		= p0.y + fH;
		if(iLenght != -1)
		{
			iLenght--;
			if(iLenght < 0){ p0.x += fNextX; break;}
		}

		if(p0 != p1)
		{
			VEC2	p2(D3_CONV_X(p0.x), D3_CONV_Y(p0.y));
			VEC2	p3(D3_CONV_X(p1.x), D3_CONV_Y(p1.y));
			Set(&p2, &p3, u0, v0, u1, v1, D3_COL(255, 255, 255, 255));
		}
		p0.x += fNextX;
	}
}

/*--------------------------------------------------------------------------------*
 <<<�摜�� printf >>>
  ����		fX, fY  = �\���ʒu
			fW	    = �����T�C�Y
			*Format = printf �Ɠ��l�̕�����w��
*-----------------------------------------------------------------------------*/
void	Printf(float fX, float fY, float fW, char* Format,...)
{
	//---������̍쐬---------------------------------
	char	str[256];
	vsprintf(str, Format, (char *)(&Format + 1));

	//---������̎��s----------------------------------------------------
	PrintfSub(fX, fY, fW, fW, fW - 1, fW + 2, -1, str);
}