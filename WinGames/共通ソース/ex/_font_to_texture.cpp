//
// �t�H���g���e�N�X�`���[�֓W�J
//


//#include	"main.h"


#undef	ADR_
#define	ADR_(x, y)		((y) * (DWORD)d3.texInfo[iTex].fWidth + (x))


static	int	iNextX, iNextY;					// ����`����W



/*------------------------------------------------------------------------------*
| <<< �e�N�X�`���[�ɃA�E�g���C���t�H���g���쐬 >>>
|	����	iTex        = �������쐬����e�N�X�`���[�ԍ�
|			iX, iY		= �����̕\�����W
|			strFontName = TrueType �t�H���g��
|			str		    = �`�敶��
|			iHeight     = �t�H���g��
|			iWidth      = �t�H���g�̉���
|			bItalic		= TRUE: �Α�
|			iPenSize    = �A�E�g���C���̑���(�s�N�Z��)
|			dwEdge      = �A�E�g���C���̐F
|			dwFill      = �h��Ԃ��F
|			iQuality    = �A���`�G�C���A�X���x( 1 �ȏ�̒l�ɂ��邱��)
*------------------------------------------------------------------------------*/
void	font_to_texture(int iTex, int iX, int iY, char *strFontName, int iWidth, int iHeight, BOOL bItalic, int iPenSize, DWORD dwEdge, DWORD dwFill, int iQuality, char* Format,...)
{

	//--- ������쐬 ------------------------------------------------
	char	str[512];
	va_list	ap;
	va_start(ap, Format);
	vsprintf(str, Format, ap);
	va_end(ap);

	int	iXBak = iX;
	iPenSize *= iQuality;						// �N�I���e�B�ɍ��킹�ăy�����ύX
   
   //--- �F�𕪉� --------------------------------------------------
	uchar	aucEdge[4], aucFill[4];
												// �֊s
	aucEdge[0] = (dwEdge >> 24) & 255;				// a
	aucEdge[1] = (dwEdge >> 16) & 255;				// r
	aucEdge[2] = (dwEdge >>  8) & 255;				// g
	aucEdge[3] =  dwEdge        & 255;				// b
												// �h��Ԃ��F
	aucFill[0] = (dwFill >> 24) & 255;				// a
	aucFill[1] = (dwFill >> 16) & 255;				// r
	aucFill[2] = (dwFill >>  8) & 255;				// g
	aucFill[3] =  dwFill        & 255;				// b

	//--- �f�o�C�X�R���e�L�X�g�쐬 ----------------------------------
	HDC	hDC    = GetDC(sys.hWnd);
	HDC	hdcMem = CreateCompatibleDC(hDC);
	ReleaseDC(sys.hWnd, hDC);					// hdcMem ������Εs�v�Ȃ̂ŁA���

	//--- �t�H���g�쐬 ----------------------------------------------
	LOGFONTA	lf;
	lf.lfHeight		    = iHeight * iQuality;					// �t�H���g�̍���
	lf.lfWeight		    = 1000;									// �t�H���g�̑���
	lf.lfWidth		    = iWidth  * iQuality;					// ���ϕ�����( 0 �Ńf�t�H���g�l)
	lf.lfEscapement	    = 0;									// ������������̊p�x
	lf.lfOrientation    = 0;									// �x�[�X���C���̊p�x
	lf.lfItalic		    = bItalic;								// �Α�
	lf.lfUnderline	    = FALSE;								// ����
	lf.lfStrikeOut      = FALSE;								// ��������
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;					// �o�͂̐��x
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;					// �N���b�s���O���x
	lf.lfCharSet	    = SHIFTJIS_CHARSET;						// �����Z�b�g�̎��ʎq
																// �s�b�`�ƃt�@�~��
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;			// �s�b�` : DEFAULT_PITCH(��̫)�AFIXED_PITCH(�Œ蕝)�AVARIABLE_PITCH(�ϕ�)
																// �t�@�~�� : FF_DECORATIVE(������)�AFF_DONTCARE(��̫)�AFF_ROMAN(�v���|�[�V���i���t�H���g)�AFF_SCRIPT(�菑�����t�H���g)�AFF_SWISS(�v���|�[�V���i���t�H���g)
	lf.lfQuality		= ANTIALIASED_QUALITY;					// �o�͕i�� : ANTIALIASED_QUALITY(�A���`�G�C���A�X)�ADEFAULT_QUALITY(��̫)�ACLEARTYPE_QUALITY(�N���A�^�C�v)�ANONANTIALIASED_QUALITY(�A���`�Ȃ�)
	//memcpy(lf.lfFaceName, "�l�r ����", strlen("�l�r ����"));	// �t�H���g��
	strcpy(lf.lfFaceName, strFontName); 
	HFONT	hFont	    = CreateFontIndirectA(&lf);				// �t�H���g�쐬
	HFONT	hOldFont	= (HFONT)SelectObject(hdcMem, hFont);	// �V�K�Ƀt�H���g�ݒ�

	//--- �������擾 ------------------------------------------------
	int	iCount = 0;									// ������
	while(str[iCount] != NULL)
	{
		TEXTMETRICA		tm;
		GLYPHMETRICS	gm;
		MAT2	mat  = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
		int		iLen = IsDBCSLeadByte(str[iCount]) ? 2 : 1;
		UINT	code = (iLen == 2 ? ((uchar)str[iCount] << 8) | (uchar)str[iCount + 1] : (uchar)str[iCount]);
		GetTextMetricsA( hdcMem, &tm);
		GetGlyphOutlineA(hdcMem, code, GGO_METRICS, &gm, 0, 0, &mat);

		RECT	rectFontRegion =					// �t�H���g�̎g�p����̈�
		{
						  gm.gmptGlyphOrigin.x					- iPenSize / 2,
			tm.tmAscent - gm.gmptGlyphOrigin.y					- iPenSize / 2,
						  gm.gmptGlyphOrigin.x + gm.gmBlackBoxX + iPenSize / 2,
			tm.tmAscent - gm.gmptGlyphOrigin.y + gm.gmBlackBoxY + iPenSize / 2
		};
		RECT	rectFontSize =						// �t�H���g�T�C�Y
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

		// �t�H���g�̔w�i��������(�œh��Ԃ�)
		HBRUSH	hBg = (HBRUSH)CreateSolidBrush(RGB(0, 0, 255));
		FillRect(hdcMem, &rectFontSize, hBg);
		DeleteObject(hBg);

		// �t�H���g��`��(�F�͗΁A�h�͐�)
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
		StrokeAndFillPath(hdcMem);					// �t�H���g��`���I������̂Ō��̐ݒ�ɖ߂�
		SelectObject(hdcMem, hOldBrush);
		SelectObject(hdcMem, hOldBmp);
		DeleteObject(hBrush);

		if(iPenSize > 0)
		{
			SelectObject(hdcMem, hOldPen);
			DeleteObject(hPen);
		}
		//--- BMP �ŐF�������ꂽ�t�H���g���e�N�X�`���[�֓W�J ------------
		int	iW		  = rectFontSize.right  / iQuality;
		int iH		  = rectFontSize.bottom / iQuality;
		int iQuality2 = iQuality * iQuality;

		// ���s�`�F�b�N
		if(iX + ((gm.gmCellIncX + iPenSize) / iQuality) >= d3.texInfo[iTex].fOriWidth)
		{
			iX      = iXBak;
			iY     += iHeight + 8;
			if(iY >= (d3.texInfo[iTex].fHeight - iHeight * 4) * iQuality)
			{
				break;
			}
		}

		D3DLOCKED_RECT	rectLock;					// �e�N�X�`���[�����b�N���Ă���f�[�^����������
		if(SUCCEEDED(d3.tex[iTex]->LockRect(0, &rectLock, 0, 0)))
		{
			int	iXOfs = (			   gm.gmptGlyphOrigin.x - iPenSize / 2) / (iQuality);
			int	iYOfs = (tm.tmAscent - gm.gmptGlyphOrigin.y - iPenSize / 2) / (iQuality);

			uint	uiSrcPitch = (rectFontSize.right * 3 + 3) / 4 * 4;
			
			// API �ŕ`���������𕶎��𕽋ω����Ȃ���e�N�X�`���[�֓W�J
			DWORD	*pdwD = (DWORD *)rectLock.pBits;
			for(int y = 0; y < iH; y++)
			{
				for(int x = 0; x < iW; x++)
				{
					uint	uiAlph = 0;
					uint	uiEdge = 0;
					uint	uiFill = 0;

					// ���ω�(�����Ńt�H���g�����炩�ɂȂ�܂�)
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

					// ���ߓx������ꍇ�̓G�b�W�F���̗p�A�s�����̏ꍇ�̓u�����h�F���̗p
					uint	a = 0xff - uiAlph;
					if(a < 0xff)
					{
						// ������
						a	   = (a * aucEdge[0]) >> 8;
						pdwD[ADR_(iX + iXOfs + x, iY + iYOfs + y)] |= D3_COL(a, aucEdge[1], aucEdge[2], aucEdge[3]);
					}
					else
					{
						// �s����
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
			// �e�N�X�`���[�̃��b�N������
			d3.tex[iTex]->UnlockRect(0);
		}

		// �s�v�ȃt�H���g���폜
		DeleteObject(hBitMap);

		// ���֐i�߂�
		iX     += (gm.gmCellIncX + iPenSize) / iQuality;
		iCount += iLen;
	}

	// ����`����W
	iNextX = iX;
	iNextY = iY + iHeight;

	// �I������
	SelectObject(hdcMem, hOldFont);
	DeleteObject(hFont);

	// �f�o�C�X�R���e�L�X�g���
	ReleaseDC(sys.hWnd, hdcMem);
}



/*------------------------------------------------------------------------------*
| <<< �e�N�X�`���[���N���A >>>
|	����	iTex = �N���A����e�N�X�`���[
*------------------------------------------------------------------------------*/
void	font_to_texture_clear(int iTex)
{
	if(d3.tex[iTex] != NULL)
	{
		D3DLOCKED_RECT	rectLock;				// �e�N�X�`���[�����b�N���Ă���f�[�^����������
		if(SUCCEEDED(d3.tex[iTex]->LockRect(0, &rectLock, 0, 0)))
		{
			DWORD	*d = (DWORD *)rectLock.pBits;
			for(int i = 0; i < (int)(d3.texInfo[iTex].fOriWidth * d3.texInfo[iTex].fHeight); i++)
			{
				d[i] = 0;
			}

			// �e�N�X�`���[�̃��b�N������
			d3.tex[iTex]->UnlockRect(0);
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< �`��I�����W�𓾂� >>>
|	�߂�l	�`��I�����W X
*------------------------------------------------------------------------------*/
int		font_to_texture_get_next_x(void)
{
	return iNextX;
}



/*------------------------------------------------------------------------------*
| <<< �`��I�����W�𓾂� >>>
|	�߂�l	�`��I�����W Y
*------------------------------------------------------------------------------*/
int		font_to_texture_get_next_y(void)
{
	return iNextY;
}

