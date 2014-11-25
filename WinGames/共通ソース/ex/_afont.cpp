//
//	アンチエイリアスフォント関係
//


//--- 定数 ----------------------------------------------------------------------------------------
#define	AFONT_W		16						// アンチフォントのデフォルトのサイズ
#define	AFONT_H		32


//--- マクロ --------------------------------------------------------------------------------------
#define	AFONT_ADR(id, x, y)	((y) * (uint)d3.texInfo[TEX_AFONT + id].fWidth + (x))



//--- 変数 ----------------------------------------------------------------------------------------
		AFONT_WORK	afont[AFONT_MAX];				// アンチフォント用




/*------------------------------------------------------------------------------*
| <<< アンチエイリアスフォントを開放 >>>
|	入力	iID = 管理番号
*------------------------------------------------------------------------------*/
void	afont_free(int iID)
{
	AFONT_WORK	*a = &afont[iID];

	//--- バッファは開放しておく ------------------------------------
	if(a->pOriStr != NULL)
	{
		free(a->pOriStr);
		a->pOriStr = NULL;
		a->pStr    = NULL;
	}
	if(a->pBuf != NULL)
	{
		free(a->pBuf);
		a->pBuf = NULL;
	}

	a->bUse  = false;
	a->bView = false;
	a->bInit = false;
	a->sSnd  = SND_NONE_;
	d3_offscreen_free(TEX_AFONT + iID);
}


/*------------------------------------------------------------------------------*
| <<< アンチエイリアスフォントを全て開放 >>>
*------------------------------------------------------------------------------*/
void	afont_free(void)
{
	for(int i = 0; i < AFONT_MAX; i++) afont_free(i);
}


/*------------------------------------------------------------------------------*
| <<< アンチエイリアスフォントのピクセル情報を得る >>>
|	入力	iID    = 管理番号
|			uiCode = 文字コード
|	戻り値	false  = 文字を得られなかった
*------------------------------------------------------------------------------*/
static	bool	AFontPixelGet(int iID, uint uiCode, TEXTMETRIC *tm)
{

	AFONT_WORK	*a = &afont[iID];

	//--- ビットマップ取得 ------------------------------------------
	MAT2	m   = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
	uint	fmt = GGO_GRAY8_BITMAP;			// ほかに　GGO_GRAY2_BITMAP、GGO_GRAY4_BITMAP がある

	//--- アンチフォントを取り出す ----------------------------------
	HDC		hDc = d3.font[a->cFontNum]->GetDC();

	int	iSize = GetGlyphOutline(hDc, uiCode, fmt, &a->gmAnt, 0, NULL, &m);
	if(iSize == 0){ return false;}
	GetGlyphOutline(hDc, uiCode, fmt, &a->gmAnt, iSize, a->pBuf, &m);
	GetTextMetrics( hDc, tm);

	//--- 文字の横幅を決める ----------------------------------------
	int		iW = d3.fontInfo[a->cFontNum].Width;
	int		iH = d3.fontInfo[a->cFontNum].Height;
	if(uiCode >= 0x100){ iW *= 2;}			// 全角ならば横幅は二倍になる

	return true;
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントをテクスチャーに直書き >>>
|	入力	iID      = 管理番号
|			buf      = VRAM のアドレス
|			usX, usY = 文字の描画座標
|			*str     = 文字(シフトJIS or Ascii)
*------------------------------------------------------------------------------*/
static	void	AFontDraw(int iID, int iTexNum, DWORD *buf, ushort usX, ushort usY, char *str)
{

	AFONT_WORK	*a = &afont[iID];

	int			iW     = d3.fontInfo[a->cFontNum].Width;
	int			iH     = d3.fontInfo[a->cFontNum].Height;
	uint		uiSize = (iW * 2) * (iH * 2);
	TEXTMETRIC	tm;

	//--- 文字コードよりアンチフォントを得る ------------------------
	// a->pBuf に文字の情報が返ってきます
	if(!ISKANJI(str[0]))					// 半角
	{
		AFontPixelGet(iID, *str, &tm);
	}
	else									// 全角
	{
		AFontPixelGet(iID, (((uchar)str[0] << 8 | (uchar)str[1])), &tm);
	}

	//--- VRAM に直で書き込み ---------------------------------------
	uchar	*p   = a->pBuf;

	uint	x, y;
	uint	h         = (uint)  a->gmAnt.gmBlackBoxY;
	uint	w         = (uint)((a->gmAnt.gmBlackBoxX + 3) & 0xfffffffc);
	uint	uiAdr     = 0;

	uint	uiTexSize = (uint)(d3.texInfo[iTexNum].fWidth * d3.texInfo[iTexNum/*TEX_AFONT + iID*/].fHeight);

    for(y = 0; y < h; y++)
	{
        for(x = 0; x < w; x++, uiAdr++)
		{
			// 65 階調で値が返ってくるので、適当に加工する
			int	c = p[uiAdr];
			if(c <= 1){ continue;}
			c = (c - 2) * 4 + 3;
			// 微妙な文字の位置を補正しながら描画
			uint	uiAdr2 = AFONT_ADR(iID,
								  usX + x +				   a->gmAnt.gmptGlyphOrigin.x,
								  usY + y + (tm.tmAscent - a->gmAnt.gmptGlyphOrigin.y));
			if(uiAdr2 >= 0 && uiAdr2 < uiTexSize)
			{
				int		a_ = (uint)(((float)((a->dwCol >> 24) &0xff) * (float)c) / 255.f);
				int		r_ = (uint)(((float)((a->dwCol >> 16) &0xff) * (float)c) / 255.f);
				int		g_ = (uint)(((float)((a->dwCol >>  8) &0xff) * (float)c) / 255.f);
				int		b_ = (uint)(((float)((a->dwCol >>  0) &0xff) * (float)c) / 255.f);

				DWORD	dwCol0 = D3_COL(a_, r_, g_, b_);
				DWORD	dwCol1 = D3_COL(a_,  1,  1,  1);

				buf[uiAdr2] = dwCol0;
				
				// 影をつけてみる
				buf[uiAdr2 + 1] = dwCol1;

				if(buf[uiAdr2 - 1] == 0){ buf[uiAdr2 - 1] = dwCol1;}

				int	h = (int)d3.texInfo[TEX_AFONT + iID].fWidth;

				if(((int)uiAdr2 - (int)h) >= 0 && buf[uiAdr2 - h] == 0){ buf[uiAdr2 - h] = dwCol1;}
				buf[uiAdr2 + h] = dwCol1;
			}
		}
    }
}



/*----------------------------------------------------------------------*
| <<< テキスト面をスクロール >>>
|	入力	iID    = 管理番号
*----------------------------------------------------------------------*/
#define	AFONT_SCROLL_SPEED	4				// 文字のスクロール速度
static	void	AfontScroll(int iID)
{

	AFONT_WORK	*a = &afont[iID];

	//--- カウンター計算 --------------------------------------------
	a->sScroll -= AFONT_SCROLL_SPEED;
	if(a->sScroll < 0) a->sScroll = 0;

	//--- スクロールアップ ------------------------------------------
	// レンダリング処理を使うと、3D 描画の際にうまく行かない
/*
	d3_offscreen_begin(TEX_AFONT + iID);
	D3_CLEAR_ZBUFFER();
	d3_stat_alpha_set(false);
	d3_stat_filter_set(D3DTEXF_POINT);				// この設定で文字がくっきり(ポイント)
	D3_TEXTURE(        TEX_AFONT + iID);
	
	float	v = d3.texInfo[TEX_AFONT + iID].fHeight - AFONT_SCROLL_SPEED;
	float	h = d3.texInfo[TEX_AFONT + iID].fHeight;
	float	u = d3.texInfo[TEX_AFONT + iID].fWidth;
	
	// 単純に１ドット下のテクスチャーを描画しているだけです。
	D3_2DDRAW(&VEC2(0, 0), &VEC2(u, v),
			  0,
			   AFONT_SCROLL_SPEED      / h,
			  1,
			  (AFONT_SCROLL_SPEED + v) / h,
			  D3_COL(255, 255, 255, 255));

	D3_2DBOX_FILL(&VEC2(0, v), &VEC2(u, h), D3_COL(255, 255, 0, 0));
	d3_offscreen_end(TEX_AFONT + iID);

	d3_stat_filter_set(D3DTEXF_LINEAR);
*/

	//--- こちら、CPU で転送 ----------------------------------------
	DWORD	*buf = d3_offscreen_capture_begin(TEX_AFONT + iID);

	uint	w = (uint)d3.texInfo[TEX_AFONT + iID].fWidth;
	uint	h = (uint)(a->usY1 -  a->usY0);//d3.texInfo[TEX_AFONT + iID].fHeight;
											// コピー
	memcpy(&buf[a->usY0 * w], &buf[(a->usY0 + AFONT_SCROLL_SPEED) * w], (h - AFONT_SCROLL_SPEED) * w * sizeof(DWORD));
											// 最下行はクリアー
	memset(&buf[(a->usY1 - AFONT_SCROLL_SPEED) * w], 0, AFONT_SCROLL_SPEED  * w * sizeof(DWORD));

	d3_offscreen_capture_end();
}



/*------------------------------------------------------------------------------*
| <<< 禁則文字チェック >>>
|	戻り値	true : 禁則文字
*------------------------------------------------------------------------------*/
static	bool	IsKinsokuCheck(uchar *str)
{

	if( ! ISKANJI(str[0])){ return false;}	// 半角？
	
	switch((str[0] << 8) | str[1])
	{
	case '、':
	case '。':
	case '」':
	case '》':
	case '』':
	case '？':
	case '）':
	case '！':
	case '“':
	case '”':
		return true;
	}
	return false;
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントで文字表示 >>>
|	入力	iID    = 管理番号
|			iX, iY = 文字の表示位置
|			Format = printf と同じフォーマット
*------------------------------------------------------------------------------*/
void	afont_printf(int iID, int iX, int iY, char* Format,...)
{

	AFONT_WORK	*a = &afont[iID];

	//--- 初期化 ----------------------------------------------------
	if(!a->bInit)
	{
		a->bInit = true;
		int	iW   = d3.fontInfo[a->cFontNum].Width * 2;
		int	iH   = d3.fontInfo[a->cFontNum].Height;
	
		a->vecP0      = VEC2(                 0,                  0);
		a->vecP1      = VEC2((float)d3.sRenderW, (float)d3.sRenderH);
		a->fSpeed     = 0.0f;				// 即時
		a->fCount     = 0;
											// バッファは文字のサイズ * 4 で確保しておく
		a->pBuf       = (uchar*)malloc((iW * 2) * (iH * 2));
		a->usX0       = 0;
		a->usY0       = 0;
		a->usX1       = d3.sRenderW;
		a->usY1       = d3.sRenderH;
		//a->sOfsW      = 0;				// 文字幅の加算値
		//a->sOfsH      = 0;				// 文字幅の加算値
		a->sSnd		  = SND_NONE_;
		a->bUse       = true;
		a->dwCol	  = D3_COL_WHITE(255);
		d3_offscreen_create(TEX_AFONT + iID, a->usX1, a->usY1, D3DFMT_A8R8G8B8);
		d3.texInfo[TEX_AFONT + iID].bRepair = true;
	}

	//--- キー待ち --------------------------------------------------
	if(a->bKey)
	{
		if(pad.bLeftClick){ a->bKey = false;}
		return;
	}

	//--- 文字列作成 ------------------------------------------------
	char	str[256];
	va_list	ap;
	va_start(ap,  Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	//--- 文字が前回と同じならばなにもしないで帰る ------------------
	if(a->pOriStr != NULL)					// 調べるのはメモリが確保されているときだけで良い
	{
		if(strcmp(a->pOriStr, str) == 0)
		{
			///if(strlen(str) == (uint)a->fCount) return;
			if(*a->pStr == NULL){ return;}
											// 文字の速度をみる(fSpeed が 0 なら即時)
			if(a->fSpeed > 0 && a->sScroll == 0)
			{
				a->fCount += a->fSpeed;
				//if(a->usCountBak == (int)a->fCount){ return;}
				//a->usCountBak = (int)a->fCount;
				///snd_play(a->sSnd);
			}
		}
		else
		{
			a->fCount     = 0;
			free(a->pOriStr);
			a->pOriStr    = NULL;
		}
	}
	if(a->pOriStr == NULL)					// NULL の時のみ文字列をコピー
	{
		a->pOriStr = (char*)malloc(strlen(str) + 1);
		strcpy(a->pOriStr, str);
		a->pStr    = a->pOriStr;			// pStr はワーク用

		// クリア
		d3_offscreen_begin(TEX_AFONT + iID);
		d3_clear(		   D3_COL(0, 0, 0, 0));
		d3_offscreen_end(  TEX_AFONT + iID);

		// 文字位置
		a->usX     = iX + a->usX0;
		a->usY     = iY + a->usY0;
		a->sScroll = 0;

		// スイッチ
		a->bView = true;
		//return;								// 次の周期より描画
	}

	//--- スクロール ------------------------------------------------
	if(a->sScroll > 0)
	{
		AfontScroll(iID);
		return;
	}

	//--- オフスクリーンキャプチャー開始 ----------------------------
	DWORD	*buf = d3_offscreen_capture_begin(TEX_AFONT + iID);

	//--- 文字書き込み ----------------------------------------------
	for(;;)
	{
		if(*a->pStr != NULL)
		{
			//--- このり表示文字数計算 ------------------------------
			if(a->fSpeed > 0)
			{
				int	iCount = (int)a->fCount - (a->pStr - a->pOriStr);
				if(ISKANJI(*a->pStr) && iCount < 2)
				{
					break;
				}
				else if(iCount < 1)
				{
					break;
				}
			}
			//--- 特定の文字列をチェック ----------------------------
			// キーウエイト
			a->bKey = strncmp(a->pStr, "@key", 4) == 0;
			if(a->bKey){ a->pStr += 4; break;}
					
			// 改行
			bool	bKai = strncmp(a->pStr, "\r\n", 2) == 0 || strncmp(a->pStr, "@n", 2) == 0;

			// 色
			uint	uiA, uiR, uiG, uiB;
			char	*s;
			if(strncmp(a->pStr, "@c", 2) == 0)
			{
				a->pStr += 2;
				char	buf[256];
				strncpy(buf, a->pStr, sizeof(buf));
				buf[255] = NULL;

				char	*token  = " ,()\r\n";
				s		 = strtok(buf,  token);	// ( を読み飛ばす
				uiA		 = atoi(s);
				s		 = strtok(NULL, token);	// red
				uiR		 = atoi(s);
				s		 = strtok(NULL, token);	// gree
				uiG		 = atoi(s);
				s		 = strtok(NULL, token);	// blue
				uiB		 = atoi(s);
				s		 = strtok(NULL, token);	// ) を読み飛ばす
				a->pStr += (s - buf);			// 文字を進める
				a->dwCol = D3_COL(uiA, uiR, uiG, uiB);
			}

			//-------------------------------------------------------

			// 描画
			if(!bKai)
			{
				AFontDraw(iID, TEX_AFONT + iID, buf, a->usX, a->usY, a->pStr);
				snd_play(a->sSnd);
			}

			// 文字を進める
			if(ISKANJI(*a->pStr) || bKai){ a->pStr += 2;}
			else						 { a->pStr += 1;}

			// 文字を進める
			if(!bKai){ a->usX += a->gmAnt.gmCellIncX + a->sOfsW;}

			// 終了チェック
			if(*a->pStr == NULL)
			{
				// 文字の表示終了位置を確定しておく
				a->usY += d3.fontInfo[a->cFontNum].Height + a->sOfsH;
				break;
			}

			// 改行チェック
			if((!IsKinsokuCheck((uchar*)a->pStr) && (a->usX + d3.fontInfo[a->cFontNum].Width * 2) > a->usX1) || bKai)
			{
				ushort	h = d3.fontInfo[a->cFontNum].Height + a->sOfsH;

				a->usX  = a->usX0 + iX;
				a->usY += h;
	
				// スクロールアップを調べる
				if(a->usY + h > a->usY1)
				{
					a->usY	  -= h;
					a->sScroll = (short)h;
				}
				break;
			}
		}
		else
		{
			break;
		}

	}

	//--- オフスクリーンキャプチャー完了 ----------------------------
	d3_offscreen_capture_end();

	//--- 表示文字数計算 --------------------------------------------
					//if(a->usCountBak == (int)a->fCount){ return;}
	//a->usCountBak = (int)a->fCount;
}



/*------------------------------------------------------------------------------*
| <<< アンチフォントで文字表示 >>>
|	入力	iID    = 管理番号
|			Format = printf と同じフォーマット
*------------------------------------------------------------------------------*/
void	afont_printf(int iID, char* Format,...)
{
	//--- 文字列作成 ------------------------------------------------
	char	str[256];
	va_list	ap;
	va_start(ap,  Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	//---
	afont_printf(iID, 0, 0, str);
}
void	afont_printf(char* Format,...)
{
	//--- 文字列作成 ------------------------------------------------
	char	str[256];
	va_list	ap;
	va_start(ap,  Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	afont_printf(0, 0, 0, str);
}



/*------------------------------------------------------------------------------*
| <<< 指定のテクスチャーへアンチフォントをレンダリング >>>
|	入力	iTex     = テクスチャー番号
|			iFontNum = フォント番号
|			iX, iY   = 文字の表示位置
|			iLen     = 表示文字数(-1 で全て)
|			Format   = printf と同じフォーマット
|	※ d3_font_draw と互換関数です
*------------------------------------------------------------------------------*/
void	afont_render(int iTex, int iFontNum, int iX, int iY, int iLen, DWORD dwCol, char* Format,...)
{

	//--- 文字作成 --------------------------------------------------
	int		iBufSize = 1024;
	char	*strBuf  = (char*)malloc(iBufSize);
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//---------------------------------------------------------------
		
	if(iLen != -1) strBuf[iLen] = NULL;

	//--- オフスクリーンキャプチャー開始 ----------------------------
	DWORD	*buf = d3_offscreen_capture_begin(iTex);

	//--- 文字書き込み ----------------------------------------------
	///Printf("Draw");
	
	AFONT_WORK	bak   =  afont[0];
	AFONT_WORK	*a    = &afont[0];
	char			*pStr = strBuf;			// 文字列
	int				x     = iX;
	int				y     = iY;

	a->cFontNum			  = iFontNum;

	int				iW    = d3.fontInfo[a->cFontNum].Width * 2;
	int				iH    = d3.fontInfo[a->cFontNum].Height;
											// バッファ確保
	a->pBuf				  = (uchar*)malloc((iW * 2) * (iH * 2));
	memset(a->pBuf, 0, (iW * 2) * (iH * 2));
	//a->pBuf				  = (uchar*)strBuf;

	for(;;)
	{
		if(*pStr != NULL)
		{
			//--- 特定の文字列をチェック ----------------------------
					
			// 改行
			bool	bKai = strncmp(pStr, "\r\n", 2) == 0;
			//-------------------------------------------------------

			// 描画
			if(!bKai){ AFontDraw(0, iTex, buf, x, y, pStr);}

			// 文字を進める
			if(ISKANJI(*pStr) || bKai){ pStr += 2;}
			else					  { pStr += 1;}

			if(pStr == NULL){ break;}

			// 文字を進める
			if(!bKai){ x += a->gmAnt.gmCellIncX;}
			
			// 改行チェック
			if((x + d3.fontInfo[iFontNum].Width * 2) > (uint)d3.texInfo[iTex].fWidth || bKai)
			{
				x  = iX;
				y += d3.fontInfo[iFontNum].Height;
			}
		}
		else
		{
			break;
		}

	}

	//--- オフスクリーンキャプチャー完了 ----------------------------
	d3_offscreen_capture_end();

	free(a->pBuf);
	afont[0] = bak;
	free(strBuf);
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントで文字表示の表示スイッチ >>>
|	入力	iID    = 管理番号
*------------------------------------------------------------------------------*/
void	afont_print_sw(int iID, bool bSw)
{
	AFONT_WORK	*a = &afont[iID];

	a->bView = bSw;
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントで一括しての表示スイッチ >>>
|	入力	iID    = 管理番号
*------------------------------------------------------------------------------*/
void	afont_print_sw(bool bSw)
{
	for(int i = 0; i < AFONT_MAX; i++)
	{
		AFONT_WORK	*a = &afont[i];
		a->bView = false;
	}
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントで使用する文字番号を設定 >>>
|	入力	iID      = 管理番号
|			iFontNum = フォント番号(d3_font_create で作成したフォント)
*------------------------------------------------------------------------------*/
void	afont_font_change(int iID, int iFontNum)
{
	afont_free(iID);

	AFONT_WORK	*a = &afont[iID];
	a->cFontNum = iFontNum;
}


/*------------------------------------------------------------------------------*
| <<< アンチフォント初期化 >>>
|	入力	iID                = 管理番号
|			iFontNum           = フォント番号(d3_font_create で作成したフォント)
|			iRenderW, iRenderH = レンダリング面のサイズ
*------------------------------------------------------------------------------*/
void	afont_create(int iID, int iFontNum, int iRenderW, int iRenderH)
{

	afont_free(iID);						// バッファ開放

	AFONT_WORK	*a = &afont[iID];

	a->cFontNum   = iFontNum;
	a->bInit	  = true;
	int	iW	 	  = d3.fontInfo[a->cFontNum].Width * 2;
	int	iH		  = d3.fontInfo[a->cFontNum].Height;

	a->usX0       = 0;
	a->usY0       = 0;
	a->usX1       = iRenderW;
	a->usY1       = iRenderH;
	a->sOfsW      = 0;						// 文字幅の加算値
	a->sOfsH      = 0;
	a->vecP0      = VEC2(              0,               0);
	a->vecP1      = VEC2((float)iRenderW, (float)iRenderH);
	a->fSpeed     = 0.0f;					// 即時
	a->fCount     = 0;
	a->sSnd		  = SND_NONE_;
	a->bUse       = true;
	a->dwCol	  = D3_COL_WHITE(255);
											// バッファは文字のサイズ * 4 で確保しておく
	a->pBuf       = (uchar*)malloc((iW * 2) * (iH * 2));
	d3_offscreen_create(TEX_AFONT + iID, a->usX1, a->usY1, D3DFMT_A8R8G8B8);
	d3.texInfo[TEX_AFONT + iID].bRepair = true;
}

void	afont_create(int iFontNum, int iRenderW, int iRenderH)
{
	afont_create(0, iFontNum, iRenderW, iRenderH);
}



/*------------------------------------------------------------------------------*
| <<< アンチフォントの文字速度設定 >>>
|	入力	iID    = 管理番号
|			fSpeed = 0.0f ～ 1.0f(0.0f で即時)
*------------------------------------------------------------------------------*/
void	afont_speed_set(int iID, float fSpeed)
{
	AFONT_WORK	*a = &afont[iID];

	a->fSpeed = fSpeed;
}



/*------------------------------------------------------------------------------*
| <<< アンチフォントの文字書き出し座標設定 >>>
|	入力	iID    = 管理番号
|			r      = 文字の書き出し位置
*------------------------------------------------------------------------------*/
void	afont_rect_set(int iID, RECT *r)
{
	AFONT_WORK	*a = &afont[iID];

	a->usX0  = (ushort)r->left;
	a->usY0  = (ushort)r->top;
	a->usX1  = (ushort)r->right;
	a->usY1  = (ushort)r->bottom;
}



/*------------------------------------------------------------------------------*
| <<< 文字送りサイズ設定 >>>	※フォントのサイズではなく、文字幅を変える
|	入力	iID    = 管理番号
|			iW, iH = 文字送り値(文字の間隔調整用)
*------------------------------------------------------------------------------*/
void	afont_font_size_set(int iID, int iW, int iH)
{
	AFONT_WORK	*a = &afont[iID];
	a->sOfsW = iW - (short)d3.fontInfo[a->cFontNum].Width * 2;
	a->sOfsH = iH - (short)d3.fontInfo[a->cFontNum].Height;
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントの文字書き出し座標設定 >>>
|	入力	iID    = 管理番号
|			iSound = 効果音番号
*------------------------------------------------------------------------------*/
void	afont_sound_set(int iID, int iSound)
{
	AFONT_WORK	*a = &afont[iID];
	a->sSnd = iSound;
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントの文字が表示終了しているか調べる >>>
|	入力	iID  = 管理番号
|	戻り値	true  : 文字描画中
|			false : 終了している
*------------------------------------------------------------------------------*/
bool	afont_print_check(int iID)
{
	AFONT_WORK	*a = &afont[iID];
	
	if( a->pStr == NULL){ return false;}
	if(*a->pStr == NULL){ return false;}
	else				{ return true;}
}


/*------------------------------------------------------------------------------*
| <<< アンチフォントで文字表示 >>>
|	入力	fX, fY = 描画位置
|			fAlp   = 半透明レート
|			bZoom  = true : ウィンドウサイズに合わせて拡大する
*------------------------------------------------------------------------------*/
void	afont_draw(float fX, float fY, float fAlp, bool bZoom)
{

	d3_stat_blend_set(D3_BLEND_NORMAL);				// 半透明設定
	d3_stat_alpha_set(true);
	d3_stat_alpha_greate_set(1);

//	d3_stat_filter_set(D3DTEXF_LINEAR);				// この設定で文字が滑らか(  リニアー)
//	d3_stat_filter_set(D3DTEXF_POINT );				// この設定で文字がくっきり(ポイント)

	AFONT_WORK	*a = afont;
	for(int i = 0; i < AFONT_MAX; i++, a++)
	{
		if(!a->bUse || !a->bView) continue;

		D3_TEXTURE(TEX_AFONT + i);
		float	w = a->vecP1.x - a->vecP0.x;
		float	h = a->vecP1.y - a->vecP0.y;
		VEC2	v0 = a->vecP0;
		VEC2	v1 = a->vecP1;
		// 画面にあわせてズーム
		if(bZoom)
		{
			v0.x *= (float)d3.sRenderW / (float)__SCR_W;
			v0.y *= (float)d3.sRenderH / (float)__SCR_H;
			v1.x *= (float)d3.sRenderW / (float)__SCR_W;
			v1.y *= (float)d3.sRenderH / (float)__SCR_H;
		}
		D3_2DDRAW(&VEC2(v0.x + fX, v0.y + fY),
				  &VEC2(v1.x + fX, v1.y + fY),
				  0,
				  0,
				  (w / d3.texInfo[TEX_AFONT + i].fWidth),
				  (h / d3.texInfo[TEX_AFONT + i].fHeight),
				  D3_COL((int)(fAlp * 255.f), 255, 255, 255));
	}
}
void	afont_draw(bool fZoom)
{
	afont_draw(0, 0, 1, fZoom);
}
void	afont_draw(void)
{
	afont_draw(false);
}
void	afont_draw(float fX, float fY)
{
	afont_draw(fX, fY, 1, false);
}


