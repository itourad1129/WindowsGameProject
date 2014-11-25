//
//	�h�l�d����
//


//#include	"..\\main.h"
#include	"WINNLS32.H"
#include	"mbstring.h"
#include	"mbctype.h"

#define	__STR_NUMBER	4						// d3_printf �Ŏg�p���� ID	�ԍ�
												// + 5 �܂ł��炢�͎g����
#define	__SE_OK			0						// ���艹
#define	__SE_CANCEL		0						// �L�����Z����
#define	__SE_CLICK		0						// �L�[�N���b�N��
#define	__SE_BACK		0						// BS�L�[��
							
#define	__FONT_WIDTH	35.0f					// �����̉���
#define	__FONT_WIDTH2	(__FONT_WIDTH /	2.f)	// �����̉����i���p�j
#define	__FONT_HEIGHT	d3.fontInfo[1].Height	// �����̏c��

#define	CHARBUFFER_VOL	1024					// �����R�[�h�o�b�t�@�ő�e��
#define	STRCOLOR1		D3DCOLOR_ARGB(255,	 0,	 64,  64)
#define	STRCOLOR2		D3DCOLOR_ARGB(255, 255,	255, 255)
#define	STRCOLOR3		D3DCOLOR_ARGB(128, 255,	255, 255)	// �ϊ����̉�
#define	STRCOLOR4		D3DCOLOR_ARGB(255,	 0,	255, 255)
#define	STRCOLOR5		D3DCOLOR_ARGB(255, 255,	255, 255)
#define	STRCOLOR6		D3DCOLOR_ARGB(255, 255,	 50,  50)
#define	STRCOLOR7		D3DCOLOR_ARGB(255, 255,	255,  20)
#define	STRCOLOR8		D3DCOLOR_ARGB(128,	 0,	  0, 100)	// �ϊ����̃o�b�N

struct	IME_WORK							// IME ���[�N
{
		int				input_sw;						// ���̓��[�h
		char			CharBuffer[	CHARBUFFER_VOL + 1];// ���͂��ꂽ������
		int				StPoint, EdPoint;				// �����O�o�b�t�@�p�|�C���^
		int				Point, StrLen;					// ��������͈ʒu�A������̒���
		char			InputString[CHARBUFFER_VOL + 1];
		int				ClauseData[	256];				// ���߃f�[�^
		int				ClauseNum;						// ���ߐ�
		int				InputPoint;						// ���͒��̕ҏW�������̈ʒu
		int				IMEUseFlag;						// �h�l�d�̎g�p���
		int				IMESwitch;						// �h�l�d�̋N�����
		int				ChangeFlag;						// �h�l�d���͂ɕω������������t���O
		int				IMEInputFlag;					// ���͏��������A�t���O
		PCANDIDATELIST	CandidateList;					// �ϊ����̃��X�g�f�[�^
		char			str_bak[CHARBUFFER_VOL];		// ���͕����̃o�b�N�A�b�v�p
};


static	IME_WORK	ime	={-2};



/*------------------------------------------------------------------------------*
| <<< �S�p�����A���p�������藐��钆����w��̑S�����ݕ������ł̔��p�������𓾂� >>>
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
| <<< �S�p�����A���p�������藐��钆����w��̔��p�������ł̑S�p�������𓾂� >>>
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
| <<< �o�b�t�@�ɃR�[�h���X�g�b�N���� >>>
*------------------------------------------------------------------------------*/
static	int	StokInputChar(char CharCode)
{
	//--- �o�b�t�@���I�[�o�[�t���[���Ă�����Ȃɂ����Ȃ� ---------
	if((ime.EdPoint	+ 1	== ime.StPoint)	|| (ime.StPoint	== 0 &&	ime.EdPoint	== CHARBUFFER_VOL))	return -1;

	//--- �o�b�t�@�ɕ����R�[�h���� -----------------------------
	ime.CharBuffer[ime.EdPoint]	= CharCode;
	ime.EdPoint++;
	if(ime.EdPoint == CHARBUFFER_VOL + 1) ime.EdPoint =	0;

	//--- �I�� ---------------------------------------------------
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< �h�l�d���b�Z�[�W�̃R�[���o�b�N�֐� >>>
*------------------------------------------------------------------------------*/
int		ime_proc(HWND hWnd,	UINT message, WPARAM wParam, LPARAM	lParam)
{

	HIMC	Imc;
	static	int	Flag = FALSE;

	//--- �h�l�d�����p��Ԃ������ꍇ�͂Ȃɂ������I�� -----
	if(ime.IMEUseFlag == FALSE)	return 0;

	ime.ChangeFlag = TRUE;					// �ω����������t���O�Z�b�g
	
	//--- �h�l�d�̊e���� ---------------------------------
	switch(message)
	{
	//--- �h�l�d���͊J�n���b�Z�[�W -----------------------
	case WM_IME_STARTCOMPOSITION:
		ime.InputPoint	 = 0;
		ime.IMEInputFlag = TRUE;
		break;

	//--- �h�l�d���͏I�����b�Z�[�W -----------------------
	case WM_IME_ENDCOMPOSITION:
		ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
		ime.IMEInputFlag = FALSE;
		break;

	//--- �h�l�d�����ҏW�ω����b�Z�[�W -------------------
	case WM_IME_COMPOSITION:
		{
			//snd_play(__SE_CLICK);
			HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
										// ���̓R���e�L�X�g���擾
			Imc	= ImmGetContext(DefHwnd);
			if(lParam &	GCS_COMPSTR)	// �ҏW�����ω����̏���
			{							// �ҏW��������̎擾
				ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
				ImmGetCompositionString(Imc, GCS_COMPSTR, ime.InputString, CHARBUFFER_VOL);
			}
			if(lParam &	GCS_RESULTSTR)	// �ҏW���������莞�̏���
			{
				ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
				ImmGetCompositionString(Imc, GCS_RESULTSTR,	ime.InputString, CHARBUFFER_VOL);
										// �o�b�t�@�ɃX�g�b�N
				int	StrLen = lstrlen(ime.InputString);
				for(int	i =	0; i < StrLen; i++)	StokInputChar(ime.InputString[i]);
										// �ҏW�������񏉊���
				ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
				ImmGetCompositionString(Imc, GCS_COMPSTR, ime.InputString, CHARBUFFER_VOL);
			}
			if(lParam &	GCS_CURSORPOS)	// �J�[�\���ʒu�ω����̏���
			{
				ime.InputPoint = GetStringPoint2(ime.InputString, ImmGetCompositionString(Imc, GCS_CURSORPOS, NULL,	0));
			}
			if(lParam &	GCS_COMPCLAUSE)	// ���ߏ��ω���
			{							// ���ߏ��̎擾
				ZeroMemory(ime.ClauseData, sizeof(int) * 256);
				ime.ClauseNum =	ImmGetCompositionString(Imc, GCS_COMPCLAUSE, (void *)ime.ClauseData, 256 * sizeof(int))	/ 4;
			}
										// ���̓R���e�L�X�g�̍폜
			ImmReleaseContext(DefHwnd, Imc);
		}
		break;

	//--- �h�l�d��ԕω����b�Z�[�W -----------------------
	case WM_IME_NOTIFY:
		switch(wParam)
		{
		//--- �h�l�d�̂n�m�A�n�e�e�ω� -----------------------
		case IMN_SETOPENSTATUS:
			if(Flag) break;
			Flag = TRUE;
			DefWindowProc(hWnd,	message, wParam, lParam);
			//--- �L�[��������Ă���Ԃ����Ŏ~�܂� ---------------
			{
//				_int64	Time = timeGetTime();
//				while(timeGetTime()	- Time < 2000){}
			}
			Flag = FALSE;
			//--- �h�l�d�̎g�p��Ԃ𓾂� -------------------------
			{
				HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
				Imc		  =	ImmGetContext(DefHwnd);
				ime.IMESwitch =	ImmGetOpenStatus(Imc);
				ImmReleaseContext(DefHwnd, Imc);
			}
			break;

		//--- ���ω����̏��� -------------------------------
		case IMN_CHANGECANDIDATE:
		case IMN_OPENCANDIDATE:
		case IMN_SETCANDIDATEPOS:
			{
				DWORD	BufSize;
				HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
										// ���̓R���e�L�X�g���擾
				Imc	= ImmGetContext(DefHwnd);
										// �o�b�t�@�T�C�Y�擾
				BufSize	= ImmGetCandidateList(Imc, 0, NULL,	0);
										// �o�b�t�@�p�������̊m��
				if((ime.CandidateList =	(CANDIDATELIST *)malloc(BufSize)) == NULL)
				{
					sys_error("IME:�����ϊ�����ۑ����郁����������܂���B");
					return 0;
				}
										// �f�[�^�̎擾
				ImmGetCandidateList(Imc, 0,	ime.CandidateList, BufSize);
										// ���̓R���e�L�X�g�̍폜
				ImmReleaseContext(DefHwnd, Imc);
			}
			break;

		//--- ���͌��E�C���h�E����悤�Ƃ��Ă��鎞�̏��� -
		case IMN_CLOSECANDIDATE:
										// �������̉��
			if(ime.CandidateList !=	NULL) free(ime.CandidateList);
			ime.CandidateList =	NULL;
			break;
		}
		break;
	}
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< IME�̓��̓��[�h������̎擾 >>>
*------------------------------------------------------------------------------*/
static	int		GetIMEInputModeStr(char	*GetBuffer)	
{

	DWORD	InputState,	SentenceState, Buf;
	char	*SelectStr;


	strcpy(GetBuffer, "���ړ���");

	if(!ime.IMESwitch) return -1;			// �h�l�d���g���Ă��Ȃ��Ƃ���-1��Ԃ�
										// ���̓R���e�L�X�g���擾
	HWND	DefHwnd	= ImmGetDefaultIMEWnd(sys.hWnd);
	HIMC	Imc		= ImmGetContext(DefHwnd);
										// ���̓��[�h�𓾂�
	ImmGetConversionStatus(Imc,	&InputState, &SentenceState);
										// ���f
	Buf	= InputState & (IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA);
	if(Buf == (IME_CMODE_NATIVE	| IME_CMODE_FULLSHAPE |	IME_CMODE_KATAKANA))
	{
		SelectStr =	"�S�p�J�^�J�i";
	}
	else
	{
		Buf	= InputState & (IME_CMODE_NATIVE | IME_CMODE_KATAKANA);
		if(Buf == (IME_CMODE_NATIVE	| IME_CMODE_KATAKANA))
		{
			SelectStr =	"���p�J�^�J�i";
		}
		else
		{
			Buf	= InputState & (IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE);
			if(Buf == (IME_CMODE_NATIVE	| IME_CMODE_FULLSHAPE))
			{
				SelectStr =	"�S�p�Ђ炪��";
			}
			else
			{
				Buf	= InputState & IME_CMODE_FULLSHAPE;
				if(Buf == IME_CMODE_FULLSHAPE)
				{
					SelectStr =	"�S�p�p��";
				}
				else
				{
					Buf	= InputState & IME_CMODE_ALPHANUMERIC;
					if(Buf == IME_CMODE_ALPHANUMERIC)
					{
						SelectStr =	"���p�p��";
					}
					else
					{
						SelectStr =	"�s���ȓ��̓��[�h";
					}
				}
			}
		}
	}
	lstrcpy(GetBuffer, SelectStr);
	ImmReleaseContext(DefHwnd, Imc);	// ���̓R���e�L�X�g�̍폜
	return 0;							// �I��
}



/*------------------------------------------------------------------------------*
| <<< ������̕��𓾂� >>>
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
| <<< ��ʏ�ɓ��͒��̕������`�悷�� >>>
*------------------------------------------------------------------------------*/
static	int	DrawIMEInputString(int x, int y, int SelectStringNum)
{
	int		StrLen;
	int		PointX,	  PointY;
	int		CPointX,  CPointY;
	int		Width;
	int		i;


	//--- �ϊ����̏��� -------------------------
	static	int		StrNumBak =	0;		// �ϊ������̌�␔
	for(i =	0; i < StrNumBak; i++) d3_print_off(SelectStringNum + i + 1);

	//--- ���͕������`�� -----------------------
	if(ime.ClauseNum !=	2)					// �ϊ����̑I�𕶐߂̋����\��
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

	//--- �J�[�\����`�� -------------------------
	StrLen = GetStringPoint(ime.InputString, ime.InputPoint);
										// �擪�����ʒu�܂ł̕����Z�o
	if(StrLen == 0)	CPointX	= 0;
	else			CPointX	= GetDrawStringWidth(ime.InputString, StrLen);
	CPointX	+= x;
	CPointY	 = y;
	if(!ime.CandidateList)					// �I�𕶎��񂪑��݂���ꍇ�̂ݕ`��
	{									// �J�[�\���̕`��
//D3_2DLINE(&_VEC2((float)CPointX, (float)CPointY),
//				   &_VEC2((float)CPointX, (float)CPointY + __FONT_HEIGHT), STRCOLOR4);
	}

	//--- ������`�� -----------------------------
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

	//--- ��⃊�X�g���o�Ă���ꍇ�͂��̕`�� -----
	if(ime.CandidateList)
	{
		DWORD	i;
		int		j, h, MaxWidth;
		char	*Str;
		
		//--- �Œ��I�����𒲂ׂ� -------------------
		MaxWidth = 0;
		j		 = 0;
		for(i =	(ime.CandidateList->dwSelection	/ SelectStringNum) * SelectStringNum; j	< SelectStringNum; i++,	j++)
		{
			if(i ==	ime.CandidateList->dwCount)	break;
			Str	  =	(char *)ime.CandidateList +	ime.CandidateList->dwOffset[i];
			Width =	GetDrawStringWidth(Str,	lstrlen(Str));
			if(Width > MaxWidth) MaxWidth =	Width;
		}

		//--- �`��͈͂��Z�b�g -----------------------
		h =	__FONT_HEIGHT;
		float	x0 = (float)CPointX;
		float	y0 = (float)CPointY	+ __FONT_HEIGHT;
		float	x1 = (float)CPointX	+ __FONT_WIDTH	+ MaxWidth;
		float	y1 = (float)CPointY	+ __FONT_HEIGHT	* (j + 1);

		if((int)y1 > SCR_H)				// �C���v�b�g�����`��ʒu��艺�̈ʒu�ɔz�u�ł��邩����
		{
			j =	(int)y1	- SCR_H;
			y1 -= j;					// ������␳
			y0 -= j;
		}
		if(x1 >	SCR_W)					// �C���v�b�g�����`�敝�̂����ŕ`��\�̈�E�[�ɓ��B���Ă����ꍇ�͕␳
		{
			j	= (int)x1 -	SCR_W;	
			x0 -= j;
			x1 -= j;
		}
		//--- �`��͈͂����œh��Ԃ� ---------------
		/*d3_2d_box_set(&_VEC3(x0 - 3,	y0 - 3,	1),
					   &_VEC3(x1 + 3,	y1 + 4,	1),	STRCOLOR3, true,
					   D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		d3_2d_box_set(&_VEC3(x0 - 1,	y0 - 1,	0),
					   &_VEC3(x1 + 1,	y1 + 2,	0),	STRCOLOR8, true,
					   D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		*/

		//--- ����`�� -----------------------------
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

	//--- �I�� -----------------------------------
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< �h�l�d���g�p���邩�ǂ������Z�b�g���� >>>
|	����	UseFlag	= true	: �g�p����
|					  false	: �g�p���Ȃ�
*------------------------------------------------------------------------------*/
static	void	SetUseIMEFlag(int UseFlag)
{

	if(UseFlag == ime.IMEUseFlag) return;	// �t���O���ȑO�Ɠ����ꍇ�͂Ȃɂ������I��
	WINNLSEnableIME(sys.hWnd, UseFlag);		// �L���t���O���Z�b�g
	ime.IMEUseFlag = UseFlag;				// �t���O�ۑ�
	if(UseFlag == FALSE)					// ����FALSE�������ꍇ�͊e�탁�������������3
	{
		if(ime.CandidateList !=	NULL) free(ime.CandidateList);
		ime.CandidateList =	NULL;
		ZeroMemory(ime.InputString,	CHARBUFFER_VOL);
		ime.IMEInputFlag = FALSE;
	}
}



/*------------------------------------------------------------------------------*
| <<< ���ړ��̓`�F�b�N >>>
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
| <<< ���͂�r���ŃL�����Z�� >>>
|	����	*str	= ������o�b�t�@
|			str_len	= ������o�b�t�@�̑傫��
*------------------------------------------------------------------------------*/
void	ime_cancel(char	*str, int str_len)
{
	d3_print_off(__STR_NUMBER);
	d3_print_off(__STR_NUMBER + 1);
	//snd_play(__SE_CANCEL);
	SetUseIMEFlag(false);				// �h�l�d����
	d3_print_off(0);					// ���[�h����
	pad.bKeyClick[KEY_ESC] = false;
	strncpy(str, ime.str_bak, str_len);
	ime.input_sw = -2;
	return;
}



/*------------------------------------------------------------------------------*
| <<< �������� >>>
|	����	*str	= ������o�b�t�@
|			str_len	= ������o�b�t�@�̑傫��
|	�߂�l	true  :	���͏I��
|			false :	���͒�
*------------------------------------------------------------------------------*/
bool	ime_input(VEC2 *pos, char *str, int str_len)
{
	
	//--- ���[�h�ʕ��� ------------------------------------------
	switch(ime.input_sw)
	{
	case -2:							// ������
		SetUseIMEFlag(true);
		ime.input_sw = 0;
		ime.StrLen	 = strlen(str);
		if(str_len > (CHARBUFFER_VOL - 1)) str_len = CHARBUFFER_VOL	- 1;
		strncpy(ime.str_bak, str, str_len);
		return false;
	}

	char	tmp[CHARBUFFER_VOL];
	static	char	InputString_bak	= NULL;


	//--- ���̓��[�h�擾���\�� ----------------------------------
	GetIMEInputModeStr(tmp);
	d3_printf(__STR_NUMBER, SCR_W - 120, 4, STRCOLOR1,	"%s", tmp);

	//--- ������̃R�s�[ ----------------------------------------
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
		//--- ���ړ��� ----------------------------------------------
		char	key	= input_key_check();
		if(key != NULL)
		{
			//snd_play(__SE_CLICK);
			ZeroMemory(tmp,	sizeof(tmp));		// ���̂�����͕ϊ��O�̕������`�悷�鏈��
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
		//--- �d�r�b�L�[�ŃL�����Z�� --------------------------------
		if(pad.bKeyClick[KEY_ESC])
		{
/*			d3_print_off(__STR_NUMBER);
			d3_print_off(__STR_NUMBER + 1);
			snd_play(__SE_CANCEL);
			SetUseIMEFlag(false);				// �h�l�d����
			d3_print_off(0);					// ���[�h����
			pad_key_1st[KEY_ESC] = FALSE;
			strncpy(str, ime.str_bak, str_len);
			ime.input_sw = -2;
*/
			ime_cancel(str,	str_len);
			return true;
		}
		//--- �G���^�[�L�[�ŏI�� ------------------------------------
		if(pad.bKeyClick[KEY_ENTER])
		{
			d3_print_off(__STR_NUMBER);
			d3_print_off(__STR_NUMBER + 1);
			//snd_play(__SE_OK);
			SetUseIMEFlag(false);				// �h�l�d����
			d3_print_off(0);					// ���[�h����
			ime.input_sw = -2;
			return true;
		}
		//--- �����w������ ------------------------------------------
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
		//--- �a�r�A�c�d�k�L�[�ňꕶ������ --------------------------
		if((PAD_KEY_REP(KEY_BACKSPACE) || pad.bKeyClick[KEY_BACKSPACE]) &&	ime.Point >= dat0)
		{
			//snd_play(__SE_BACK);
			ZeroMemory(tmp,	sizeof(tmp));		// ���̂�����͕ϊ��O�̕������`�悷�鏈��
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
			ZeroMemory(tmp,	sizeof(tmp));		// ���̂�����͕ϊ��O�̕������`�悷�鏈��
			strncat(tmp, &str[0], ime.Point);
			strcat(tmp,	&str[ime.Point + dat1]);
			strcpy(str,	tmp);
			ime.StrLen = (int)strlen(str);
			if(ime.Point > ime.StrLen) ime.Point = ime.StrLen;
		}
	}
	//--- �����ϊ����ɂd�r�b�L�[�������ꂽ�ꍇ ------------------
	else
	{
		if(pad.bKeyClick[KEY_ESC])	pad.bKeyClick[KEY_ESC]	= FALSE;
	}

	//--- ���ۂɕ����`�� ----------------------------------------
	ZeroMemory(tmp,	sizeof(tmp));		// ���̂�����͕ϊ��O�̕������`�悷�鏈��
	strncat(tmp, &str[0], ime.Point);
	strcat(&tmp[ime.Point],	ime.InputString);
	strcat(&tmp[ime.Point +	strlen(ime.InputString)], &str[ime.Point]);

	d3_printf(__STR_NUMBER	+ 1, (int)pos->x, (int)pos->y, STRCOLOR1, tmp);
	int	x =	GetDrawStringWidth(tmp,	ime.Point);//strlen(tmp));
	DrawIMEInputString((int)pos->x + x,	(int)pos->y, __STR_NUMBER +	2);

	//--- ����ׂ̈ɕϊ�������̐擪�̂ݕۑ� --------------------
	InputString_bak	= ime.InputString[0];

	return false;
}



/*------------------------------------------------------------------------------*
| <<< �������͏����� >>>
|	����	*str = ���͕�����
*------------------------------------------------------------------------------*/
void	ime_clear(char *str)
{
	ime.Point =	0;
}