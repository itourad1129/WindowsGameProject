//
//	�����\��
//


//#include	"main.h"


//--- �萔��` ------------------------------------------------------------------------------------
#define	PRINTF_MAX		32					// �����̓o�^��
#define	PRINTF_BUF_MAX	128					// ������
#define	PRINTF_SIZE		32					// �����̈�ӂ̃T�C�Y
#define	PRINTF_BACK_A	255					// �����̓h��Ԃ��F
#define	PRINTF_BACK_R	0
#define	PRINTF_BACK_G	0
#define	PRINTF_BACK_B	0

#define	MOJI_MAX		PRINTF_CHARA_MAX	// �ő啶����

#define	Z_				0					// �y�ʒu

#define	TEX_			TEX_MOJI_			// �g�p�e�N�X�`���[

#define	FNAME_			"Ascii.png"			// �t�H���g��


//--- �\���̒�` ----------------------------------------------------------------------------------
struct	PRINTF_WORK_					// Printf ���[�N
{
	int		iX, iY;							// �\�����W(�X�N���[�����W)
	uint	uiSize;							// �����T�C�Y
	BOOL	bBack;							// true : �w�i�h��Ԃ�
											// �w�i�F
	uchar	ucBackA, ucBackR, ucBackG, ucBackB;
											// �����F
	uchar	ucFrontA, ucFrontR, ucFrontG, ucFrontB;
};


//--- �ϐ���` ------------------------------------------------------------------------------------
PRINTF_WORK_	print_[PRINTF_MAX];			// ���[�N
											// �����i�[�p
static	char	strBuf[PRINTF_MAX][PRINTF_BUF_MAX];
static	uchar	ucPrintCount;				// �����̕\���ԍ�
											// �w�i�F
static	uchar	ucBackAlp, ucBackRed, ucBackGreen, ucBackBlue;
											// �����F
static	uchar	ucFrontAlp, ucFrontRed, ucFrontGreen, ucFrontBlue;
											// �����o�^�p
static	int			iVertexCount;
static	D3_VTX_TLX	vtxBuf[MOJI_MAX * 6];


//--- �֐��錾 ------------------------------------------------------------------------------------
static	void	CharaDraw(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1);



/*------------------------------------------------------------------------------*
| <<< ���ɓo�^���镶���F�ݒ� >>>
|	����	a, r, g, b = �F
*------------------------------------------------------------------------------*/
void	printf_front_color_set(uchar a, uchar r, uchar g, uchar b)
{
	ucFrontAlp   = a;
	ucFrontRed   = r;
	ucFrontGreen = g;
	ucFrontBlue  = b;
}



/*------------------------------------------------------------------------------*
| <<< ���ɓo�^���镶���̔w�i�F�ݒ� >>>
|	����	a, r, g, b = �F
*------------------------------------------------------------------------------*/
void	printf_back_color_set(uchar a, uchar r, uchar g, uchar b)
{
	ucBackAlp   = a;
	ucBackRed   = r;
	ucBackGreen = g;
	ucBackBlue  = b;
}



/*------------------------------------------------------------------------------*
| <<< �������Z�b�g >>>
*------------------------------------------------------------------------------*/
void	printf_reset(void)
{												// �������Z�b�g
	ucPrintCount = 0;
}



/*------------------------------------------------------------------------------*
| <<< �����\�������� >>>
*------------------------------------------------------------------------------*/
void	printf_init(void)
{

	//FOLDER_SET("�摜");
	//d3_tex_load(TEX_, FNAME_);
	
	// �����F
	ucFrontAlp = ucFrontRed = ucFrontGreen = ucFrontBlue  = 255;

	// �w�i�F
	ucBackAlp = 255;
	ucBackRed = ucBackGreen = ucBackBlue = 0;
}



/*------------------------------------------------------------------------------*
| <<< ���_���쐬 >>>
|	����	*p0, *p1		= �\���̍���A�E��
|			(u0,v0)-(u1,v1)	= �����̃T�C�Y
|			dwCol			= �����F
*------------------------------------------------------------------------------*/
static	void	CharaDraw(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1, DWORD dwCol)
{

	if((iVertexCount + 6) >= MOJI_MAX){ return;}

	float	x0  = D3_CONV_X(p0->x);
	float	x1  = D3_CONV_X(p1->x);
	float	y0  = D3_CONV_Y(p0->y);
	float	y1  = D3_CONV_Y(p1->y);
	//DWORD	c   = D3_COL_NORMAL;

	D3_VTX_TLX_	vt[] =
	{
		x0, y1, Z_, 1, dwCol, u0, v1,
		x0, y0, Z_, 1, dwCol, u0, v0,
		x1, y0, Z_, 1, dwCol, u1, v0,
		x0, y1, Z_, 1, dwCol, u0, v1,
		x1, y0, Z_, 1, dwCol, u1, v0,
		x1, y1, Z_, 1, dwCol, u1, v1
	};
	memcpy(&vtxBuf[iVertexCount], vt, sizeof(D3_VTX_TLX)	* 6);
	iVertexCount +=	6;
}



/*------------------------------------------------------------------------------*
| <<< �����\�� >>>
*------------------------------------------------------------------------------*/
void	printf_flush(void)
{

	//--- ���������₷�����邽�߂ɔw�i��h��Ԃ� ------------------
	int				i;
	PRINTF_WORK_	*p = print_;
	for(i = 0; i < ucPrintCount; i++, p++)
	{
		if(!p->bBack || p->ucBackA == 0){ continue;}
		float	fX    = (float)p->iX;
		float	fY    = (float)p->iY;
		float	fSize = (float)p->uiSize;
		char	*str  = strBuf[i];
		int		j     = 0;
		while(*str != NULL)
		{
			if(*str == '\n')			// \n �ŉ��s
			{
				d3_2d_box_fill_draw(&VEC2(fX, fY), &VEC2(fX + j * fSize, fY + fSize + 2), D3_COL(p->ucBackA, p->ucBackR, p->ucBackG, p->ucBackB));
				j   = 0;
				fY += fSize + 2;
				str++;
				continue;
			}
			j++;
			str++;
		}
		if(j > 0){ d3_2d_box_fill_draw(&VEC2(fX, fY), &VEC2(fX + j * fSize, fY + fSize + 2), D3_COL(p->ucBackA, p->ucBackR, p->ucBackG, p->ucBackB));}
	}

	//--- �`�� ------------------------------------------------------
	p = print_;
	for(i = 0; i < ucPrintCount; i++, p++)
	{
		float	fX    = (float)p->iX;
		float	fY    = (float)p->iY;
		float	fSize = (float)p->uiSize;
		char	*str  = strBuf[i];
		
		while(*str != NULL)
		{
			if(*str == '\n')			// \n �ŉ��s
			{
				fX  = (float)p->iX;
				fY += fSize + 2;
				str++;
				continue;
			}							// �R���g���[���R�[�h�̓X�L�b�v
			else if((uchar)*str < ' ')
			{
				str++;
				continue;
			}
		
			if((uchar)*str != ' ')
			{
				float	uv_size	= 16.f / 256;
				float	fOfs    =  2.f / d3.texInfo[TEX_].fOriWidth;
				float	v0 = (float)(*str % 16) * uv_size + fOfs;
				float	u0 = (float)(*str / 16) * uv_size + fOfs;
				float	v1 = v0 + uv_size - fOfs * 2;
				float	u1 = u0 + uv_size - fOfs * 2;
				CharaDraw(&VEC2(fX, fY), &VEC2(fX + fSize, fY + fSize), u0, v0, u1, v1, D3_COL(p->ucFrontA, p->ucFrontR, p->ucFrontG, p->ucFrontB));
			}
			str++;
			fX += fSize;
		}
	}
	ucPrintCount = 0;

	//--- �������t���b�V�� ------------------------------------------
	if(iVertexCount == 0){ return;}

	d3_light_set(            true);
	d3_stat_culling_set(     false);
	d3_tex_set(              TEX_);
	d3_stat_filter_set(      D3_FILTER_LINEAR);

	d3_stat_transform_set(&d3.mtxIdentity);
	d3_shader_set(        D3_SHADER_TYPE_OFF);

	d3_stat_alpha_set(       true);
	d3_stat_alpha_greate_set(16);
	D3_RENDER(D3DPT_TRIANGLELIST, vtxBuf, TLX, iVertexCount / 3)

	d3.iPolyCount += iVertexCount / 3;
	iVertexCount   = 0;

	d3_stat_culling_set(	 true);
}



/*------------------------------------------------------------------------------*
| <<< �摜�� printf	>>>						���Q�c���W�p
|	����	iX,	iY	= �\���ʒu
|			iSize   = �����T�C�Y
|			bBack   = true : �w�i��h��Ԃ�
|			*Format	= printf �Ɠ��l�̕�����w��
*------------------------------------------------------------------------------*/
void	printf_2d(int iX, int iY, int iSize, BOOL bBack, char *Format,...)
{

	if(ucPrintCount >= PRINTF_MAX)
	{
		return;
	}
	else
	{
		PRINTF_WORK_	*p = &print_[ucPrintCount];
	
		va_list	ap;
		va_start(ap, Format);
		vsprintf(strBuf[ucPrintCount], Format, ap);
		va_end(ap);

		p->iX       = iX;
		p->iY       = iY;
		p->uiSize   = iSize;
		p->bBack    = bBack;
		p->ucBackA  = ucBackAlp;			// �w�i�F
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// �����F
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �摜�� printf	>>>						���Q�c���W�p
|	����	iX,	iY	= �\���ʒu
|			iSize   = �����T�C�Y
|			*Format	= printf �Ɠ��l�̕�����w��
*------------------------------------------------------------------------------*/
void	printf_2d(int iX, int iY, int iSize, char *Format,...)
{

	if(ucPrintCount >= PRINTF_MAX)
	{
		return;
	}
	else
	{
		PRINTF_WORK_	*p = &print_[ucPrintCount];
	
		va_list	ap;
		va_start(ap, Format);
		vsprintf(strBuf[ucPrintCount], Format, ap);
		va_end(ap);

		p->iX       = iX;
		p->iY       = iY;
		p->uiSize   = iSize;
		p->bBack    = FALSE;
		p->ucBackA  = ucBackAlp;			// �w�i�F
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// �����F
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �摜�� printf	>>>						���Q�c���W�p
|	����	iX,	iY	= �\���ʒu
|			*Format	= printf �Ɠ��l�̕�����w��
*------------------------------------------------------------------------------*/
void	printf_2d(int iX, int iY, char *Format,...)
{

	if(ucPrintCount >= PRINTF_MAX)
	{
		return;
	}
	else
	{
		PRINTF_WORK_	*p = &print_[ucPrintCount];
	
		va_list	ap;
		va_start(ap, Format);
		vsprintf(strBuf[ucPrintCount], Format, ap);
		va_end(ap);

		p->iX       = iX;
		p->iY       = iY;
		p->uiSize   = 16;
		p->bBack    = TRUE;
		p->ucBackA  = ucBackAlp;			// �w�i�F
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// �����F
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �摜�� printf	>>>						���Q�c���W�p
|	����	*Format	= printf �Ɠ��l�̕�����w��
*------------------------------------------------------------------------------*/
void	printf_2d(char *Format,...)
{

	if(ucPrintCount >= PRINTF_MAX)
	{
		return;
	}
	else
	{
		PRINTF_WORK_	*p = &print_[ucPrintCount];
	
		va_list	ap;
		va_start(ap, Format);
		vsprintf(strBuf[ucPrintCount], Format, ap);
		va_end(ap);

		p->iX       = 0;
		p->iY       = 0;
		p->uiSize   = 16;
		p->bBack    = TRUE;
		p->ucBackA  = ucBackAlp;			// �w�i�F
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// �����F
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



#ifdef	__CALC

	/*------------------------------------------------------------------------------*
	| <<< �摜�� printf	>>>						���R�c���W�p
	|	����	*pvecPos = �\���ʒu
	|			iSize    = �����T�C�Y
	|			bBack    = true : �w�i��h��Ԃ�
	|			*Format	 = printf �Ɠ��l�̕�����w��
	*------------------------------------------------------------------------------*/
	void	printf_3d(VEC3 *pvecPos, int iSize, BOOL bBack, char *Format,...)
	{

		if(ucPrintCount >= PRINTF_MAX)
		{
			return;
		}
		else
		{
			PRINTF_WORK_	*p = &print_[ucPrintCount];
		
			va_list	ap;
			va_start(ap, Format);
			vsprintf(strBuf[ucPrintCount], Format, ap);
			va_end(ap);

			// �R�c���W���X�N���[�����W�𓾂�
			VEC3	v;
			calc_vec3_project(&v, pvecPos, NULL);
			p->iX       = (int)v.x;
			p->iY       = (int)v.y;
			p->uiSize   = iSize;
			p->bBack    = bBack;
			p->ucBackA  = ucBackAlp;			// �w�i�F
			p->ucBackR  = ucBackRed;
			p->ucBackG  = ucBackGreen;
			p->ucBackB  = ucBackBlue;
			p->ucFrontA = ucFrontAlp;			// �����F
			p->ucFrontR = ucFrontRed;
			p->ucFrontG = ucFrontGreen;
			p->ucFrontB = ucFrontBlue;
			ucPrintCount++;
		}
	}



	/*------------------------------------------------------------------------------*
	| <<< �摜�� printf	>>>						���R�c���W�p
	|	����	*pvecPos = �\���ʒu
	|			iSize    = �����T�C�Y
	|			*Format	 = printf �Ɠ��l�̕�����w��
	*------------------------------------------------------------------------------*/
	void	printf_3d(VEC3 *pvecPos, int iSize, char *Format,...)
	{

		if(ucPrintCount >= PRINTF_MAX)
		{
			return;
		}
		else
		{
			PRINTF_WORK_	*p = &print_[ucPrintCount];
		
			va_list	ap;
			va_start(ap, Format);
			vsprintf(strBuf[ucPrintCount], Format, ap);
			va_end(ap);

			// �R�c���W���X�N���[�����W�𓾂�
			VEC3	v;
			calc_vec3_project(&v, pvecPos, NULL);
			p->iX       = (int)v.x;
			p->iY       = (int)v.y;
			p->uiSize   = iSize;
			p->bBack    = TRUE;
			p->ucBackA  = ucBackAlp;			// �w�i�F
			p->ucBackR  = ucBackRed;
			p->ucBackG  = ucBackGreen;
			p->ucBackB  = ucBackBlue;
			p->ucFrontA = ucFrontAlp;			// �����F
			p->ucFrontR = ucFrontRed;
			p->ucFrontG = ucFrontGreen;
			p->ucFrontB = ucFrontBlue;
			ucPrintCount++;
		}
	}



	/*------------------------------------------------------------------------------*
	| <<< �摜�� printf	>>>						���R�c���W�p
	|	����	*pvecPos = �\���ʒu
	|			*Format	 = printf �Ɠ��l�̕�����w��
	*------------------------------------------------------------------------------*/
	void	printf_3d(VEC3 *pvecPos, char *Format,...)
	{

		if(ucPrintCount >= PRINTF_MAX)
		{
			return;
		}
		else
		{
			PRINTF_WORK_	*p = &print_[ucPrintCount];
		
			va_list	ap;
			va_start(ap, Format);
			vsprintf(strBuf[ucPrintCount], Format, ap);
			va_end(ap);

			// �R�c���W���X�N���[�����W�𓾂�
			VEC3	v;
			calc_vec3_project(&v, pvecPos, NULL);
			p->iX       = (int)(v.x / d3.fZoom - d3.vecOfs.x / 2);
			p->iY       = (int)(v.y / d3.fZoom - d3.vecOfs.y / 2);
			p->uiSize   = 16;
			p->bBack    = TRUE;
			p->ucBackA  = ucBackAlp;			// �w�i�F
			p->ucBackR  = ucBackRed;
			p->ucBackG  = ucBackGreen;
			p->ucBackB  = ucBackBlue;
			p->ucFrontA = ucFrontAlp;			// �����F
			p->ucFrontR = ucFrontRed;
			p->ucFrontG = ucFrontGreen;
			p->ucFrontB = ucFrontBlue;
			ucPrintCount++;
		}
	}
#endif
