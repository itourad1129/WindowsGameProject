//
// フォントをテクスチャーへ展開
//


//#include	"main.h"


#undef	ADR_
#define	ADR_(x, y)		((y) * (DWORD)d3.texInfo[iTex].fWidth + (x))


static	int	iNextX, iNextY;					// 次回描画座標



/*------------------------------------------------------------------------------*
| <<< テクスチャーにアウトラインフォントを作成 >>>
|	入力	iTex        = 文字を作成するテクスチャー番号
|			iX, iY		= 文字の表示座標
|			strFontName = TrueType フォント名
|			str		    = 描画文字
|			iHeight     = フォント高
|			iWidth      = フォントの横幅
|			bItalic		= TRUE: 斜体
|			iPenSize    = アウトラインの太さ(ピクセル)
|			dwEdge      = アウトラインの色
|			dwFill      = 塗りつぶし色
|			iQuality    = アンチエイリアス精度( 1 以上の値にすること)
*------------------------------------------------------------------------------*/
void	font_to_texture(int iTex, int iX, int iY, char *strFontName, int iWidth, int iHeight, BOOL bItalic, int iPenSize, DWORD dwEdge, DWORD dwFill, int iQuality, char* Format,...)
{

	//--- 文字列作成 ------------------------------------------------
	char	str[512];
	va_list	ap;
	va_start(ap, Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	int	iXBak = iX;
	iPenSize *= iQuality;						// クオリティに合わせてペン幅変更
   
   //--- 色を分解 --------------------------------------------------
	uchar	aucEdge[4], aucFill[4];
												// 輪郭
	aucEdge[0] = (dwEdge >> 24) & 255;				// a
	aucEdge[1] = (dwEdge >> 16) & 255;				// r
	aucEdge[2] = (dwEdge >>  8) & 255;				// g
	aucEdge[3] =  dwEdge        & 255;				// b
												// 塗りつぶし色
	aucFill[0] = (dwFill >> 24) & 255;				// a
	aucFill[1] = (dwFill >> 16) & 255;				// r
	aucFill[2] = (dwFill >>  8) & 255;				// g
	aucFill[3] =  dwFill        & 255;				// b

	//--- デバイスコンテキスト作成 ----------------------------------
	HDC	hDC    = GetDC(sys.hWnd);
	HDC	hdcMem = CreateCompatibleDC(hDC);
	ReleaseDC(sys.hWnd, hDC);					// hdcMem があれば不要なので、解放

	//--- フォント作成 ----------------------------------------------
	LOGFONTA	lf;
	lf.lfHeight		    = iHeight * iQuality;					// フォントの高さ
	lf.lfWeight		    = 1000;									// フォントの太さ
	lf.lfWidth		    = iWidth  * iQuality;					// 平均文字幅( 0 でデフォルト値)
	lf.lfEscapement	    = 0;									// 文字送り方向の角度
	lf.lfOrientation    = 0;									// ベースラインの角度
	lf.lfItalic		    = bItalic;								// 斜体
	lf.lfUnderline	    = FALSE;								// 下線
	lf.lfStrikeOut      = FALSE;								// 取り消し線
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;					// 出力の精度
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;					// クリッピング精度
	lf.lfCharSet	    = SHIFTJIS_CHARSET;						// 文字セットの識別子
																// ピッチとファミリ
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;			// ピッチ : DEFAULT_PITCH(ﾃﾞﾌｫ)、FIXED_PITCH(固定幅)、VARIABLE_PITCH(可変幅)
																// ファミリ : FF_DECORATIVE(装飾つき)、FF_DONTCARE(ﾃﾞﾌｫ)、FF_ROMAN(プロポーショナルフォント)、FF_SCRIPT(手書き風フォント)、FF_SWISS(プロポーショナルフォント)
	lf.lfQuality		= ANTIALIASED_QUALITY;					// 出力品質 : ANTIALIASED_QUALITY(アンチエイリアス)、DEFAULT_QUALITY(ﾃﾞﾌｫ)、CLEARTYPE_QUALITY(クリアタイプ)、NONANTIALIASED_QUALITY(アンチなし)
	//memcpy(lf.lfFaceName, "ＭＳ 明朝", strlen("ＭＳ 明朝"));	// フォント名
	strcpy(lf.lfFaceName, strFontName); 
	HFONT	hFont	    = CreateFontIndirectA(&lf);				// フォント作成
	HFONT	hOldFont	= (HFONT)SelectObject(hdcMem, hFont);	// 新規にフォント設定

	//--- 文字を取得 ------------------------------------------------
	int	iCount = 0;									// 文字数
	while(str[iCount] != NULL)
	{
		TEXTMETRICA		tm;
		GLYPHMETRICS	gm;
		MAT2	mat  = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
		int		iLen = IsDBCSLeadByte(str[iCount]) ? 2 : 1;
		UINT	code = (iLen == 2 ? ((uchar)str[iCount] << 8) | (uchar)str[iCount + 1] : (uchar)str[iCount]);
		GetTextMetricsA( hdcMem, &tm);
		GetGlyphOutlineA(hdcMem, code, GGO_METRICS, &gm, 0, 0, &mat);

		RECT	rectFontRegion =					// フォントの使用する領域
		{
						  gm.gmptGlyphOrigin.x					- iPenSize / 2,
			tm.tmAscent - gm.gmptGlyphOrigin.y					- iPenSize / 2,
						  gm.gmptGlyphOrigin.x + gm.gmBlackBoxX + iPenSize / 2,
			tm.tmAscent - gm.gmptGlyphOrigin.y + gm.gmBlackBoxY + iPenSize / 2
		};
		RECT	rectFontSize =						// フォントサイズ
		{
			0,
			0,
			(gm.gmBlackBoxX + iPenSize + iQuality - 1) / iQuality * iQuality,
			(gm.gmBlackBoxY + iPenSize + iQuality - 1) / iQuality * iQuality
		};

		BITMAPINFO	bitBmp;
		memset(&bitBmp, 0, sizeof(bitBmp));
		bitBmp.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		bitBmp.bmiHeader.biWidth    =  rectFontSize.right;
		bitBmp.bmiHeader.biHeight   = -rectFontSize.bottom;
		bitBmp.bmiHeader.biPlanes   = 1;
		bitBmp.bmiHeader.biBitCount = 24;
		uchar	*p					= 0;
		HBITMAP	hBitMap				= CreateDIBSection(0, &bitBmp, DIB_RGB_COLORS, (void**)&p, 0, 0);
		HBITMAP	hOldBmp				= (HBITMAP)SelectObject(hdcMem, hBitMap);

		// フォントの背景を初期化(青で塗りつぶし)
		HBRUSH	hBg = (HBRUSH)CreateSolidBrush(RGB(0, 0, 255));
		FillRect(hdcMem, &rectFontSize, hBg);
		DeleteObject(hBg);

		// フォントを描く(色は緑、塗は赤)
		HPEN	hPen, hOldPen;
		if(iPenSize > 0)
		{
			hPen    = (HPEN)CreatePen(PS_SOLID, iPenSize, RGB(0, 255, 0));
			hOldPen = (HPEN)SelectObject(hdcMem, hPen);
		}
		HBRUSH	hBrush    = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
		HBRUSH	hOldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);
		SetBkMode(		  hdcMem, TRANSPARENT);
		BeginPath(		  hdcMem);
		TextOutA(		  hdcMem, -rectFontRegion.left, -rectFontRegion.top, &str[iCount], iLen);
		EndPath(		  hdcMem);
		StrokeAndFillPath(hdcMem);					// フォントを描き終わったので元の設定に戻す
		SelectObject(hdcMem, hOldBrush);
		SelectObject(hdcMem, hOldBmp);
		DeleteObject(hBrush);

		if(iPenSize > 0)
		{
			SelectObject(hdcMem, hOldPen);
			DeleteObject(hPen);
		}
		//--- BMP で色分けされたフォントをテクスチャーへ展開 ------------
		int	iW		  = rectFontSize.right  / iQuality;
		int iH		  = rectFontSize.bottom / iQuality;
		int iQuality2 = iQuality * iQuality;

		// 改行チェック
		if(iX + ((gm.gmCellIncX + iPenSize) / iQuality) >= d3.texInfo[iTex].fOriWidth)
		{
			iX      = iXBak;
			iY     += iHeight + 8;
			if(iY >= (d3.texInfo[iTex].fHeight - iHeight * 4) * iQuality)
			{
				break;
			}
		}

		D3DLOCKED_RECT	rectLock;					// テクスチャーをロックしてからデータを書き込む
		if(SUCCEEDED(d3.tex[iTex]->LockRect(0, &rectLock, 0, 0)))
		{
			int	iXOfs = (			   gm.gmptGlyphOrigin.x - iPenSize / 2) / (iQuality);
			int	iYOfs = (tm.tmAscent - gm.gmptGlyphOrigin.y - iPenSize / 2) / (iQuality);

			uint	uiSrcPitch = (rectFontSize.right * 3 + 3) / 4 * 4;
			
			// API で描いた文字を文字を平均化しながらテクスチャーへ展開
			DWORD	*pdwD = (DWORD *)rectLock.pBits;
			for(int y = 0; y < iH; y++)
			{
				for(int x = 0; x < iW; x++)
				{
					uint	uiAlph = 0;
					uint	uiEdge = 0;
					uint	uiFill = 0;

					// 平均化(ここでフォントが滑らかになります)
					for(int yy = 0; yy < iQuality; yy++)
					{
						for(int xx = 0; xx < iQuality; xx++)
						{
							uiAlph += p[(y * iQuality + yy) * uiSrcPitch + (x * iQuality + xx) * 3 + 0];
							uiEdge += p[(y * iQuality + yy) * uiSrcPitch + (x * iQuality + xx) * 3 + 1];
							uiFill += p[(y * iQuality + yy) * uiSrcPitch + (x * iQuality + xx) * 3 + 2];
						}
					}
					uiAlph /= iQuality2;
					uiEdge /= iQuality2;
					uiFill /= iQuality2;

					// 透過度がある場合はエッジ色を採用、不透明の場合はブレンド色を採用
					uint	a = 0xff - uiAlph;
					if(a < 0xff)
					{
						// 半透明
						a	   = (a * aucEdge[0]) >> 8;
						pdwD[ADR_(iX + iXOfs + x, iY + iYOfs + y)] |= D3_COL(a, aucEdge[1], aucEdge[2], aucEdge[3]);
					}
					else
					{
						// 不透明
						uint	r, g, b;
						r = ((aucEdge[1] * uiEdge) >> 8) + ((aucFill[1] * uiFill) >> 8);
						g = ((aucEdge[2] * uiEdge) >> 8) + ((aucFill[2] * uiFill) >> 8);
						b = ((aucEdge[3] * uiEdge) >> 8) + ((aucFill[3] * uiFill) >> 8);
						a = ((aucEdge[0] * uiEdge) >> 8) + ((aucFill[0] * uiFill) >> 8);
						if(r > 0 || g > 0 || b > 0)
						{
							pdwD[ADR_(iX + iXOfs + x + 0, iY + iYOfs + y + 0)]  = D3_COL(  a, r, g, b);
							pdwD[ADR_(iX + iXOfs + x + 1, iY + iYOfs + y + 1)] |= D3_COL(255, 0, 0, 0);
						}
					}
				}
			}
			// テクスチャーのロックを解除
			d3.tex[iTex]->UnlockRect(0);
		}

		// 不要なフォントを削除
		DeleteObject(hBitMap);

		// 次へ進める
		iX     += (gm.gmCellIncX + iPenSize) / iQuality;
		iCount += iLen;
	}

	// 次回描画座標
	iNextX = iX;
	iNextY = iY + iHeight;

	// 終了処理
	SelectObject(hdcMem, hOldFont);
	DeleteObject(hFont);

	// デバイスコンテキスト解放
	ReleaseDC(sys.hWnd, hdcMem);
}



/*------------------------------------------------------------------------------*
| <<< テクスチャーをクリア >>>
|	入力	iTex = クリアするテクスチャー
*------------------------------------------------------------------------------*/
void	font_to_texture_clear(int iTex)
{
	if(d3.tex[iTex] != NULL)
	{
		D3DLOCKED_RECT	rectLock;				// テクスチャーをロックしてからデータを書き込む
		if(SUCCEEDED(d3.tex[iTex]->LockRect(0, &rectLock, 0, 0)))
		{
			DWORD	*d = (DWORD *)rectLock.pBits;
			for(int i = 0; i < (int)(d3.texInfo[iTex].fOriWidth * d3.texInfo[iTex].fHeight); i++)
			{
				d[i] = 0;
			}

			// テクスチャーのロックを解除
			d3.tex[iTex]->UnlockRect(0);
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< 描画終了座標を得る >>>
|	戻り値	描画終了座標 X
*------------------------------------------------------------------------------*/
int		font_to_texture_get_next_x(void)
{
	return iNextX;
}



/*------------------------------------------------------------------------------*
| <<< 描画終了座標を得る >>>
|	戻り値	描画終了座標 Y
*------------------------------------------------------------------------------*/
int		font_to_texture_get_next_y(void)
{
	return iNextY;
}

