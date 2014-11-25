//
//	アンチエイリアスフォントヘッダ
//

#ifndef	__AFONT_H

#define	__AFONT_H


#define	AFONT_MAX		16					// アンチフォント総数



//--- アンチフォント用 ----------------------------------------------------------------------------
struct	AFONT_WORK							// アンチフォントのワーク
{
	bool			bView;						// 描画スイッチ
	bool			bUse;						// 使用フラグ(レンダリング面を作ったの判定に使用)
	bool			bInit;						// 初期化フラグ
	bool			bKey;						// キーウエイト待ち
	char			cFontNum;					// 使用フォント番号
	ushort			usX0,  usY0;				// 文字の書き出し初期座標
	ushort			usX1,  usY1;

	ushort			usX,   usY;					// 現在の文字描画位置

	short			sOfsW, sOfsH;				// 文字送り値(微調整よう)
	GLYPHMETRICS	gmAnt;
	VEC2			vecP0, vecP1;				// 表示座標
	DWORD			dwCol;						// 文字色
	uchar			*pBuf;						// フォント格納用のバッファ
	char			*pOriStr;					// 文字列格納用
	char			*pStr;						// pOriStr のワークよう。文字列の描画はこちらで行う
	float			fCount;						// 文字描画のカウンター
	float			fSpeed;						// 文字列の描画速度(0.0fならば即時)

	short			sScroll;					// 文字のスクロール用
	short			sSnd;						// 文字表示時の効果音(SND_NULLで無音)
	float			fA, fR, fG, fB;				// 文字色
};


//--- 外部参照宣言 --------------------------------------------------------------------------------
extern	void	afont_free(int iID);
extern	void	afont_free(void);

extern	void	afont_printf(int iID, int iX, int iY, char* Format,...);					// アンチフォントで文字表示
extern	void	afont_printf(int iID,                 char* Format,...);
extern	void	afont_printf(                         char* Format,...);

extern	void	afont_render(int iTex, int iID, int iX, int iY, int iLen, DWORD dwCol, char* Format,...);
extern	void	afont_draw(float fX, float fY, float fAlp, bool bZoom);
extern	void	afont_draw(float fX, float fY, bool bZoom);
extern	void	afont_draw(bool fZoom);
extern	void	afont_draw(void);
extern	void	afont_draw(float fX, float fY);

extern	void	afont_create(int iID, int iFontNum, int iRenderW, int iRenderH);			// アンチフォント初期化
extern	void	afont_create(		 int iFontNum, int iRenderW, int iRenderH);
extern	void	afont_speed_set(int iID, float fSpeed);
extern	void	afont_rect_set(int iID, RECT *r);

/*------------------------------------------------------------------------------*
| <<< 文字送りサイズ設定 >>>	※フォントのサイズではなく、文字幅を変える
|	入力	iID    = 管理番号
|			iW, iH = 文字送り値(文字の間隔調整用)
*------------------------------------------------------------------------------*/
extern	void	afont_font_size_set(int iID, int iW, int iH);

extern	void	afont_sound_set(int iID, int iSound);
extern	bool	afont_print_check(int iID);
extern	void	afont_print_sw(int iID, bool bSw);
extern	void	afont_print_sw(bool bSw);


#endif					// end _afont.h


