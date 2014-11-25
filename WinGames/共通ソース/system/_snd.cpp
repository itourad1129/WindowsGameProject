//
//	DirectAudio	�֌W�̃v���O����
//

//	���ȉ��̍�Ƃ��s�����Ƃ� MP3 ���ǂݍ��߂܂��B
//	�ߌ�̃R�[�_�[�@�@	gogo �t�@�C����	-riff wave
//	SCMPX				Encoding MPx ->	WAVE�w�b�_�Ƀ`�F�b�N



//--- �X�C�b�` ------------------------------------------------------------------------------------
#pragma warning(disable:4819)
#define	__DSOUND

//--- �C���N���[�h --------------------------------------------------------------------------------
#ifndef	__DX90B
	#include <DShow.h>
#endif

//--- �萔 ----------------------------------------------------------------------------------------
#define	MP3_MAX			SND_MAX			// �l�o�R�̓����Đ���

enum									// �l�o�R�̍Đ����
{
	__STOP,
	__PLAY,
	__PAUSE,
};


//--- ���C�u�����B --------------------------------------------------------------------------------
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "strmiids.lib")


//--- �\���� --------------------------------------------------------------------------------------
#ifdef	__SND

struct	DA_WORK							// DirectAudio �p���[�N
{
	IDirectMusicPerformance8	*pPerformance;		// �p�t�H�[�}���X
	IDirectMusicLoader8			*pLoader;			// ���[�_�[

	LPDIRECTSOUND8				pDS;		// DirectSound�I�u�W�F�N�g
	LPDIRECTSOUNDBUFFER			pPB;		// �v���C�}���o�b�t�@
	LONG						lInitVolume;// �����{�����[��

};

struct	SND_WORK						// �ǂݍ��݃f�[�^�̏����	��SND_MAX �쐬����
{
	//--- Sound�AMidi �p --------------------------------------------
	ushort						usDelay;				// �x������
	ushort						usStopCount;			// ���荞�݋֎~�J�E���g
	bool						bLoop;					// true : �J��Ԃ��Đ�
	bool						bAfterLoad;				// true : �ǂݍ���łȂ��̂Ń��[�h����
	bool						bSystemPause;			// �V�X�e�����̋����|�[�Y(��A�N�e�B�u���Ɏg�p)
	char						strFile[MAX_PATH + 1];	// �t�@�C����
	IDirectMusicSegment8		*pSegment;				// �Z�O�����g	�������ɓǂݍ��܂��
	IDirectMusicSegmentState	*pState;				// �Z�O�����g���
	IDirectMusicAudioPath8		*pAudioPath;			// �I�[�f�B�I�p�X
	MUSIC_TIME					lTime;					// ��~����
	bool						bLoopReplay;			// true : ���[�v�Đ������f���ꂽ

	//--- DirectSound -----------------------------------------------
	LPDIRECTSOUNDBUFFER	pSB;					// �Z�J���_���o�b�t�@
	DWORD				dwPosition;				// �ꎞ��~�ʒu
	bool				bPause;					// true : �ꎞ��~���ł���

	//--- �ȉ��AMP3�p -----------------------------------------------
	bool				bMp3;					// true : DirectShow ���g�p���Ă���
	IGraphBuilder		*pGraph;				// ����t�@�C���Đ��p�f�[�^�\����
	IBasicAudio			*pAudio;				// �{�����[���A�p��
	IMediaControl		*pMediaControl;
	IMediaSeeking		*pMediaSeeking;
	IMediaEventEx		*pEvent;
	LONGLONG			llPausePosition;		// �ꎞ��~�ʒu
	bool				bStandby;
	MMRESULT			uiID;					// �^�C�}�[�h�c
	TIMECAPS			ptc;
	int					iPlayState;				// �Đ����
	bool				bLoad;					// true : ���炩�̃f�[�^��ǂݍ���ł���
	float				fVolume;				// ����	-10000 �` 0	(�ő�)
	float				fVolumeSpeed;
	float				fPitch;					// �s�b�`
	int					iTime;					// ���ʕύX����
};


//--- �ϐ� ----------------------------------------------------------------------------------------
static	DA_WORK		da;						// DirectAudio �p�̃��[�N
static	SND_WORK	*snd_work;				// �T�E���h�̃��[�N	malloc �ɂ�� SND_MAX �쐬����

static	void	segment_release(int	iNum);


//--- DirectAudio �p ------------------------------------------------
static	void	segment_release(int	iNum);
static	bool	CreateBuffer(int iNum, HMMIO hMMI, LPWAVEFORMATEX pFmt, DWORD dwLen);
static	void	snd_system_pause(int iNum, bool bSw);

//--- DirectSound �p ------------------------------------------------
static	void	WavPause(int iNum, bool bSw);
static	void	WavSystemPause(int iNum, bool bSw);
static	void	WavPlay(int iNum, bool bLoop);
static	void	WavStop(int iNum);
static	void	WavFree(int iNum);
static	bool	WavLoad(int iNum, char *strFile);
static	void	WavVolume(int iNum, float fVolume);
static	void	WavPitch (int iNum, float fPitch);

//--- �l�o�R --------------------------------------------------------
static	void	mp3_system_pause(int iNum, bool bSw);
static	void	mp3_play(int iNum);
static	void	mp3_stop(int iNum);
static	void	mp3_free(int iNum);
static	bool	mp3_load(int iNum, char *strFile, bool bLoop);
static	void	mp3_volume(int iNum, float fVolume, int iTime);
static	void	mp3_pause(int iNum, bool bSw);
static	void	mp3_main(void);



/*------------------------------------------------------------------------------*
|	<<< �E�F�[�u�t�H�[�}�b�g���擾 >>>
|	����	dwFreq = �T���v�����O���g��
|			wBit   = �ʎq���r�b�g��
|			wCh    = �`�����l����
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
| <<< �T�E���h���������� >>>
*------------------------------------------------------------------------------*/
bool	snd_init(void)
{
	//--- ���łɏ������ς݂����ׂ� ----------------------------------
	if(snd_work != NULL){ return false;}

	//--- COM �̏����� ----------------------------------------------
	if(FAILED(CoInitialize(NULL))){ return false;}

	//--- �������[�̊m�� --------------------------------------------
	snd_work = (SND_WORK*)malloc(sizeof(SND_WORK) *	SND_MAX);
	memset(snd_work, 0,	sizeof(SND_WORK) * SND_MAX);

	//--- �ʏ�̃T�E���h --------------------------------------------
	HRESULT	hr;								// �p�t�H�[�}���X�̍쐬
	hr = CoCreateInstance(CLSID_DirectMusicPerformance,	NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void **)&da.pPerformance);
	if(FAILED(hr)){ return sys_error("SND: �p�t�H�[�}���X�̍쐬�Ɏ��s���܂����B");}
	hr = da.pPerformance->InitAudio(		// �p�t�H�[�}���X�̏�����
		NULL,									// IDirectMusic�C���^�[�t�F�C�X�͕s�v
		NULL,									// IDirectSound�C���^�[�t�F�C�X�͕s�v
		sys.hWnd,								// �E�B���h�E�̃n���h��
		DMUS_APATH_DYNAMIC_STEREO,
		64,										// �p�t�H�[�}���X�E�`�����l���̐�
		DMUS_AUDIOF_ALL,						// �V���Z�T�C�U�̋@�\
		NULL);									// �I�[�f�B�I�E�p�����[�^�ɂ̓f�t�H���g���g�p

	if(FAILED(hr)){ return sys_error("SND: �p�t�H�[�}���X�̏������̏������Ɏ��s���܂����B");}
											// ���[�_�[�̍쐬
	hr = CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, (void **)&da.pLoader);
	if(FAILED(hr)){ return sys_error("SND: ���[�_�[�̍쐬�Ɏ��s���܂����B");}

	//--- DirectSound -----------------------------------------------
	DirectSoundCreate8(NULL, &da.pDS, NULL);
	da.pDS->SetCooperativeLevel(sys.hWnd, DSSCL_PRIORITY);

	DSBUFFERDESC	desc;
	ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize  = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLPAN;

	da.pDS->CreateSoundBuffer(&desc, &da.pPB, NULL);
											// �E�F�[�u�t�H�[�}�b�g�̐ݒ�
	WAVEFORMATEX	wfmt = GetWaveFormat(44100, 16, 2);
	da.pPB->SetFormat(&wfmt);
	da.pPB->GetVolume(&da.lInitVolume);		// �����{�����[���̎擾

	return true;
}



/*------------------------------------------------------------------------------*
| <<< �Z�O�����g�̊J�� >>>
|	����	iNum = �Ǘ��ԍ�
*------------------------------------------------------------------------------*/
static	void	segment_release(int	iNum)
{

	SND_WORK	*s = &snd_work[iNum];
											// ���F�f�[�^�̃A�����[�h
	if(s->pSegment){ s->pSegment->Unload(da.pPerformance);}
	SAFE_RELEASE(s->pSegment);				// �Z�O�����g�J��
	SAFE_RELEASE(s->pState);				// �X�e�C�^�X�J��
	SAFE_RELEASE(s->pAudioPath);			// �I�[�f�B�I�p�X���
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�o�b�t�@�̍쐬 >>>
|	����	hMMMI = �t�@�C���n���h��
|			pFmt  = �E�G�[�u�t�H�[�}�b�g
|			dwLen = �E�G�[�u�T�C�Y
*------------------------------------------------------------------------------*/
static	bool	CreateBuffer(int iNum, HMMIO hMMI, LPWAVEFORMATEX pFmt, DWORD dwLen)
{

	//--- �o�b�t�@�̍쐬 --------------------------------------------
	DSBUFFERDESC	desc;
	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize		   = sizeof(DSBUFFERDESC);
	desc.dwFlags 	   = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	desc.dwBufferBytes = dwLen;
	desc.lpwfxFormat   = pFmt;

	HRESULT	hr = da.pDS->CreateSoundBuffer(&desc, &snd_work[iNum].pSB, NULL);
	if(FAILED(hr))
	{										// �f�[�^���Z���ăG�t�F�N�g���g�p�ł��Ȃ�
		if(hr == DSERR_BUFFERTOOSMALL){ return sys_error("data is too small.");}
											//�������s���i�T�E���h�J�[�h���́H�j
		if(hr == DSERR_OUTOFMEMORY   ){ return sys_error("out of memory.");}
		return sys_error("�T�E���h�o�b�t�@���쐬�ł��܂���B");
	}

	//--- �o�b�t�@�̃��b�N ------------------------------------------
	LPVOID	write1, write2;
	DWORD	length1,length2;
	if(snd_work[iNum].pSB->Lock(0, dwLen, &write1, &length1, &write2, &length2, 0) == DSERR_BUFFERLOST)
	{
		snd_work[iNum].pSB->Restore();
		return false;
	}
	//--- data�`�����N���o�b�t�@�Ƀ��[�h ----------------------------
	if((DWORD)mmioRead(hMMI,(char *)write1, length1) == length1)
	{
		if(length2 != 0){ mmioRead(hMMI, (char *)write2, length2);}
	}
	//--- �o�b�t�@�̃A�����b�N --------------------------------------
	if(snd_work[iNum].pSB->Unlock(write1, length1, write2, length2) != DS_OK) return false;

	return true;
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�ꎞ��~ >>>
|	����	iNum = �Ǘ��ԍ�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
*------------------------------------------------------------------------------*/
static	void	WavPause(int iNum, bool bSw)
{

	if(snd_work[iNum].pSB == NULL){ return;}
	
	//--- �X�e�C�^�X�擾 --------------------------------------------
	SND_WORK	*s = &snd_work[iNum];
	DWORD	dwStatus;
	s->pSB->GetStatus(&dwStatus);

	//--- �ꎞ��~ --------------------------------------------------
	if(bSw && dwStatus & DSBSTATUS_PLAYING && !s->bPause)
	{
		s->pSB->GetCurrentPosition(&s->dwPosition, NULL);
		s->pSB->Stop();
		s->bPause = true;
	}
	//--- �ꎞ��~���� ----------------------------------------------
	else if(!bSw && s->bPause)
	{
		s->pSB->SetCurrentPosition(s->dwPosition);

		s->pSB->Play(0, 0, s->bLoop ? DSBPLAY_LOOPING : 0);
		s->bPause = false;
	}
}



/*------------------------------------------------------------------------------*
| <<< �V�X�e���Ŏg�p����T�E���h�ꎞ��~ >>>
|	����	iNum = �Ǘ��ԍ�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
*------------------------------------------------------------------------------*/
static	void	WavSystemPause(int iNum, bool bSw)
{

	if(snd_work[iNum].pSB == NULL){ return;}
	
	//--- �X�e�C�^�X�擾 --------------------------------------------
	SND_WORK	*s = &snd_work[iNum];
	DWORD	dwStatus;
	s->pSB->GetStatus(&dwStatus);

	//--- �ꎞ��~ --------------------------------------------------
	if(bSw && dwStatus & DSBSTATUS_PLAYING && !s->bSystemPause)
	{
		s->pSB->GetCurrentPosition(&s->dwPosition, NULL);
		s->pSB->Stop();
		s->bSystemPause = true;
	}
	//--- �ꎞ��~���� ----------------------------------------------
	else if(!bSw && s->bSystemPause)
	{
		s->pSB->SetCurrentPosition(s->dwPosition);
		s->pSB->Play(0, 0, s->bLoop ? DSBPLAY_LOOPING : 0);
		s->bSystemPause = false;
	}
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�̍Đ� >>>
|	����	iNum  = �Ǘ��ԍ�
|			bLoop = true : ���[�v�Đ�
*------------------------------------------------------------------------------*/
static	void	WavPlay(int iNum, bool bLoop)
{

	LPDIRECTSOUNDBUFFER	p = snd_work[iNum].pSB;

	if(p == NULL){ return;}
//	if(p->SetCurrentPosition(0L) != DS_OK)
	{
		p->Stop();							// �Đ����ɍĐ��ʒu���Z�b�g�ł��Ȃ��J�[�h������͗l
		p->SetCurrentPosition(0);
	}
	HRESULT	hr = p->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0);

	//--- �����[�h���� ----------------------------------------------
	if(hr == DSERR_BUFFERLOST)
	{
		//sys_error("DSERR_BUFFERLOST:%s", m_strName.c_str());
		//PrimaryBufferVerify();				// ��Ƀv���C�}���o�b�t�@�����؂���
		DWORD	dw;
		da.pPB->GetStatus(&dw);
		if(dw & DSBSTATUS_BUFFERLOST)		// ���X�g���͕���
		{
			sys_error("PB:DSBSTATUS_BUFFERLOST");
			da.pPB->Restore();
		}
		p->Restore();						// �ǂ̓� Free ���邪�O�̂���
		WavLoad(iNum, snd_work[iNum].strFile);
	}
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�̉��ʂ��Z�b�g >>>
|	����	iNum  = �Ǘ��ԍ�
|			fVol  = ����(0.0f �` 1.0f(�ő�(default))
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
| <<< �T�E���h�̍Đ����g�����Z�b�g >>>
|	����	iNum   = �Ǘ��ԍ�
|			fPitch = 44100 ���f�t�H���g���ʂƂ��Ă̍Đ����g��
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
| <<< �T�E���h��~ >>>
|	����	iNum = �ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
static	void	WavStop(int iNum)
{
	if(snd_work[iNum].pSB != NULL){ snd_work[iNum].pSB->Stop();}
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�J�� >>>
|	����	iNum = �ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
static	void	WavFree(int iNum)
{
	WavStop(iNum);
	SAFE_RELEASE(snd_work[iNum].pSB);
}



/*------------------------------------------------------------------------------*
| <<< �����f�[�^�̓Ǎ��� >>>
|	����	iNum    = �ǂݍ��ݔԍ�
|			strFile = �t�@�C����
*------------------------------------------------------------------------------*/
static	bool	WavLoad(int iNum, char *strFile)
{
	//--- �����Ȃ��� ----------------------------------------------
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

	//--- �I�[�v�� --------------------------------------------------
	hMMI = mmioOpen(strFile, NULL, MMIO_READ | MMIO_ALLOCBUF);
	if(hMMI == NULL){ return sys_error("WavLoad %s ���J���܂���", strFile);}

	//--- RIFF�`�����N�̓ǂݍ��݁��`�F�b�N --------------------------
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hMMI, &parent, NULL, MMIO_FINDRIFF) != 0){ return sys_error("WavLoad : %s WAVE�`���ł͂���܂���", strFile);}

	//--- fmt�`�����N�̓ǂݍ��݁��`�F�b�N ---------------------------
	child.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(mmioDescend(hMMI, &child, &parent, 0) != 0){ return sys_error("WavLoad : %s WAVE�`���ł͂���܂���", strFile);}

	//--- �t�H�[�}�b�g�̎擾 ----------------------------------------
	mmioRead(hMMI, (char*)&wfmtx, sizeof(wfmtx));

	//--- PCM�t�H�[�}�b�g���H ---------------------------------------
	if(wfmtx.wFormatTag != WAVE_FORMAT_PCM){ return sys_error("WavLoad : %s �t�H�[�}�b�g���s���ł��B", strFile);}
	if(mmioAscend(hMMI, &child, 0) != 0){ return sys_error("WavLoad : %s �A�N�Z�X�ł��܂���B", strFile);}

	//-- data�`�����N�̓ǂݍ��݁��`�F�b�N ---------------------------
	child.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hMMI, &child, &parent, MMIO_FINDCHUNK) != 0){ return sys_error("WavLoad : %s chunk is not found", strFile);}

	//--- data�`�����N�����擾 --------------------------------------
	len = child.cksize;

	//--- �o�b�t�@���쐬�A���[�h ------------------------------------
	if(CreateBuffer(iNum, hMMI, &wfmtx, len))
	{
		//--- �N���[�Y ----------------------------------------------
		mmioClose(hMMI, 0);
		return true;
	}

	mmioClose(hMMI, 0);
	return false;
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�̓ǂݍ��� >>>
|	����	iNum        = �ǂݍ��ݔԍ�
|			*strOriFile = WAVE�t�@�C�����܂��̓��\�[�X��
|			bLoop       = true : �������[�v
|			bLoadAtOnce = true : �����ǂݍ���
|	�߂�l	true : �ǂݍ��ݐ���
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
	
	snd_free(iNum);							// �ǂݍ��݃f�[�^��j��

	SND_WORK	*s = &snd_work[iNum];

	char	strFile[MAX_PATH];
	strcpy(strFile, strOriFile);
	STRLWR(strFile);						// �啶������������

	//--- �����ł͖����ꍇ�̏��� ------------------------------------
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
		
	//--- mp3 �� ���[�v���̂� DirectShow �ŏ������� ---------------------------
	if(strstr(strFile, ".mp3"))// || bLoop)
	{
		return mp3_load(iNum, strFile, bLoop);
	}

	//--- wav �̌��ʉ��AMIDI �� DirectSound �ŏ������� ------------------------
	s->bMp3 = false;

#ifndef __DSOUND
											// �Z�O�����g�쐬�iWave�t�@�C�������[�h�j
	MultiByteToWideChar(CP_ACP,	0, strFile, -1, wstrFileName, MAX_PATH);

	hr = da.pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,
									   IID_IDirectMusicSegment8,
									   wstrFileName,
									   (LPVOID*)&s->pSegment);
	if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s���ǂݍ��߂܂���ł����B", strFile);}

	if(bLoop)								// �Z�O�����g���J��Ԃ��Đ�����ݒ�ɂ���
	{
		hr = s->pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
		if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s �̃��[�v�񐔐ݒ�Ɏ��s���܂����B", strFile);}
	}
	if(strstr(strFile, ".mid"))				// �X�^���_�[�hMIDI�t�@�C���̏ꍇ�́A�p�����[�^��ݒ肷��
	{
		hr = s->pSegment->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, 0, 0, NULL);
		if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s �̃p�����[�^�ݒ�Ɏ��s���܂����B", strFile);}
	}		
											// �o���h�̃_�E�����[�h�i�蓮�j
	hr	= s->pSegment->Download(da.pPerformance);
	if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s �̃_�E�����[�h�Ɏ��s���܂����B", strFile);}
											// �I�[�f�B�I�p�X�̎擾(���ʒ��ߗp)
	hr = da.pPerformance->CreateStandardAudioPath( 
					DMUS_APATH_DYNAMIC_STEREO,	// �p�X�̎�ށB
					1,							// �p�t�H�[�}���X �`�����l���̐��B
					TRUE,						// �����ŃA�N�e�B�u�ɂȂ�B
					&s->pAudioPath);			// �I�[�f�B�I�p�X���󂯎��|�C���^�B
	if(FAILED(hr)){ snd_free(iNum); return sys_error("SND: %s �̃I�[�f�B�I�E�p�X�̍쐬�Ɏ��s");}
#else

	WavLoad(iNum, strFile);

#endif

	strcpy(s->strFile, strFile);			// �t�@�C�����ݒ�

	s->bLoop = bLoop;						// ���[�v�ݒ�
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�̓ǂݍ��� >>>
|	����	iNum        = �ǂݍ��ݔԍ�
|			*strOriFile = WAVE�t�@�C�����܂��̓��\�[�X��
|			bLoop       = true : �������[�v
|	�߂�l	true : �ǂݍ��ݐ���
*------------------------------------------------------------------------------*/
bool	snd_load(int iNum, char *strOriFile, bool bLoop)
{
	return snd_load(iNum, strOriFile, bLoop, !sys.bAfterLoad);
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�f�[�^�̊J�� >>>
|	����	iNum = �Ǘ��ԍ�
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
| <<< �T�E���h�̒�~ >>>
|	����	iNum = �Ǘ��ԍ�
*------------------------------------------------------------------------------*/
void	snd_stop(int iNum)
{

	if(snd_work	==      NULL){ return;}
	if((uint)iNum >= SND_MAX){ return;}


	SND_WORK	*s = &snd_work[iNum];
	s->usDelay	   = 0;
	
	//--- MP3 �� ----------------------------------------------------
	if(s->bMp3)
	{
		mp3_stop(iNum);
	}
	//--- DirectMusic �� --------------------------------------------
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
| <<< �T�E���h�̈ꎞ��~ >>>
|	����	iNum = �Ǘ��ԍ�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
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
				s->pState->GetSeek(&s->lTime);	// ��~���Ԃ��擾(����AMidi�̂ݗL��)
				da.pPerformance->Stop(NULL, NULL, 0, 0);
			}
			else if(s->bLoop && s->bLoopReplay)	// ���[�v�Đ��̍ĊJ
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
| <<< �T�E���h�S�̃|�[�Y >>>
|	����	bSw = true : �|�[�Y���|����
*------------------------------------------------------------------------------*/
void	snd_pause_all(bool bSw)
{
//	mp3_pause_all(bSw);
	for(int i = 0; i < SND_MAX; i++){ snd_pause(i, bSw);}
}



/*------------------------------------------------------------------------------*
| <<< �V�X�e���Ŏg�p����T�E���h�̈ꎞ��~(��A�N�e�B�u���Ɏg�p) >>>
|	����	iNum = �Ǘ��ԍ�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
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
				// WAV �̏ꍇ�͓r���Đ����ł��Ȃ��̂ŁA���ʂ��I�t�ɂ��Č떂����
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
| <<< �V�X�e���ŃT�E���h�S�̃|�[�Y >>>
|	����	bSw = true : �|�[�Y���|����
*------------------------------------------------------------------------------*/
void	snd_system_pause_all(bool bSw)
{
	for(int i = 0; i < SND_MAX; i++){ snd_system_pause(i, bSw);}
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h���C�� >>>
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
		if(s->usDelay >	0)			// �f�B���C����
		{
			if((--s->usDelay) == 0){ snd_play(i);}
		}
		else if(s->usStopCount > 0)	// ���荞�݋֎~����
		{
			s->usStopCount--;
		}
	}
#endif
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�̍Đ� >>>
|	����	iNum = �Ǘ��ԍ�
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

	if(s->usStopCount > 0){ return;}			// �T�E���h�̓�x�炵�h�~
	s->usStopCount = SND_DISABLE_TIME;			// ���ԍ����󂯕t���Ȃ�����

	//--- MP3 �� ----------------------------------------------------
	if(s->bMp3)
	{
		mp3_play(  iNum);
		mp3_volume(iNum, 1, 0);
	}
	//--- DirectMusic �� --------------------------------------------
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
| <<< �x�����ԕt���T�E���h�̍Đ� >>>
|	����	iNum   = �Ǘ��ԍ�
|			iDelay = �x������
*------------------------------------------------------------------------------*/
void	snd_play_delay(int iNum, int iDelay)
{
	snd_work[iNum].usDelay = iDelay;
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�̉��ʂ��Z�b�g >>>
|	����	iNum  = �Ǘ��ԍ�
|			fVol  = ����(0.0f �` 1.0f(�ő�(default))
|			iTime = ���ʂ̕ω�����(__DSOUND ���̂�)
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
			{									// -9600 ������ 0 �Ȃ̂ŕϊ�����
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
| <<< �T�E���h�̃s�b�`�ύX >>>
|	����	iNum   = �Ǘ��ԍ�
|			fPitch = 44100 ���f�t�H���g���ʂƂ��Ă̍Đ����g��
|			iTime  = ���ʂ̕ω�����(__DSOUND ���̂�)
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
			{									// -9600 ������ 0 �Ȃ̂ŕϊ�����
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
| <<< �T�E���h�I������ >>>
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
	SAFE_RELEASE(da.pLoader);				// ���[�_���J��
											// �p�t�H�[�}���X���J��
	//--- �������[�̊J�� --------------------------------------------
	if(snd_work	!= NULL) free(snd_work);
	snd_work = NULL;
}



//--- �ȉ��A�l�o�R --------------------------------------------------------------------------------

/*----------------------------------------------------------------------*
| <<< �l�o�R�J�� >>>
|	����	iNum = �Ǘ��ԍ�
*----------------------------------------------------------------------*/
static	void	mp3_free(int iNum)
{

	SND_WORK	*w = &snd_work[iNum];

	if(!w->bLoad) return;					// �ǂݍ���ł��Ȃ��Ȃ�΂����ŋA��

	if(w->bStandby){ mp3_stop(iNum);}		// �Đ����������ꍇ�͒�~����

	w->iPlayState = __STOP;					// ��~���
	if(w->bStandby){ timeKillEvent(w->uiID); timeEndPeriod(w->ptc.wPeriodMin);}
											// ���蓖�Ă��C���^�[�t�F�[�X�����
	if(w->pMediaSeeking){ w->pMediaSeeking->Release(); w->pMediaSeeking = NULL;}
	if(w->pAudio)		{ w->pAudio->Release();		   w->pAudio		= NULL;}
	if(w->pMediaControl){ w->pMediaControl->Release(); w->pMediaControl = NULL;}
	if(w->pGraph)		{ w->pGraph->Release();		   w->pGraph		= NULL;}
	if(w->pEvent)		{ w->pEvent->Release();		   w->pEvent		= NULL;}

	w->bLoad    = false;					// �ǂݍ���ł��Ȃ���Ԃɂ���
	w->bStandby = false;
}



/*----------------------------------------------------------------------*
| <<< �l�o�R�S��~ >>>
*----------------------------------------------------------------------*/
void	mp3_free(void)
{
	for(int	i =	0; i < MP3_MAX;	++i) mp3_free(i);
}



/*----------------------------------------------------------------------*
| <<< �l�o�R�̃R�[���o�b�N >>>
|	����	iNum = �Ǘ��ԍ�
*----------------------------------------------------------------------*/
long	alEventCode[10];
void	snd_mp3_proc(int iNum)
{

	long		lEventCode,	lParam1, lParam2;
	LONGLONG	llSetPos = 0, llEndPos = 0;
	SND_WORK	*s = &snd_work[iNum];

	if(!s->bLoad)    return;				// �ǂݍ���ł��Ȃ��Ȃ�΂����ŋA��
	if(!s->bStandby) return;				// ��~���Ȃ�΃��^�[��

	s->pEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 100);
	if(lEventCode == EC_COMPLETE
	|| lEventCode == EC_END_OF_SEGMENT
	|| lEventCode == EC_ERROR_STILLPLAYING
	|| lEventCode == EC_ERRORABORT
	|| lEventCode == EC_STREAM_ERROR_STOPPED
	|| lEventCode == EC_USERABORT
	|| lEventCode == EC_PAUSED
	|| lEventCode == EC_STREAM_ERROR_STILLPLAYING)
	{										// �������[�v�Đ��w�肾�����ꍇ�̓��[�v����

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
		else if(s->iPlayState == __PLAY)	// ����ȊO�̏ꍇ�̓N���[���A�b�v
		{
			mp3_stop(iNum);
		}
	}
}



/*----------------------------------------------------------------------*
| <<< �l�o�R�t�@�C���̃��[�h >>>
|	����	iNum     = �Ǘ��ԍ�
|			*strFile = �l�o�R�t�@�C����
|			bLoop    = true �Ȃ烋�[�v
*----------------------------------------------------------------------*/
static	bool	mp3_load(int iNum, char *strFile, bool bLoop)
{

	if(snd_work	  == NULL){     return false;}
	if((uint)iNum >=  MP3_MAX){ return false;}


	WCHAR		path[MAX_PATH];
	SND_WORK	*s = &snd_work[iNum];

	s->bMp3 = true;

	if(s->bStandby){ mp3_stop(iNum);}		// �Đ����������ꍇ�͂܂��N���[���A�b�v���s��
											// �t�@�C������Unicode�ɕϊ�����
	MultiByteToWideChar(CP_ACP,	0, strFile, -1, path, sizeof(path) / sizeof(path[0]));
											// �t�B���^�O���t�}�l�[�W�����쐬���A
											// �C���^�[�t�F�C�X���N�G������B
	if(!(FAILED(CoCreateInstance(CLSID_FilterGraph,	NULL, CLSCTX_INPROC, IID_IGraphBuilder,	(void **)&s->pGraph))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IBasicAudio,	 (void **)&s->pAudio))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IMediaControl, (void **)&s->pMediaControl))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IMediaEventEx, (void **)&s->pEvent))))
	{
	if(!(FAILED(s->pGraph->QueryInterface(IID_IMediaSeeking, (void **)&s->pMediaSeeking))))
	{										// �O���t���쐬�B�d�v:
											// �g�p�V�X�e���̃t�@�C��������ɕύX���邱�ƁB
		if(s->pGraph->RenderFile(path, NULL) ==	S_OK)
		{
			s->pMediaControl->Run();		// �O���t�̎��s�B
			//s->pMediaControl->Pause();
			s->pMediaControl->Stop();
			s->pAudio->put_Volume((int)s->fVolume);
			long evCode;
			s->pEvent->WaitForCompletion(INFINITE, &evCode);

			s->bStandby	  = true;			// �Đ����s�t���O�̃Z�b�g
			s->bLoop	  = bLoop;			// ���[�v���Z�b�g
			s->bLoad      = true;
			s->iPlayState = __STOP;
											// �C�x���g�ʒm�̃Z�b�g
			int	d =	MP3_MESSAGE0 + iNum;
			s->pEvent->SetNotifyWindow((OAHWND)sys.hWnd, d,	0);
			return true;					// �I��
		}
	}}}}}

	return false;							// �I��
}



/*----------------------------------------------------------------------*
| <<< �l�o�R�t�@�C���̈ꎞ��~ >>>
|	����	iNum = �Ǘ��ԍ�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
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
| <<< �l�o�R�t�@�C���̈ꎞ��~ >>>
|	����	iNum = �Ǘ��ԍ�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
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
| <<< �l�o�R�t�@�C���̈ꎞ��~ >>>
|	����	iNum = �Ǘ��ԍ�
*----------------------------------------------------------------------*/
static	void	mp3_stop(int iNum)
{

	LONGLONG	SetPos = 0,	EndPos = 0;

	if(!snd_work[iNum].bLoad){ return;}
	if((uint)iNum >= MP3_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];
	if(s->bStandby)							// �Đ����s�t���O�̃Z�b�g
	{
		s->pMediaControl->Pause();
		s->pMediaSeeking->SetPositions(&SetPos, AM_SEEKING_AbsolutePositioning,	&EndPos, AM_SEEKING_NoFlush);
		s->iPlayState = __STOP;
	}
}



/*----------------------------------------------------------------------*
| <<< �l�o�R�t�@�C���̍Đ� >>>
|	����	iNum = �Ǘ��ԍ�
*----------------------------------------------------------------------*/
static	void	mp3_play(int iNum)
{

	if((uint)iNum >= MP3_MAX){ return;}

	SND_WORK	*s = &snd_work[iNum];

	if(!s->bLoad) return;					// ���ǂݍ��݂Ȃ�Ώ������Ȃ�
//	mp3_stop(iNum);							// �Đ����������ꍇ�̓X�g�b�v
	if(s->iPlayState == __STOP)				// ���ʂ��ő�ɂ��čĐ��J�n
	{
		s->iTime      = 0;
		s->fVolume    = 0;
		s->pAudio->put_Volume((int)s->fVolume);
		s->pMediaControl->Run();
		s->iPlayState = __PLAY;
	}
}



/*----------------------------------------------------------------------*
| <<< �l�o�R�̉��ʐݒ� >>>
|	����	iNum    = �Ǘ��ԍ�
|			fVolume = 0 �` 1 (�ő�)
|			iTime   = �ύX����
*----------------------------------------------------------------------*/
static	void	mp3_volume(int iNum, float fVolume, int iTime)
{

	int		iStart = iNum, iEnd = iNum + 1;

											// -1 �Ȃ�S�Ă��ΏۂƂȂ�
	if(iNum == -1){ iStart = 0, iEnd = MP3_MAX;}

	SND_WORK	*s = &snd_work[iStart];
	for(int	i =	iStart; i < iEnd; i++, s++)
	{
		if(!s->bLoad){ continue;}			// mp3 �̃f�[�^�����[�h���Ă��邩���ׂ�
		fVolume = (fVolume - 1) * 10000.f;			// -10000 ������ 0 �Ȃ̂ŕϊ�����
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
| <<< �l�o�R�̉��ʊĎ� >>>
*----------------------------------------------------------------------*/
static	void	mp3_main(void)
{

	SND_WORK	*s = snd_work;

	for(int	i =	0; i < MP3_MAX;	i++, s++)
	{
		if(!s->bLoad) continue;				// ���ǂݍ��݂Ȃ�Ώ������Ȃ�
		//--- �ȉ��A���ʕω����� ------------------------------------
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