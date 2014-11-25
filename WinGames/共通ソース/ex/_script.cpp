//
//	�X�N���v�g����
//

//#include	"main.h"

//--- �萔 --------------------------------------------------------
#define	BUF_MAX		30000
#define	FONT		4						// �t�H���g�ԍ��i2 �` 7 �͈̔͂Łj
#define	FONT_W		20
#define	FONT_H		40

#define	TEXT_X0		32						// �����̕`��J�n�ʒu
#define	TEXT_Y0		320
#define	TEXT_X1		640 - 96				// �����̕`��I���ʒu
#define	TEXT_Y1		480 - 16

#define	FONT_NAME	"HG�ۺ޼��M-PRO"		// �����ꍇ�͓���̃t�H���g�ɂȂ�

#define	GOSUB_MAX	8						// @gosub �̓���q�ő吔
#define	VAR_MAX		64						// �ϐ��̍ő吔
#define	BG_MAX		3						// �摜�̖���

enum
{
	TEX_SCRIPT       = 40,
	TEX_SCRIPT_BG0,
	TEX_SCRIPT_BG1,
	TEX_SCRIPT_BG2,
	TEX_MENU_FRAME0,								// �������牺�̓��j���[�p
	TEX_MENU_FRAME1,
	TEX_MENU0,
	TEX_MENU1,
	TEX_MENU2,
};


//--- �ϐ��֌W ----------------------------------------------------
struct	VAR_WORK						// �ϐ��̍\����
{
	char	strName[64];					// �ϐ������p�� 64 �����܂ŁB
	float	fDat;							// �l
};

struct	GOSUB_WORK						// @gosub �̍\����
{
	char	strName[MAX_PATH + 1];			// �t���p�X�Ńt�@�C����������B
	int		iCount;							// �߂�ׂ��ꏊ
};

struct	SCRIPT_WORK
{
	char	strName[MAX_PATH + 1];			// ���ݎ��s���̃X�N���v�g��

	bool	bKeyWait;						// �L�[�҂�
	short	sDelay;

	//--- �o�b�t�@�p ------------------------------------------------
	char	strBuf[BUF_MAX];				// �X�N���v�g�ǂݍ��݃o�b�t�@
	int		iBufCount;						// �X�N���v�g�̍Đ��ʒu

	//--- �����\���p ------------------------------------------------
	short	sTextX,    sTextY;				// �����̕\���ʒu
	short	sTextAddX, sTextAddY;			// ���̕����̈ʒu
	char	strText[256];					// �}������
	int		iTextCount;


	//--- �T�u���[�`���p --------------------------------------------
	bool		bEnd;						// �����I���X�C�b�`(true �Ȃ璼���ɏ����𒆎~���܂�)
	GOSUB_WORK	gosub[GOSUB_MAX];			// �T�u���[�`�����[�N
	short		sGosubCount;				// �T�u���[�`���p�̓���q��

	//--- �X�N���v�g�p�ϐ� ------------------------------------------
	VAR_WORK	var[VAR_MAX];				// �X�N���v�g���̕ϐ�

	//--- ���j���[ --------------------------------------------------
	bool		bMenu;						// true : ���j���[�g�p��

	//--- �摜 ------------------------------------------------------
	float		fBGAlp[     BG_MAX];		// �A���t�@�l
	float		fBGAlpSpeed[BG_MAX];		// �t�F�[�h���̑��x

	//--- �G���[ ----------------------------------------------------
	char		strError[512];				// �G���[�̃R�}���h���i�[�p
};

SCRIPT_WORK	script, *s;


/*---- �R�}���h�e�[�u�� -----------------------------*/
static	char	*CommandTbl[] =
{
	" ",					// 00 : ���p�X�y�[�X
	"\t",					// 01 : �^�u
	"\r\n",					// 02 : �{���̉��s�R�[�h
	"//",					// 03 : �R�����g
	"@n",					// 04 : ���s
	"@wait",				// 05 : �E�G�C�g
	"@clear",				// 06 : ������N���A�[
	"@key",					// 07 : �L�[�҂�
	"#",					// 08 : ���x��
	"@goto",				// 09 : �������W�����v
	"@gosub",				// 10 : �T�u���[�`���R�[��
	"@return",				// 11 : ���^�[��
	"$",					// 12 : �ϐ�
	"%",					// 13 : �ϐ��̕\��
	"@if",					// 14 : ��������
	"@menu",				// 15 : ���j���[
	"@tex_load",			// 16 : �e�N�X�`���[���[�h
	"@fade_in",				// 17 : ���C���[�t�F�[�h�C��
	"@fade_out",			// 18 : ���C���[�t�F�[�h�A�E�g
	""
};

static	void	ComGosub(void);
static	void	ComComment(void);
static	int		ComGoto(void);



/*------------------------------------------------------------------------------*
| <<< ������̐擪�ɂ���󔒁A�^�u��i�߂� >>>
|	����	*pstrBuf = ������
|			piPos    = �Q�ƈʒu
|	�߂�l	�����������
*------------------------------------------------------------------------------*/
static	int		SPACE_DEL(char *pstrBuf, int *piPos)
{
	int	iLen = 0;
	for(;;)
	{
		uchar	a =pstrBuf[*piPos];
		if(a == ' ' || a == '\t'){ *piPos += 1, iLen++;}
		else					 { break;}
	}
	return	iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf ��蕶����𓾂� >>>
|	��"(����ٸ��ð���)�Ŏn�܂�A" �ŏI��
|	����	strBuf = �Ώە�����
|	�o��	strOut = ���o����������
|	�߂�l	���o����������
*------------------------------------------------------------------------------*/
static	int		GET_STRING(char	*strOut, char *strBuf)
{
	if(sys.bError) return 0;				// �G���[���o�Ă���ꍇ�͏������Ȃ�

	int	i, iLen	= 0;

	//--- �擪�� " ��T�� -----------------------------------------
	SPACE_DEL(strBuf, &iLen);
	if(strBuf[iLen]	== '"') 
	{
		iLen +=	1;
	}
	else
	{
		return 0;
	}

	//--- ���� " ��T�� -----------------------------------------
	for(i =	0;;i++,	iLen +=	1)
	{
		if(strBuf[iLen]	== NULL)
		{
			return 0;
		}
		else if(strBuf[iLen] ==	'"')
		{
			iLen +=	1;
			break;
		}
		if(ISKANJI(strBuf[iLen]))
		{ 	strOut[i + 0] =	strBuf[iLen	+ 0];
			strOut[i + 1] =	strBuf[iLen	+ 1];
			i++, iLen += 1;
		}
		else
		{
			strOut[i + 0] =	strBuf[iLen];
		}
	}
	
	//--- NULL ����	-----------------------------------------------
	if(iLen	> 0) strOut[i] = NULL; 
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf ��胉�x�����擾���� >>>
|	���A���_�[�o�[�A���p�p���A�S�p�����݂̂��擾����B����ȊO�Ȃ�ΏI��
|	����	strBuf = �Ώە�����
|	�o��	strOut = ���o����������
|	�߂�l	���o����������
*------------------------------------------------------------------------------*/
#define	GET_LABEL	LABEL_GET
static	int		LABEL_GET(char *strOut,	char *strBuf)
{

	if(sys.bError) return 0;				// �G���[���o�Ă���ꍇ�͏������Ȃ�

	int	i, iLen	= 0;

	SPACE_DEL(strBuf, &iLen);				// �擪�̋󔒂����
	for(i =	0;;	iLen++)						// strOut �Ƀ��x�������o��
	{
		uchar	a =	strBuf[iLen];
		if((uchar)(a - '0')	<=	9){	strOut[i] =	a; i++;	continue;}	// �������H
		if((uchar)(a - 'a')	<  26){	strOut[i] =	a; i++;	continue;}	// ���������H
		if((uchar)(a - 'A')	<  26){	strOut[i] =	a; i++;	continue;}	// �啶�����H
		if((uchar)a		   == '_'){	strOut[i] =	a; i++;	continue;}	// �A���_�[�o�[���H
		if(ISKANJI(a))												// �S�p�H
		{
			strOut[i + 0] =	strBuf[iLen	+ 0];
			strOut[i + 1] =	strBuf[iLen	+ 1];
			i += 2,	iLen++;
			continue;
		}
		break;
	}
	strOut[i] =	NULL;
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf ��萔�l�𓾂�	>>>
|	����	*Buf = ������̈ʒu
|	�o��	fOut = �������蓾�����l
|	�߂�l	���o����������
*------------------------------------------------------------------------------*/
static	int		PARAM_GET(float	*fOut, char	*strBuf)
{

	if(sys.bError){ return 0;}				// �G���[���o�Ă���ꍇ�͏������Ȃ�

	float	fAns      =	0.f;
	float	fFlag     =	1.f;
	int		iLen      =	0;					// ������
	int		iSpaceLen = 0;

	iSpaceLen += SPACE_DEL(strBuf, &iLen);	// �擪�̋󔒂����

	if(strBuf[iLen]	== '-')					// �ꕶ���ڂ̓}�C�i�X���H
	{
		fFlag  = -1;
		iLen++;
	}
	int	i;
	for(i =	0;;	i++)						// �������������o���B
	{
		uchar	a =	strBuf[iLen] - '0';		// �ꕶ���Q�b�g���ĕ����̂O�������Ă݂�
		if(a < 10)							// ���l�̂P�O�ȉ��Ȃ�΂���͕����ɂȂ�B
		{
			fAns  *= 10;					// �O�̒l���P�O�{����
			fAns  += a;						// �����l�𑫂��B
			iLen++;							// ���̕����֐i�߂�B
		}
		else
		{
			break;							// ��������Ȃ����炱����for��������
		}
	}
	if(strBuf[iLen]	!= '.')
	{
		iSpaceLen += SPACE_DEL(strBuf, &iLen);		// ���̋󔒂����
		*fOut =	fAns * fFlag;
		if(iLen	== 0 || iSpaceLen == iLen){ sys.bError = true;}
		return iLen;								// �����_�łȂ��Ȃ�A���l��Ԃ��Ă����܂��B
	}
	float	fMulti = 0.1f;
	iLen++;
	for(;;i++)								// �������������o���B
	{										// �ꕶ���Q�b�g���ĕ����̂O�������Ă݂�
		uchar	a =	(uchar)(strBuf[iLen] - '0');
		if(a < 10)							// ���l�̂P�O�ȉ��Ȃ�΂���͕����ɂȂ�B
		{
			fAns   += (float)a * fMulti;	// �����l��multi�{���đ����B
			fMulti *= 0.1f;					// multi������1/10����i����������i�߂�j�B
			iLen++;							// ���̕����֐i�߂�B
		}
		else
		{
			break;							// ��������Ȃ����炱����for��������
		}
	}

	if(iLen	== 0){ sys.bError = true;}

	SPACE_DEL(strBuf, &iLen);				// ���̋󔒂����
	*fOut =	fAns * fFlag;
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< �e�L�X�g��͂̃G���[���� >>>
*------------------------------------------------------------------------------*/
void	ScriptError(char *Format, ...)
{
	int	iKai = 1, iPos = 0;

	while(s->iBufCount > iPos)
	{										// ������i�߂�
		if(ISKANJI(s->strBuf[iPos])) iPos += 2;
											// ���s�񐔂𐔂���
		else if(strncmp(&s->strBuf[iPos], "\r\n", 2) == 0) iKai++, iPos += 2;
		else if(s->strBuf[iPos] == NULL) break;			// NULL �ł����[�v���~�߂�B
		else iPos++;
	}

	char	str[512];
	vsprintf(str, Format, (char	*)(&Format + 1));
	sys_error("%s: %d �s�� : %s", s->strName, iKai, str);
}


/*------------------------------------------------------------------------------*
| <<< �ϐ�������ϐ��ԍ������� >>>			���V�K�ɏo�Ă������͓̂o�^����B
|	����	strBuf = ������̈ʒu
|	�o��	fOut   = �����񂩂�E���o�������l
|	�߂�l	�ϐ����̓o�^�ԍ�
*------------------------------------------------------------------------------*/
int		VarNumGet(int *iOut, char *strBuf)
{
	int		iLen;
	char	strName[256];						// �ϐ����i�[�p

	iLen		 = GET_LABEL(strName, strBuf);
	int	iNameLen = strlen(   strName);
	int	i;
	for(i = 0;;i++)								// �ϐ���(name)���o�^����Ă��邩���ׂ�
	{											// �o�^����Ă��Ȃ���Γo�^����B
		if(strlen(s->var[i].strName) == 0)		// �����񂪓o�^����Ă��Ȃ����
		{										// �o�^���ă`�F�b�N�I��
			strcpy(s->var[i].strName, strName);	// �ϐ�����o�^����
			break;								// ���[�v������
		}
		if(strncmp(s->var[i].strName, strName, iNameLen) == 0)
		{
			break;								// �����Ȃ�΂����Ń��[�v������
		}
	}
	*iOut = i;									// �o�^�ԍ��Z�b�g

	SPACE_DEL(strBuf, &iLen);					// ���̋󔒂����
	return iLen;								// �Ō�ɕ�������Ԃ�
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h�̃p�����[�^�[�𓾂� >>>
|	����	strBuf = ������̈ʒu
|	�o��	fOut   = �����񂩂�E���o�������l
|	�߂�l	�����񂩂�E���o��������
*------------------------------------------------------------------------------*/
int		ParamGet(float *fOut, char *strBuf)
{
	if(sys.bError) return 0;				// �G���[���������Ă���Ƃ��͏������Ȃ�

	float	fAns  = 0.f;
	int		iLen  = 0;
	float	fFlag = 1.f;

	SPACE_DEL(strBuf, &iLen);				// �擪�̋󔒂����
	if(strBuf[iLen] == '-')					// �ꕶ���ڂ̓}�C�i�X���H
	{
		fFlag = -1;
		iLen++;
	}
	SPACE_DEL(strBuf, &iLen);				// �擪�̋󔒂����
	if(strBuf[iLen] != '$')					// �ꕶ���ڂ� $ ���H $ �Ȃ� else �ȍ~�̕ϐ�������
	{
		iLen += PARAM_GET(&fAns, &strBuf[iLen]);
	}
	else
	{
		iLen++;								// $ ��i�߂�
		int	iID;
		iLen += VarNumGet(&iID, &strBuf[iLen]);
		fAns  = s->var[iID].fDat;
	}

	SPACE_DEL(strBuf, &iLen);				// ���̋󔒂����
	*fOut = fAns * fFlag;

	if(sys.bError){ ScriptError("�ϐ��A���l�Ɍ�肪����܂��B"); return iLen;}
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< �֑������`�F�b�N >>>
|	�߂�l	true : �֑�����
*------------------------------------------------------------------------------*/
static	bool	IsKinsokuCheck(uchar *str)
{

	if( ! ISKANJI(str[0])) return false;		// ���p�H
	
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
| <<< �R�}���h : �t�F�[�h�A�E�g >>>			@fade_out ���C���[�ԍ�, ����
*------------------------------------------------------------------------------*/
static	void	ComFadeOut(void)
{
	//--- ���C���[�ԍ� --------------------------------------------
	float	fID;
	s->iBufCount += ParamGet(&fID, &s->strBuf[s->iBufCount]);
	if(fID >= 3){ sys.bError = true; return;}
	int		iID = (int)fID;

	//--- ���� ----------------------------------------------------
	float	fTime;
	s->iBufCount++;								// �J���}��ǂݔ�΂�
	s->iBufCount += ParamGet(&fTime, &s->strBuf[s->iBufCount]);

	//--- �t�F�[�h�C�� --------------------------------------------
	if(s->fBGAlp[iID] > 0)
	{
		s->fBGAlpSpeed[iID] = -(s->fBGAlp[iID] / fTime);
	}
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �t�F�[�h�C�� >>>			@fade_in ���C���[�ԍ�, ����
*------------------------------------------------------------------------------*/
static	void	ComFadeIn(void)
{
	//--- ���C���[�ԍ� --------------------------------------------
	float	fID;
	s->iBufCount += ParamGet(&fID, &s->strBuf[s->iBufCount]);
	if(fID >= 3){ sys.bError = true; return;}
	int		iID = (int)fID;

	//--- ���� ----------------------------------------------------
	float	fTime;
	s->iBufCount++;								// �J���}��ǂݔ�΂�
	s->iBufCount += ParamGet(&fTime, &s->strBuf[s->iBufCount]);

	//--- �t�F�[�h�C�� --------------------------------------------
	if(s->fBGAlp[iID] < 255)
	{
		s->fBGAlpSpeed[iID] = (255 - s->fBGAlp[iID]) / fTime;
	}
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �e�N�X�`���[�ǂݍ��� >>>	@tex_load �e�N�X�`���[�ԍ�,�t�@�C����
*------------------------------------------------------------------------------*/
static	void	ComTexLoad(void)
{
	//--- �ǂݍ��ݔԍ� --------------------------------------------
	float	fID;
	s->iBufCount += ParamGet(&fID, &s->strBuf[s->iBufCount]);
	if(fID >= 3){ sys.bError = true; return;}	

	//--- �t�@�C�����𓾂� ----------------------------------------
	s->iBufCount += 1;							// �J���}��ǂݔ�΂�
	char	strFile[256];
	s->iBufCount += GET_STRING(strFile, &s->strBuf[s->iBufCount]);

	//--- �����œǂݍ��� ------------------------------------------
	FOLDER_PUSH();
	FOLDER_SET("�X�N���v�g");
	int		b = d3_tex_load(TEX_SCRIPT_BG0 + (int)fID, strFile);
	FOLDER_POP();
	if(b == -1) ScriptError("�e�N�X�`���[���ǂݍ��߂܂���");
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���j���[ >>>				@menu x, y, ���ڇ@:���ڇA;
*------------------------------------------------------------------------------*/
static	void	ComMenu(void)
{
#if 0
	//--- XY ���W�����o�� -----------------------------------------
	s->iBufCount += ParamGet(&vecMenu.x, &s->strBuf[s->iBufCount]); s->iBufCount++;	// �w���W
	s->iBufCount += ParamGet(&vecMenu.y, &s->strBuf[s->iBufCount]); s->iBufCount++;	// �x���W

	SPACE_DEL(s->strBuf, &s->iBufCount);

	s->bMenu = true;							// ���̃t���O�� true �Ȃ烁�j���[���[�h
	//--- ���j���[�̍��ڂ����o�� --------------------------------
	char	strMenu[128];
	int		iCount = 0;
	for(int i = 0;;)								// �Z�~�R���������s�ɕς���
	{
		if(s->strBuf[s->iBufCount] == NULL) break;	// NULL �łȂ烁�j���[�I���
		if(ISKANJI(s->strBuf[s->iBufCount]))		// �S�p�Ȃ�Ε����o�^
		{
			strMenu[i    ] = s->strBuf[s->iBufCount + 0];
			strMenu[i + 1] = s->strBuf[s->iBufCount + 1];
			i              += 2;
			s->iBufCount   += 2;
		}
		else if(s->strBuf[s->iBufCount] <= ' ')		// �X�y�[�X�ȉ��Ȃ�΍폜
		{
			s->iBufCount++;
		}
		else if(s->strBuf[s->iBufCount] == ':')		// �R�����Ŏ��̍��ڂ�
		{
			strMenu[i] = NULL;
			s->iBufCount++;
			MenuMake(iCount, strMenu);
			iCount++;
			i = 0;
		}
		else if(s->strBuf[s->iBufCount] == ';')		// �Z�~�R�����Ń��j���[�I��
		{
			strMenu[i] = NULL;
			s->iBufCount++;
			MenuMake(iCount, strMenu);
			iCount++;
			break;
		}
		else										// ���p�͓o�^
		{
			strMenu[i] = s->strBuf[s->iBufCount];
			i++;
			s->iBufCount++;
		}
	}
#endif
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : if �� >>>					@if �����W�����v
*------------------------------------------------------------------------------*/
static	void	Comif(void)
{
	bool	bJump = false;
	float	fDat, fDat2;
												// ���ӂ̒l�𓾂�
	s->iBufCount += ParamGet(&fDat, &s->strBuf[s->iBufCount]);
	char	*a = &s->strBuf[s->iBufCount];
	s->iBufCount += 2;							// �E�ӂ̒l�𓾂�
	s->iBufCount += ParamGet(&fDat2, &s->strBuf[s->iBufCount]);

	//--- �����`�F�b�N --------------------------------------------
	if(     strncmp(a, "==", 2) == 0)			// == �`�F�b�N
	{
		if(fDat == fDat2) bJump = true;
	}
	else if(strncmp(a, "!=", 2) == 0)			// != �`�F�b�N
	{
		if(fDat != fDat2) bJump = true;
	}
	else if(strncmp(a, "<=", 2) == 0)			// <= �`�F�b�N
	{
		if(fDat <= fDat2) bJump = true;
	}
	else if(strncmp(a, ">=", 2) == 0)			// >= �`�F�b�N
	{
		if(fDat >= fDat2) bJump = true;
	}
	else if(strncmp(a, "<", 1) == 0)			// < �`�F�b�N
	{
		if(fDat < fDat2) bJump = true;
	}
	else if(strncmp(a, ">", 1) == 0)			// > �`�F�b�N
	{
		if(fDat > fDat2) bJump = true;
	}
	//--- �����𖞂����Ă���΃W�����v -------------------
	if(bJump)								// ���� @goto �Ɠ����ł�
	{
		SPACE_DEL(&s->strBuf[s->iBufCount], &s->iBufCount);
		if(strncmp(&s->strBuf[s->iBufCount], "@gosub", 6) == 0)
		{
			s->iBufCount += 6;
			ComGosub();						// ��قǃR�����g���͂����܂��B
		}
		else if(strncmp(&s->strBuf[s->iBufCount], "@goto", 5) == 0)
		{
			s->iBufCount += 5;
			ComGoto();
		}
		else
		{
			ComGoto();
		}
	}
	//--- �����𖞂����Ă��Ȃ��Ƃ��̏��� -------------------
	else									// ���s�R�[�h�܂Ői�߂�
	{
		ComComment();						// ��قǃR�����g���͂�����
	}
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �ϐ��𕶎��� >>>			%d:�ϐ���, %f:�ϐ���
*------------------------------------------------------------------------------*/
static	void	ComVarToStr(void)
{
	int		a = s->iBufCount - 1;
	char	b = s->strBuf[s->iBufCount];		// a �ɂ͕�����'d'�܂���'f'������

	s->iBufCount++;								// d ��i�߂�
	SPACE_DEL(&s->strBuf[s->iBufCount], &s->iBufCount);
	s->iBufCount++;								// : ��i�߂�
	s->iBufCount++;								// $ ��i�߂�
	int		iID;								// ��������ϐ���ID�𓾂�
	s->iBufCount += VarNumGet(&iID, &s->strBuf[s->iBufCount]);

	s->iTextCount = 0;
	if(b == 'd')								// %d �H
	{
		sprintf(s->strText, "%d", (int)s->var[iID].fDat);
	}
	else if(b == 'f')							// %f �H
	{
		sprintf(s->strText, "%.4f", s->var[iID].fDat);
	}
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �ϐ� >>>					$ �ϐ�
*------------------------------------------------------------------------------*/
static	void	ComVar(void)
{
	int		iNum;								// ���ӁA�ϐ��̓o�^�ԍ��𓾂�
	s->iBufCount += VarNumGet(&iNum, &s->strBuf[s->iBufCount]);

	int		iType;								// ���Z�^�C�v�𓾂�
	char	*a = &s->strBuf[s->iBufCount + 0];

	if(		strncmp(a, "+=", 2) == 0) s->iBufCount += 2, iType = 0;
	else if(strncmp(a, "-=", 2) == 0) s->iBufCount += 2, iType = 1;
	else if(strncmp(a, "*=", 2) == 0) s->iBufCount += 2, iType = 2;
	else if(strncmp(a, "/=", 2) == 0) s->iBufCount += 2, iType = 3;
	else if(strncmp(a, "=",  1) == 0) s->iBufCount++,    iType = 4;

	float	fDat;								// �E�ӂ𓾂�
	s->iBufCount += ParamGet(&fDat, &s->strBuf[s->iBufCount]);

	switch(iType)								// ���ۂɉ��Z
	{
	case 0: s->var[iNum].fDat += fDat; break;	// +=
	case 1: s->var[iNum].fDat -= fDat; break;	// -=
	case 2: s->var[iNum].fDat *= fDat; break;	// *=
	case 3: s->var[iNum].fDat /= fDat; break;	// /=
	case 4: s->var[iNum].fDat  = fDat; break;	// =
	}
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �T�u���[�`���R�[�� >>>		@gosub
*------------------------------------------------------------------------------*/
static	void	ComGosub(void)
{
	for(int i = GOSUB_MAX - 1; i != 0; i--) memcpy(&s->gosub[i], &s->gosub[i - 1], sizeof(GOSUB_WORK));
	int	a = ComGoto();
	s->gosub[1].iCount = a;
	s->sGosubCount++;
	s->var[0].fDat = -100;						// ���j���[�I����̃W�����v��Ŏx�Ⴊ���e�ɓK���Ȓl���Z�b�g
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���^�[�� >>>				@return
*------------------------------------------------------------------------------*/
static	void	ComReturn(void)
{
	if(s->sGosubCount == 0)					// @gosub �ŌĂяo���Ă����Ȃ��̂� @return ���o�Ă����ꍇ
	{
		s->bEnd = true;
		return;
	}
	for(int i = 0; i < GOSUB_MAX - 1; i++) memcpy(&s->gosub[i], &s->gosub[i + 1], sizeof(GOSUB_WORK));
	ScriptLoad(s->gosub[0].strName, false);
	s->sGosubCount--;
	s->iBufCount = s->gosub[0].iCount;
	s->var[0].fDat = -100;						// ���j���[�I����̃W�����v��Ŏx�Ⴊ���e�ɓK���Ȓl���Z�b�g
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �W�����v >>>				@goto �������W�����v
|	�߂�l	�s���܂ł̕�����
*------------------------------------------------------------------------------*/
static	int		ComGoto(void)
{
	int	iLen = 0;

	SPACE_DEL(s->strBuf, &s->iBufCount);		// �擪�̋󔒂����

	//--- �t�@�C�����𓾂� ----------------------------------------
	bool	bFile = false;
	char	strFile[256];
	if(s->strBuf[s->iBufCount] == '"')
	{
		bFile		  = true;
		s->iBufCount += GET_STRING(strFile, &s->strBuf[s->iBufCount]);
		SPACE_DEL(s->strBuf, &s->iBufCount);
		if(s->strBuf[s->iBufCount] == ',') s->iBufCount++;
		SPACE_DEL(s->strBuf, &s->iBufCount);
	}
	//--- ���x�� --------------------------------------------------
	bool	bLabel = false;
	char	strLabel[512];						// �����Ƀ��x�����i�[�����i���p512�����j
	char	*p;									// @goto �̌�ɑ������x���̈ʒu
	SPACE_DEL(s->strBuf, &s->iBufCount);
	if(s->strBuf[s->iBufCount] == '#')
	{
		bLabel      = true;
		strLabel[0] = '#';						// ���x���̐擪�� # ���Z�b�g
		p	        = &s->strBuf[s->iBufCount];	// p ��@goto�ɑ������x���̏ꏊ���i�[
		s->iBufCount++;
												// ���x���𓾂�
		s->iBufCount += GET_LABEL(&strLabel[1], &s->strBuf[s->iBufCount]);
		strcat(strLabel, ":");
	}
	iLen = s->iBufCount;

	//--- �t�@�C�����ړ� ------------------------------------------
	if(bFile)
	{
		if(!ScriptLoad(strFile, false))
		{
			sys.bError = true;
			sprintf(s->strError, "@goto �Ńt�@�C���w%s�x��������܂���",  strFile);
		}
	}
	//--- ���x����T�� --------------------------------------------
	if(!sys.bError && bLabel)
	{
		int iLabelLen = strlen(strLabel);
		for(int	i =	0;;)					// ���͂̐擪��胉�x������T��
		{										// �R�����g�͍s���܂ŃX�L�b�v
			if(s->strBuf[i] == NULL)			// �G���[����
			{
				sprintf(s->strError, "%s : @goto �Łw%s�x�Ƃ������x����������܂���B\r\n", s->gosub[0].strName, strLabel); break;
			}
			else if(strncmp(&s->strBuf[i], "//", 2) == 0)
			{
				i += 2;
				for(;;)
				{								// ������i�߂�
					if(ISKANJI(s->strBuf[i]))	i += 2; else i++;
												// ���s���`�F�b�N�A���s�Ȃ�΃��[�v�~�߂�
					if(s->strBuf[i] == '\r' && s->strBuf[i + 1] == '\n'){ i += 2; break;}
												// NULL	�ł����[�v���~�߂�B
					if(s->strBuf[i] == NULL) break;
				}
			}
			else if(strncmp(&s->strBuf[i], strLabel, iLabelLen) == 0)
			{
				s->iBufCount = i + iLabelLen; break;
			}
			else if(ISKANJI(s->strBuf[i])) i += 2; else i++;
		}
	}
	return iLen;
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���x�� >>>					# ������s�܂ł����x���ɂȂ�B���x���͉������Ȃ�
*------------------------------------------------------------------------------*/
static	void	ComLabel(void)
{
	char	strName[512];
											// ���x���𓾂�
	s->iBufCount += GET_LABEL(strName, &s->strBuf[s->iBufCount]);

	//--- ���x���ɃZ�~�R�������t���Ă��Ȃ��Ƃ��̏��� --------------
	if(s->strBuf[s->iBufCount] != ':')
	{
		sys.bError = true;
		sprintf(s->strError, "���x���w%s�x�̖����ɃZ�~�R����������܂���B�܂��͕s���ȕ������g���Ă��܂��B", strName);
	}
	s->iBufCount++;
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �L�[�E�G�C�g >>>			@key
*------------------------------------------------------------------------------*/
static	void	ComKeyWait(void)
{
	if(pad.bLeftPush){ s->iBufCount -= 4;}	// 
	else			 { s->bKeyWait = true;}	// ���̃t���O���I���ɂ���� ScriptMain ���ŃL�[�҂��ɂȂ�
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �R�����g >>>				//		�� ���s�܂ŃR�����g
*------------------------------------------------------------------------------*/
static	void	ComComment(void)
{
	for(;;)
	{										// ������i�߂�
		if(ISKANJI(s->strBuf[s->iBufCount])) s->iBufCount += 2;
		else								 s->iBufCount += 1;
											// ���s���`�F�b�N�A���s�Ȃ�΃��[�v�~�߂�
		if(s->strBuf[s->iBufCount] == '\r' && s->strBuf[s->iBufCount + 1] == '\n'){ s->iBufCount += 2; break;}
											// NULL �ł����[�v���~�߂�B
		if(s->strBuf[s->iBufCount] == NULL) break;
	}
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���s >>>					@n �ŉ��s�ł��B
*------------------------------------------------------------------------------*/
static	void	ComKaigyo(void)
{
	s->sTextX      = TEXT_X0;
	s->sTextAddX   = 0;
	s->sTextY     += FONT_H + 8;
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �N���A�[ >>>				@clear
*------------------------------------------------------------------------------*/
static	void	ComClear(void)
{
	d3_offscreen_begin(TEX_SCRIPT);
	d3_clear();
	d3_offscreen_end(TEX_SCRIPT);
	s->sTextX    = TEXT_X0;
	s->sTextY    = TEXT_Y0;
	s->sTextAddX = 0;
	s->sTextAddY = 0;
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h : �E�G�C�g >>>				@wait �Ŏw��b�҂B60�ň�b
*------------------------------------------------------------------------------*/
static	void	ComWait(void)
{
	float	a;								// ���Ԃ𓾂�
	s->iBufCount += ParamGet(&a, &s->strBuf[s->iBufCount]);
	s->sDelay	  = (int)a;
}


/*------------------------------------------------------------------------------*
| <<< �R�}���h�`�F�b�N >>>
|	�߂�l	true : ��x�ł��R�}���h�����s����
*------------------------------------------------------------------------------*/
static	bool	CommandCheck(void)
{
	
	bool	bRet = false;
	int		i;

	for(i = 0;;i++)
	{
		if(sys.bError) return bRet;				// �G���[�Ȃ�Β��~����

		int	iLen = strlen(CommandTbl[i]);		// ��strlen = ������̒����𒲂ׂ�(�R�}���h�̕����̒���)
		if(iLen == 0) break;					// ������̒������O�Ȃ�I�[�Ƃ݂Ȃ�(�ǂ̃R�}���h�ł��Ȃ��̂ŏ������~)
												// strncmp �ŃX�N���v�g�ƃR�}���h�����v���邩���ׂ�
		if(strncmp(&s->strBuf[s->iBufCount], CommandTbl[i], iLen) == 0)
		{
			s->iBufCount += iLen;				// �R�}���h�̕�������i�߂�
			int		a	  = s->iBufCount;
			bRet          = true;
												// �G���[���o���Ƃ��̏ꍇ�ɔ������b�Z�[�W���쐬(�����ŃG���[���o�Ă���킯�ł͂Ȃ�)
			sprintf(s->strError, "%s �ɃG���[������܂��B", CommandTbl[i]);
			bool	bContinue = true;
			switch(i)
			{
			case 0:			// 00 : ���p�X�y�[�X
			case 1:			// 01 : �^�u
			case 2:			// 02 : �{���̉��s�R�[�h
				break;
			case 3:			// 03 : �R�����g			//
				ComComment();   break;
			case 4:			// 04 : ���s
				ComKaigyo();	break;
			case 5:			// 05 : �E�G�C�g			@wait
				ComWait();		bContinue = false; break;
			case 6:			// 06 : �N���A�[			@clear
				ComClear();		break;
			case 7:			// 07 : �L�[�҂�			@key
				ComKeyWait();	bContinue = false; break;
			case 8:			// 08 : ���x��				#		���Ȃɂ��������Ȃ��ŃX�L�b�v����B
				ComLabel();		break;
			case 9:			// 09 : �������W�����v		@goto	���w�胉�x���܂ŃW�����v
				ComGoto();		break;
			case 10:		// 10 : �T�u���[�`���R�[��	@gosub	���w�胉�x�����Ăяo��
				ComGosub();		break;
			case 11:		// 11 : ���^�[��			@return	��@gosub�ŌĂяo���ꂽ�̂�߂�
				ComReturn();	break;
			case 12:		// 12 : �ϐ�				$
				ComVar();		break;
			case 13:		// 13 : �ϐ��𕶎���		%d, %f
				ComVarToStr();	bContinue = false; break;
			case 14:		// 14 : ��������			@if
				Comif();		break;
			case 15:		// 15 : ���j���[			@menu
				ComMenu();		bContinue = false; break;
			case 16:		// 16 : �e�N�X�`���[���[�h	@tex_load
				ComTexLoad();	break;
			case 17:		// 17 : �t�F�[�h�C��		@fade_in
				ComFadeIn();	break;
			case 18:		// 18 : �t�F�[�h�A�E�g		@fade_out
				ComFadeOut();	break;
			}
			if(sys.bError)
			{
				s->iBufCount = a;
				ScriptError(s->strError);
				return true;
			}
			if(!bContinue) return true;
			i = -1;							// ������x�ACommandTbl�̍ŏ����璲�ׂ�
		}
	}

	if(s->strBuf[s->iBufCount] == '@' && i != 0)
	{
		sys.bError = true;
		ScriptError("�s���ȃR�}���h�ł��B");
		return true;
	}
	return bRet;
}



/*----------------------------------------------------------------------*
| <<< �X�N���v�g����� >>>
|	����	iTex = �g�p����e�N�X�`���[�ԍ�
|	�߂�l	true : �X�N���v�g���I������
*----------------------------------------------------------------------*/
bool	ScriptMain(void)
{
	//--- �G���[���o�Ă���Ȃ�Έȍ~�A�������Ȃ� --------------------
	if(sys.bError){ return false;}

	//--- �E�G�C�g���� ----------------------------------------------
	if(s->sDelay > 0){ s->sDelay--; return false;}

	//--- �L�[�E�G�C�g���� ------------------------------------------
	if(s->bKeyWait)									// @key�̏���
	{
		char	a[] = {(char)0xf0, NULL};
		if((sys.iTimeCount % 60) > 30) a[0]++;
		//Printf((float)s->sTextX + s->sTextAddX + 12, (float)s->sTextY + 16, 20, a);
		if(pad.bLeftRelease){ s->bKeyWait = false;}
		else				{ return false;}
	}
	//--- �R�}���h�`�F�b�N ------------------------------------------
	while(s->strBuf[s->iBufCount] != NULL || s->strText[s->iTextCount] != NULL)
	{
		//--- �R�}���h�`�F�b�N --------------------------------------
		bool	bRet = CommandCheck();
		if(bRet || sys.bError){ break;}							// �R�}���h�����s���Ă����當���͕`�悵�Ȃ�

		//--- ���p�E�S�p�����`��Z�b�g ------------------------------
		int		iLen;
		char	str[3];
		if(s->strText[s->iTextCount] == NULL)
		{
			if(ISKANJI(s->strBuf[s->iBufCount])){ iLen = 2;}	// �S�p
			else								{ iLen = 1;}	// ���p
			strncpy(str, &s->strBuf[s->iBufCount], iLen);
			s->iBufCount += iLen;
		}
		else
		{
			if(ISKANJI(s->strText[s->iTextCount])){ iLen = 2;}	// �S�p
			else								  { iLen = 1;}	// ���p
			strncpy(str, &s->strText[s->iTextCount], iLen);
			s->iTextCount += iLen;
		}

		d3_offscreen_begin(TEX_SCRIPT);
		d3_clear_zbuffer();

		s->sTextX += s->sTextAddX;					// ������i�߂�
		s->sTextY += s->sTextAddY;					// ������i�߂�

		//--- ���s --------------------------------------------------
		if(s->sTextX > TEXT_X1)						// �E�[�ɍ����|����������s����
		{
			if(!IsKinsokuCheck((uchar*)str))
			{
				s->sTextX      = TEXT_X0;
				s->sTextAddX   = 0;
				s->sTextY     += FONT_H + 8;
			}
		}
		//--- �����`�� ----------------------------------------------
		RECT	r = {s->sTextX, s->sTextY, 0, 0};
		d3.font[FONT]->DrawText(NULL, str, iLen, &r, DT_NOCLIP, D3_COL(255, 255, 255, 255));
		d3_offscreen_end(TEX_SCRIPT);

		//--- ������i�߂� ------------------------------------------
		if(iLen == 1){ s->sTextAddX = (int)(FONT_W * 1.7f);}// �`�悵�������̑傫�����Z�b�g
		else		 { s->sTextAddX = FONT_W * 2;		   }
		if(!pad.bLeftPush){ break;}
	}
	//--- �X�N���v�g�I���`�F�b�N ------------------------------------
	if(s->strBuf[s->iBufCount] != NULL){ return true; }		// �X�N���v�g�͏I������
	else							   { return false;}
}



/*----------------------------------------------------------------------*
| <<< �X�N���v�g��`�� >>>
*----------------------------------------------------------------------*/
void	ScriptDraw(void)
{
	for(int i = 0; i < BG_MAX; i++)
	{
		s->fBGAlp[i] += s->fBGAlpSpeed[i];
		if(s->fBGAlpSpeed[i] < 0 && s->fBGAlp[i] <=   0) s->fBGAlp[i] =   0, s->fBGAlpSpeed[i] = 0;
		if(s->fBGAlpSpeed[i] > 0 && s->fBGAlp[i] >= 255) s->fBGAlp[i] = 255, s->fBGAlpSpeed[i] = 0;

		if(s->fBGAlp[i] == 0) continue;
		d3_tex_set(TEX_SCRIPT_BG0 + i);
		d3_2d_box_fill_tex_draw(
				&VEC2(0,										0										 ),
				&VEC2(d3.texInfo[TEX_SCRIPT_BG0 + i].fOriWidth, d3.texInfo[TEX_SCRIPT_BG0 + i].fOriHeight),
				0,
				0,
				d3.texInfo[TEX_SCRIPT_BG0 + i].fU,
				d3.texInfo[TEX_SCRIPT_BG0 + i].fV,
				D3_COL((int)s->fBGAlp[i], 255, 255, 255));
	}

	d3_stat_filter_set(D3DTEXF_LINEAR);			// ���̐ݒ�ŁA�������{�P�܂�
	d3_tex_set(TEX_SCRIPT);
	d3_stat_blend_set(D3_BLEND_NORMAL);			// �������ݒ�
	d3.dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	d3.dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	d3_2d_box_fill_tex_draw(
			  &VEC2(0,		 0		),
			  &VEC2(__SCR_W, __SCR_H),
			  0,
			  0,
			  __SCR_W / d3.texInfo[TEX_SCRIPT].fWidth,
			  __SCR_H / d3.texInfo[TEX_SCRIPT].fHeight,
			  D3_COL(255, 255, 255, 255));
}



/*------------------------------------------------------------------------------*
| <<< �X�N���v�g�ǂݍ��� >>>
|	����	*strFile = �ǂݍ��݃t�@�C��
|			bReset   = true : ��ʎ�������Z�b�g����
|	�߂�l	false : �ǂݍ��ݎ��s
*------------------------------------------------------------------------------*/
bool	ScriptLoad(char *strFile, bool bReset)
{
	HANDLE	h;										// �ǂݍ��݃n���h��
	DWORD	rs;										// �_�~�[

	s = &script;									// �X�N���v�g�̃|�C���^��ݒ�
													// �ȍ~�Ascript.�`	��	s->	�ƂȂ�
	//--- �t�@�C�����J�� --------------------------------------------
	strcpy(s->strName, strFile);
	FOLDER_PUSH();
	FOLDER_SET("�X�N���v�g");
	h = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	FOLDER_POP();
	if(h == INVALID_HANDLE_VALUE) return false;

	//--- �ǂݍ��� --------------------------------------------------
	int	iSize  = GetFileSize(h, NULL) + 1;			// �t�@�C���̃T�C�Y�𓾂�
	memset(s->strBuf, 0, sizeof(s->strBuf));
	ReadFile(h, s->strBuf, iSize, &rs, NULL);
	CloseHandle(h);									// �t�@�C�������

	//--- @gosub �p�Ƀt�@�C�������΃p�X�ŕۑ� ---------------------
	sprintf(s->gosub[0].strName, "%s", strFile);

	if(bReset)
	{
		//--- �t�H���g�ƃ����_�����O�ʂ̍쐬 ------------------------
		d3_font_create(FONT, FONT_W, FONT_H, FONT_NAME, false);
														// �����_�����O�ʂ��쐬
		d3_offscreen_create(TEX_SCRIPT, d3.sRenderW, d3.sRenderH);
		d3.texInfo[TEX_SCRIPT].bRepair = true;			// ����ŉ摜�T�C�Y���ς���Ă����A����

		//--- �����̕\���ʒu ----------------------------------------
		s->sTextX    = TEXT_X0;
		s->sTextY    = TEXT_Y0;
		s->sTextAddX = 0;

		//--- �ϐ����� ----------------------------------------------
		strcpy(s->var[0].strName, "���j���[�̓���");
	}

	s->iBufCount = 0;
	return true;
}



