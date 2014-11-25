//
//	���[�r�[���e�N�X�`���[�ɗ������ރv���O����
//


#define		TRANS_ASM						// VRAM �]�����A�Z���u���ōs���X�C�b�`(16�r�b�g���̂�)
#define		MOVIE_MAX	D3_TEX_MAX2			// ���[�r�[�̍ő吔

#define		INTERMEDIATE_BUFFER				// ���ԃo�b�t�@���g�p����(win7�A�t���X�N���[�����ɂ͕K���g�p�̂��ƁBXP���͓��ɕω��Ȃ�)


#ifdef	_DEBUG
	#undef	ASSERT
	#define	ASSERT(a)
#endif

//--- ���C�u�����B --------------------------------------------------------------------------------
#ifdef	_DEBUG
	#pragma	comment(lib, "strmbasd.lib")
#else
	#pragma	comment(lib, "strmbase.lib")
#endif



/*----------------------------------------------------------------------*
| <<< �N���X�̒�` >>>
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

	ushort					usTexNum;			// �e�N�X�`���[�ԍ�
	int						iWidth,	iHeight;	// ���[�r�[�̕��ƍ���
	int						iPitch;				// Video Pitch
	char					strFile[MAX_PATH];	// �t�@�C����
};

struct	MOVIE_TO_TEXTURE_WORK				// ����t�@�C���Đ��p�f�[�^�\����
{
	bool					bSystemPause;		// �V�X�e�����̋����|�[�Y(��A�N�e�B�u���Ɏg�p)
	bool					bPlayState;			// true	: �Đ���
	bool					bLoadFlag;			// ture	: ���[�h���Ă���
	bool					bSound;				// true	: �T�E���h�Đ�����
	bool					b16Bit;				// true : �P�U�r�b�g�Ńe�N�X�`���[�쐬
	LONGLONG				llPausePosition;	// �ꎞ��~�ʒu(�� LONGLONG �͂U�S�o�C�g�^)
	double					dEndTime;			// �I������
	int						iLoop;				// ���[�v��		��-1�Ŗ������[�v
	uint					uiFrame;			// �Đ��R�}��
	float					fRate;				// �Đ����[�g
	int						iPauseFlag;			// �ꎞ��~�܂ł̃J�E���^�[(-1:�Ȃ�΍Đ����)
												//  1 ��������ƁA�P�t���[�����e�N�X�`���[�Ɏ��o���ꎞ��~(�l�͂O)
												// -1 ��������ƁAmovie_to_texture_main �ňꎞ��~���|����
	CComPtr<IGraphBuilder>	pGB;				// �O���t�r���_�[
	CComPtr<IMediaControl>	pMC;				// Media Control
	CComPtr<IMediaPosition>	pMP;				// Media Position
	CComPtr<IMediaEvent>	pME;				// Media Event
	CComPtr<IBaseFilter>	pBF;

	CComPtr<IMediaSeeking>	pMS;				// Media Seeking
	CComPtr<IBasicAudio	>	pBA;				// Basic Audio

	CTextureRenderer		*pcTR;				// DirectShow TextureRenderer �N���X

	//--- ���ԃo�b�t�@ ----------------------------------------------
	#ifdef	INTERMEDIATE_BUFFER
		bool				bTrans;				// true : �v���[�r�[�]��
		DWORD				*pdwBuf;			// ���[�r�[�W�J�p�o�b�t�@
		uint				uiPitch;			// �e�N�X�`���[1���C���̃o�C�g��
		uint				uiSize;				// �W�J�p�Ɋm�ۂ����������T�C�Y
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
| <<< ���[�r�[�̃^�C�v�𒲂ׂ� >>>
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
| <<< ���[�r�[�̃^�C�v���e�N�X�`���[���쐬 >>>
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
		sys_error("%s �̓e�X�N�`���[�ɂ��邱�Ƃ��ł��Ȃ����[�r�[�t�H�[�}�b�g�ł��B", strFile);
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

#ifdef	INTERMEDIATE_BUFFER
	// ���ԃo�b�t�@��p��
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
| <<< ���[�r�[���e�X�N�`���[�ɗ������� >>>
|	�� 16 �r�b�g�A32 �r�b�g�݂̂̑Ή��ƂȂ�܂��B
*----------------------------------------------------------------------*/
HRESULT	CTextureRenderer::DoRenderSample(IMediaSample *pSample)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[usTexNum];

	BYTE	*pBmpBuffer, *pTxtBuffer;
	
	// ���[�r�[�Đ�����~���Ă����炱���ŏI��
	if(movie[usTexNum].bPlayState == false){ return E_FAIL;}

	// �e�|�C���^�[�� NULL �łȂ������ׂ�
	if(pSample			== NULL){ return E_POINTER;}
	if(d3.tex[usTexNum] == NULL){ return E_UNEXPECTED;}

	// ���[�r�[�̓W�J�o�b�t�@�𓾂�
	pSample->GetPointer(&pBmpBuffer);	// Get the video bitmap	buffer

	// �e�N�X�`���[�o�b�t�@(pTxtBuffer)��p��
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

		// ���x���グ�邽�߃A�Z���u���ŋL�q
		// �����ł͋ɗ� VRAM �ւ̃A�N�Z�X�����炵�Ă���BVRAM �S�o�C�g�]�����Ă���
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

				//--- ��ڂ̃f�[�^���H
				mov		edx, [edi + 3]		// ��C�ɂS�o�C�g�ǂݍ���(���̂����R�o�C�g���g�p����)

				mov		eax, edx			// edi + 5 ��ǂݍ��݂ł���
				and		eax, 0xf80000 
				shr		eax, 8 + 1

				mov		bx, dx				// edi + 4 ��ǂݍ��݂ł���
				and		bx, 0xf800
				shr		bx, 8 - 2

				and		dx, 0xf8			// edi + 3
				shr		dx, 3				

				or		bx, ax
				or		bx, dx
				shl		ebx, 16				// ebx �̏�P�U�r�b�g�ɑޔ�

				//--- ��ڂ̃f�[�^���H
				mov		edx, [edi + 0]		// ��C�ɂS�o�C�g�ǂݍ���(���̂����R�o�C�g���g�p����)

				mov		eax, edx			// edi + 2 ��ǂݍ��݂ł���
				and		eax, 0xf80000 
				shr		eax, 8 + 1

				mov		bx, dx				// edi + 1 ��ǂݍ��݂ł���
				and		bx, 0xf800
				shr		bx, 8 - 2

				and		dx, 0xf8			// edi + 0
				shr		dx, 3

				or		bx, ax
				or		bx, dx

				//--- �f�[�^����������		// ��x�ɂS�o�C�g�������ނ̂� VRAM �A�N�Z�X���ɂ͗L���ł���
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

	// �Đ��R�}���J�E���g�A�b�v
	w->uiFrame++;

	// �ꎞ��~�v���̏ꍇ�A��R�}���o������A�t���O���|����
	if(w->iPauseFlag == 1){ w->iPauseFlag++;}

	#ifndef	INTERMEDIATE_BUFFER
		if(FAILED(d3.tex[usTexNum]->UnlockRect(0))){ return E_FAIL;}
	#endif

	return S_OK;
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[�̏I���A��� >>>
|	����	iNum = ���[�r�[�ԍ�
*------------------------------------------------------------------------------*/
static	void	movie_to_texture_release(int iNum)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];
	if(w->bLoadFlag	== false){ return;}

	if(!(!w->pMC)){ w->pMC->Stop();   }		// �Đ����~����
											// �e��I�u�W�F�N�g�̉��
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
	w->bSystemPause = false;				// �Đ���~�t���O�N���A


	//SAFE_DELETE(w->pcTR);					// ����A�����Aw->pBF.Release �ŊJ������Ă��銴���ł��B
											// �Ȃ̂ŕs�v�ł�
	w->pcTR = NULL;

	d3_tex_free(iNum);

#ifdef	INTERMEDIATE_BUFFER
	if(w->pdwBuf != NULL){ free(w->pdwBuf); w->pdwBuf = NULL;}
#endif
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[������������� >>>
|	int	iNum  = -1 : �S�ĊJ��							�� D3_TEX_MAX2 �܂�
|		     != -1 : �w��ԍ��̃��[�r�[���J��
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
| <<< ���[�r�[�̓ǂݍ��� >>>
|	����	iNum    = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			strFile = �t�@�C����
|			b16Bit  = 16 �r�b�g�Ńe�N�X�`���[�ɗ�������
*------------------------------------------------------------------------------*/
bool	movie_to_texture_load(int iNum, char *strFile, bool b16Bit)
{

	HRESULT					hr;
	WCHAR					wFileName[MAX_PATH];
	CComPtr<IBaseFilter>	pFSrc;			// Source Filter
	CComPtr<IPin>			pFSrcPinOut;	// Source Filter Output	Pin	  

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];
											// �t�@�C������	UNICODE	��
	MultiByteToWideChar(CP_ACP,	0, strFile, -1, wFileName, MAX_PATH);
	movie_to_texture_release(iNum);

	//--- �t�B���^�[�O���t�쐬 ------------------------------------------------
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&w->pGB);
	if(FAILED(hr)){ return sys_error("FilterGraph ���쐬�ł��܂���B");}

	//--- �t�B���^�[�ǉ� -----------------------------------------------------
	CTextureRenderer	*c = new CTextureRenderer(NULL, &hr);
	c->usTexNum = iNum;						// �e�N�X��[�ԍ��o�^
	w->b16Bit   = b16Bit;					// �e�N�X�`���[�^�C�v
	w->bSound	= true;
	strcpy(c->strFile, strFile);			// �t�@�C�����ۑ�
	w->pcTR = c;
	w->pBF  = c;
	hr      = w->pGB->AddFilter(w->pBF, L"TextureRenderer");
	if(FAILED(hr)){ return sys_error("�e�N�X�`���[�����_�[�̓o�^�Ɏ��s���܂����B");}

	hr = w->pGB->AddSourceFilter(wFileName, L"SOURCE", &pFSrc);
	if(FAILED(hr)){ return sys_error("�\�[�X�t�B���^�[�̓o�^�Ɏ��s���܂����B");}

	//--- �e��C���^�[�t�F�[�X�쐬 -------------------------------------------
#if 0
	if(SUCCEEDED(hr	= pFSrc->FindPin(L"Output",	&pFSrcPinOut)))
	{
		// for AVI, MPEG1, VFW
		if(!w->bSound)						//	�I�[�f�B�I�g���b�N���Đ����Ȃ��ꍇ
		{
			CComPtr<IPin> pFTRPinIn;		// Texture Renderer	Input Pin
			if(FAILED(hr = c->FindPin(L"In",	&pFTRPinIn))){ return sys_error("�s���̓o�^�Ɏ��s���܂����B");}
			// �ȉ��̉ӏ��ŃG���[���ł�̂ō폜����
	/*		if(FAILED(hr = w->pGB->Connect(pFSrcPinOut, pFTRPinIn)))
			{
				return sys_error("�s���̐ڑ��Ɏ��s���܂����B");
			}
			*/
		}
		else								// �I�[�f�B�I�g���b�N���Đ�����ꍇ
		{
			if(FAILED(hr = w->pGB->Render(pFSrcPinOut)))
			{
				return sys_error("�s���̓o�^�Ɏ��s���܂����B");
			}
		}
	}
	else
#endif
	{
		// for WMV, MPEG2
		if(FAILED(hr = w->pGB->QueryInterface(&w->pBA))){ return sys_error("�I�[�f�B�I�̓o�^�Ɏ��s���܂����B");}

		if(FAILED(hr = w->pGB->RenderFile(wFileName, NULL)))
		{
			return sys_error("�����_�����O�Ɏ��s���܂����B");
		}
		if(!w->bSound)						//	�I�[�f�B�I�g���b�N���Đ����Ȃ��ꍇ
		{
			w->pBA->put_Volume(-10000);
		}
		else
		{
			w->pBA->put_Volume(0);
		}
	}

	//--- �e�t�B���^�[�̍쐬 --------------------------------------
	w->pGB->QueryInterface(&w->pMC);
	w->pGB->QueryInterface(&w->pMP);
	w->pGB->QueryInterface(&w->pME);
	w->pGB->QueryInterface(&w->pMS);

	//--- ���[�v�p�ɏI�����Ԃ𓾂� --------------------------------
	w->pMP->get_StopTime(&w->dEndTime);

	//--- ���ݒ� ----------------------------------------------------
	w->bLoadFlag = true;					// ���[�h�����t���O
	w->fRate	 = 1;						// �Đ����[�g(����)

	return true;
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[�̉��ʐݒ� >>>
|	����	iNum = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			fVol = ����(0.0f �` 1.0f(�ő�(default))
*------------------------------------------------------------------------------*/
void	movie_to_texture_volume(int iNum, float fVol)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(w->pBA != NULL)
	{
		fVol = (fVol - 1) * 10000.f;		// -10000 ������ 0 �Ȃ̂ŕϊ�����
		w->pBA->put_Volume((long)fVol);
	}
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[�̓ǂݍ��� >>>				�� �t���J���[��p
|	����	iNum    = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			strFile = �t�@�C����
*------------------------------------------------------------------------------*/
bool	movie_to_texture_load(int iNum, char *strFile)
{
	return movie_to_texture_load(iNum, strFile, false);
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[���I�����������ׂ� >>>
|	����	iNum = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|	�߂�l	true : �I�����Ă���
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
			//hr = w->pMP->put_CurrentPosition(0);	// ����A���[�r�[��擪�ɖ߂�����
		}
											// w->pME->GetEven t�̌�ɕK���Ăяo������
		hr = w->pME->FreeEventParams(lEventCode, lParam1, lParam2);
	}

	return bRet;
}



/*------------------------------------------------------------------------------*
| <<< �T�E���h�ݒ� >>>					���Đ����͕ύX�s�Amovie_to_texture_play �O�Őݒ�̂���
|	����	iNum = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			bSw  = true  : �T�E���h�Đ�
|				   false : �T�E���h�Đ����Ȃ�	���f�t�H���g
*------------------------------------------------------------------------------*/
void	movie_to_texture_sound_set(int iNum, bool bSw)
{
	movie[iNum].bSound = bSw;
}



/*------------------------------------------------------------------------------*
| <<< �Đ����x�ݒ� >>>
|	����	iNum  =	���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			fRate = 1.0f : ����
|					2.0f : ��{��
|					0.5f : 1/2 �{��
*------------------------------------------------------------------------------*/
void	movie_to_texture_play_rate_set(int iNum, float fRate)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->bLoadFlag){ return;}				// ���[�h���Ă��Ȃ��Ȃ�΋A��

	if(fRate < 0.0f || fRate >= 10.f){ return;}
	if(fRate == w->fRate){             return;}

	w->fRate = fRate;
	w->pMP->put_Rate(fRate);
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[�e�N�X�`���[�ɗ������ݍĐ� >>>
|	����	iNum  =	���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			iLoop =	���[�v��				��-1�Ŗ���
*------------------------------------------------------------------------------*/
bool	movie_to_texture_play(int iNum, int iLoop)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(	w->bPlayState						// �Đ����Ȃ�΋A��
	|| !w->bLoadFlag){ return false;}		// ���[�h���Ă��Ȃ��Ȃ�΋A��

	if(FAILED(w->pMC->Run())){ return sys_error("���[�r�[�̍Đ����ł��܂���ł����B");}
	w->bPlayState =	true;					// �Đ����s�t���O�̃Z�b�g
	w->iLoop	  =	iLoop;					// ���[�v��
	w->uiFrame    = 0;						// �Đ��ʒu

	return true;
}



/*------------------------------------------------------------------------------*
| <<< �Đ��ʒu��ύX���� >>>
|	����	iNum   = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			llTime = ����
*------------------------------------------------------------------------------*/
void	movie_to_texture_pos_set(int iNum, LONGLONG llTime)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->bLoadFlag){ return;}

	if(w->pMS != NULL)
	{
		w->pMC->Stop();					// �Đ���~
		if(w->pMC != NULL){ w->pMC->Pause();}
		w->pMS->SetPositions(&llTime, AM_SEEKING_AbsolutePositioning, &llTime, AM_SEEKING_NoFlush);
		if(w->pMC != NULL){ w->pMC->Run();}
	}
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[��~ >>>
|	����	iNum = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|	��	pause �͈ꎞ��~�Astop �͒�~�ƂȂ�܂��B
*------------------------------------------------------------------------------*/
void	movie_to_texture_stop(int iNum)
{

	LONGLONG				SetPos = 0,	EndPos = 0;
	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(w->bPlayState &&	w->bLoadFlag)	// �Đ����H
	{
		w->pMC->Stop();					// �Đ���~
		w->pMS->SetPositions(&SetPos, AM_SEEKING_AbsolutePositioning, &EndPos, AM_SEEKING_NoFlush);
		w->bPlayState =	false;
	}
}



/*------------------------------------------------------------------------------*
| <<< �S���[�r�[��~ >>>
*------------------------------------------------------------------------------*/
void	movie_to_texture_stop_all(void)
{
	for(int	i =	0; i < MOVIE_MAX; i++) movie_to_texture_stop(i);
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[�̈ꎞ��~ >>>
|	����	iNum = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|			bSw  = true  : �|�[�Y��������
|				   false : �|�[�Y����
*------------------------------------------------------------------------------*/
void	movie_to_texture_pause(int iNum, bool bSw)
{

	LONGLONG				lEndPos = 0;
	MOVIE_TO_TEXTURE_WORK	*w      = &movie[iNum];

	if(w->bLoadFlag && w->iPauseFlag == 0)	// ���[�h���Ă���H
	{
		if(bSw && w->bPlayState)			// �Đ���~?
		{
			w->iPauseFlag = 1;
		}
		else if(!bSw && !w->bPlayState)		// �Đ��ĊJ?
		{
			w->iPauseFlag = -1;
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< �S���[�r�[�̈ꎞ��~ >>>
|	����	bSw = true  : �|�[�Y��������
|				  false : �|�[�Y����
*------------------------------------------------------------------------------*/
void	movie_to_texture_pause_all(bool	bSw)
{
	for(int	i =	0; i < MOVIE_MAX; i++){ movie_to_texture_pause(i, bSw);}
}



/*------------------------------------------------------------------------------*
| <<< �V�X�e�����g�p����S���[�r�[�̈ꎞ��~(��A�N�e�B�u���Ɏg�p) >>>
|	����	bSw = true  : �|�[�Y��������
|				  false : �|�[�Y����
*------------------------------------------------------------------------------*/
void	movie_to_texture_system_pause(bool bSw)
{

	for(int i = 0; i < MOVIE_MAX; i++)
	{
		LONGLONG				lEndPos = 0;
		MOVIE_TO_TEXTURE_WORK	*w      = &movie[i];

		if(!w->bLoadFlag){ continue;}		// ���[�h���Ă���H
		
		if(bSw && !w->bSystemPause)			// �Đ���~?
		{
			if(w->pMS != NULL){ w->pMS->GetPositions(&w->llPausePosition, &lEndPos);}
			if(w->pMC != NULL){ w->pMC->Pause();}
			w->bSystemPause = true;			// ������~
		}
		else if(!bSw && w->bSystemPause)	// �Đ��ĊJ?
		{
			if(w->bPlayState)
			{
				if(w->pMS != NULL){ w->pMS->SetPositions(&w->llPausePosition, AM_SEEKING_AbsolutePositioning, &w->llPausePosition, AM_SEEKING_NoFlush);}
				if(w->pMC != NULL){ w->pMC->Run();}
			}
			w->bSystemPause = false;		// ������~����
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< �S���[�r�[���[�v�`�F�b�N >>>
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
		if(time	>= w->dEndTime)					// ���[�r�[�̍Đ����Ԃ��I�������ꍇ
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
| <<< ���[�r�[�̃T�C�Y�𓾂� >>>
|	����	iNum     = ���[�r�[�Ǘ��ԍ�(�e�N�X�`���[�ԍ�)	�� D3_TEX_MAX2 �܂�
|	�o��	*piWidth = ����
|			*piHeight = �c��
*------------------------------------------------------------------------------*/
void	movie_size_get(int iNum, int *piWidth, int *piHeight)
{

	MOVIE_TO_TEXTURE_WORK	*w = &movie[iNum];

	if(!w->bLoadFlag){ return;}				// �����[�h�̏ꍇ

	CTextureRenderer	*c = w->pcTR;		// �e�N�X�`���[�����_�[�N���X

	*piWidth  = c->iWidth;
	*piHeight = c->iHeight;
}



/*------------------------------------------------------------------------------*
| <<< ���[�r�[�̃��C������ >>>
|	�� ���[�r�[�Đ����� game_main �ň�x�Ăяo���̂���
*------------------------------------------------------------------------------*/
void	movie_to_texture_main(void)
{

	// ���[�v�`�F�b�N
	movie_to_texture_loop_check();

	// �|�[�Y�`�F�b�N
	for(int	i =	0; i < MOVIE_MAX; i++)
	{
		MOVIE_TO_TEXTURE_WORK	*w = &movie[i];

		// �����[�h�����ׂ�
		if(!w->bLoadFlag || w->bSystemPause){ continue;}

		// �|�[�Y�͈�t���[�����o������Ɋ|����
		if(w->iPauseFlag == 2)
		{
			LONGLONG		lEndPos = 0;
			if(w->pMS != NULL){ w->pMS->GetPositions(&w->llPausePosition, &lEndPos);}
			if(w->pMC != NULL){ w->pMC->Pause();}
			w->bPlayState = false;
			w->iPauseFlag = 0;
		}
		// �ꎞ��~�����̏ꍇ
		if(w->iPauseFlag == -1)
		{
			if(w->pMS != NULL){ w->pMS->SetPositions(&w->llPausePosition, AM_SEEKING_AbsolutePositioning, &w->llPausePosition, AM_SEEKING_NoFlush);}
			if(w->pMC != NULL){ w->pMC->Run();}
			w->bPlayState = true;
			w->iPauseFlag = 0;
		}

		//--- �]�� --------------------------------------------------
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

