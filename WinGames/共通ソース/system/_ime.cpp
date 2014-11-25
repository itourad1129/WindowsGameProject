//
//	ＩＭＥ処理
//


//#include	"..\\main.h"
#include	"WINNLS32.H"
#include	"mbstring.h"
#include	"mbctype.h"

#define	__STR_NUMBER	4						// d3_printf で使用する ID	番号
												// + 5 までぐらいは使われる
#define	__SE_OK			0						// 決定音
#define	__SE_CANCEL		0						// キャンセル音
#define	__SE_CLICK		0						// キークリック音
#define	__SE_BACK		0						// BSキー音
							
#define	__FONT_WIDTH	35.0f					// 文字の横幅
#define	__FONT_WIDTH2	(__FONT_WIDTH /	2.f)	// 文字の横幅（半角）
#define	__FONT_HEIGHT	d3.fontInfo[1].Height	// 文字の縦幅

#define	CHARBUFFER_VOL	1024					// 文字コードバッファ最大容量
#define	STRCOLOR1		D3DCOLOR_ARGB(255,	 0,	 64,  64)
#define	STRCOLOR2		D3DCOLOR_ARGB(255, 255,	255, 255)
#define	STRCOLOR3		D3DCOLOR_ARGB(128, 255,	255, 255)	// 変換候補の縁
#define	STRCOLOR4		D3DCOLOR_ARGB(255,	 0,	255, 255)
#define	STRCOLOR5		D3DCOLOR_ARGB(255, 255,	255, 255)
#define	STRCOLOR6		D3DCOLOR_ARGB(255, 255,	 50,  50)
#define	STRCOLOR7		D3DCOLOR_ARGB(255, 255,	255,  20)
#define	STRCOLOR8		D3DCOLOR_ARGB(128,	 0,	  0, 100)	// 変換候補のバック

struct	IME_WORK							// IME ワーク
{
		int				input_sw;						// 入力モード
		char			CharBuffer[	CHARBUFFER_VOL + 1];// 入力された文字列
		int				StPoint, EdPoint;				// リングバッファ用ポインタ
		int				Point, StrLen;					// 文字列入力位置、文字列の長さ
		char			InputString[CHARBUFFER_VOL + 1];
		int				ClauseData[	256];				// 文節データ
		int				ClauseNum;						// 文節数
		int				InputPoint;						// 入力中の編集文字列上の位置
		int				IMEUseFlag;						// ＩＭＥの使用状態
		int				IMESwitch;						// ＩＭＥの起動状態
		int				ChangeFlag;						// ＩＭＥ入力に変化があったかフラグ
		int				IMEInputFlag;					// 入力処理中か、フラグ
		PCANDIDATELIST	CandidateList;					// 変換候補のリストデータ
		char			str_bak[CHARBUFFER_VOL];		// 入力文字のバックアップ用
};


static	IME_WORK	ime	={-2};



/*------------------------------------------------------------------------------*
| <<< 全角文字、半角文字入り乱れる中から指定の全半混在文字数での半角文字数を得る >>>
*------------------------------------------------------------------------------*/
static	int	GetStringPoint(char	*String, int Point)
{
	int		i, p = 0;

	for(i =	0; i < Point; i	++)
	{
		if(*(String	+ p) ==	'\0') break; 
		if(_mbsbtype((const	uchar *)String,	p) == _MBC_SINGLE) p++;
		else												   p +=	2;
	}
	return	p;
}



/*------------------------------------------------------------------------------*
| <<< 全角文字、半角文字入り乱れる中から指定の半角文字数での全角文字数を得る >>>
*------------------------------------------------------------------------------*/
static	int	GetStringPoint2(char *String, int Point)
{
	int		i, p;

	p =	i =	0;
	while(i	< Point)
	{
		if(*(String	+ p) ==	'\0') break; 
		if(_mbsbtype((const	uchar *)String,	p) == _MBC_SINGLE) i++;
		else												   i +=	2;
		p++;
	}
	return p;
}



/*------------------------------------------------------------------------------*
| <<< バッファにコードをストックする >>>
*------------------------------------------------------------------------------*/
static	int	StokInputChar(char CharCode)
{
	//--- バッファがオーバーフローしていたらなにもしない ---------
	if((ime.EdPoint	+ 1	== ime.StPoint)	|| (ime.StPoint	== 0 &&	ime.EdPoint	== CHARBUFFER_VOL))	return -1;

	//--- バッファに文字コードを代入 -----------------------------
	ime.CharBuffer[ime.EdPoint]	= CharCode;
	ime.EdPoint++;
	if(ime.EdPoint == CHARBUFFER_VOL + 1) ime.EdPoint =	0;

	//--- 終了 ---------------------------------------------------
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< ＩＭＥメッセージのコールバック関数 >>>
*------------------------------------------------------------------------------*/
int		ime_proc(HWND hWnd,	UINT message, WPARAM wParam, LPARAM	lParam)
{

	HIMC	Imc;
	static	int	Flag = FALSE;

	//--- ＩＭＥ無視用状態だった場合はなにもせず終了 -----
	if(ime.IMEUseFlag == FALSE)	return 0;

	ime.ChangeFlag = TRUE;					// 変化があったフラグセット
	
	//--- ＩＭＥの各処理 ---------------------------------
	switch(message)
	{
	//--- ＩＭＥ入力開始メッセージ -----------------------
	case WM_IME_STARTCOMPOSITION:
		ime.InputPoint	 = 0;
		ime.IMEInputFlag = TRUE;
		break;

	//--- ＩＭＥ入力終了メッセージ -----------------------
	case WM_IME_ENDCOMPOSITION:
		ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
		ime.IMEInputFlag = FALSE;
		break;

	//--- ＩＭＥ文字編集変化メッセージ -------------------
	case WM_IME_COMPOSITION:
		{
			//snd_play(__SE_CLICK);
			HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
										// 入力コンテキストを取得
			Imc	= ImmGetContext(DefHwnd);
			if(lParam &	GCS_COMPSTR)	// 編集文字変化時の処理
			{							// 編集中文字列の取得
				ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
				ImmGetCompositionString(Imc, GCS_COMPSTR, ime.InputString, CHARBUFFER_VOL);
			}
			if(lParam &	GCS_RESULTSTR)	// 編集中文字決定時の処理
			{
				ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
				ImmGetCompositionString(Imc, GCS_RESULTSTR,	ime.InputString, CHARBUFFER_VOL);
										// バッファにストック
				int	StrLen = lstrlen(ime.InputString);
				for(int	i =	0; i < StrLen; i++)	StokInputChar(ime.InputString[i]);
										// 編集中文字列初期化
				ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
				ImmGetCompositionString(Imc, GCS_COMPSTR, ime.InputString, CHARBUFFER_VOL);
			}
			if(lParam &	GCS_CURSORPOS)	// カーソル位置変化時の処理
			{
				ime.InputPoint = GetStringPoint2(ime.InputString, ImmGetCompositionString(Imc, GCS_CURSORPOS, NULL,	0));
			}
			if(lParam &	GCS_COMPCLAUSE)	// 文節情報変化時
			{							// 文節情報の取得
				ZeroMemory(ime.ClauseData, sizeof(int) * 256);
				ime.ClauseNum =	ImmGetCompositionString(Imc, GCS_COMPCLAUSE, (void *)ime.ClauseData, 256 * sizeof(int))	/ 4;
			}
										// 入力コンテキストの削除
			ImmReleaseContext(DefHwnd, Imc);
		}
		break;

	//--- ＩＭＥ状態変化メッセージ -----------------------
	case WM_IME_NOTIFY:
		switch(wParam)
		{
		//--- ＩＭＥのＯＮ、ＯＦＦ変化 -----------------------
		case IMN_SETOPENSTATUS:
			if(Flag) break;
			Flag = TRUE;
			DefWindowProc(hWnd,	message, wParam, lParam);
			//--- キーが押されている間ここで止まる ---------------
			{
//				_int64	Time = timeGetTime();
//				while(timeGetTime()	- Time < 2000){}
			}
			Flag = FALSE;
			//--- ＩＭＥの使用状態を得る -------------------------
			{
				HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
				Imc		  =	ImmGetContext(DefHwnd);
				ime.IMESwitch =	ImmGetOpenStatus(Imc);
				ImmReleaseContext(DefHwnd, Imc);
			}
			break;

		//--- 候補変化時の処理 -------------------------------
		case IMN_CHANGECANDIDATE:
		case IMN_OPENCANDIDATE:
		case IMN_SETCANDIDATEPOS:
			{
				DWORD	BufSize;
				HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
										// 入力コンテキストを取得
				Imc	= ImmGetContext(DefHwnd);
										// バッファサイズ取得
				BufSize	= ImmGetCandidateList(Imc, 0, NULL,	0);
										// バッファ用メモリの確保
				if((ime.CandidateList =	(CANDIDATELIST *)malloc(BufSize)) == NULL)
				{
					sys_error("IME:漢字変換候補を保存するメモリがありません。");
					return 0;
				}
										// データの取得
				ImmGetCandidateList(Imc, 0,	ime.CandidateList, BufSize);
										// 入力コンテキストの削除
				ImmReleaseContext(DefHwnd, Imc);
			}
			break;

		//--- 入力候補ウインドウを閉じようとしている時の処理 -
		case IMN_CLOSECANDIDATE:
										// メモリの解放
			if(ime.CandidateList !=	NULL) free(ime.CandidateList);
			ime.CandidateList =	NULL;
			break;
		}
		break;
	}
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< IMEの入力モード文字列の取得 >>>
*------------------------------------------------------------------------------*/
static	int		GetIMEInputModeStr(char	*GetBuffer)	
{

	DWORD	InputState,	SentenceState, Buf;
	char	*SelectStr;


	strcpy(GetBuffer, "直接入力");

	if(!ime.IMESwitch) return -1;			// ＩＭＥが使われていないときは-1を返す
										// 入力コンテキストを取得
	HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
	HIMC	Imc		= ImmGetContext(DefHwnd);
										// 入力モードを得る
	ImmGetConversionStatus(Imc,	&InputState, &SentenceState);
										// 判断
	Buf	= InputState & (IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA);
	if(Buf == (IME_CMODE_NATIVE	| IME_CMODE_FULLSHAPE |	IME_CMODE_KATAKANA))
	{
		SelectStr =	"全角カタカナ";
	}
	else
	{
		Buf	= InputState & (IME_CMODE_NATIVE | IME_CMODE_KATAKANA);
		if(Buf == (IME_CMODE_NATIVE	| IME_CMODE_KATAKANA))
		{
			SelectStr =	"半角カタカナ";
		}
		else
		{
			Buf	= InputState & (IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE);
			if(Buf == (IME_CMODE_NATIVE	| IME_CMODE_FULLSHAPE))
			{
				SelectStr =	"全角ひらがな";
			}
			else
			{
				Buf	= InputState & IME_CMODE_FULLSHAPE;
				if(Buf == IME_CMODE_FULLSHAPE)
				{
					SelectStr =	"全角英数";
				}
				else
				{
					Buf	= InputState & IME_CMODE_ALPHANUMERIC;
					if(Buf == IME_CMODE_ALPHANUMERIC)
					{
						SelectStr =	"半角英数";
					}
					else
					{
						SelectStr =	"不明な入力モード";
					}
				}
			}
		}
	}
	lstrcpy(GetBuffer, SelectStr);
	ImmReleaseContext(DefHwnd, Imc);	// 入力コンテキストの削除
	return 0;							// 終了
}



/*------------------------------------------------------------------------------*
| <<< 文字列の幅を得る >>>
*------------------------------------------------------------------------------*/
static	int	GetDrawStringWidth(char	*String, int StrLen)
{

	float	ret	= 0;

	for(int	i =	0; i < StrLen; i++)
	{
		if(String[i] ==	NULL) break;
		if(_mbsbtype((const	uchar*)&String[i], 0) == _MBC_LEAD){ ret +=	__FONT_WIDTH; i++;}
		else													 ret +=	__FONT_WIDTH2;
	}
	return (int)ret;
}



/*------------------------------------------------------------------------------*
| <<< 画面上に入力中の文字列を描画する >>>
*------------------------------------------------------------------------------*/
static	int	DrawIMEInputString(int x, int y, int SelectStringNum)
{
	int		StrLen;
	int		PointX,	  PointY;
	int		CPointX,  CPointY;
	int		Width;
	int		i;


	//--- 変換候補の消去 -------------------------
	static	int		StrNumBak =	0;		// 変換文字の候補数
	for(i =	0; i < StrNumBak; i++) d3_print_off(SelectStringNum + i + 1);

	//--- 入力文字列を描画 -----------------------
	if(ime.ClauseNum !=	2)					// 変換時の選択文節の強調表示
	{
		int		Point =	GetStringPoint(ime.InputString,	ime.InputPoint);	
		for(i =	0; i < ime.ClauseNum - 1 &&	ime.ClauseData[i] != Point;	i++	){}
		if(i !=	ime.ClauseNum -	1)
		{
			int		Width;
			Point =	GetDrawStringWidth(ime.InputString,	Point);
			Width =	ime.ClauseData[i + 1] -	ime.ClauseData[i];
			Width =	GetDrawStringWidth(&ime.InputString[ime.ClauseData[i]],	Width);
		}
	}
	d3_printf(SelectStringNum,	x, y, STRCOLOR6, ime.InputString); 

	//--- カーソルを描画 -------------------------
	StrLen = GetStringPoint(ime.InputString, ime.InputPoint);
										// 先頭文字位置までの幅を算出
	if(StrLen == 0)	CPointX	= 0;
	else			CPointX	= GetDrawStringWidth(ime.InputString, StrLen);
	CPointX	+= x;
	CPointY	 = y;
	if(!ime.CandidateList)					// 選択文字列が存在する場合のみ描画
	{									// カーソルの描画
//D3_2DLINE(&_VEC2((float)CPointX, (float)CPointY),
//				   &_VEC2((float)CPointX, (float)CPointY + __FONT_HEIGHT), STRCOLOR4);
	}

	//--- 下線を描画 -----------------------------
	{
		int	StrNum = 0;
		int	PointX = x;
		for(int	i =	0; i < ime.ClauseNum; i++)
		{
			int		StrWidth = GetDrawStringWidth(&ime.InputString[StrNum],	ime.ClauseData[i] -	StrNum);
			if(StrWidth	== 0) continue;
			float	x0		 = (float)(PointX				 );
			float	y0		 = (float)(y	  +	__FONT_HEIGHT) + 2.f;
			float	x1		 = (float)(PointX +	StrWidth	 );
			D3_2DLINE(&VEC2(x0, y0), &VEC2(x1, y0), STRCOLOR5);
			PointX += StrWidth;
			StrNum	= ime.ClauseData[i];
		}
	}

	//--- 候補リストが出ている場合はその描画 -----
	if(ime.CandidateList)
	{
		DWORD	i;
		int		j, h, MaxWidth;
		char	*Str;
		
		//--- 最長選択候補を調べる -------------------
		MaxWidth = 0;
		j		 = 0;
		for(i =	(ime.CandidateList->dwSelection	/ SelectStringNum) * SelectStringNum; j	< SelectStringNum; i++,	j++)
		{
			if(i ==	ime.CandidateList->dwCount)	break;
			Str	  =	(char *)ime.CandidateList +	ime.CandidateList->dwOffset[i];
			Width =	GetDrawStringWidth(Str,	lstrlen(Str));
			if(Width > MaxWidth) MaxWidth =	Width;
		}

		//--- 描画範囲をセット -----------------------
		h =	__FONT_HEIGHT;
		float	x0 = (float)CPointX;
		float	y0 = (float)CPointY	+ __FONT_HEIGHT;
		float	x1 = (float)CPointX	+ __FONT_WIDTH	+ MaxWidth;
		float	y1 = (float)CPointY	+ __FONT_HEIGHT	* (j + 1);

		if((int)y1 > SCR_H)				// インプット文字描画位置より下の位置に配置できるか検証
		{
			j =	(int)y1	- SCR_H;
			y1 -= j;					// 高さを補正
			y0 -= j;
		}
		if(x1 >	SCR_W)					// インプット文字描画幅のせいで描画可能領域右端に到達していた場合は補正
		{
			j	= (int)x1 -	SCR_W;	
			x0 -= j;
			x1 -= j;
		}
		//--- 描画範囲を黒で塗りつぶし ---------------
		/*d3_2d_box_set(&_VEC3(x0 - 3,	y0 - 3,	1),
					   &_VEC3(x1 + 3,	y1 + 4,	1),	STRCOLOR3, true,
					   D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		d3_2d_box_set(&_VEC3(x0 - 1,	y0 - 1,	0),
					   &_VEC3(x1 + 1,	y1 + 2,	0),	STRCOLOR8, true,
					   D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		*/

		//--- 候補を描画 -----------------------------
		PointX = (int)x0;
		PointY = (int)y0;
		j	   = 0;
		for(i =	(ime.CandidateList->dwSelection	/ SelectStringNum) * SelectStringNum; j	< SelectStringNum; i++,	j ++)
		{
			if(i ==	ime.CandidateList->dwCount)	break;
			d3_printf(SelectStringNum + j + 1,	PointX + 8,	PointY + j * __FONT_HEIGHT,
						(i == ime.CandidateList->dwSelection) ?	STRCOLOR6 :	STRCOLOR2,
						(char *)ime.CandidateList +	ime.CandidateList->dwOffset[i]); 
		}
		StrNumBak =	j;
	}

	//--- 終了 -----------------------------------
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< ＩＭＥを使用するかどうかをセットする >>>
|	入力	UseFlag	= true	: 使用する
|					  false	: 使用しない
*------------------------------------------------------------------------------*/
static	void	SetUseIMEFlag(int UseFlag)
{

	if(UseFlag == ime.IMEUseFlag) return;	// フラグが以前と同じ場合はなにもせず終了
	WINNLSEnableIME(sys.hWnd, UseFlag);		// 有効フラグをセット
	ime.IMEUseFlag = UseFlag;				// フラグ保存
	if(UseFlag == FALSE)					// もしFALSEだった場合は各種メモリを解放する3
	{
		if(ime.CandidateList !=	NULL) free(ime.CandidateList);
		ime.CandidateList =	NULL;
		ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
		ime.IMEInputFlag = FALSE;
	}
}



/*------------------------------------------------------------------------------*
| <<< 直接入力チェック >>>
*------------------------------------------------------------------------------*/
static	char	input_key_check(void)
{

	char	key1[] = "abcdefghijklmnopqrstuvwxyz01234567890123456789.+-/*-^\\@[;:],./\\ ";
	char	key0[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0!\"#$%&'()0123456789.+-/*=~|`{+*}<>?_ ";

	for(int	i =	KEY_A; i <=	KEY_SPACE; i++)
	{
		if(PAD_KEY_REP(i) || pad.bKeyClick[i])
		{ 
			if((pad.bKeyPush[KEY_LSHIFT] || pad.bKeyPush[KEY_RSHIFT])){ return	key0[i];}
			else													  {  return	key1[i];}
		}
	}
	return NULL;
}



/*------------------------------------------------------------------------------*
| <<< 入力を途中でキャンセル >>>
|	入力	*str	= 文字列バッファ
|			str_len	= 文字列バッファの大きさ
*------------------------------------------------------------------------------*/
void	ime_cancel(char	*str, int str_len)
{
	d3_print_off(__STR_NUMBER);
	d3_print_off(__STR_NUMBER + 1);
	//snd_play(__SE_CANCEL);
	SetUseIMEFlag(false);				// ＩＭＥ無効
	d3_print_off(0);					// モード消去
	pad.bKeyClick[KEY_ESC] = false;
	strncpy(str, ime.str_bak, str_len);
	ime.input_sw = -2;
	return;
}



/*------------------------------------------------------------------------------*
| <<< 文字入力 >>>
|	入力	*str	= 文字列バッファ
|			str_len	= 文字列バッファの大きさ
|	戻り値	true  :	入力終了
|			false :	入力中
*------------------------------------------------------------------------------*/
bool	ime_input(VEC2 *pos, char *str, int str_len)
{
	
	//--- モード別分岐 ------------------------------------------
	switch(ime.input_sw)
	{
	case -2:							// 初期化
		SetUseIMEFlag(true);
		ime.input_sw = 0;
		ime.StrLen	 = strlen(str);
		if(str_len > (CHARBUFFER_VOL - 1)) str_len = CHARBUFFER_VOL	- 1;
		strncpy(ime.str_bak, str, str_len);
		return false;
	}

	char	tmp[CHARBUFFER_VOL];
	static	char	InputString_bak	= NULL;


	//--- 入力モード取得＆表示 ----------------------------------
	GetIMEInputModeStr(tmp);
	d3_printf(__STR_NUMBER, SCR_W - 120, 4, STRCOLOR1,	"%s", tmp);

	//--- 文字列のコピー ----------------------------------------
	if(ime.CharBuffer[0] !=	NULL)			
	{
		int		len	= strlen(ime.CharBuffer);
		ZeroMemory(tmp,	sizeof(tmp));
		strncat(tmp, &str[0], ime.Point);
		strcat(&tmp[ime.Point],	ime.CharBuffer);
		strcat(&tmp[ime.Point +	strlen(ime.CharBuffer)], &str[ime.Point]);
		strncpy(str, tmp, str_len);
		if(_mbsbtype((const	uchar*)&str[0],	str_len	- 1) ==	_MBC_LEAD) str[str_len - 1]	= NULL;
		ime.Point  += len; if(ime.Point	 > str_len)	ime.Point  = str_len;
		ime.StrLen += len; if(ime.StrLen > str_len)	ime.StrLen = str_len;
		ZeroMemory(ime.CharBuffer,	CHARBUFFER_VOL);
		ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
		ime.InputPoint = 0;
		ime.StPoint	   = 0;
		ime.EdPoint	   = 0;
		ime.ClauseNum  = 0;
	}
	else if(ime.InputString[0] == NULL && InputString_bak == NULL)
	{
		//--- 直接入力 ----------------------------------------------
		char	key	= input_key_check();
		if(key != NULL)
		{
			//snd_play(__SE_CLICK);
			ZeroMemory(tmp,	sizeof(tmp));		// このあたりは変換前の文字列を描画する処理
			strncat(tmp, &str[0], ime.Point);
			tmp[ime.Point] = key;
			strcat(tmp,	&str[ime.Point]);
			strncpy(str, tmp, str_len);
			if(_mbsbtype((const	uchar*)&str[0],	str_len	- 1) ==	_MBC_LEAD) str[str_len - 1]	= NULL;
			ime.StrLen = (int)strlen(str);
			ime.Point++;
			if(ime.Point  >	str_len) ime.Point	= str_len;
			if(ime.StrLen >	str_len) ime.StrLen	= str_len;
		}
		//--- ＥＳＣキーでキャンセル --------------------------------
		if(pad.bKeyClick[KEY_ESC])
		{
/*			d3_print_off(__STR_NUMBER);
			d3_print_off(__STR_NUMBER + 1);
			snd_play(__SE_CANCEL);
			SetUseIMEFlag(false);				// ＩＭＥ無効
			d3_print_off(0);					// モード消去
			pad_key_1st[KEY_ESC] = FALSE;
			strncpy(str, ime.str_bak, str_len);
			ime.input_sw = -2;
*/
			ime_cancel(str,	str_len);
			return true;
		}
		//--- エンターキーで終了 ------------------------------------
		if(pad.bKeyClick[KEY_ENTER])
		{
			d3_print_off(__STR_NUMBER);
			d3_print_off(__STR_NUMBER + 1);
			//snd_play(__SE_OK);
			SetUseIMEFlag(false);				// ＩＭＥ無効
			d3_print_off(0);					// モード消去
			ime.input_sw = -2;
			return true;
		}
		//--- ←→背押した ------------------------------------------
		int	dat0, dat1;
		if(_mbsbtype((const	uchar*)&str[ime.Point -	2],	0) != _MBC_LEAD	)  dat0	= 1;
		else															   dat0	= 2;
		if(_mbsbtype((const	uchar*)&str[ime.Point +	0],	0) == _MBC_SINGLE) dat1	= 1;
		else															   dat1	= 2;
		if((PAD_KEY_REP(KEY_LEFT ) || pad.bKeyClick[KEY_LEFT])	&& ime.Point > 0)
		{
			//snd_play(__SE_CLICK);
			if((ime.Point -= dat0) < 0)	ime.Point =	0;
		}
		if((PAD_KEY_REP(KEY_RIGHT) || pad.bKeyClick[KEY_RIGHT]))
		{
			//snd_play(__SE_CLICK);
			if((ime.Point += dat1) > ime.StrLen) ime.Point = ime.StrLen;
		}
		//--- ＢＳ、ＤＥＬキーで一文字消去 --------------------------
		if((PAD_KEY_REP(KEY_BACKSPACE) || pad.bKeyClick[KEY_BACKSPACE]) &&	ime.Point >= dat0)
		{
			//snd_play(__SE_BACK);
			ZeroMemory(tmp,	sizeof(tmp));		// このあたりは変換前の文字列を描画する処理
			strncat(tmp, &str[0], ime.Point	- dat0);
			strcat(	tmp, &str[ime.Point]);
			strcpy(	str, tmp);
			ime.StrLen = (int)strlen(str);
			if(ime.Point > ime.StrLen) ime.Point  =	ime.StrLen;
			else					   ime.Point -=	dat0;
		}
		if((PAD_KEY_REP(KEY_DEL) ||	pad.bKeyClick[KEY_DEL]) &&	ime.StrLen >= dat1)
		{
			//snd_play(__SE_BACK);
			ZeroMemory(tmp,	sizeof(tmp));		// このあたりは変換前の文字列を描画する処理
			strncat(tmp, &str[0], ime.Point);
			strcat(tmp,	&str[ime.Point + dat1]);
			strcpy(str,	tmp);
			ime.StrLen = (int)strlen(str);
			if(ime.Point > ime.StrLen) ime.Point = ime.StrLen;
		}
	}
	//--- 文字変換中にＥＳＣキーが押された場合 ------------------
	else
	{
		if(pad.bKeyClick[KEY_ESC])	pad.bKeyClick[KEY_ESC]	= FALSE;
	}

	//--- 実際に文字描画 ----------------------------------------
	ZeroMemory(tmp,	sizeof(tmp));		// このあたりは変換前の文字列を描画する処理
	strncat(tmp, &str[0], ime.Point);
	strcat(&tmp[ime.Point],	ime.InputString);
	strcat(&tmp[ime.Point +	strlen(ime.InputString)], &str[ime.Point]);

	d3_printf(__STR_NUMBER	+ 1, (int)pos->x, (int)pos->y, STRCOLOR1, tmp);
	int	x =	GetDrawStringWidth(tmp,	ime.Point);//strlen(tmp));
	DrawIMEInputString((int)pos->x + x,	(int)pos->y, __STR_NUMBER +	2);

	//--- 次回の為に変換文字列の先頭のみ保存 --------------------
	InputString_bak	= ime.InputString[0];

	return false;
}



/*------------------------------------------------------------------------------*
| <<< 文字入力初期化 >>>
|	入力	*str = 入力文字列
*------------------------------------------------------------------------------*/
void	ime_clear(char *str)
{
	ime.Point =	0;
}