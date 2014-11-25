//
//	文字表示
//


//#include	"main.h"


//--- 定数定義 ------------------------------------------------------------------------------------
#define	PRINTF_MAX		32					// 文字の登録数
#define	PRINTF_BUF_MAX	128					// 文字数
#define	PRINTF_SIZE		32					// 文字の一辺のサイズ
#define	PRINTF_BACK_A	255					// 文字の塗りつぶし色
#define	PRINTF_BACK_R	0
#define	PRINTF_BACK_G	0
#define	PRINTF_BACK_B	0

#define	MOJI_MAX		PRINTF_CHARA_MAX	// 最大文字数

#define	Z_				0					// Ｚ位置

#define	TEX_			TEX_MOJI_			// 使用テクスチャー

#define	FNAME_			"Ascii.png"			// フォント名


//--- 構造体定義 ----------------------------------------------------------------------------------
struct	PRINTF_WORK_					// Printf ワーク
{
	int		iX, iY;							// 表示座標(スクリーン座標)
	uint	uiSize;							// 文字サイズ
	BOOL	bBack;							// true : 背景塗りつぶし
											// 背景色
	uchar	ucBackA, ucBackR, ucBackG, ucBackB;
											// 文字色
	uchar	ucFrontA, ucFrontR, ucFrontG, ucFrontB;
};


//--- 変数定義 ------------------------------------------------------------------------------------
PRINTF_WORK_	print_[PRINTF_MAX];			// ワーク
											// 文字格納用
static	char	strBuf[PRINTF_MAX][PRINTF_BUF_MAX];
static	uchar	ucPrintCount;				// 文字の表示番号
											// 背景色
static	uchar	ucBackAlp, ucBackRed, ucBackGreen, ucBackBlue;
											// 文字色
static	uchar	ucFrontAlp, ucFrontRed, ucFrontGreen, ucFrontBlue;
											// 文字登録用
static	int			iVertexCount;
static	D3_VTX_TLX	vtxBuf[MOJI_MAX * 6];


//--- 関数宣言 ------------------------------------------------------------------------------------
static	void	CharaDraw(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1);



/*------------------------------------------------------------------------------*
| <<< 次に登録する文字色設定 >>>
|	入力	a, r, g, b = 色
*------------------------------------------------------------------------------*/
void	printf_front_color_set(uchar a, uchar r, uchar g, uchar b)
{
	ucFrontAlp   = a;
	ucFrontRed   = r;
	ucFrontGreen = g;
	ucFrontBlue  = b;
}



/*------------------------------------------------------------------------------*
| <<< 次に登録する文字の背景色設定 >>>
|	入力	a, r, g, b = 色
*------------------------------------------------------------------------------*/
void	printf_back_color_set(uchar a, uchar r, uchar g, uchar b)
{
	ucBackAlp   = a;
	ucBackRed   = r;
	ucBackGreen = g;
	ucBackBlue  = b;
}



/*------------------------------------------------------------------------------*
| <<< 文字リセット >>>
*------------------------------------------------------------------------------*/
void	printf_reset(void)
{												// 文字リセット
	ucPrintCount = 0;
}



/*------------------------------------------------------------------------------*
| <<< 文字表示初期化 >>>
*------------------------------------------------------------------------------*/
void	printf_init(void)
{

	//FOLDER_SET("画像");
	//d3_tex_load(TEX_, FNAME_);
	
	// 文字色
	ucFrontAlp = ucFrontRed = ucFrontGreen = ucFrontBlue  = 255;

	// 背景色
	ucBackAlp = 255;
	ucBackRed = ucBackGreen = ucBackBlue = 0;
}



/*------------------------------------------------------------------------------*
| <<< 頂点を作成 >>>
|	入力	*p0, *p1		= 表示の左上、右下
|			(u0,v0)-(u1,v1)	= 文字のサイズ
|			dwCol			= 文字色
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
| <<< 文字表示 >>>
*------------------------------------------------------------------------------*/
void	printf_flush(void)
{

	//--- 文字を見やすくするために背景を塗りつぶす ------------------
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
			if(*str == '\n')			// \n で改行
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

	//--- 描画 ------------------------------------------------------
	p = print_;
	for(i = 0; i < ucPrintCount; i++, p++)
	{
		float	fX    = (float)p->iX;
		float	fY    = (float)p->iY;
		float	fSize = (float)p->uiSize;
		char	*str  = strBuf[i];
		
		while(*str != NULL)
		{
			if(*str == '\n')			// \n で改行
			{
				fX  = (float)p->iX;
				fY += fSize + 2;
				str++;
				continue;
			}							// コントロールコードはスキップ
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

	//--- 文字をフラッシュ ------------------------------------------
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
| <<< 画像で printf	>>>						※２Ｄ座標用
|	入力	iX,	iY	= 表示位置
|			iSize   = 文字サイズ
|			bBack   = true : 背景を塗りつぶす
|			*Format	= printf と同様の文字列指定
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
		p->ucBackA  = ucBackAlp;			// 背景色
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// 文字色
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< 画像で printf	>>>						※２Ｄ座標用
|	入力	iX,	iY	= 表示位置
|			iSize   = 文字サイズ
|			*Format	= printf と同様の文字列指定
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
		p->ucBackA  = ucBackAlp;			// 背景色
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// 文字色
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< 画像で printf	>>>						※２Ｄ座標用
|	入力	iX,	iY	= 表示位置
|			*Format	= printf と同様の文字列指定
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
		p->ucBackA  = ucBackAlp;			// 背景色
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// 文字色
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< 画像で printf	>>>						※２Ｄ座標用
|	入力	*Format	= printf と同様の文字列指定
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
		p->ucBackA  = ucBackAlp;			// 背景色
		p->ucBackR  = ucBackRed;
		p->ucBackG  = ucBackGreen;
		p->ucBackB  = ucBackBlue;
		p->ucFrontA = ucFrontAlp;			// 文字色
		p->ucFrontR = ucFrontRed;
		p->ucFrontG = ucFrontGreen;
		p->ucFrontB = ucFrontBlue;
		ucPrintCount++;
	}
}



#ifdef	__CALC

	/*------------------------------------------------------------------------------*
	| <<< 画像で printf	>>>						※３Ｄ座標用
	|	入力	*pvecPos = 表示位置
	|			iSize    = 文字サイズ
	|			bBack    = true : 背景を塗りつぶす
	|			*Format	 = printf と同様の文字列指定
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

			// ３Ｄ座標よりスクリーン座標を得る
			VEC3	v;
			calc_vec3_project(&v, pvecPos, NULL);
			p->iX       = (int)v.x;
			p->iY       = (int)v.y;
			p->uiSize   = iSize;
			p->bBack    = bBack;
			p->ucBackA  = ucBackAlp;			// 背景色
			p->ucBackR  = ucBackRed;
			p->ucBackG  = ucBackGreen;
			p->ucBackB  = ucBackBlue;
			p->ucFrontA = ucFrontAlp;			// 文字色
			p->ucFrontR = ucFrontRed;
			p->ucFrontG = ucFrontGreen;
			p->ucFrontB = ucFrontBlue;
			ucPrintCount++;
		}
	}



	/*------------------------------------------------------------------------------*
	| <<< 画像で printf	>>>						※３Ｄ座標用
	|	入力	*pvecPos = 表示位置
	|			iSize    = 文字サイズ
	|			*Format	 = printf と同様の文字列指定
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

			// ３Ｄ座標よりスクリーン座標を得る
			VEC3	v;
			calc_vec3_project(&v, pvecPos, NULL);
			p->iX       = (int)v.x;
			p->iY       = (int)v.y;
			p->uiSize   = iSize;
			p->bBack    = TRUE;
			p->ucBackA  = ucBackAlp;			// 背景色
			p->ucBackR  = ucBackRed;
			p->ucBackG  = ucBackGreen;
			p->ucBackB  = ucBackBlue;
			p->ucFrontA = ucFrontAlp;			// 文字色
			p->ucFrontR = ucFrontRed;
			p->ucFrontG = ucFrontGreen;
			p->ucFrontB = ucFrontBlue;
			ucPrintCount++;
		}
	}



	/*------------------------------------------------------------------------------*
	| <<< 画像で printf	>>>						※３Ｄ座標用
	|	入力	*pvecPos = 表示位置
	|			*Format	 = printf と同様の文字列指定
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

			// ３Ｄ座標よりスクリーン座標を得る
			VEC3	v;
			calc_vec3_project(&v, pvecPos, NULL);
			p->iX       = (int)(v.x / d3.fZoom - d3.vecOfs.x / 2);
			p->iY       = (int)(v.y / d3.fZoom - d3.vecOfs.y / 2);
			p->uiSize   = 16;
			p->bBack    = TRUE;
			p->ucBackA  = ucBackAlp;			// 背景色
			p->ucBackR  = ucBackRed;
			p->ucBackG  = ucBackGreen;
			p->ucBackB  = ucBackBlue;
			p->ucFrontA = ucFrontAlp;			// 文字色
			p->ucFrontR = ucFrontRed;
			p->ucFrontG = ucFrontGreen;
			p->ucFrontB = ucFrontBlue;
			ucPrintCount++;
		}
	}
#endif
