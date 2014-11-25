//
//	DirectAudio	関係のプログラム
//

//	※以下の作業を行うことで MP3 が読み込めます。
//	午後のコーダー　　	gogo ファイル名	-riff wave
//	SCMPX				Encoding MPx ->	WAVEヘッダにチェック



//--- スイッチ ------------------------------------------------------------------------------------
#pragma warning(disable:4819)
#define	__DSOUND

//--- インクルード --------------------------------------------------------------------------------
#ifndef	__DX90B
	#include <DShow.h>
#endif

//--- 定数 ----------------------------------------------------------------------------------------
#define	MP3_MAX			SND_MAX			// ＭＰ３の同時再生数

enum									// ＭＰ３の再生状態
{
	__STOP,
	__PLAY,
	__PAUSE,
};


//--- ライブラリィ --------------------------------------------------------------------------------
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "strmiids.lib")


//--- 構造体 --------------------------------------------------------------------------------------
#ifdef	__SND

struct	DA_WORK							// DirectAudio 用ワーク
{
	IDirectMusicPerformance8	*pPerformance;		// パフォーマンス
	IDirectMusicLoader8			*pLoader;			// ローダー

	LPDIRECTSOUND8				pDS;		// DirectSoundオブジェクト
	LPDIRECTSOUNDBUFFER			pPB;		// プライマリバッファ
	LONG						lInitVolume;// 初期ボリューム

};

struct	SND_WORK						// 読み込みデータの諸情報	※SND_MAX 個作成する
{
	//--- Sound、Midi 用 --------------------------------------------
	ushort						usDelay;				// 遅延時間
	ushort						usStopCount;			// 割り込み禁止カウント
	bool						bLoop;					// true : 繰り返し再生
	bool						bAfterLoad;				// true : 読み込んでないのでロードする
	bool						bSystemPause;			// システム側の強制ポーズ(非アクティブ時に使用)
	char						strFile[MAX_PATH + 1];	// ファイル名
	IDirectMusicSegment8		*pSegment;				// セグメント	※ここに読み込まれる
	IDirectMusicSegmentState	*pState;				// セグメント状態
	IDirectMusicAudioPath8		*pAudioPath;			// オーディオパス
	MUSIC_TIME					lTime;					// 停止時間
	bool						bLoopReplay;			// true : ループ再生が中断された

	//--- DirectSound -----------------------------------------------
	LPDIRECTSOUNDBUFFER	pSB;					// セカンダリバッファ
	DWORD				dwPosition;				// 一時停止位置
	bool				bPause;					// true : 一時停止中である

	//--- 以下、MP3用 -----------------------------------------------
	bool				bMp3;					// true : DirectShow を使用している
	IGraphBuilder		*pGraph;				// 動画ファイル再生用データ構造体
	IBasicAudio			*pAudio;				// ボリューム、パン
	IMediaControl		*pMediaControl;
	IMediaSeeking		*pMediaSeeking;
	IMediaEventEx		*pEvent;
	LONGLONG			llPausePosition;		// 一時停止位置
	bool				bStandby;
	MMRESULT			uiID;					// タイマーＩＤ
	TIMECAPS			ptc;
	int					iPlayState;				// 再生状態
	bool				bLoad;					// true : 何らかのデータを読み込んでいる
	float				fVolume;				// 音量	-10000 ～ 0	(最大)
	float				fVolumeSpeed;
	float				fPitch;					// ピッチ
	int					iTime;					// 音量変更時間
};


//--- 変数 ----------------------------------------------------------------------------------------
static	DA_WORK		da;						// DirectAudio 用のワーク
static	SND_WORK	*snd_work;				// サウンドのワーク	malloc により SND_MAX 個作成する

static	void	segment_release(int	iNum);


//--- DirectAudio 用 ------------------------------------------------
static	void	segment_release(int	iNum);
static	bool	CreateBuffer(int iNum, HMMIO hMMI, LPWAVEFORMATEX pFmt, DWORD dwLen);
static	void	snd_system_pause(int iNum, bool bSw);

//--- DirectSound 用 ------------------------------------------------
static	void	WavPause(int iNum, bool bSw);
static	void	WavSystemPause(int iNum, bool bSw);
static	void	WavPlay(int iNum, bool bLoop);
static	void	WavStop(int iNum);
static	void	WavFree(int iNum);
static	bool	WavLoad(int iNum, char *strFile);
static	void	WavVolume(int iNum, float fVolume);
static	void	WavPitch (int iNum, float fPitch);

//--- ＭＰ３ --------------------------------------------------------
static	void	mp3_system_pause(int iNum, bool bSw);
static	void	mp3_play(int iNum);
static	void	mp3_stop(int iNum);
static	void	mp3_free(int iNum);
static	bool	mp3_load(int iNum, char *strFile, bool bLoop);
static	void	mp3_volume(int iNum, float fVolume, int iTime);
static	void	mp3_pause(int iNum, bool bSw);
static	void	mp3_main(void);



/*------------------------------------------------------------------------------*
|	<<< ウェーブフォーマットを取得 >>>
|	入力	dwFreq = サンプリング周波数
|			wBit   = 量子化ビット数
|			wCh    = チャンネル数
*------------------------------------------------------------------------------*/
inline	WAVEFORMATEX	GetWaveFormat(DWORD dwFreq, WORD wBit, WORD wCh)
{
	WAVEFORMATEX	wfmt;

	wfmt.wFormatTag		 = WAVE_FORMAT_PCM;
	wfmt.nChannels		 = wCh;
	wfmt.nSamplesPerSec	 = dwFreq;
	wfmt.wBitsPerSample  = wBit;
	wfmt.nBlockAlign	 = (wBit / 8) * wCh;
	wfmt.nAvgBytesPerSec = dwFreq * wfmt.nBlockAlign;
	wfmt.cbSize			 = 0;

	return	wfmt;
}



/*------------------------------------------------------------------------------*
| <<< サウンド処理初期化 >>>
*------------------------------------------------------------------------------*/
bool	snd_init(void)
{
	//--- すでに初期化済みか調べる ----------------------------------
	if(snd_work != NULL){ return false;}

	//--- COM の初期化 ----------------------------------------------
	if(FAILED(CoInitialize(NULL))){ return false;}

	//--- メモリーの確保 --------------------------------------------
	snd_work = (SND_WORK*)malloc(sizeof(SND_WORK) *	SND_MAX);
	memset(snd_work, 0,	sizeof(SND_WORK) * SND_MAX);

	//--- 通常のサウンド --------------------------------------------
	HRESULT	hr;								// パフォーマンスの作成
	hr = CoCreateInstance(CLSID_DirectMusicPerformance,	NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void **)&da.pPerformance);
	if(FAILED(hr)){ return sys_error("SND: パフォーマンスの作成に失敗しました。");}
	hr = da.pPerformance->InitAudio(		// パフォーマンスの初期化
		NULL,									// IDirectMusicインターフェイスは不要
		NULL,									// IDirectSoundインターフェイスは不要
		sys.hWnd,								// ウィンドウのハンドル
		DMUS_APATH_DYNAMIC_STEREO,
		64,										// パフォーマンス・チャンネルの数
		DMUS_AUDIOF_ALL,						// シンセサイザの機能
		NULL);									// オーディオ・パラメータにはデフォルトを使用

	if(FAILED(hr)){ return sys_error("SND: パフォーマンスの初期化の初期化に失敗しました。");}
											// ローダーの作成
	hr = CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, (void **)&da.pLoader);
	if(FAILED(hr)){ return sys_error("SND: ローダーの作成に失敗しました。");}

	//--- DirectSound -----------------------------------------------
	DirectSoundCreate8(NULL, &da.pDS, NULL);
	da.pDS->SetCooperativeLevel(sys.hWnd, DSSCL_PRIORITY);

	DSBUFFERDESC	desc;
	ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize  = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLPAN;

	da.pDS->CreateSoundBuffer(&desc, &da.pPB, NULL);
											// ウェーブフォーマットの設定
	WAVEFORMATEX	wfmt = GetWaveFormat(44100, 16, 2);
	da.pPB->SetFormat(&wfmt);
	da.pPB->GetVolume(&da.lInitVolume);		// 初期ボリュームの取得

	return true;
}



/*------------------------------------------------------------------------------*
| <<< セグメントの開放 >>>
|	入力	iNum = 管理番号
*------------------------------------------------------------------------------*/
static	void	segment_release(int	iNum)
{

	SND_WORK	*s = &snd_work[iNum];
											// 音色データのアンロード
	if(s->pSegment){ s->pSegment->Unload(da.pPerformance);}
	SAFE_RELEASE(s->pSegment);				// セグメント開放
	SAFE_RELEASE(s->pState);				// ステイタス開放
	SAFE_RELEASE(s->pAudioPath);			// オーディオパス解放
}



/*------------------------------------------------------------------------------*
| <<< サウンドバッファの作成 >>>
|	入力	hMMMI = ファイルハンドラ
|			pFmt  = ウエーブフォーマット
|			dwLen = ウエーブサイズ
*------------------------------------------------------------------------------*/
static	bool	CreateBuffer(int iNum, HMMIO hMMI, LPWAVEFORMATEX pFmt, DWORD dwLen)
{

	//--- バッファの作成 --------------------------------------------
	DSBUFFERDESC	desc;
	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize		   = sizeof(DSBUFFERDESC);
	desc.dwFlags 	   = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	desc.dwBufferBytes = dwLen;
	desc.lpwfxFormat   = pFmt;

	HRESULT	hr = da.pDS->CreateSoundBuffer(&desc, &snd_work[iNum].pSB, NULL);
	if(FAILED(hr))
	{										// データが短くてエフェクトを使用できない
		if(hr == DSERR_BUFFERTOOSMALL){ return sys_error("data is too small.");}
											//メモリ不足（サウンドカード内の？）
		if(hr == DSERR_OUTOFMEMORY   ){ return sys_error("out of memory.");}
		return sys_error("サウンドバッファが作成できません。");
	}

	//--- バッファのロック ------------------------------------------
	LPVOID	write1, write2;
	DWORD	length1,length2;
	if(snd_work[iNum].pSB->Lock(0, dwLen, &write1, &length1, &write2, &length2, 0) == DSERR_BUFFERLOST)
	{
		snd_work[iNum].pSB->Restore();
		return false;
	}
	//--- dataチャンクをバッファにロード ----------------------------
	if((DWORD)mmioRead(hMMI,(char *)write1, length1) == length1)
	{
		if(length2 != 0){ mmioRead(hMMI, (char *)write2, length2);}
	}
	//--- バッファのアンロック --------------------------------------
	if(snd_work[iNum].pSB->Unlock(write1, length1, write2, length2) != DS_OK) return false;

	return true;
}



/*------------------------------------------------------------------------------*
| <<< サウンド一時停止 >>>
|	入力	iNum = 管理番号
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*------------------------------------------------------------------------------*/
static	void	WavPause(int iNum, bool bSw)
{

	if(snd_work[iNum].pSB == NULL){ return;}
	
	//--- ステイタス取得 --------------------------------------------
	SND_WORK	*s = &snd_work[iNum];
	DWORD	dwStatus;
	s->pSB->GetStatus(&dwStatus);

	//--- 一時停止 --------------------------------------------------
	if(bSw && dwStatus & DSBSTATUS_PLAYING && !s->bPause)
	{
		s->pSB->GetCurrentPosition(&s->dwPosition, NULL);
		s->pSB->Stop();
		s->bPause = true;
	}
	//--- 一時停止解除 ----------------------------------------------
	else if(!bSw && s->bPause)
	{
		s->pSB->SetCurrentPosition(s->dwPosition);

		s->pSB->Play(0, 0, s->bLoop ? DSBPLAY_LOOPING : 0);
		s->bPause = false;
	}
}



/*------------------------------------------------------------------------------*
| <<< システムで使用するサウンド一時停止 >>>
|	入力	iNum = 管理番号
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*------------------------------------------------------------------------------*/
static	void	WavSystemPause(int iNum, bool bSw)
{

	if(snd_work[iNum].pSB == NULL){ return;}
	
	//--- ステイタス取得 --------------------------------------------
	SND_WORK	*s = &snd_work[iNum];
	DWORD	dwStatus;
	s->pSB->GetStatus(&dwStatus);

	//--- 一時停止 --------------------------------------------------
	if(bSw && dwStatus & DSBSTATUS_PLAYING && !s->bSystemPause)
	{
		s->pSB->GetCurrentPosition(&s->dwPosition, NULL);
		s->pSB->Stop();
		s->bSystemPause = true;
	}
	//--- 一時停止解除 ----------------------------------------------
	else if(!bSw && s->bSystemPause)
	{
		s->pSB->SetCurrentPosition(s->dwPosition);
		s->pSB->Play(0, 0, s->bLoop ? DSBPLAY_LOOPING : 0);
		s->bSystemPause = false;
	}
}



/*------------------------------------------------------------------------------*
| <<< サウンドの再生 >>>
|	入力	iNum  = 管理番号
|			bLoop = true : ループ再生
*------------------------------------------------------------------------------*/
static	void	WavPlay(int iNum, bool bLoop)
{

	LPDIRECTSOUNDBUFFER	p = snd_work[iNum].pSB;

	if(p == NULL){ return;}
//	if(p->SetCurrentPosition(0L) != DS_OK)
	{
		p->Stop();							// 再生中に再生位置をセットできないカードがある模様
		p->SetCurrentPosition(0);
	}
	HRESULT	hr = p->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0);

	//--- リロード処理 ----------------------------------------------
	if(hr == DSERR_BUFFERLOST)
	{
		//sys_error("DSERR_BUFFERLOST:%s", m_strName.c_str());
		//PrimaryBufferVerify();				// 先にプライマリバッファを検証する
		DWORD	dw;
		da.pPB->GetStatus(&dw);
		if(dw & DSBSTATUS_BUFFERLOST)		// ロスト時は復元
		{
			sys_error("PB:DSBSTATUS_BUFFERLOST");
			da.pPB->Restore();
		}
		p->Restore();						// どの道 Free するが念のため
		WavLoad(iNum, snd_work[iNum].strFile);
	}
}



/*------------------------------------------------------------------------------*
| <<< サウンドの音量をセット >>>
|	入力	iNum  = 管理番号
|			fVol  = 音量(0.0f ～ 1.0f(最大(default))
*------------------------------------------------------------------------------*/
static	void	WavVolume(int iNum, float fVol)
{

	if(snd_work[iNum].pSB == NULL){ return;}
	
	SND_WORK	*s = &snd_work[iNum];

	s->fVolume = ((1 - fVol) * (DSBVOLUME_MIN - DSBVOLUME_MAX));
	if(     s->fVolume < DSBVOLUME_MIN){ s->fVolume = DSBVOLUME_MIN;}
	else if(s->fVolume > DSBVOLUME_MAX){ s->fVolume = DSBVOLUME_MAX;}

	s->pSB->SetVolume((LONG)s->fVolume);
}



/*------------------------------------------------------------------------------*
| <<< サウンドの再生周波数をセット >>>
|	入力	iNum   = 管理番号
|			fPitch = 44100 をデフォルト音量としての再生周波数
*------------------------------------------------------------------------------*/
static	void	WavPitch(int iNum, float fPitch)
{

	if(snd_work[iNum].pSB == NULL){ return;}
	
	SND_WORK	*s = &snd_work[iNum];

	//s->fPitch = ((fPitch) * (DSBFREQUENCY_MAX - DSBFREQUENCY_MIN));
	s->fPitch = fPitch;
	if(     s->fPitch < DSBFREQUENCY_MIN){ s->fPitch = DSBFREQUENCY_MIN;}
	else if(s->fPitch > DSBFREQUENCY_MAX){ s->fPitch = DSBFREQUENCY_MAX;}

	s->pSB->SetFrequency((LONG)s->fPitch);
}



/*------------------------------------------------------------------------------*
| <<< サウンド停止 >>>
|	入力	iNum = 読み込み番号
*------------------------------------------------------------------------------*/
static	void	WavStop(int iNum)
{
	if(snd_work[iNum].pSB != NULL){ snd_work[iNum].pSB->Stop();}
}



/*------------------------------------------------------------------------------*
| <<< サウンド開放 >>>
|	入力	iNum = 読み込み番号
*------------------------------------------------------------------------------*/
static	void	WavFree(int iNum)
{
	WavStop(iNum);
	SAFE_RELEASE(snd_work[iNum].pSB);
}



/*------------------------------------------------------------------------------*
| <<< 音声データの読込み >>>
|	入力	iNum    = 読み込み番号
|			strFile = ファイル名
*------------------------------------------------------------------------------*/
static	bool	WavLoad(int iNum, char *strFile)
{
	//--- 既存なら解放 ----------------------------------------------
	if(snd_work[iNum].pSB != NULL){ WavFree(iNum);}

	//---------------------------------------------------------------
	HMMIO			hMMI;
	MMCKINFO		parent, child;
	WAVEFORMATEX	wfmtx;
	DWORD			len;
	BOOL			ret = FALSE;

	parent.ckid			= (FOURCC)0;
	parent.cksize 		= 0;
	parent.fccType		= (FOURCC)0;
	parent.dwDataOffset = 0;
	parent.dwFlags		= 0;
	child				= parent;

	//--- オープン --------------------------------------------------
	hMMI = mmioOpen(strFile, NULL, MMIO_READ | MMIO_ALLOCBUF);
	if(hMMI == NULL){ return sys_error("WavLoad %s が開けません", strFile);}

	//--- RIFFチャンクの読み込み＆チェック --------------------------
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hMMI, &parent, NULL, MMIO_FINDRIFF) != 0){ return sys_error("WavLoad : %s WAVE形式ではありません", strFile);}

	//--- fmtチャンクの読み込み＆チェック ---------------------------
	child.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(mmioDescend(hMMI, &child, &parent, 0) != 0){ return sys_error("WavLoad : %s WAVE形式ではありません", strFile);}

	//--- フォーマットの取得 ----------------------------------------
	mmioRead(hMMI, (char*)&wfmtx, sizeof(wfmtx));

	//--- PCMフォーマットか？ ---------------------------------------
	if(wfmtx.wFormatTag != WAVE_FORMAT_PCM){ return sys_error("WavLoad : %s フォーマットが不明です。", strFile);}
	if(mmioAscend(hMMI, &child, 0) != 0){ return sys_error("WavLoad : %s アクセスできません。", strFile);}

	//-- dataチャンクの読み込み＆チェック ---------------------------
	child.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hMMI, &child, &parent, MMIO_FINDCHUNK) != 0){ return sys_error("WavLoad : %s chunk is not found", strFile);}

	//--- dataチャンク長を取得 --------------------------------------
	len = child.cksize;

	//--- バッファを作成、ロード ------------------------------------
	if(CreateBuffer(iNum, hMMI, &wfmtx, len))
	{
		//--- クローズ ----------------------------------------------
		mmioClose(hMMI, 0);
		return true;
	}

	mmioClose(hMMI, 0);
	return false;
}



/*------------------------------------------------------------------------------*
| <<< サウンドの読み込み >>>
|	入力	iNum        = 読み込み番号
|			*strOriFile = WAVEファイル名またはリソース名
|			bLoop       = true : 無限ループ
|			bLoadAtOnce = true : 即時読み込み
|	戻り値	true : 読み込み成功
*------------------------------------------------------------------------------*/
bool	snd_load(int iNum, char *strOriFile, bool bLoop, bool bLoadAtOnce)
{

	if(snd_work	==      NULL){ return false;}
	if((uint)iNum >= SND_MAX){ return false;}

#ifndef __DSOUND
	HRESULT	hr;
	WCHAR	wstrFileName[MAX_PATH + 1];
#endif

	if(da.pLoader == NULL) return false;
	
	snd_free(iNum);							// 読み込みデータを破棄

	SND_WORK	*s = &snd_work[iNum];

	char	strFile[MAX_PATH];
	strcpy(strFile, strOriFile);
	STRLWR(strFile);						// 大文字を小文字へ

	//--- 即時では無い場合の処理 ------------------------------------
	if(!bLoadAtOnce)
	{
		char	path[_MAX_PATH	+ 1];
		char	name[_MAX_FNAME	+ 1];
		char	ext[ _MAX_EXT	+ 1];
		sys_div_path(path, name, ext, strFile);
		sprintf(s->strFile, "%s%s%s", path, name, ext);
		s->bAfterLoad = true;
		s->bLoop      = bLoop;
		return true;
	}
	//---------------------------------------------------------------
		
	//--- mp3 や ループものは DirectShow で処理する ---------------------------
	if(strstr(strFile, ".mp3"))// || bLoop)
	{
		return mp3_load(iNum, strFile, bLoop);
	}

	//--- wav の効果音、MIDI は DirectSound で処理する ------------------------
	s->bMp3 = false;

#ifndef __DSOUND
											// セグメント作成（Waveファイルをロード）
	MultiByteToWideChar(CP_ACP,	0, strFile, -1, wstrFileName, MAX_PATH);

	hr = da.pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,
									   IID_IDirectMusicSegment8,
									   wstrFileName,
									   (LPVOID*)&s->pSegment);
	if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %sが読み込めませんでした。", strFile);}

	if(bLoop)								// セグメントを繰り返し再生する設定にする
	{
		hr = s->pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
		if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s のループ回数設定に失敗しました。", strFile);}
	}
	if(strstr(strFile, ".mid"))				// スタンダードMIDIファイルの場合は、パラメータを設定する
	{
		hr = s->pSegment->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, 0, 0, NULL);
		if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s のパラメータ設定に失敗しました。", strFile);}
	}		
											// バンドのダウンロード（手動）
	hr	= s->pSegment->Download(da.pPerformance);
	if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s のダウンロードに失敗しました。", strFile);}
											// オーディオパスの取得(音量調節用)
	hr = da.pPerformance->CreateStandardAudioPath( 
					DMUS_APATH_DYNAMIC_STEREO,	// パスの種類。
					1,							// パフォーマンス チャンネルの数。
					TRUE,						// ここでアクティブになる。
					&s->pAudioPath);			// オーディオパスを受け取るポインタ。
	if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s のオーディオ・パスの作成に失敗");}
#else

	WavLoad(iNum, strFile);

#endif

	strcpy(s->strFile, strFile);			// ファイル名設定

	s->bLoop = bLoop;						// ループ設定
	return true;
}



/*------------------------------------------------------------------------------*
| <<< サウンドの読み込み >>>
|	入力	iNum        = 読み込み番号
|			*strOriFile = WAVEファイル名またはリソース名
|			bLoop       = true : 無限ループ
|	戻り値	true : 読み込み成功
*------------------------------------------------------------------------------*/
bool	snd_load(int iNum, char *strOriFile, bool bLoop)
{
	return snd_load(iNum, strOriFile, bLoop, !sys.bAfterLoad);
}



/*------------------------------------------------------------------------------*
| <<< サウンドデータの開放 >>>
|	入力	iNum = 管理番号
*------------------------------------------------------------------------------*/
void	snd_free(int iNum)
{

	if(snd_work	==      NULL){ return;}
	if((uint)iNum >= SND_MAX){ return;}

	if(snd_work[iNum].bMp3)
	{
		mp3_free(iNum);
	}
	else if(snd_work[iNum].pSegment != NULL)
	{
#ifdef	__DSOUND
		WavFree(iNum);
#else
		snd_stop(iNum);
		segment_release(iNum);
#endif
	}
}



/*------------------------------------------------------------------------------*
| <<< サウンドの停止 >>>
|	入力	iNum = 管理番号
*------------------------------------------------------------------------------*/
void	snd_stop(int iNum)
{

	if(snd_work	==      NULL){ return;}
	if((uint)iNum >= SND_MAX){ return;}


	SND_WORK	*s = &snd_work[iNum];
	s->usDelay	   = 0;
	
	//--- MP3 時 ----------------------------------------------------
	if(s->bMp3)
	{
		mp3_stop(iNum);
	}
	//--- DirectMusic 時 --------------------------------------------
	else 
#ifndef	__DSOUND
	if(s->pSegment != NULL)
	{
		da.pPerformance->StopEx(s->pSegment, 0, 0);
		s->lTime = 0;
	}
#else
	WavStop(iNum);
#endif
}



/*------------------------------------------------------------------------------*
| <<< サウンドの一時停止 >>>
|	入力	iNum = 管理番号
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*------------------------------------------------------------------------------*/
void	snd_pause(int iNum, bool bSw)
{

	if(snd_work	==      NULL){ return;}
	if((uint)iNum >= SND_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];

	if(s->bMp3)
	{
		mp3_pause(iNum, bSw);
	}
	else
	{
#ifdef	__DSOUND
		WavPause(iNum, bSw);
#else
		if(s->pSegment != NULL && s->pState != NULL)
		{
			if(bSw)
			{
				if(s->bLoop && da.pPerformance->IsPlaying(s->pSegment, NULL) == S_OK)
				{
					s->bLoopReplay = true;
				}
				s->pState->GetSeek(&s->lTime);	// 停止時間を取得(これ、Midiのみ有効)
				da.pPerformance->Stop(NULL, NULL, 0, 0);
			}
			else if(s->bLoop && s->bLoopReplay)	// ループ再生の再開
			{
				s->bLoopReplay = false;
				s->pSegment->SetStartPoint(s->lTime);
				da.pPerformance->PlaySegmentEx(s->pSegment, NULL, NULL, DMUS_SEGF_BEAT, 0, &s->pState, NULL, s->pAudioPath);
			}
		}
#endif
	}
}



/*------------------------------------------------------------------------------*
| <<< サウンド全体ポーズ >>>
|	入力	bSw = true : ポーズを掛ける
*------------------------------------------------------------------------------*/
void	snd_pause_all(bool bSw)
{
//	mp3_pause_all(bSw);
	for(int i = 0; i < SND_MAX; i++){ snd_pause(i, bSw);}
}



/*------------------------------------------------------------------------------*
| <<< システムで使用するサウンドの一時停止(非アクティブ時に使用) >>>
|	入力	iNum = 管理番号
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*------------------------------------------------------------------------------*/
static	void	mp3_system_pause(int iNum, bool bSw);
static	void	snd_system_pause(int iNum, bool bSw)
{

	if(snd_work	==      NULL){ return;}
	if((uint)iNum >= SND_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];

	if(s->bMp3)
	{
		mp3_system_pause(iNum, bSw);
	}
	else
	{
#ifdef	__DSOUND
		WavPause(iNum, bSw);
#else
		if(s->pSegment != NULL && s->pState != NULL)
		{
			if(bSw && !s->bSystemPause)
			{
				s->pState->GetSeek(&s->lTime);
				da.pPerformance->Stop(NULL, NULL, 0, 0);
				// WAV の場合は途中再生ができないので、音量をオフにして誤魔化す
				//s->pAudioPath->SetVolume(-9600, 0);
				//s->bSystemPause = true;
			}
			else if(!bSw && s->bSystemPause)
			{
				//s->pAudioPath->SetVolume(s->fVolume, 0);
				s->pSegment->SetStartPoint(s->lTime);
				da.pPerformance->PlaySegmentEx(s->pSegment, NULL, NULL, NULL/*DMUS_SEGF_BEAT*/, 0, &s->pState, NULL, s->pAudioPath);
				s->bSystemPause = false;
			}
		}
#endif
	}
}



/*------------------------------------------------------------------------------*
| <<< システムでサウンド全体ポーズ >>>
|	入力	bSw = true : ポーズを掛ける
*------------------------------------------------------------------------------*/
void	snd_system_pause_all(bool bSw)
{
	for(int i = 0; i < SND_MAX; i++){ snd_system_pause(i, bSw);}
}



/*------------------------------------------------------------------------------*
| <<< サウンドメイン >>>
*------------------------------------------------------------------------------*/
void	snd_main(void)
{
#ifndef	__SND
	return;
#else

	if(snd_work	== NULL){ return;}

	mp3_main();

	SND_WORK	*s = &snd_work[0];

	for(int	i =	0; i < SND_MAX;	i++, s++)
	{
		if(s->usDelay >	0)			// ディレイ処理
		{
			if((--s->usDelay) == 0){ snd_play(i);}
		}
		else if(s->usStopCount > 0)	// 割り込み禁止処理
		{
			s->usStopCount--;
		}
	}
#endif
}



/*------------------------------------------------------------------------------*
| <<< サウンドの再生 >>>
|	入力	iNum = 管理番号
*------------------------------------------------------------------------------*/
void	snd_play(int iNum)
{

	if(snd_work	  == NULL   ){ return;}
	if((uint)iNum >= SND_MAX){ return;}

	
	SND_WORK	*s = &snd_work[iNum];
	
	if(s->bAfterLoad)
	{
		snd_load(iNum, s->strFile, s->bLoop, true);
		s->bAfterLoad = false;
	}

	if(s->usStopCount > 0){ return;}			// サウンドの二度鳴らし防止
	s->usStopCount = SND_DISABLE_TIME;			// 同番号を受け付けない時間

	//--- MP3 時 ----------------------------------------------------
	if(s->bMp3)
	{
		mp3_play(  iNum);
		mp3_volume(iNum, 1, 0);
	}
	//--- DirectMusic 時 --------------------------------------------
	else
#ifndef	__DSOUND
	if(s->pSegment != NULL)
	{
		da.pPerformance->PlaySegmentEx(s->pSegment, NULL, NULL, NULL/*DMUS_SEGF_BEAT | DMUS_SEGF_SECONDARY*/, 0, &s->pState, NULL, s->pAudioPath);
	}
#else
	WavPlay(iNum, snd_work[iNum].bLoop);
#endif
}



/*------------------------------------------------------------------------------*
| <<< 遅延時間付きサウンドの再生 >>>
|	入力	iNum   = 管理番号
|			iDelay = 遅延時間
*------------------------------------------------------------------------------*/
void	snd_play_delay(int iNum, int iDelay)
{
	snd_work[iNum].usDelay = iDelay;
}



/*------------------------------------------------------------------------------*
| <<< サウンドの音量をセット >>>
|	入力	iNum  = 管理番号
|			fVol  = 音量(0.0f ～ 1.0f(最大(default))
|			iTime = 音量の変化時間(__DSOUND 時のみ)
*------------------------------------------------------------------------------*/
void	snd_volume(int iNum, float fVol, int iTime)
{

	SND_WORK	*s = &snd_work[iNum];

	if(!s->bMp3)
	{
		#ifdef	__DSOUND
			WavVolume(iNum, fVol);
		#else
			if(s->pAudioPath != NULL)
			{									// -9600 が音量 0 なので変換する
				s->fVolume = (fVol - 1) * 9600.f;
				s->pAudioPath->SetVolume((long)s->fVolume, iTime);
			}
		#endif
	}
	else
	{
		mp3_volume(iNum, fVol, iTime);
	}
}



/*------------------------------------------------------------------------------*
| <<< サウンドのピッチ変更 >>>
|	入力	iNum   = 管理番号
|			fPitch = 44100 をデフォルト音量としての再生周波数
|			iTime  = 音量の変化時間(__DSOUND 時のみ)
*------------------------------------------------------------------------------*/
void	snd_pitch(int iNum, float fPitch, int iTime)
{

	SND_WORK	*s = &snd_work[iNum];

	if(!s->bMp3)
	{
		#ifdef	__DSOUND
			WavPitch(iNum, fPitch);
		#else
			if(s->pAudioPath != NULL)
			{									// -9600 が音量 0 なので変換する
				s->fPitch = fPitch;
				s->pAudioPath->SetFrequency((long)s->fPitch, iTime);
			}
		#endif
	}
	else
	{
		//mp3_pitch(iNum, fVol, iTime);
	}
}



/*------------------------------------------------------------------------------*
| <<< サウンド終了処理 >>>
*------------------------------------------------------------------------------*/
void	snd_end(void)
{

	if(snd_work	== NULL) return;

	for(int	i =	0; i < SND_MAX;	++i)
	{
		snd_stop(i);
		snd_free(i);
		mp3_free(i);
	}

	if(da.pPerformance){ da.pPerformance->CloseDown();}
	SAFE_RELEASE(da.pPerformance);
	SAFE_RELEASE(da.pLoader);				// ローダを開放
											// パフォーマンスを開放
	//--- メモリーの開放 --------------------------------------------
	if(snd_work	!= NULL) free(snd_work);
	snd_work = NULL;
}



//--- 以下、ＭＰ３ --------------------------------------------------------------------------------

/*----------------------------------------------------------------------*
| <<< ＭＰ３開放 >>>
|	入力	iNum = 管理番号
*----------------------------------------------------------------------*/
static	void	mp3_free(int iNum)
{

	SND_WORK	*w = &snd_work[iNum];

	if(!w->bLoad) return;					// 読み込んでいないならばここで帰る

	if(w->bStandby){ mp3_stop(iNum);}		// 再生中だった場合は停止する

	w->iPlayState = __STOP;					// 停止状態
	if(w->bStandby){ timeKillEvent(w->uiID); timeEndPeriod(w->ptc.wPeriodMin);}
											// 割り当てたインターフェースを解放
	if(w->pMediaSeeking){ w->pMediaSeeking->Release(); w->pMediaSeeking = NULL;}
	if(w->pAudio)		{ w->pAudio->Release();		   w->pAudio		= NULL;}
	if(w->pMediaControl){ w->pMediaControl->Release(); w->pMediaControl = NULL;}
	if(w->pGraph)		{ w->pGraph->Release();		   w->pGraph		= NULL;}
	if(w->pEvent)		{ w->pEvent->Release();		   w->pEvent		= NULL;}

	w->bLoad    = false;					// 読み込んでいない状態にする
	w->bStandby = false;
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３全停止 >>>
*----------------------------------------------------------------------*/
void	mp3_free(void)
{
	for(int	i =	0; i < MP3_MAX;	++i) mp3_free(i);
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３のコールバック >>>
|	入力	iNum = 管理番号
*----------------------------------------------------------------------*/
long	alEventCode[10];
void	snd_mp3_proc(int iNum)
{

	long		lEventCode,	lParam1, lParam2;
	LONGLONG	llSetPos = 0, llEndPos = 0;
	SND_WORK	*s = &snd_work[iNum];

	if(!s->bLoad)    return;				// 読み込んでいないならばここで帰る
	if(!s->bStandby) return;				// 停止中ならばリターン

	s->pEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 100);
	if(lEventCode == EC_COMPLETE
	|| lEventCode == EC_END_OF_SEGMENT
	|| lEventCode == EC_ERROR_STILLPLAYING
	|| lEventCode == EC_ERRORABORT
	|| lEventCode == EC_STREAM_ERROR_STOPPED
	|| lEventCode == EC_USERABORT
	|| lEventCode == EC_PAUSED
	|| lEventCode == EC_STREAM_ERROR_STILLPLAYING)
	{										// もしループ再生指定だった場合はループする

		for(int i = 0; i < 9; i++)
		{
			alEventCode[i] = alEventCode[i + 1];
		}
		alEventCode[9] = lEventCode;
		/*
d3_printf("%d,%d,%d,%d,%d,\r\n"
		  "%d,%d,%d,%d,%d,\r\n",
		  alEventCode[0],
		  alEventCode[1],
		  alEventCode[2],
		  alEventCode[3],
		  alEventCode[4],
		  alEventCode[5],
		  alEventCode[6],
		  alEventCode[7],
		  alEventCode[8],
		  alEventCode[9]
		  );
*/
		if(s->bLoop)
		{
			s->pMediaSeeking->SetPositions(&llSetPos, AM_SEEKING_AbsolutePositioning, &llEndPos, AM_SEEKING_NoFlush);
		}
		else if(s->iPlayState == __PLAY)	// それ以外の場合はクリーンアップ
		{
			mp3_stop(iNum);
		}
	}
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３ファイルのロード >>>
|	入力	iNum     = 管理番号
|			*strFile = ＭＰ３ファイル名
|			bLoop    = true ならループ
*----------------------------------------------------------------------*/
static	bool	mp3_load(int iNum, char *strFile, bool bLoop)
{

	if(snd_work	  == NULL){     return false;}
	if((uint)iNum >=  MP3_MAX){ return false;}


	WCHAR		path[MAX_PATH];
	SND_WORK	*s = &snd_work[iNum];

	s->bMp3 = true;

	if(s->bStandby){ mp3_stop(iNum);}		// 再生中だった場合はまずクリーンアップを行う
											// ファイル名をUnicodeに変換する
	MultiByteToWideChar(CP_ACP,	0, strFile, -1, path, sizeof(path) / sizeof(path[0]));
											// フィルタグラフマネージャを作成し、
											// インターフェイスをクエリする。
	if(!(FAILED(CoCreateInstance(CLSID_FilterGraph,	NULL, CLSCTX_INPROC, IID_IGraphBuilder,	(void **)&s->pGraph))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IBasicAudio,	 (void **)&s->pAudio))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IMediaControl, (void **)&s->pMediaControl))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IMediaEventEx, (void **)&s->pEvent))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IMediaSeeking, (void **)&s->pMediaSeeking))))
	{										// グラフを作成。重要:
											// 使用システムのファイル文字列に変更すること。
		if(s->pGraph->RenderFile(path, NULL) ==	S_OK)
		{
			s->pMediaControl->Run();		// グラフの実行。
			//s->pMediaControl->Pause();
			s->pMediaControl->Stop();
			s->pAudio->put_Volume((int)s->fVolume);
			long evCode;
			s->pEvent->WaitForCompletion(INFINITE, &evCode);

			s->bStandby	  = true;			// 再生実行フラグのセット
			s->bLoop	  = bLoop;			// ループをセット
			s->bLoad      = true;
			s->iPlayState = __STOP;
											// イベント通知のセット
			int	d =	MP3_MESSAGE0 + iNum;
			s->pEvent->SetNotifyWindow((OAHWND)sys.hWnd, d,	0);
			return true;					// 終了
		}
	}}}}}

	return false;							// 終了
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３ファイルの一時停止 >>>
|	入力	iNum = 管理番号
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*----------------------------------------------------------------------*/
static	void	mp3_pause(int iNum, bool bSw)
{

	if(snd_work	  == NULL){     return;}
	if((uint)iNum >=  MP3_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];

	if(!s->bLoad){ return;}

	if(s->bStandby)
	{
		LONGLONG	llEndPos = 0;

		if(bSw && s->iPlayState == __PLAY)
		{
			s->pMediaSeeking->GetPositions(&s->llPausePosition, &llEndPos);
			s->pMediaControl->Pause();
			s->iPlayState =	__PAUSE;
		}
		else if(!bSw && s->iPlayState == __PAUSE)
		{
			s->pMediaControl->Run();
			s->pMediaSeeking->SetPositions(&s->llPausePosition, AM_SEEKING_AbsolutePositioning, &s->llPausePosition, AM_SEEKING_NoFlush);
			s->iPlayState = __PLAY;
		}
	}
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３ファイルの一時停止 >>>
|	入力	iNum = 管理番号
|			bSw  = true  : ポーズをかける
|				   false : ポーズ解除
*----------------------------------------------------------------------*/
static	void	mp3_system_pause(int iNum, bool bSw)
{

	if(snd_work	  == NULL){     return;}
	if((uint)iNum >=  MP3_MAX){ return;}


	SND_WORK	*s     = &snd_work[iNum];

	if(!s->bLoad){ return;}
	
	if(s->bStandby)// && s->iPlayState == __PLAY)
	{
		LONGLONG	llEndPos = 0;

		if(bSw && !s->bSystemPause)
		{
			s->pMediaSeeking->GetPositions(&s->llPausePosition, &llEndPos);
			s->pMediaControl->Pause();
			s->bSystemPause = true;
		}
		else if(s->bSystemPause)
		{
			s->pMediaControl->Run();
			s->pMediaSeeking->SetPositions(&s->llPausePosition, AM_SEEKING_AbsolutePositioning, &s->llPausePosition, AM_SEEKING_NoFlush);
			s->bSystemPause = false;
		}
	}
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３ファイルの一時停止 >>>
|	入力	iNum = 管理番号
*----------------------------------------------------------------------*/
static	void	mp3_stop(int iNum)
{

	LONGLONG	SetPos = 0,	EndPos = 0;

	if(!snd_work[iNum].bLoad){ return;}
	if((uint)iNum >= MP3_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];
	if(s->bStandby)							// 再生実行フラグのセット
	{
		s->pMediaControl->Pause();
		s->pMediaSeeking->SetPositions(&SetPos, AM_SEEKING_AbsolutePositioning,	&EndPos, AM_SEEKING_NoFlush);
		s->iPlayState = __STOP;
	}
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３ファイルの再生 >>>
|	入力	iNum = 管理番号
*----------------------------------------------------------------------*/
static	void	mp3_play(int iNum)
{

	if((uint)iNum >= MP3_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];

	if(!s->bLoad) return;					// 未読み込みならば処理しない
//	mp3_stop(iNum);							// 再生中だった場合はストップ
	if(s->iPlayState == __STOP)				// 音量を最大にして再生開始
	{
		s->iTime      = 0;
		s->fVolume    = 0;
		s->pAudio->put_Volume((int)s->fVolume);
		s->pMediaControl->Run();
		s->iPlayState = __PLAY;
	}
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３の音量設定 >>>
|	入力	iNum    = 管理番号
|			fVolume = 0 ～ 1 (最大)
|			iTime   = 変更時間
*----------------------------------------------------------------------*/
static	void	mp3_volume(int iNum, float fVolume, int iTime)
{

	int		iStart = iNum, iEnd = iNum + 1;

											// -1 なら全てが対象となる
	if(iNum == -1){ iStart = 0, iEnd = MP3_MAX;}

	SND_WORK	*s = &snd_work[iStart];
	for(int	i =	iStart; i < iEnd; i++, s++)
	{
		if(!s->bLoad){ continue;}			// mp3 のデータをロードしているか調べる
		fVolume = (fVolume - 1) * 10000.f;			// -10000 が音量 0 なので変換する
		if(iTime <= 0)
		{
			s->fVolume = fVolume;
			if(s->iPlayState == __PLAY){ s->pAudio->put_Volume((int)fVolume);}
		}
		else
		{
			s->iTime		= iTime;
			s->fVolumeSpeed = (s->fVolume + fVolume) / (float)iTime;
		}
	}
}



/*----------------------------------------------------------------------*
| <<< ＭＰ３の音量監視 >>>
*----------------------------------------------------------------------*/
static	void	mp3_main(void)
{

	SND_WORK	*s = snd_work;

	for(int	i =	0; i < MP3_MAX;	i++, s++)
	{
		if(!s->bLoad) continue;				// 未読み込みならば処理しない
		//--- 以下、音量変化処理 ------------------------------------
		if(s->iPlayState == __PLAY && s->iTime > 0)
		{
			s->iTime--;
			s->fVolume += s->fVolumeSpeed;
			if(     s->fVolume >      0){ s->fVolume =      0;}
			else if(s->fVolume < -10000){ s->fVolume = -10000;}
			s->pAudio->put_Volume((int)s->fVolume);
		}
	}
}



#endif