//
// 画像で printf
//

#include	"main.h"

//---定数を定義-------------------------------------------------------------------------
#define TEX_		TEX_MOJI			//ここにテクスチャーの番号を入れる
#define Z_			0			//Z位置
#define MOJI_MAX	1000		//登録できる文字数

//---変数を定義-------------------------------------------------------------------------
static D3_VTX_TLX  vtxBuf[MOJI_MAX * 6];	//登録用バッファ
static int		   iVtxCount;				//登録数

/*----------------------------------------------------------------------------------*
<< 文字を初期化 >>
*-----------------------------------------------------------------------*/
void	PrintfInit(void)
{
	iVtxCount = 0;
}

/*---------------------------------------------------------------------------------*
<<< バッファに溜まった文字をラッシュ >>>
*---------------------------------------------------------------------*/
void	PrintfFlush(void)
{
	//ここでは登録されたポリゴンデータを描きだしています。
	if(iVtxCount == 0){return;}

	d3_shader_set(			D3_SHADER_TYPE_OFF);
	d3_stat_alpha_greate_set(16);
	d3_light_set(			true);
	d3_tex_set(				TEX_);
	D3_POLY_RENDER(TRIANGLELIST, vtxBuf, TLX, iVtxCount / 3);
}

/*-----------------------------------------------------------------------*
 <<<頂点を登録>>>
 入力	*p0, *p1	    = 表示の左上、右下
		(u0,v0)-(u1,v1) = 文字のサイズ
		c				= 色
*-------------------------------------------------------------------*/
inline	void	Set(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1, DWORD c)
{
	//ここでは引数より頂点を作成し、登録しています。
	//頂点が三個でひとつのポリゴンとなります。
	//四角形ならば２ポリゴンが必要となります。

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
 <<< 画像で printf >>>	※文字列を解析し、一文字ずつ頂点を登録していきます。
 入力	fX, fY			= 表示位置
		fW, fH			= 文字サイズ
		fNextX, fNextY  = 
		iLenght			= 表示する文字の数(-1 なら全文字)
		*str			= 文字列
*--------------------------------------------------------------------------*/
static void	PrintfSub(float fX, float fY, float fW, float fH, float fNextX, float fNextY, int iLenght, char *str)
{
	//--- 文字列を一文字ずつ表示 ----------------------------
	float	u0, v0, u1, v1;
	VEC2	p0, p1;
	float	fUVSize = 16.f / d3.texInfo[TEX_].fWidth;

	//---文字列を一文字ずつ表示 ------------------
	p0.x		=fX;
	p0.y		=fY;

	for(int i = 0;; i++)
	{
		if(str[i] == NULL){break;}	     //終端か？
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
 <<<画像で printf >>>
  入力		fX, fY  = 表示位置
			fW	    = 文字サイズ
			*Format = printf と同様の文字列指定
*-----------------------------------------------------------------------------*/
void	Printf(float fX, float fY, float fW, char* Format,...)
{
	//---文字列の作成---------------------------------
	char	str[256];
	vsprintf(str, Format, (char *)(&Format + 1));

	//---文字列の実行----------------------------------------------------
	PrintfSub(fX, fY, fW, fW, fW - 1, fW + 2, -1, str);
}