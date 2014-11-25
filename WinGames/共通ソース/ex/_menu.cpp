//
//	メニュー処理
//


//#include	"main.h"
//#include	"_text.h"
//#include	"_menu.h"
#if 0

//--- 定数 --------------------------------------------------------
#define	FONT		3						// フォント番号（2 ～ 7 の範囲で）
#define	TEX_FRAME	0
#define	TEX_MENU	1
#define	ADD_W		 4.0f
#define	ADD_H		-2.5f

//--- 変数 --------------------------------------------------------
static	int		iSelect;					// 選択項目の番号
static	int		iBlinkTime;					// 点滅時間
static	VEC3	vecX0Y0, vecX1Y1;			// 矩形領域の描画範囲



/*------------------------------------------------------------------------------*
| <<< 8 × 8 のパーツを並べてウィンドウの描画 >>>
|	入力	p0 = 左上の座標
|			p1 = 右下の座標
*------------------------------------------------------------------------------*/
void	MenuFrameDraw(VEC3 *p0, VEC3 *p1)
{
	FOLDER_SET("スクリプト");
	d3_tex_load(TEX_FRAME, "枠.png");
	D3_TEXTURE(0, TEX_FRAME);
	float	u0, v0, u1, v1;

	if(sys.bFrameSkip) return;
	
	for(float x = p0->x; x < p1->x; x += 8.f)
	{
		for(float y = p0->y; y < p1->y; y += 8.f)
		{
			if(     x == p0->x     && y == p0->y    ){ u0 =  0; v0 =  0; u1 =  8; v1 =  8;}	// 左上
			else if(x >= p1->x - 8 && y == p0->y    ){ u0 = 16; v0 =  0; u1 = 24; v1 =  8;}	// 右上
			else if(x == p0->x     && y >= p1->y - 8){ u0 =  0; v0 = 16; u1 =  8; v1 = 24;}	// 左下
			else if(x >= p1->x - 8 && y >= p1->y - 8){ u0 = 16; v0 = 16; u1 = 24; v1 = 24;}	// 右下
			else if(x == p0->x                      ){ u0 =  0; v0 =  8; u1 =  8; v1 = 16;}	// 左
			else if(x >= p1->x - 8                  ){ u0 = 16; v0 =  8; u1 = 24; v1 = 16;}	// 右
			else if(                  y == p0->y    ){ u0 =  8; v0 =  0; u1 = 16; v1 =  8;}	// 上
			else if(                  y >= p1->y - 8){ u0 =  8; v0 = 16; u1 = 16; v1 = 24;}	// 下
			else									 { u0 =  8; v0 =  8; u1 = 16; v1 = 16;}	// 真ん中
			D3_2DDRAW(&VEC3(x,     y,     0.02f),
					  &VEC3(x + 8, y + 8, 0.02f),
					  u0 / d3.texInfo[TEX_FRAME].fWidth,
					  v0 / d3.texInfo[TEX_FRAME].fHeight,
					  u1 / d3.texInfo[TEX_FRAME].fWidth,
					  v1 / d3.texInfo[TEX_FRAME].fHeight,
					  D3_COL(255, 255, 255, 255));
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< マウスが選択して項目の背景を描画 >>>
|	入力	w     = メニューワーク
|			iLine = 横の文字数
|			iMax  = 項目の数
|	戻り値	-1    : なにも選択していない
|			 0 ～ : 選択した項目の番号
*------------------------------------------------------------------------------*/
static	int		MenuBoxDraw(MENU_WORK *w, int iLine, int iMax)
{
	int		ret = -1;

	D3_BLEND_NORMAL();						// 半透明オン

	//--- 項目選択時に点滅 -------------------------------------
	if(iBlinkTime != 0)
	{
		iBlinkTime -= 8;
		float	r   = (iBlinkTime / 60.f) * 255.f;
		D3_2DBOX_FILL(&vecX0Y0, &vecX1Y1,  D3_COL(128, (int)r, 0, 0));
		if(iBlinkTime == 0) return iSelect;
		else				return -1;
	}
	//--- メニューの項目を描画しつクリックチェック ----------------
	for(int i = 0; i < iLine; i++)
	{
		float	fW  = w->fX + iMax * (d3.fontInfo[FONT].Width  + ADD_W);
		float	fH0 = w->fY + i    * (d3.fontInfo[FONT].Height + ADD_H);
		float	fH1 = fH0   +         d3.fontInfo[FONT].Height;
		VEC3	p0(w->fX, fH0, 0.01f);
		VEC3	p1(fW,    fH1, 0.01f);
		if(pad.sX > p0.x && pad.sX < p1.x		// マウスが指されている範囲を塗りつぶす
		&& pad.sY > p0.y && pad.sY < p1.y)
		{
			D3_2DBOX_FILL(&p0, &p1, D3_COL(128, 255, 0, 0));
			if(pad.pl && iBlinkTime == 0)
			{
				vecX0Y0	   = p0;
				vecX1Y1	   = p1;
				iBlinkTime = 60 * 4;
				ret        = i;
				//snd_play(SND_OK);
				iSelect    = i;
				break;
			}
		}
	}
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< メニュー描画 >>>
|	入力	w = メニューワーク
*------------------------------------------------------------------------------*/
void	MenuDraw(MENU_WORK *w)
{
	//--- 文字フォントを作成する ----------------------------------
	D3_ZWRITE(false);
	D3_ZREAD( false);
	float	fW = (float)(w->iStrMax * (d3.fontInfo[FONT].Width  + ADD_W));
	float	fH = (float)(w->iLine   * (d3.fontInfo[FONT].Height + ADD_H));
	MenuFrameDraw(&VEC3(w->fX      - 8, w->fY      - 8, 0),
				  &VEC3(w->fX + fW + 8, w->fY + fH + 8, 0));

	//--- 選択項目に枠を描画 --------------------------------------
	w->iAns = MenuBoxDraw(w, w->iLine, w->iStrMax);

	//--- メニューの項目を描画 ------------------------------------
	D3_ZREAD( false);
	D3_ZWRITE(false);
	D3_TEXTURE(0, TEX_MENU);
	D3_2DDRAW(&VEC3(w->fX, w->fY, 0), &VEC3(w->fX + 640, w->fY + 480, 0), 0, 0, 1, 1, D3_COL(255, 255, 255, 255));
	D3_ZREAD( true);
	D3_ZWRITE(true);
}



/*------------------------------------------------------------------------------*
| <<< メニューメイン >>>	※ここではメニューの基本処理を行います。
|	入力	w = メニューワーク
|	戻り値	-2 : キャンセル
|			-1 : なにも選択していない
|			0～: 選択したメニュー番号
*------------------------------------------------------------------------------*/
int		MenuMain(MENU_WORK *w)
{

	if(!w->bFirst)
	{
		if(pad.pl) return -1;
		w->bFirst = true;
	}

	//--- キャンセル時には -2、それ以外は指定の値を返す -----------
	if(pad.cr)
	{
		//snd_play(SND_CANCEL);
		return -2;
	}
	else
	{
		return w->iAns;
	}
}



/*------------------------------------------------------------------------------*
| <<< メニュー終了 >>>	※ここでは文字フォントを開放しています。
*------------------------------------------------------------------------------*/
void	MenuEnd(void)
{
	SAFE_RELEASE(d3.font[FONT]);
}



/*------------------------------------------------------------------------------*
| <<< メニュー初期化 >>>
*------------------------------------------------------------------------------*/
void	MenuInit(void)
{
	d3_offscreen_create(TEX_MENU, 512, 512);
	//d3.texInfo[TEX_MENU].bRepair = true;		// このスイッチを true にするとフルスクリーン切り替え時に画像が保持される
}



/*------------------------------------------------------------------------------*
| <<< メニュー作図 >>>
|	入力	str = メニューの文字列
*------------------------------------------------------------------------------*/
void	MenuMake(char *str)
{
	//--- 文字フォントを作成する ----------------------------------
	d3_font_create(FONT, 16, 40, "HG丸ｺﾞｼｯｸM-PRO", false);

	//--- メニューの項目を描画 ------------------------------------
	d3_offscreen_begin(TEX_MENU);
	d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3_COL(0, 0, 0, 0), 1.0f, 0);

	RECT	r = {0, 0, 0, 0};
//	d3.font[FONT]->DrawText(NULL, str, -1, &r, DT_NOCLIP, D3DCOL(255, 255, 255, 255));

	d3_offscreen_end(TEX_MENU);
}


#endif