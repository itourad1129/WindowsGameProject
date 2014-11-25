//
//	�A���`�G�C���A�X�t�H���g�֌W
//


//--- �萔 ----------------------------------------------------------------------------------------
#define	AFONT_W		16						// �A���`�t�H���g�̃f�t�H���g�̃T�C�Y
#define	AFONT_H		32


//--- �}�N�� --------------------------------------------------------------------------------------
#define	AFONT_ADR(id, x, y)	((y) * (uint)d3.texInfo[TEX_AFONT + id].fWidth + (x))



//--- �ϐ� ----------------------------------------------------------------------------------------
		AFONT_WORK	afont[AFONT_MAX];				// �A���`�t�H���g�p




/*------------------------------------------------------------------------------*
| <<< �A���`�G�C���A�X�t�H���g���J�� >>>
|	����	iID = �Ǘ��ԍ�
*------------------------------------------------------------------------------*/
void	afont_free(int iID)
{
	AFONT_WORK	*a = &afont[iID];

	//--- �o�b�t�@�͊J�����Ă��� ------------------------------------
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
| <<< �A���`�G�C���A�X�t�H���g��S�ĊJ�� >>>
*------------------------------------------------------------------------------*/
void	afont_free(void)
{
	for(int i = 0; i < AFONT_MAX; i++) afont_free(i);
}


/*------------------------------------------------------------------------------*
| <<< �A���`�G�C���A�X�t�H���g�̃s�N�Z�����𓾂� >>>
|	����	iID    = �Ǘ��ԍ�
|			uiCode = �����R�[�h
|	�߂�l	false  = �����𓾂��Ȃ�����
*------------------------------------------------------------------------------*/
static	bool	AFontPixelGet(int iID, uint uiCode, TEXTMETRIC *tm)
{

	AFONT_WORK	*a = &afont[iID];

	//--- �r�b�g�}�b�v�擾 ------------------------------------------
	MAT2	m   = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
	uint	fmt = GGO_GRAY8_BITMAP;			// �ق��Ɂ@GGO_GRAY2_BITMAP�AGGO_GRAY4_BITMAP ������

	//--- �A���`�t�H���g�����o�� ----------------------------------
	HDC		hDc = d3.font[a->cFontNum]->GetDC();

	int	iSize = GetGlyphOutline(hDc, uiCode, fmt, &a->gmAnt, 0, NULL, &m);
	if(iSize == 0){ return false;}
	GetGlyphOutline(hDc, uiCode, fmt, &a->gmAnt, iSize, a->pBuf, &m);
	GetTextMetrics( hDc, tm);

	//--- �����̉��������߂� ----------------------------------------
	int		iW = d3.fontInfo[a->cFontNum].Width;
	int		iH = d3.fontInfo[a->cFontNum].Height;
	if(uiCode >= 0x100){ iW *= 2;}			// �S�p�Ȃ�Ή����͓�{�ɂȂ�

	return true;
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g���e�N�X�`���[�ɒ����� >>>
|	����	iID      = �Ǘ��ԍ�
|			buf      = VRAM �̃A�h���X
|			usX, usY = �����̕`����W
|			*str     = ����(�V�t�gJIS or Ascii)
*------------------------------------------------------------------------------*/
static	void	AFontDraw(int iID, int iTexNum, DWORD *buf, ushort usX, ushort usY, char *str)
{

	AFONT_WORK	*a = &afont[iID];

	int			iW     = d3.fontInfo[a->cFontNum].Width;
	int			iH     = d3.fontInfo[a->cFontNum].Height;
	uint		uiSize = (iW * 2) * (iH * 2);
	TEXTMETRIC	tm;

	//--- �����R�[�h���A���`�t�H���g�𓾂� ------------------------
	// a->pBuf �ɕ����̏�񂪕Ԃ��Ă��܂�
	if(!ISKANJI(str[0]))					// ���p
	{
		AFontPixelGet(iID, *str, &tm);
	}
	else									// �S�p
	{
		AFontPixelGet(iID, (((uchar)str[0] << 8 | (uchar)str[1])), &tm);
	}

	//--- VRAM �ɒ��ŏ������� ---------------------------------------
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
			// 65 �K���Œl���Ԃ��Ă���̂ŁA�K���ɉ��H����
			int	c = p[uiAdr];
			if(c <= 1){ continue;}
			c = (c - 2) * 4 + 3;
			// �����ȕ����̈ʒu��␳���Ȃ���`��
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
				
				// �e�����Ă݂�
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
| <<< �e�L�X�g�ʂ��X�N���[�� >>>
|	����	iID    = �Ǘ��ԍ�
*----------------------------------------------------------------------*/
#define	AFONT_SCROLL_SPEED	4				// �����̃X�N���[�����x
static	void	AfontScroll(int iID)
{

	AFONT_WORK	*a = &afont[iID];

	//--- �J�E���^�[�v�Z --------------------------------------------
	a->sScroll -= AFONT_SCROLL_SPEED;
	if(a->sScroll < 0) a->sScroll = 0;

	//--- �X�N���[���A�b�v ------------------------------------------
	// �����_�����O�������g���ƁA3D �`��̍ۂɂ��܂��s���Ȃ�
/*
	d3_offscreen_begin(TEX_AFONT + iID);
	D3_CLEAR_ZBUFFER();
	d3_stat_alpha_set(false);
	d3_stat_filter_set(D3DTEXF_POINT);				// ���̐ݒ�ŕ�������������(�|�C���g)
	D3_TEXTURE(        TEX_AFONT + iID);
	
	float	v = d3.texInfo[TEX_AFONT + iID].fHeight - AFONT_SCROLL_SPEED;
	float	h = d3.texInfo[TEX_AFONT + iID].fHeight;
	float	u = d3.texInfo[TEX_AFONT + iID].fWidth;
	
	// �P���ɂP�h�b�g���̃e�N�X�`���[��`�悵�Ă��邾���ł��B
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

	//--- ������ACPU �œ]�� ----------------------------------------
	DWORD	*buf = d3_offscreen_capture_begin(TEX_AFONT + iID);

	uint	w = (uint)d3.texInfo[TEX_AFONT + iID].fWidth;
	uint	h = (uint)(a->usY1 -  a->usY0);//d3.texInfo[TEX_AFONT + iID].fHeight;
											// �R�s�[
	memcpy(&buf[a->usY0 * w], &buf[(a->usY0 + AFONT_SCROLL_SPEED) * w], (h - AFONT_SCROLL_SPEED) * w * sizeof(DWORD));
											// �ŉ��s�̓N���A�[
	memset(&buf[(a->usY1 - AFONT_SCROLL_SPEED) * w], 0, AFONT_SCROLL_SPEED  * w * sizeof(DWORD));

	d3_offscreen_capture_end();
}



/*------------------------------------------------------------------------------*
| <<< �֑������`�F�b�N >>>
|	�߂�l	true : �֑�����
*------------------------------------------------------------------------------*/
static	bool	IsKinsokuCheck(uchar *str)
{

	if( ! ISKANJI(str[0])){ return false;}	// ���p�H
	
	switch((str[0] << 8) | str[1])
	{
	case '�A':
	case '�B':
	case '�v':
	case '�t':
	case '�x':
	case '�H':
	case '�j':
	case '�I':
	case '�g':
	case '�h':
		return true;
	}
	return false;
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�ŕ����\�� >>>
|	����	iID    = �Ǘ��ԍ�
|			iX, iY = �����̕\���ʒu
|			Format = printf �Ɠ����t�H�[�}�b�g
*------------------------------------------------------------------------------*/
void	afont_printf(int iID, int iX, int iY, char* Format,...)
{

	AFONT_WORK	*a = &afont[iID];

	//--- ������ ----------------------------------------------------
	if(!a->bInit)
	{
		a->bInit = true;
		int	iW   = d3.fontInfo[a->cFontNum].Width * 2;
		int	iH   = d3.fontInfo[a->cFontNum].Height;
	
		a->vecP0      = VEC2(                 0,                  0);
		a->vecP1      = VEC2((float)d3.sRenderW, (float)d3.sRenderH);
		a->fSpeed     = 0.0f;				// ����
		a->fCount     = 0;
											// �o�b�t�@�͕����̃T�C�Y * 4 �Ŋm�ۂ��Ă���
		a->pBuf       = (uchar*)malloc((iW * 2) * (iH * 2));
		a->usX0       = 0;
		a->usY0       = 0;
		a->usX1       = d3.sRenderW;
		a->usY1       = d3.sRenderH;
		//a->sOfsW      = 0;				// �������̉��Z�l
		//a->sOfsH      = 0;				// �������̉��Z�l
		a->sSnd		  = SND_NONE_;
		a->bUse       = true;
		a->dwCol	  = D3_COL_WHITE(255);
		d3_offscreen_create(TEX_AFONT + iID, a->usX1, a->usY1, D3DFMT_A8R8G8B8);
		d3.texInfo[TEX_AFONT + iID].bRepair = true;
	}

	//--- �L�[�҂� --------------------------------------------------
	if(a->bKey)
	{
		if(pad.bLeftClick){ a->bKey = false;}
		return;
	}

	//--- ������쐬 ------------------------------------------------
	char	str[256];
	va_list	ap;
	va_start(ap,  Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	//--- �������O��Ɠ����Ȃ�΂Ȃɂ����Ȃ��ŋA�� ------------------
	if(a->pOriStr != NULL)					// ���ׂ�̂̓��������m�ۂ���Ă���Ƃ������ŗǂ�
	{
		if(strcmp(a->pOriStr, str) == 0)
		{
			///if(strlen(str) == (uint)a->fCount) return;
			if(*a->pStr == NULL){ return;}
											// �����̑��x���݂�(fSpeed �� 0 �Ȃ瑦��)
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
	if(a->pOriStr == NULL)					// NULL �̎��̂ݕ�������R�s�[
	{
		a->pOriStr = (char*)malloc(strlen(str) + 1);
		strcpy(a->pOriStr, str);
		a->pStr    = a->pOriStr;			// pStr �̓��[�N�p

		// �N���A
		d3_offscreen_begin(TEX_AFONT + iID);
		d3_clear(		   D3_COL(0, 0, 0, 0));
		d3_offscreen_end(  TEX_AFONT + iID);

		// �����ʒu
		a->usX     = iX + a->usX0;
		a->usY     = iY + a->usY0;
		a->sScroll = 0;

		// �X�C�b�`
		a->bView = true;
		//return;								// ���̎������`��
	}

	//--- �X�N���[�� ------------------------------------------------
	if(a->sScroll > 0)
	{
		AfontScroll(iID);
		return;
	}

	//--- �I�t�X�N���[���L���v�`���[�J�n ----------------------------
	DWORD	*buf = d3_offscreen_capture_begin(TEX_AFONT + iID);

	//--- ������������ ----------------------------------------------
	for(;;)
	{
		if(*a->pStr != NULL)
		{
			//--- ���̂�\���������v�Z ------------------------------
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
			//--- ����̕�������`�F�b�N ----------------------------
			// �L�[�E�G�C�g
			a->bKey = strncmp(a->pStr, "@key", 4) == 0;
			if(a->bKey){ a->pStr += 4; break;}
					
			// ���s
			bool	bKai = strncmp(a->pStr, "\r\n", 2) == 0 || strncmp(a->pStr, "@n", 2) == 0;

			// �F
			uint	uiA, uiR, uiG, uiB;
			char	*s;
			if(strncmp(a->pStr, "@c", 2) == 0)
			{
				a->pStr += 2;
				char	buf[256];
				strncpy(buf, a->pStr, sizeof(buf));
				buf[255] = NULL;

				char	*token  = " ,()\r\n";
				s		 = strtok(buf,  token);	// ( ��ǂݔ�΂�
				uiA		 = atoi(s);
				s		 = strtok(NULL, token);	// red
				uiR		 = atoi(s);
				s		 = strtok(NULL, token);	// gree
				uiG		 = atoi(s);
				s		 = strtok(NULL, token);	// blue
				uiB		 = atoi(s);
				s		 = strtok(NULL, token);	// ) ��ǂݔ�΂�
				a->pStr += (s - buf);			// ������i�߂�
				a->dwCol = D3_COL(uiA, uiR, uiG, uiB);
			}

			//-------------------------------------------------------

			// �`��
			if(!bKai)
			{
				AFontDraw(iID, TEX_AFONT + iID, buf, a->usX, a->usY, a->pStr);
				snd_play(a->sSnd);
			}

			// ������i�߂�
			if(ISKANJI(*a->pStr) || bKai){ a->pStr += 2;}
			else						 { a->pStr += 1;}

			// ������i�߂�
			if(!bKai){ a->usX += a->gmAnt.gmCellIncX + a->sOfsW;}

			// �I���`�F�b�N
			if(*a->pStr == NULL)
			{
				// �����̕\���I���ʒu���m�肵�Ă���
				a->usY += d3.fontInfo[a->cFontNum].Height + a->sOfsH;
				break;
			}

			// ���s�`�F�b�N
			if((!IsKinsokuCheck((uchar*)a->pStr) && (a->usX + d3.fontInfo[a->cFontNum].Width * 2) > a->usX1) || bKai)
			{
				ushort	h = d3.fontInfo[a->cFontNum].Height + a->sOfsH;

				a->usX  = a->usX0 + iX;
				a->usY += h;
	
				// �X�N���[���A�b�v�𒲂ׂ�
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

	//--- �I�t�X�N���[���L���v�`���[���� ----------------------------
	d3_offscreen_capture_end();

	//--- �\���������v�Z --------------------------------------------
					//if(a->usCountBak == (int)a->fCount){ return;}
	//a->usCountBak = (int)a->fCount;
}



/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�ŕ����\�� >>>
|	����	iID    = �Ǘ��ԍ�
|			Format = printf �Ɠ����t�H�[�}�b�g
*------------------------------------------------------------------------------*/
void	afont_printf(int iID, char* Format,...)
{
	//--- ������쐬 ------------------------------------------------
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
	//--- ������쐬 ------------------------------------------------
	char	str[256];
	va_list	ap;
	va_start(ap,  Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	afont_printf(0, 0, 0, str);
}



/*------------------------------------------------------------------------------*
| <<< �w��̃e�N�X�`���[�փA���`�t�H���g�������_�����O >>>
|	����	iTex     = �e�N�X�`���[�ԍ�
|			iFontNum = �t�H���g�ԍ�
|			iX, iY   = �����̕\���ʒu
|			iLen     = �\��������(-1 �őS��)
|			Format   = printf �Ɠ����t�H�[�}�b�g
|	�� d3_font_draw �ƌ݊��֐��ł�
*------------------------------------------------------------------------------*/
void	afont_render(int iTex, int iFontNum, int iX, int iY, int iLen, DWORD dwCol, char* Format,...)
{

	//--- �����쐬 --------------------------------------------------
	int		iBufSize = 1024;
	char	*strBuf  = (char*)malloc(iBufSize);
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//---------------------------------------------------------------
		
	if(iLen != -1) strBuf[iLen] = NULL;

	//--- �I�t�X�N���[���L���v�`���[�J�n ----------------------------
	DWORD	*buf = d3_offscreen_capture_begin(iTex);

	//--- ������������ ----------------------------------------------
	///Printf("Draw");
	
	AFONT_WORK	bak   =  afont[0];
	AFONT_WORK	*a    = &afont[0];
	char			*pStr = strBuf;			// ������
	int				x     = iX;
	int				y     = iY;

	a->cFontNum			  = iFontNum;

	int				iW    = d3.fontInfo[a->cFontNum].Width * 2;
	int				iH    = d3.fontInfo[a->cFontNum].Height;
											// �o�b�t�@�m��
	a->pBuf				  = (uchar*)malloc((iW * 2) * (iH * 2));
	memset(a->pBuf, 0, (iW * 2) * (iH * 2));
	//a->pBuf				  = (uchar*)strBuf;

	for(;;)
	{
		if(*pStr != NULL)
		{
			//--- ����̕�������`�F�b�N ----------------------------
					
			// ���s
			bool	bKai = strncmp(pStr, "\r\n", 2) == 0;
			//-------------------------------------------------------

			// �`��
			if(!bKai){ AFontDraw(0, iTex, buf, x, y, pStr);}

			// ������i�߂�
			if(ISKANJI(*pStr) || bKai){ pStr += 2;}
			else					  { pStr += 1;}

			if(pStr == NULL){ break;}

			// ������i�߂�
			if(!bKai){ x += a->gmAnt.gmCellIncX;}
			
			// ���s�`�F�b�N
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

	//--- �I�t�X�N���[���L���v�`���[���� ----------------------------
	d3_offscreen_capture_end();

	free(a->pBuf);
	afont[0] = bak;
	free(strBuf);
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�ŕ����\���̕\���X�C�b�` >>>
|	����	iID    = �Ǘ��ԍ�
*------------------------------------------------------------------------------*/
void	afont_print_sw(int iID, bool bSw)
{
	AFONT_WORK	*a = &afont[iID];

	a->bView = bSw;
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�ňꊇ���Ă̕\���X�C�b�` >>>
|	����	iID    = �Ǘ��ԍ�
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
| <<< �A���`�t�H���g�Ŏg�p���镶���ԍ���ݒ� >>>
|	����	iID      = �Ǘ��ԍ�
|			iFontNum = �t�H���g�ԍ�(d3_font_create �ō쐬�����t�H���g)
*------------------------------------------------------------------------------*/
void	afont_font_change(int iID, int iFontNum)
{
	afont_free(iID);

	AFONT_WORK	*a = &afont[iID];
	a->cFontNum = iFontNum;
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g������ >>>
|	����	iID                = �Ǘ��ԍ�
|			iFontNum           = �t�H���g�ԍ�(d3_font_create �ō쐬�����t�H���g)
|			iRenderW, iRenderH = �����_�����O�ʂ̃T�C�Y
*------------------------------------------------------------------------------*/
void	afont_create(int iID, int iFontNum, int iRenderW, int iRenderH)
{

	afont_free(iID);						// �o�b�t�@�J��

	AFONT_WORK	*a = &afont[iID];

	a->cFontNum   = iFontNum;
	a->bInit	  = true;
	int	iW	 	  = d3.fontInfo[a->cFontNum].Width * 2;
	int	iH		  = d3.fontInfo[a->cFontNum].Height;

	a->usX0       = 0;
	a->usY0       = 0;
	a->usX1       = iRenderW;
	a->usY1       = iRenderH;
	a->sOfsW      = 0;						// �������̉��Z�l
	a->sOfsH      = 0;
	a->vecP0      = VEC2(              0,               0);
	a->vecP1      = VEC2((float)iRenderW, (float)iRenderH);
	a->fSpeed     = 0.0f;					// ����
	a->fCount     = 0;
	a->sSnd		  = SND_NONE_;
	a->bUse       = true;
	a->dwCol	  = D3_COL_WHITE(255);
											// �o�b�t�@�͕����̃T�C�Y * 4 �Ŋm�ۂ��Ă���
	a->pBuf       = (uchar*)malloc((iW * 2) * (iH * 2));
	d3_offscreen_create(TEX_AFONT + iID, a->usX1, a->usY1, D3DFMT_A8R8G8B8);
	d3.texInfo[TEX_AFONT + iID].bRepair = true;
}

void	afont_create(int iFontNum, int iRenderW, int iRenderH)
{
	afont_create(0, iFontNum, iRenderW, iRenderH);
}



/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�̕������x�ݒ� >>>
|	����	iID    = �Ǘ��ԍ�
|			fSpeed = 0.0f �` 1.0f(0.0f �ő���)
*------------------------------------------------------------------------------*/
void	afont_speed_set(int iID, float fSpeed)
{
	AFONT_WORK	*a = &afont[iID];

	a->fSpeed = fSpeed;
}



/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�̕��������o�����W�ݒ� >>>
|	����	iID    = �Ǘ��ԍ�
|			r      = �����̏����o���ʒu
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
| <<< ��������T�C�Y�ݒ� >>>	���t�H���g�̃T�C�Y�ł͂Ȃ��A��������ς���
|	����	iID    = �Ǘ��ԍ�
|			iW, iH = ��������l(�����̊Ԋu�����p)
*------------------------------------------------------------------------------*/
void	afont_font_size_set(int iID, int iW, int iH)
{
	AFONT_WORK	*a = &afont[iID];
	a->sOfsW = iW - (short)d3.fontInfo[a->cFontNum].Width * 2;
	a->sOfsH = iH - (short)d3.fontInfo[a->cFontNum].Height;
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�̕��������o�����W�ݒ� >>>
|	����	iID    = �Ǘ��ԍ�
|			iSound = ���ʉ��ԍ�
*------------------------------------------------------------------------------*/
void	afont_sound_set(int iID, int iSound)
{
	AFONT_WORK	*a = &afont[iID];
	a->sSnd = iSound;
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�̕������\���I�����Ă��邩���ׂ� >>>
|	����	iID  = �Ǘ��ԍ�
|	�߂�l	true  : �����`�撆
|			false : �I�����Ă���
*------------------------------------------------------------------------------*/
bool	afont_print_check(int iID)
{
	AFONT_WORK	*a = &afont[iID];
	
	if( a->pStr == NULL){ return false;}
	if(*a->pStr == NULL){ return false;}
	else				{ return true;}
}


/*------------------------------------------------------------------------------*
| <<< �A���`�t�H���g�ŕ����\�� >>>
|	����	fX, fY = �`��ʒu
|			fAlp   = ���������[�g
|			bZoom  = true : �E�B���h�E�T�C�Y�ɍ��킹�Ċg�傷��
*------------------------------------------------------------------------------*/
void	afont_draw(float fX, float fY, float fAlp, bool bZoom)
{

	d3_stat_blend_set(D3_BLEND_NORMAL);				// �������ݒ�
	d3_stat_alpha_set(true);
	d3_stat_alpha_greate_set(1);

//	d3_stat_filter_set(D3DTEXF_LINEAR);				// ���̐ݒ�ŕ��������炩(  ���j�A�[)
//	d3_stat_filter_set(D3DTEXF_POINT );				// ���̐ݒ�ŕ�������������(�|�C���g)

	AFONT_WORK	*a = afont;
	for(int i = 0; i < AFONT_MAX; i++, a++)
	{
		if(!a->bUse || !a->bView) continue;

		D3_TEXTURE(TEX_AFONT + i);
		float	w = a->vecP1.x - a->vecP0.x;
		float	h = a->vecP1.y - a->vecP0.y;
		VEC2	v0 = a->vecP0;
		VEC2	v1 = a->vecP1;
		// ��ʂɂ��킹�ăY�[��
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


