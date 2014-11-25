//
//	タイマーを描画
//

#include	"main.h"

/*----------------------------------------------------------*
| <<< サークル描画 >>>
|	入力	iType	= 1 : ライン
|			= 0 : 塗りつぶし
|		iNum	= 円のどの位置か
|		iDiv	= 円の分割数
|		vecPos	= 表示座標
|		r0,  r1 = 半径		※ r0 が内側
|		dwCol	= 塗りつぶし色
*-----------------------------------------------------------*/
static	void	CircleDraw(int	iType,	int   iNum, int	 iDiv,	VEC2 * pPos, float r0, float r1, DWORD dwCol)
{
	D3_VTX_TL_  vt[4 + 1];
	float	    fR	  = D3DX_PI / (iDiv / 2.f);
	float	    fZoom = 1.55f;
	float	    s0,	c0, s1, c1;
	
	SINCOS(fR * (iDiv - iNum),	  &s0, &c0);
	SINCOS(fR * (iDiv - iNum - 0.6f), &s1, &c1);
	
	vt[0].x	  = D3_CONV_X(pPos->x + (r0 * c0) * fZoom);
	vt[0].y	  = D3_CONV_Y(pPos->y + r0 * s0);
	vt[0].z	  = 0;
	vt[0].rhw = 1.f;
	vt[0].c	  = dwCol;
	vt[1].x	  = D3_CONV_X(pPos->x + (r0 * c1) * fZoom);
	vt[1].y	  = D3_CONV_Y(pPos->y + r0 * s1);
	vt[1].z	  = 0;
	vt[1].rhw = 1.f;
	vt[1].c	  = dwCol;
	vt[2].x	  = D3_CONV_X(pPos->x + (r1 * c1) * fZoom);
	vt[2].y	  = D3_CONV_Y(pPos->y + r1 * s1);
	vt[2].z	  = 0;
	vt[2].rhw = 1.f;
	vt[2].c	  = dwCol;
	vt[3].x	  = D3_CONV_X(pPos->x + (r1 * c0) * fZoom);
	vt[3].y	  = D3_CONV_Y(pPos->y + r1 * s0);
	vt[3].z	  = 0;
	vt[3].rhw = 1.f;
	vt[3].c	  = dwCol;
	vt[4]	  = vt[0];

	d3_light_set(	    false);
	d3_stat_blend_set(D3_BLEND_ADD);
	d3_tex_set(	    -1);
	d3_material_set(    &d3.matNormal);

	if(iType == 1){ D3_RENDER(D3DPT_LINESTRIP,	vt,  TL, 4);}
	else	      { D3_RENDER(D3DPT_TRIANGLEFAN,	vt,  TL, 4);}

	d3_light_set(	   true);
	d3_stat_blend_set(D3_BLEND_NORMAL);
}

/*------------------------------------------------------------------*
|  <<< 時計描画 >>>
|     入力	fTime = 現在の時間
|		fMax  = 最大時間
*-------------------------------------------------------------------*/
void	TimerDraw(float fTime, float fMax)
{
	DWORD	c;
	int	i, j;

	fMax = (fTime / fMax) * 60.f;
	for(i = 0; i < 60; i++)
	{
	    if(i < fMax){ c = D3_COL(200, 255, 10, 10); j = 0;}
	    else	{ c = D3_COL(200, 255, 10, 10); j = 1;}
	    CircleDraw(j,  (i + 15) % 60, 60, &VEC2(320, 240), 190, 200, c);
	}
}