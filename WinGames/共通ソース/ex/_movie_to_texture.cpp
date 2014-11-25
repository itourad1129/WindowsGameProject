//
//	ムービーをテクスチャーに流し込むプログラム
//


#define		TRANS_ASM						// VRAM 転送をアセンブラで行うスイッチ(16ビット時のみ)
#define		MOVIE_MAX	D3_TEX_MAX2			// ムービーの最大数

#define		INTERMEDIATE_BUFFER				// 中間バッファを使用する(win7、フルスクリーン時には必ず使用のこと。XP時は特に変化なし)


#ifdef	_DEBUG
	#undef	ASSERT
	#define	ASSERT(a)
#endif

//--- ライブラリィ --------------------------------------------------------------------------------
#ifdef	_DEBUG
	#pragma	comment(lib, "strmbasd.lib")
#else
	#pragma	comment(lib, "strmbase.lib")
#endif



/*----------------------------------------------------------------------*
| <<< クラスの定義 >>>
|	{71771540-2017-11cf-AE26-0020AFD79767}
*----------------------------------------------------------------------*/
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;
class CTextureRenderer : public	CBaseVideoRenderer
{
public:
	CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr);
	~CTextureRenderer();
public:
	HRESULT	CheckMediaType(const CMediaType	*pmt);		// Format acceptable?
	HRESULT	SetMediaType(  const CMediaType	*pmt);		// Video format	notification
	HRESULT	DoRenderSample(IMediaSample	*pMediaSample);	// New video sample

	ushort					usTexNum;			// テクスチヤー番号
	int						iWidth,	iHeight;	// ムービーの幅と高さ
	int						iPitch;				// Video Pitch
	char					strFile[MAX_PATH];	// ファイル名
};

struct	MOVIE_TO_TEXTURE_WORK				// 動画ファイル再生用データ構造体
{
	bool					bSystemPause;		// システム側の強制ポーズ(非アクティブ時に使用)
	bool					bPlayState;			// true	: 再生中
	bool					bLoadFlag;			// ture	: ロードしている
	bool					bSound;				// true	: サウンド再生する
	bool					b16Bit;				// true : １６ビットでテクスチャー作成
	LONGLONG				llPausePosition;	// 一時停止位置(※ LONGLONG は６４バイト型)
	double					dEndTime;			// 終了時間
	int						iLoop;				// ループ数		※-1で無限ループ
	uint					uiFrame;			// 再生コマ数
	float					fRate;				// 再生レート
	int						iPauseFlag;			// 一時停止までのカウンター(-1:ならば再生状態)
												//  1 を代入すると、１フレームをテクスチャーに取り出し一時停止(値は０)
												// -1 を代入すると、movie_to_texture_main で一時停止が掛かる
	CComPtr<IGraphBuilder>	pGB;				// グラフビルダー
	CComPtr<IMediaControl>	pMC;				// Media Control
	CComPtr<IMediaPosition>	pMP;				// Media Position
	CComPtr<IMediaEvent>	pME;				// Media Event
	CComPtr<IBaseFilter>	pBF;

	CComPtr<IMediaSeeking>	pMS;				// Media Seeking
	CComPtr<IBasicAudio	>	pBA;				// Basic Audio

	CTextureRenderer		*pcTR;				// DirectShow TextureRenderer クラス

	//--- 中間バッファ ----------------------------------------------
	#ifdef	INTERMEDIATE_BUFFER
		bool				bTrans;				// true : 要ムービー転送
		DWORD				*pdwBuf;			// ムービー展開用バッファ
		uint				uiPitch;			// テクスチャー1ラインのバイト数
		uint				uiSize;				// 展開用に確保したメモリサイズ
	#endif

};


static	MOVIE_TO_TEXTURE_WORK	movie[MOVIE_MAX];



/*----------------------------------------------------------------------*
| <<< CTextureRenderer constructor >>>
*----------------------------------------------------------------------*/
CTextureRenderer::CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr) : CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr)
{
	ASSERT(phr);							// Store and AddRef	the	texture	for	our	use.
	if(phr){ *phr = S_OK;}
}



/*----------------------------------------------------------------------*
| <<< CTextureRenderer destructor >>>
*----------------------------------------------------------------------*/
CTextureRenderer::~CTextureRenderer()
{
	// Do nothing
}



/*----------------------------------------------------------------------*
| <<< ムービーのタイプを調べる >>>
*----------------------------------------------------------------------*/
HRESULT	CTextureRenderer::CheckMediaType(const CMediaType *pmt)
{

	HRESULT		hr	 = E_FAIL;
	VIDEOINFO	*pvi = 0;


	CheckPointer(pmt, E_POINTER);
											// Reject the connection if	this is	not	a video	type
	if(*pmt->FormatType() != FORMAT_VideoInfo){	return E_INVALIDARG;}
	pvi	= (VIDEOINFO *)pmt->Format();		// Only	accept RGB24 video
	if(IsEqualGUID(*pmt->Type(),	MEDIATYPE_Video)
	&& IsEqualGUID(*pmt->Subtype(),	MEDIASUBTYPE_RGB24)){ hr = S_OK;}
	return hr;
}



/*----------------------------------------------------------------------*
| <<< ムービーのタイプよりテクスチャーを作成 >>>
*----------------------------------------------------------------------*/
HRESULT	CTextureRenderer::SetMediaType(const CMediaType	*pmt)
{

	VIDEOINFO				*pviBmp;

	MOVIE_TO_TEXTURE_WORK	*w = &movie[usTexNum];


	pviBmp	= (VIDEOINFO *)pmt->Format();
	iWidth	= pviBmp->bmiHeader.biWidth;
	iHeight	= abs(pviBmp->bmiHeader.biHeight);
	iPitch	= (iWidth *	3 +	3) & ~(3);

	if(w->b16Bit)
	{
		d3_tex_create(usTexNum, iWidth, iHeight, D3DFMT_A1R5G5B5);
	}
	else
	{
		d3_tex_create(usTexNum, iWidth, iHeight, D3DFMT_X8R8G8B8);
	}
	
	if(d3.texInfo[usTexNum].usFormat != D3DFMT_X8R8G8B8
	&& d3.texInfo[usTexNum].usFormat != D3DFMT_A1R5G5B5)
	{
		sys_error("%s はテスクチャーにすることができないムービーフォーマットです。", strFile);
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

#ifdef	INTERMEDIATE_BUFFER
	// 中間バッファを用意
	if(w->pdwBuf != NULL){ free(w->pdwBuf); w->pdwBuf = NULL;}

	int	iSize = sizeof(DWORD);
	if(w->b16Bit){ iSize = sizeof(WORD);}
	w->uiSize  = (uint)((d3.texInfo[usTexNum].fWidth * d3.texInfo[usTexNum].fHeight) * iSize);
	w->pdwBuf  = (DWORD*)malloc(w->uiSize);
	w->uiPitch = (uint)((d3.texInfo[usTexNum].fWidth) * iSize);
#endif

	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< ムービーをテスクチャーに流し込む >>>
|	※ 16 ビット、32 ビットのみの対応となります。
*----------------------------------------------------------------------*/
HRESULT	CTextureRenderer::DoRenderSample(IMediaSample *pSample)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[usTexNum];

	BYTE	*pBmpBuffer, *pTxtBuffer;
	
	// ムービー再生が停止していたらここで終了
	if(movie[usTexNum].bPlayState == false){ return E_FAIL;}

	// 各ポインターが NULL でないか調べる
	if(pSample			== NULL){ return E_POINTER;}
	if(d3.tex[usTexNum] == NULL){ return E_UNEXPECTED;}

	// ムービーの展開バッファを得る
	pSample->GetPointer(&pBmpBuffer);	// Get the video bitmap	buffer

	// テクスチャーバッファ(pTxtBuffer)を用意
	uint	uiNextLine;
	#ifdef	INTERMEDIATE_BUFFER
		pTxtBuffer = (BYTE*)w->pdwBuf;
		w->bTrans  = true;
		uiNextLine = w->uiPitch;
	#else
		D3DLOCKED_RECT	d3dlr;
		if(FAILED(d3.tex[usTexNum]->LockRect(0, &d3dlr, 0, 0))){ return E_FAIL;}
		pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
		uiNextLine = d3dlr.Pitch;
	#endif

	if(d3.texInfo[usTexNum].usFormat == D3DFMT_X8R8G8B8) 
	{
		UINT	row, col, dwWidth;
		dwWidth = iWidth / 4;			// aligned width of	the	row, in	DWORDS
										// (pixel by 3 bytes over sizeof(DWORD))
		for(row	= 0; row < (UINT)iHeight; row++)
		{
			DWORD	*pdwS = (DWORD *)pBmpBuffer;
			DWORD	*pdwD = (DWORD *)pTxtBuffer;
			for(col	= 0; col < dwWidth; col++)
			{
				pdwD[0]	=   pdwS[0]	       | 0xFF000000;
				pdwD[1]	= ((pdwS[1]	<<	8) | 0xFF000000) | (pdwS[0]	>> 24);
				pdwD[2]	= ((pdwS[2]	<< 16) | 0xFF000000) | (pdwS[1]	>> 16);
				pdwD[3]	=					 0xFF000000  | (pdwS[2]	>>  8);
				pdwD   += 4;
				pdwS   += 3;
			}
			BYTE	*pbS	= (BYTE	*)pdwS;
			for(col	= 0; col < (UINT)iWidth	% 4; col++)
			{
				*pdwD =	0xFF000000 | (pbS[2] <<	16)	| (pbS[1] << 8)	| (pbS[0]);
				pdwD++;
				pbS	 +=	3;
			}
			pBmpBuffer += iPitch;
			pTxtBuffer += uiNextLine;
		}
	}
	else if(d3.texInfo[usTexNum].usFormat == D3DFMT_A1R5G5B5)
	{
#ifndef	TRANS_ASM
		for(int	y =	0; y < iHeight;	y++)
		{
			BYTE	*pBmpBufferOld = pBmpBuffer;
			BYTE	*pTxtBufferOld = pTxtBuffer;

			for(int	x =	0; x < iWidth; x++)
			{
				*(WORD *)pTxtBuffer	= 
							(WORD)(0x8000	+
								  ((pBmpBuffer[2]	& 0xF8)	<< 7) +
								  ((pBmpBuffer[1]	& 0xF8)	<< 2) +
								  ( pBmpBuffer[0] >> 3));
				pTxtBuffer += 2;
				pBmpBuffer += 3;
			}
			pBmpBuffer = pBmpBufferOld + iPitch;
			pTxtBuffer = pTxtBufferOld + uiNextLine;
		}
#else
		int		iXMax        = iWidth / 2;
		int		iYMax        = iHeight	  - 1;
		int		iAddPitch    = iPitch     - iWidth * 3;
		int		iAddTxtPitch = uiNextLine - iWidth * 2;

		// 速度を上げるためアセンブラで記述
		// ここでは極力 VRAM へのアクセスを減らしている。VRAM ４バイト転送している
		__asm
		{
				push	eax
				push	ebx
				push	ecx
				push	edx
				push	edi
				push	esi

				mov		esi, pTxtBuffer
				mov		edi, pBmpBuffer
				mov		ecx, iYMax

			LOOP_Y:
				push	ecx
				mov		ecx, iXMax

			LOOP_X:

				//--- 一つ目のデータ加工
				mov		edx, [edi + 3]		// 一気に４バイト読み込み(そのうち３バイトを使用する)

				mov		eax, edx			// edi + 5 を読み込みでいる
				and		eax, 0xf80000 
				shr		eax, 8 + 1

				mov		bx, dx				// edi + 4 を読み込みでいる
				and		bx, 0xf800
				shr		bx, 8 - 2

				and		dx, 0xf8			// edi + 3
				shr		dx, 3				

				or		bx, ax
				or		bx, dx
				shl		ebx, 16				// ebx の上１６ビットに退避

				//--- 二つ目のデータ加工
				mov		edx, [edi + 0]		// 一気に４バイト読み込み(そのうち３バイトを使用する)

				mov		eax, edx			// edi + 2 を読み込みでいる
				and		eax, 0xf80000 
				shr		eax, 8 + 1

				mov		bx, dx				// edi + 1 を読み込みでいる
				and		bx, 0xf800
				shr		bx, 8 - 2

				and		dx, 0xf8			// edi + 0
				shr		dx, 3

				or		bx, ax
				or		bx, dx

				//--- データを書き込む		// 一度に４バイト書き込むので VRAM アクセス時には有効である
				or		ebx, 0x80008000
				mov		[esi + 0], ebx

				add		edi, 6
				add		esi, 4

				loopnz	LOOP_X

				add		edi, iAddPitch
				add		esi, iAddTxtPitch
				pop		ecx
				loopnz	LOOP_Y

				pop		esi
				pop		edi
				pop		edx
				pop		ecx
				pop		ebx
				pop		eax
		};
#endif
	}

	// 再生コマ数カウントアップ
	w->uiFrame++;

	// 一時停止要求の場合、一コマ取り出した後、フラグを掛ける
	if(w->iPauseFlag == 1){ w->iPauseFlag++;}

	#ifndef	INTERMEDIATE_BUFFER
		if(FAILED(d3.tex[usTexNum]->UnlockRect(0))){ return E_FAIL;}
	#endif

	return S_OK;
}



/*------------------------------------------------------------------------------*
| <<< ムービーの終了、解放 >>>
|	入力	iNum = ムービー番号
*------------------------------------------------------------------------------*/
static	void	movie_to_texture_release(int iNum)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];
	if(w->bLoadFlag	== false){ return;}

	if(!(!w->pMC)){ w->pMC->Stop();   }		// 再生を停止する
											// 各種オブジェクトの解放
	if(!(!w->pMC)){ w->pMC.Release(); w->pMC = NULL;}
	if(!(!w->pME)){ w->pME.Release(); w->pME = NULL;}
	if(!(!w->pMP)){ w->pMP.Release(); w->pMP = NULL;}
	if(!(!w->pMS)){ w->pMS.Release(); w->pMS = NULL;}
	if(!(!w->pBA)){ w->pBA.Release(); w->pBA = NULL;}
	if(!(!w->pGB)){ w->pGB.Release(); w->pGB = NULL;}
	if(!(!w->pBF)){ w->pBF.Release(); w->pBF = NULL;}
	w->bPlayState   = false;
	w->bLoadFlag    = false;
	w->bSound	    = false;
	w->bSystemPause = false;				// 再生停止フラグクリア


	//SAFE_DELETE(w->pcTR);					// これ、多分、w->pBF.Release で開放されている感じです。
											// なので不要です
	w->pcTR = NULL;

	d3_tex_free(iNum);

#ifdef	INTERMEDIATE_BUFFER
	if(w->pdwBuf != NULL){ free(w->pdwBuf); w->pdwBuf = NULL;}
#endif
}



/*------------------------------------------------------------------------------*
| <<< ムービーをメモリより解放 >>>
|	int	iNum  = -1 : 全て開放							※ D3_TEX_MAX2 個まで
|		     != -1 : 指定番号のムービーを開放
*------------------------------------------------------------------------------*/
void	movie_to_texture_free(int iNum)
{
	int	iStart, iEnd;
	if(iNum == -1){ iStart =    0, iEnd = MOVIE_MAX;}
	else		  { iStart = iNum, iEnd = iNum + 1; }

	for(int	i =	iStart; i < iEnd; i++)
	{
		movie_to_texture_stop(   i);
		movie_to_texture_release(i);
	}
}



/*------------------------------------------------------------------------------*
| <<< ムービーの読み込み >>>
|	入力	iNum    = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			strFile = ファイル名
|			b16Bit  = 16 ビットでテクスチャーに流し込む
*------------------------------------------------------------------------------*/
bool	movie_to_texture_load(int iNum, char *strFile, bool b16Bit)
{

	HRESULT					hr;
	WCHAR					wFileName[MAX_PATH];
	CComPtr<IBaseFilter>	pFSrc;			// Source Filter
	CComPtr<IPin>			pFSrcPinOut;	// Source Filter Output	Pin	  

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];
											// ファイル名を	UNICODE	へ
	MultiByteToWideChar(CP_ACP,	0, strFile, -1, wFileName, MAX_PATH);
	movie_to_texture_release(iNum);

	//--- フィルターグラフ作成 ------------------------------------------------
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&w->pGB);
	if(FAILED(hr)){ return sys_error("FilterGraph が作成できません。");}

	//--- フィルター追加 -----------------------------------------------------
	CTextureRenderer	*c = new CTextureRenderer(NULL, &hr);
	c->usTexNum = iNum;						// テクスゃー番号登録
	w->b16Bit   = b16Bit;					// テクスチャータイプ
	w->bSound	= true;
	strcpy(c->strFile, strFile);			// ファイル名保存
	w->pcTR = c;
	w->pBF  = c;
	hr      = w->pGB->AddFilter(w->pBF, L"TextureRenderer");
	if(FAILED(hr)){ return sys_error("テクスチャーレンダーの登録に失敗しました。");}

	hr = w->pGB->AddSourceFilter(wFileName, L"SOURCE", &pFSrc);
	if(FAILED(hr)){ return sys_error("ソースフィルターの登録に失敗しました。");}

	//--- 各種インターフェース作成 -------------------------------------------
#if 0
	if(SUCCEEDED(hr	= pFSrc->FindPin(L"Output",	&pFSrcPinOut)))
	{
		// for AVI, MPEG1, VFW
		if(!w->bSound)						//	オーディオトラックを再生しない場合
		{
			CComPtr<IPin> pFTRPinIn;		// Texture Renderer	Input Pin
			if(FAILED(hr = c->FindPin(L"In",	&pFTRPinIn))){ return sys_error("ピンの登録に失敗しました。");}
			// 以下の箇所でエラーがでるので削除した
	/*		if(FAILED(hr = w->pGB->Connect(pFSrcPinOut, pFTRPinIn)))
			{
				return sys_error("ピンの接続に失敗しました。");
			}
			*/
		}
		else								// オーディオトラックを再生する場合
		{
			if(FAILED(hr = w->pGB->Render(pFSrcPinOut)))
			{
				return sys_error("ピンの登録に失敗しました。");
			}
		}
	}
	else
#endif
	{
		// for WMV, MPEG2
		if(FAILED(hr = w->pGB->QueryInterface(&w->pBA))){ return sys_error("オーディオの登録に失敗しました。");}

		if(FAILED(hr = w->pGB->RenderFile(wFileName, NULL)))
		{
			return sys_error("レンダリングに失敗しました。");
		}
		if(!w->bSound)						//	オーディオトラックを再生しない場合
		{
			w->pBA->put_Volume(-10000);
		}
		else
		{
			w->pBA->put_Volume(0);
		}
	}

	//--- 各フィルターの作成 --------------------------------------
	w->pGB->QueryInterface(&w->pMC);
	w->pGB->QueryInterface(&w->pMP);
	w->pGB->QueryInterface(&w->pME);
	w->pGB->QueryInterface(&w->pMS);

	//--- ループ用に終了時間を得る --------------------------------
	w->pMP->get_StopTime(&w->dEndTime);

	//--- 諸設定 ----------------------------------------------------
	w->bLoadFlag = true;					// ロード成功フラグ
	w->fRate	 = 1;						// 再生レート(等速)

	return true;
}



/*------------------------------------------------------------------------------*
| <<< ムービーの音量設定 >>>
|	入力	iNum = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			fVol = 音量(0.0f ～ 1.0f(最大(default))
*------------------------------------------------------------------------------*/
void	movie_to_texture_volume(int iNum, float fVol)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(w->pBA != NULL)
	{
		fVol = (fVol - 1) * 10000.f;		// -10000 が音量 0 なので変換する
		w->pBA->put_Volume((long)fVol);
	}
}



/*------------------------------------------------------------------------------*
| <<< ムービーの読み込み >>>				※ フルカラー専用
|	入力	iNum    = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			strFile = ファイル名
*------------------------------------------------------------------------------*/
bool	movie_to_texture_load(int iNum, char *strFile)
{
	return movie_to_texture_load(iNum, strFile, false);
}



/*------------------------------------------------------------------------------*
| <<< ムービーが終了したか調べる >>>
|	入力	iNum = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|	戻り値	true : 終了している
*------------------------------------------------------------------------------*/
bool	movie_to_texture_status_check(int iNum)
{

	long					lEventCode;
	long					lParam1;
	long					lParam2;
	bool					bRet = false;
	HRESULT					hr;
	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->pME){ return bRet;}
											// Check for completion	events
	hr = w->pME->GetEvent(&lEventCode, (LONG_PTR *)	&lParam1, (LONG_PTR	*) &lParam2, 0);
	if(SUCCEEDED(hr))
	{										// If we have reached the end of the media file, reset to beginning
		if(EC_COMPLETE == lEventCode) 
		{
			bRet = true;
			//hr = w->pMP->put_CurrentPosition(0);	// これ、ムービーを先頭に戻す処理
		}
											// w->pME->GetEven tの後に必ず呼び出す処理
		hr = w->pME->FreeEventParams(lEventCode, lParam1, lParam2);
	}

	return bRet;
}



/*------------------------------------------------------------------------------*
| <<< サウンド設定 >>>					※再生中は変更不可、movie_to_texture_play 前で設定のこと
|	入力	iNum = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			bSw  = true  : サウンド再生
|				   false : サウンド再生しない	※デフォルト
*------------------------------------------------------------------------------*/
void	movie_to_texture_sound_set(int iNum, bool bSw)
{
	movie[iNum].bSound = bSw;
}



/*------------------------------------------------------------------------------*
| <<< 再生速度設定 >>>
|	入力	iNum  =	ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			fRate = 1.0f : 等速
|					2.0f : 二倍速
|					0.5f : 1/2 倍速
*------------------------------------------------------------------------------*/
void	movie_to_texture_play_rate_set(int iNum, float fRate)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->bLoadFlag){ return;}				// ロードしていないならば帰る

	if(fRate < 0.0f || fRate >= 10.f){ return;}
	if(fRate == w->fRate){             return;}

	w->fRate = fRate;
	w->pMP->put_Rate(fRate);
}



/*------------------------------------------------------------------------------*
| <<< ムービーテクスチャーに流し込み再生 >>>
|	入力	iNum  =	ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			iLoop =	ループ数				※-1で無限
*------------------------------------------------------------------------------*/
bool	movie_to_texture_play(int iNum, int iLoop)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(	w->bPlayState						// 再生中ならば帰る
	|| !w->bLoadFlag){ return false;}		// ロードしていないならば帰る

	if(FAILED(w->pMC->Run())){ return sys_error("ムービーの再生ができませんでした。");}
	w->bPlayState =	true;					// 再生実行フラグのセット
	w->iLoop	  =	iLoop;					// ループ数
	w->uiFrame    = 0;						// 再生位置

	return true;
}



/*------------------------------------------------------------------------------*
| <<< 再生位置を変更する >>>
|	入力	iNum   = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			llTime = 時間
*------------------------------------------------------------------------------*/
void	movie_to_texture_pos_set(int iNum, LONGLONG llTime)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->bLoadFlag){ return;}

	if(w->pMS != NULL)
	{
		w->pMC->Stop();					// 再生停止
		if(w->pMC != NULL){ w->pMC->Pause();}
		w->pMS->SetPositions(&llTime, AM_SEEKING_AbsolutePositioning, &llTime, AM_SEEKING_NoFlush);
		if(w->pMC != NULL){ w->pMC->Run();}
	}
}



/*------------------------------------------------------------------------------*
| <<< ムービー停止 >>>
|	入力	iNum = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|	※	pause は一時停止、stop は停止となります。
*------------------------------------------------------------------------------*/
void	movie_to_texture_stop(int iNum)
{

	LONGLONG				SetPos = 0,	EndPos = 0;
	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(w->bPlayState &&	w->bLoadFlag)	// 再生中？
	{
		w->pMC->Stop();					// 再生停止
		w->pMS->SetPositions(&SetPos, AM_SEEKING_AbsolutePositioning, &EndPos, AM_SEEKING_NoFlush);
		w->bPlayState =	false;
	}
}



/*------------------------------------------------------------------------------*
| <<< 全ムービー停止 >>>
*------------------------------------------------------------------------------*/
void	movie_to_texture_stop_all(void)
{
	for(int	i =	0; i < MOVIE_MAX; i++) movie_to_texture_stop(i);
}



/*------------------------------------------------------------------------------*
| <<< ムービーの一時停止 >>>
|	入力	iNum = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*------------------------------------------------------------------------------*/
void	movie_to_texture_pause(int iNum, bool bSw)
{

	LONGLONG				lEndPos = 0;
	MOVIE_TO_TEXTURE_WORK	*w      = &movie[iNum];

	if(w->bLoadFlag && w->iPauseFlag == 0)	// ロードしている？
	{
		if(bSw && w->bPlayState)			// 再生停止?
		{
			w->iPauseFlag = 1;
		}
		else if(!bSw && !w->bPlayState)		// 再生再開?
		{
			w->iPauseFlag = -1;
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< 全ムービーの一時停止 >>>
|	入力	bSw = true  : ポーズをかける
|				  false : ポーズ解除
*------------------------------------------------------------------------------*/
void	movie_to_texture_pause_all(bool	bSw)
{
	for(int	i =	0; i < MOVIE_MAX; i++){ movie_to_texture_pause(i, bSw);}
}



/*------------------------------------------------------------------------------*
| <<< システムが使用する全ムービーの一時停止(非アクティブ時に使用) >>>
|	入力	bSw = true  : ポーズをかける
|				  false : ポーズ解除
*------------------------------------------------------------------------------*/
void	movie_to_texture_system_pause(bool bSw)
{

	for(int i = 0; i < MOVIE_MAX; i++)
	{
		LONGLONG				lEndPos = 0;
		MOVIE_TO_TEXTURE_WORK	*w      = &movie[i];

		if(!w->bLoadFlag){ continue;}		// ロードしている？
		
		if(bSw && !w->bSystemPause)			// 再生停止?
		{
			if(w->pMS != NULL){ w->pMS->GetPositions(&w->llPausePosition, &lEndPos);}
			if(w->pMC != NULL){ w->pMC->Pause();}
			w->bSystemPause = true;			// 強制停止
		}
		else if(!bSw && w->bSystemPause)	// 再生再開?
		{
			if(w->bPlayState)
			{
				if(w->pMS != NULL){ w->pMS->SetPositions(&w->llPausePosition, AM_SEEKING_AbsolutePositioning, &w->llPausePosition, AM_SEEKING_NoFlush);}
				if(w->pMC != NULL){ w->pMC->Run();}
			}
			w->bSystemPause = false;		// 強制停止解除
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< 全ムービーループチェック >>>
*------------------------------------------------------------------------------*/
static	bool	bLoop = false;
static	void	movie_to_texture_loop_check(void)
{

	if(bLoop) return;

	for(int	i =	0; i < MOVIE_MAX; i++)
	{
		MOVIE_TO_TEXTURE_WORK	*w = &movie[i];

		if(!w->bPlayState){ continue;}

		REFTIME	time;
		w->pMP->get_CurrentPosition(&time);
		if(time	>= w->dEndTime)					// ムービーの再生時間が終了した場合
		{
			if((uint)w->iLoop >	0 || w->iLoop == -1)
			{
				LONGLONG	a =	0;
				if(w->pMS != NULL)
				{
					w->pMC->Stop();

					//w->pMS->SetPositions(&a, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
					w->pMP->put_CurrentPosition(0);

					w->pMC->Run();

					//bLoop = true;
					//w->pMP->put_CurrentPosition(0);
				}
			}
			if(w->iLoop	> 0){ w->iLoop--;}
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< ムービーのサイズを得る >>>
|	入力	iNum     = ムービー管理番号(テクスチヤー番号)	※ D3_TEX_MAX2 個まで
|	出力	*piWidth = 横幅
|			*piHeight = 縦幅
*------------------------------------------------------------------------------*/
void	movie_size_get(int iNum, int *piWidth, int *piHeight)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->bLoadFlag){ return;}				// 未ロードの場合

	CTextureRenderer	*c = w->pcTR;		// テクスチャーレンダークラス

	*piWidth  = c->iWidth;
	*piHeight = c->iHeight;
}



/*------------------------------------------------------------------------------*
| <<< ムービーのメイン処理 >>>
|	※ ムービー再生時は game_main で一度呼び出しのこと
*------------------------------------------------------------------------------*/
void	movie_to_texture_main(void)
{

	// ループチェック
	movie_to_texture_loop_check();

	// ポーズチェック
	for(int	i =	0; i < MOVIE_MAX; i++)
	{
		MOVIE_TO_TEXTURE_WORK	*w = &movie[i];

		// 未ロードか調べる
		if(!w->bLoadFlag || w->bSystemPause){ continue;}

		// ポーズは一フレーム取り出した後に掛ける
		if(w->iPauseFlag == 2)
		{
			LONGLONG		lEndPos = 0;
			if(w->pMS != NULL){ w->pMS->GetPositions(&w->llPausePosition, &lEndPos);}
			if(w->pMC != NULL){ w->pMC->Pause();}
			w->bPlayState = false;
			w->iPauseFlag = 0;
		}
		// 一時停止解除の場合
		if(w->iPauseFlag == -1)
		{
			if(w->pMS != NULL){ w->pMS->SetPositions(&w->llPausePosition, AM_SEEKING_AbsolutePositioning, &w->llPausePosition, AM_SEEKING_NoFlush);}
			if(w->pMC != NULL){ w->pMC->Run();}
			w->bPlayState = true;
			w->iPauseFlag = 0;
		}

		//--- 転送 --------------------------------------------------
#ifdef	INTERMEDIATE_BUFFER
		if(!w->bTrans){ continue;}

		w->bTrans = false;

		D3DLOCKED_RECT	d3dlr;
		if(FAILED(d3.tex[i]->LockRect(0, &d3dlr, 0, 0))){ continue;}
	
		BYTE	*pbBuf = static_cast<byte *>(d3dlr.pBits);

		memcpy(pbBuf, w->pdwBuf, w->uiSize);
		if(FAILED(d3.tex[i]->UnlockRect(0))){ continue;}
#endif
	}
}

