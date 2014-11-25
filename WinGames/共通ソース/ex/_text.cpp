//
//	�X�N���v�g����
//

//#include	"main.h"
//#include	"_text.h"
//#include	"_menu.h"


//--- �}�N�� ------------------------------------------------------
#define	TEXT_FONT		4					// �t�H���g�ԍ��i2 �` 7 �͈̔͂Łj
#define	RENDER_SIZE		512					// �����̃����_�����O�T�C�Y
#define	TEXT_X1			0					// �����̕`��J�n�ʒu
#define	TEXT_X2			(RENDER_SIZE - 64)	// �����̕`��I���ʒu
#define	FONT_NAME		"HG�ۺ޼��M-PRO"	// �����ꍇ�͓���̃t�H���g�ɂȂ�
#define	BUF_MAX			1					// ��x�ɕ`�悷�镶����

#define	VAR_MAX			256					// �ϐ��̍ő吔
#define	GOSUB_MAX		8					// �ϐ��̍ő吔
#define	LAYER_MAX		4					// ���C���[�̑����i�O����ԏ�j
#define	LAYER_TEXT_NUM	1					// LAYER_MAX ���̃e�L�X�g�ʂ̈ʒu

//--- �\���� ------------------------------------------------------
struct	VAR_WORK						// �ϐ��̍\����
{
	char	strName[64];					// �ϐ������p�� 64 �����܂ŁB
	float	fDat;							// �l
};

struct	GOSUB_WORK						// �ϐ��̍\����
{
	char	strName[MAX_PATH + 1];			// �t���p�X�Ńt�@�C����������B
	int		iCount;							// �l
};

struct	LAYER_WORK						// ���C���[�\����
{
	int		iType;							// ���C���[�^�C�v	0:�摜�A1�`���낢��
	int		iTex;							// �e�N�X�`���[�ԍ�
	float	fAlp;							// �A���t�@�l
	float	fU0, fV0, fU1, fV1;				// �摜�؂�o���� UV �l
	VEC3	vecP0, vecP1;					// �摜�̍���ƉE��
};

//--- �ϐ��֌W ----------------------------------------------------
static	int			iX, iY;					// �`��w�ʒu�A�x�ʒu
static	int			iNextX;					// ���ɕ�����`�悷��Ƃ��� iX �ɑ������l
static	char		*Text;
static	int			iCount;					// �����̈ʒu
static	int			iDelay = 0;				// @wait �p
											// �t�H���g�̃T�C�Y
static	int			iWidth = 16, iHeight = 40;
static	MENU_WORK	Menu;
static	bool		bMenu;
static	bool		bKeyWait;				// true : �L�[�E�G�C�g
static	bool		bKeyMark;				// true : �L�[�E�G�C�g�̃}�[�N�\��
static	DWORD		dwColor = 0xffffffff;				// �����̐F
static	char		strPath[MAX_PATH + 1];	// �x�[�X�t�H���_�[
static	char		strErrorCommand[ 512];	// �G���[�̃R�}���h���i�[�p
static	char		strBuf[BUF_MAX  + 32];	// �`�悷�镶��
static	int			iBufCount;				// �`�悷�镶����

static	int			iGosubCount;
static	GOSUB_WORK	GosubWork[GOSUB_MAX];
static	VAR_WORK	Var[        VAR_MAX];
static	LAYER_WORK	LayerWork[LAYER_MAX];


/*---- �R�}���h�e�[�u�� -----------------------------*/
static	char	CommandTbl[][16] =
{
	" ",					// 00 : ���p�X�y�[�X
	"\t",					// 01 : �^�u
	"\r\n",					// 02 : �{���̉��s�R�[�h
	"@n",					// 03 : �V�K�̉��s
	"@wait",				// 04 : �E�G�C�g
	"#",					// 05 : ���x��
	"@goto",				// 06 : �������W�����v
	"$",					// 07 : �ϐ�
	"@if",					// 08 : ��������
	"@menu",				// 09 : ���j���[
	"%",					// 10 : �ϐ��̕\��
	"@clear",				// 11 : ������N���A�[
	"@key",					// 12 : �L�[�҂�
	"@Key",					// 13 : �L�[�҂��i�}�[�N�\�����Ȃ��j
	"@gosub",				// 14 : �T�u���[�`���R�[��
	"@return",				// 15 : ���^�[��
	"//",					// 16 : �R�����g
	"@tex_load",			// 17 : �e�N�X�`���[���[�h
	"@tex_set",				// 18 : ���C���[�Ƀe�N�X�`���[�ԍ��ݒ�
	"@fade_in",				// 19 : ���C���[�t�F�[�h�C��
	"@fade_out",			// 20 : ���C���[�t�F�[�h�A�E�g
	"@move",				// 21 : ���C���[�ړ�
	"@sound",				// 22 : ���ʉ�
	"@bgm",					// 23 : �a�f�l
	"",						// �R�}���h�`�F�b�N�̏I�[�ɂȂ�
};							// ���I�[�͕K������邱��


static	void	script_error(void);
static	int		VarNumGet(int *iOut, char *strBuf);
static	void	ComGosub(void);
static	void	ComComment(void);



/*------------------------------------------------------------------------------*
| <<< �e�L�X�g��͂̃G���[���� >>>
*------------------------------------------------------------------------------*/
static	void	script_error(void)
{
	int	iKai = 1, iPos = 0;
	while(iCount > iPos)
	{										// ������i�߂�
		if(ISKANJI(Text[iPos])) iPos += 2;
		else					iPos += 1;
											// ���s�񐔂𐔂���
		if(Text[iPos] == '\r' && Text[iPos + 1] == '\n') iKai++, iPos += 2;
		if(Text[iPos] == NULL) break;		// NULL �ł����[�v���~�߂�B
	}

	sys_error("%d �s�� : %s", iKai, strErrorCommand);
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h�̃p�����[�^�[�𓾂� >>>
|	����	strBuf = ������̈ʒu
|	�o��	fOut   = �����񂩂�E���o�������l
|	�߂�l	�����񂩂�E���o��������
*------------------------------------------------------------------------------*/
static	int		ParamGet(float *fOut, char *strBuf)
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
		fAns   = Var[iID].fDat;
	}

	SPACE_DEL(strBuf, &iLen);				// ���̋󔒂����
	*fOut = fAns * fFlag;

	if(sys.bError){ script_error(); return iLen;}

	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< �X�N���v�g������ >>>
|	����	iTex = �g�p����e�N�X�`���[�ԍ�
*------------------------------------------------------------------------------*/
void	script_init(int iTex)
{
											// �t�H���g�̍쐬
	d3_font_create(TEXT_FONT, 16, 40, "HG�ۺ޼��M-PRO", false);

	d3_offscreen_create(iTex, RENDER_SIZE, RENDER_SIZE);
	d3.texInfo[iTex].bRepair = true;		// ���̃X�C�b�`�� true �ɂ���ƃt���X�N���[���؂�ւ����ɉ摜���ێ������
	iX     = iNextX = iY = iCount = 0;
	iDelay = 0;
	sprintf(Var[0].strName, "���j���[�̓���");
	sprintf(Var[1].strName, "�}�E�X�̂w���W");
	sprintf(Var[2].strName, "�}�E�X�̂x���W");

	//--- �����̐F ------------------------------------------------
	dwColor	= D3_COL(255, 255, 255, 255);

	//--- �摜���[�N������ ----------------------------------------
	int	i;
	for(i = 0; i < LAYER_MAX; i++)
	{
		LayerWork[i].fAlp  = 0;
		LayerWork[i].fU0   = LayerWork[i].fV0 = 0;
		LayerWork[i].fU1   = LayerWork[i].fV1 = 1;
		LayerWork[i].vecP0 = VEC3(  0,   0, 0);
		LayerWork[i].vecP1 = VEC3(640, 480, 0);
	}
	//--- �e�L�X�g�ʏ����� ----------------------------------------
	i				  = LAYER_TEXT_NUM;
	LayerWork[i].fAlp = 255;
	LayerWork[i].iTex = iTex;

	//--- �t�H���_�[�Z�b�g ----------------------------------------
	sprintf(strPath, "%s�X�N���v�g\\", sys.strExeFolder);
}



/*------------------------------------------------------------------------------*
| <<< �X�N���v�g�J�� >>>
*------------------------------------------------------------------------------*/
void	script_free(void)
{
	if(Text != NULL)						// �������[���J��
	{
		free(Text);
		Text = NULL;
	}
}



/*------------------------------------------------------------------------------*
| <<< �X�N���v�g�I�� >>>
|	����	iTex = �g�p���Ă����e�N�X�`���[�ԍ�
*------------------------------------------------------------------------------*/
void	script_end(int iTex)
{
	d3_offscreen_free(iTex);				// �����_�����O�ʂ��J��
}



/*------------------------------------------------------------------------------*
| <<< �֑������`�F�b�N >>>
|	�߂�l	true : �֑�����
*------------------------------------------------------------------------------*/
static	bool	IsKinsokuCheck(uchar *str)
{

	if(ISKANJI(str)) return false;		// ���p�H
	
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
| <<< �ϐ�������ϐ��ԍ������� >>>			���V�K�ɏo�Ă������͓̂o�^����B
|	����	strBuf = ������̈ʒu
|	�o��	fOut   = �����񂩂�E���o�������l
|	�߂�l	�ϐ����̓o�^�ԍ�
*------------------------------------------------------------------------------*/
static	int		VarNumGet(int *iOut, char *strBuf)
{
	int		iLen;
	char	strName[256];					// �ϐ����i�[�p

	iLen		 = GET_LABEL(strName, strBuf);
	int	iNameLen = strlen(   strName);
	int	i;
	for(i = 0;;i++)							// �ϐ���(name)���o�^����Ă��邩���ׂ�
	{										// �o�^����Ă��Ȃ���Γo�^����B
		if(strlen(Var[i].strName) == 0)		// �����񂪓o�^����Ă��Ȃ����
		{									// �o�^���ă`�F�b�N�I��
			strcpy(Var[i].strName, strName);// �ϐ�����o�^����
			break;							// ���[�v������
		}
		if(strncmp(Var[i].strName, strName, iNameLen) == 0)
		{
			break;							// �����Ȃ�΂����Ń��[�v������
		}
	}
	*iOut = i;								// �o�^�ԍ��Z�b�g

	SPACE_DEL(strBuf, &iLen);				// ���̋󔒂����
	return iLen;							// �Ō�ɕ�������Ԃ�
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���s >>>					@n �ŉ��s�ł��B
*------------------------------------------------------------------------------*/
static	void	ComKaigyo(void)
{
	strBuf[iBufCount] = '\n';				// �o�b�t�@�ɉ��s�}�[�N�������B
	iBufCount++;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �E�G�C�g >>>				@wait �Ŏw��b�҂B60�ň�b
*------------------------------------------------------------------------------*/
static	void	ComWait(void)
{
	//--- ���� ----------------------------------------------------
	float	a;
	iCount += ParamGet(&a, &Text[iCount]);
	iDelay  = (int)a;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���x�� >>>					# ������s�܂ł����x���ɂȂ�B���x���͉������Ȃ�
*------------------------------------------------------------------------------*/
static	void	ComLabel(void)
{
	char	strName[512];
											// ���x���𓾂�
	iCount += GET_LABEL(strName, &Text[iCount]);

	//--- ���x���ɃZ�~�R�������t���Ă��Ȃ��Ƃ��̏��� --------------
	if(Text[iCount] != ':')
	{
		sys.bError = true;
		sys_error("���x���w%s�x�ɃZ�~�R����������܂���B�܂��͕s���ȕ������g���Ă��܂��B", strName);
	}
	iCount++;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �W�����v >>>				@goto �������W�����v
|	�߂�l	�s���܂ł̕�����
*------------------------------------------------------------------------------*/
static	int		ComGoto(void)
{
	int	iLen = 0;

	SPACE_DEL(Text, &iCount);				// �擪�̋󔒂����

	//--- �t�@�C�����𓾂� ----------------------------------------
	bool	bFile = false;
	char	strFile[256];
	if(Text[iCount] == '"')
	{
		bFile   = true;
		iCount += GET_STRING(strFile, &Text[iCount]);
		SPACE_DEL(Text, &iCount);
		if(Text[iCount] == ',') iCount++;
		SPACE_DEL(Text, &iCount);
	}
	//--- ���x�� --------------------------------------------------
	bool	bLabel = false;
	char	strLabel[512];					// �����Ƀ��x�����i�[�����i���p512�����j
	char	*p;								// @goto �̌�ɑ������x���̈ʒu
	SPACE_DEL(Text, &iCount);
	if(Text[iCount] == '#')
	{
		bLabel      = true;
		strLabel[0] = '#';					// ���x���̐擪�� # ���Z�b�g
		p		    = &Text[iCount];		// p ��@goto�ɑ������x���̏ꏊ���i�[
		iCount++;
											// ���x���𓾂�
		iCount += GET_LABEL(&strLabel[1], &Text[iCount]);
		strcat(strLabel, ":");
	}
	iLen = iCount;
	//--- �t�@�C�����ړ� ------------------------------------------
	if(bFile)
	{
		script_load(strFile);
		iCount	  = 0;
	}
	//--- ���x����T�� --------------------------------------------
	if(!sys.bError && bLabel)
	{
		int iLabelLen = strlen(strLabel);
		for(int i = 0;;i++)					// ���͂̐擪��胉�x������T��
		{									// @goto �ɑ������x���͂����ŃX�L�b�v
			if(p == &Text[i]) continue;
			if(strncmp(&Text[i], strLabel, iLabelLen) == 0)
			{
				iCount = i + iLabelLen; break;
			}
			if(Text[i] == NULL)				// �G���[����
			{
				sys.bError = true;
				sys_error("@goto �ŃW�����v��̃��x���w%s�x��������܂���",  strLabel); break;
			}
		}
	}
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �ϐ� >>>					$ �ϐ�
*------------------------------------------------------------------------------*/
static	void	ComVar(void)
{
	int		iNum;							// ���ӁA�ϐ��̓o�^�ԍ��𓾂�
	iCount += VarNumGet(&iNum, &Text[iCount]);

	int		iType;							// ���Z�^�C�v�𓾂�
	char	a = Text[iCount + 0];
	char	b = Text[iCount + 1];

	if(		a == '+' && b == '=') iCount += 2, iType = 0;
	else if(a == '-' && b == '=') iCount += 2, iType = 1;
	else if(a == '*' && b == '=') iCount += 2, iType = 2;
	else if(a == '/' && b == '=') iCount += 2, iType = 3;
	else if(a == '='            ) iCount++,    iType = 4;

	float	fDat;							// �E�ӂ𓾂�
	iCount += ParamGet(&fDat, &Text[iCount]);

	switch(iType)							// ���ۂɉ��Z
	{
	case 0: Var[iNum].fDat += fDat; break;	// +=
	case 1: Var[iNum].fDat -= fDat; break;	// -=
	case 2: Var[iNum].fDat *= fDat; break;	// *=
	case 3: Var[iNum].fDat /= fDat; break;	// /=
	case 4: Var[iNum].fDat  = fDat; break;	// =
	}
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : if �� >>>					@if �����W�����v
*------------------------------------------------------------------------------*/
static	void	Comif(void)
{
	bool	bJump = false;
	float	fDat, fDat2;
											// ���ӂ̒l�𓾂�
	iCount += ParamGet(&fDat, &Text[iCount]);
	char	a = Text[iCount + 0];
	char	b = Text[iCount + 1];

	iCount += 2;							// �E�ӂ̒l�𓾂�
	iCount += ParamGet(&fDat2, &Text[iCount]);
	//--- �����`�F�b�N --------------------------------------------
	if(a == '=' && b == '=')				// == �`�F�b�N
	{
		if(fDat == fDat2) bJump = true;
	}
	else if(a == '!' && b == '=')			// != �`�F�b�N
	{
		if(fDat != fDat2) bJump = true;
	}
	else if(a == '<' && b == '=')			// <= �`�F�b�N
	{
		if(fDat <= fDat2) bJump = true;
	}
	else if(a == '>' && b == '=')			// >= �`�F�b�N
	{
		if(fDat >= fDat2) bJump = true;
	}
	else if(a == '<')						// < �`�F�b�N
	{
		if(fDat < fDat2) bJump = true;
	}
	else if(a == '>')						// > �`�F�b�N
	{
		if(fDat > fDat2) bJump = true;
	}
	//--- �����𖞂����Ă���΃W�����v -------------------
	if(bJump)								// ���� @goto �Ɠ����ł�
	{
		SPACE_DEL(Text, &iCount);
		if(strncmp(&Text[iCount], "@gosub", 6) == 0)
		{
			iCount += 6;
			ComGosub();
		}
		else if(strncmp(&Text[iCount], "@goto", 5) == 0)
		{
			iCount += 5;
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
		ComComment();
	}
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���j���[ >>>				@menu �����W�����v
*------------------------------------------------------------------------------*/
static	void	ComMenu(void)
{
	iCount += ParamGet(&Menu.fX, &Text[iCount]); iCount++;	// �w���W
	iCount += ParamGet(&Menu.fY, &Text[iCount]); iCount++;	// �x���W

	SPACE_DEL(Text, &iCount);

	bMenu = true;							// ���̃t���O�� true �Ȃ烁�j���[���[�h
	//--- ���j���[�̍��ڂ����o�� --------------------------------
	int	iStrMax  = 0;
	Menu.iStrMax = 0;
	Menu.iLine   = 0;
	for(int i = 0;;)						// �Z�~�R���������s�ɕς���
	{
		if(Text[iCount] == NULL) break;		// NULL �łȂ烁�j���[�I���
		if(ISKANJI(Text[iCount]))			// �S�p�Ȃ�Ε����o�^
		{
			Menu.str[i    ] = Text[iCount    ];
			Menu.str[i + 1] = Text[iCount + 1];
			i              += 2;
			iCount         += 2;
			iStrMax		   += 2;
		}
		else if(Text[iCount] <= ' ')		// �X�y�[�X�ȉ��Ȃ�΍폜
		{
			iCount++;
		}
		else if(Text[iCount] == ':')		// �R�����Ŏ��̍��ڂ�
		{
			Menu.str[i] = '\n';
			i++;
			iCount++;
			Menu.iLine++;
			if(Menu.iStrMax < iStrMax) Menu.iStrMax = iStrMax;
			iStrMax = 0;
		}
		else if(Text[iCount] == ';')		// �Z�~�R�����Ń��j���[�I��
		{
			iCount++;
			Menu.iLine++;
			if(Menu.iStrMax < iStrMax) Menu.iStrMax = iStrMax;
			break;
		}
		else								// ���p�͓o�^
		{
			Menu.str[i] = Text[iCount];
			i++;
			iCount++;
			iStrMax++;
		}
	}
	MenuMake(Menu.str);					// ���j���[�쐬
	Menu.bFirst = false;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �ϐ��𕶎��� >>>			%d:�ϐ���, %f:�ϐ���
*------------------------------------------------------------------------------*/
static	void	ComVarToStr(void)
{
	char	a = Text[iCount];

	iCount++;								// d ��i�߂�
	SPACE_DEL(Text, &iCount);
	iCount++;								// : ��i�߂�
	iCount++;								// $ ��i�߂�
	int	iID;
											// ��������ϐ���ID�𓾂�
	iCount += VarNumGet(&iID, &Text[iCount]);

	char	str[256];
	if(a == 'd')							// %d �H
	{
		sprintf(str, "%d", (int)Var[iID].fDat);
	}
	else if(a == 'f')						// %f �H
	{
		sprintf(str, "%f", Var[iID].fDat);
	}

	strcpy(&strBuf[iBufCount], str);		// ������o�^����
	iBufCount += strlen(str);
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �N���A�[ >>>				@c
*------------------------------------------------------------------------------*/
static	void	ComClear(void)
{
	strBuf[iBufCount] = '\b';				// �o�b�t�@�� BS �}�[�N�������B
	iBufCount++;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �L�[�E�G�C�g >>>			@key
*------------------------------------------------------------------------------*/
static	void	ComKeyWait(void)
{
	bKeyWait = true;						// ���̃t���O���I���ɂ���� TextMain ���ŃL�[�҂��ɂȂ�
	bKeyMark = true;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �L�[�E�G�C�g >>>			@Key
*------------------------------------------------------------------------------*/
static	void	ComKeyWaitNoMark(void)
{
	bKeyWait = true;						// ���̃t���O���I���ɂ���� TextMain ���ŃL�[�҂��ɂȂ�
	bKeyMark = false;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �T�u���[�`���R�[�� >>>		@gosub
*------------------------------------------------------------------------------*/
static	void	ComGosub(void)
{
	for(int i = GOSUB_MAX - 1; i != 0; i--) memcpy(&GosubWork[i], &GosubWork[i - 1], sizeof(GOSUB_WORK));
	int	a = ComGoto();
	GosubWork[1].iCount = a;
	iGosubCount++;
	Var[0].fDat = -100;						// ���j���[�I����̃W�����v��Ŏx�Ⴊ���e�ɓK���Ȓl���Z�b�g
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���^�[�� >>>				@return
*------------------------------------------------------------------------------*/
static	void	ComReturn(void)
{
	if(iGosubCount == 0)
	{
		free(Text);	Text = NULL; return;
	}
	for(int i = 0; i < GOSUB_MAX - 1; i++) memcpy(&GosubWork[i], &GosubWork[i + 1], sizeof(GOSUB_WORK));
	script_load(GosubWork[0].strName);
	iGosubCount--;
	iCount		= GosubWork[0].iCount;
	Var[0].fDat = -100;						// ���j���[�I����̃W�����v��Ŏx�Ⴊ���e�ɓK���Ȓl���Z�b�g
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �R�����g >>>				//		�� ���s�܂ŃR�����g
*------------------------------------------------------------------------------*/
static	void	ComComment(void)
{
	for(;;)
	{										// ������i�߂�
		if(ISKANJI(Text[iCount])) iCount += 2;
		else					  iCount += 1;
											// ���s���`�F�b�N�A���s�Ȃ�΃��[�v�~�߂�
		if(Text[iCount] == '\r' && Text[iCount + 1] == '\n'){ iCount += 2; break;}
											// NULL �ł����[�v���~�߂�B
		if(Text[iCount] == NULL) break;
	}
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �e�N�X�`���[�ǂݍ��� >>>	@tex_load �e�N�X�`���[�ԍ�,�t�@�C����
*------------------------------------------------------------------------------*/
static	void	ComTexLoad(void)
{
	//--- �ǂݍ��ݔԍ� --------------------------------------------
	float	fID;
	iCount += ParamGet(&fID, &Text[iCount]);

	//--- �t�@�C�����𓾂� ----------------------------------------
	iCount += 1;							// �J���}��ǂݔ�΂�
	char	strFile[256];
	iCount += GET_STRING(strFile, &Text[iCount]);

	//--- �����œǂݍ��� ------------------------------------------
	FOLDER_SET("�X�N���v�g\\�摜");
	bool	b = (d3_tex_load((int)fID, strFile) != -1);
	if(!b){ script_error(); return;}
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �e�N�X�`���[�ԍ��ݒ� >>>	@tex_set ���C���[�ԍ�,�e�N�X�`���[�ԍ�
*------------------------------------------------------------------------------*/
static	void	ComTexSet(void)
{
	//--- ���C���[�ԍ� --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay, &Text[iCount]);

	//--- �e�N�X�`���[�ԍ� ----------------------------------------
	iCount += 1;							// �J���}��ǂݔ�΂�
	float	fTex;
	iCount += ParamGet(&fTex, &Text[iCount]);
	
	int	iLay = (int)fLay;
	int	iTex = (int)fTex;
	LayerWork[iLay].iTex = iTex;			// �ԍ��ݒ�

	//--- �t�u�ݒ� ------------------------------------------------
	LayerWork[iLay].fU1 = d3.texInfo[iTex].fU;
	LayerWork[iLay].fV1 = d3.texInfo[iTex].fV;

	//--- �e�N�X�`���[�T�C�Y���摜�T�C�Y���v�Z���� --------------
	LayerWork[iLay].vecP0   = VEC3(0, 0, 0);
	LayerWork[iLay].vecP1.x = d3.texInfo[iTex].fOriWidth;
	LayerWork[iLay].vecP1.y = d3.texInfo[iTex].fOriHeight;
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �t�F�[�h�C�� >>>			@fade_in ���C���[�ԍ�, ����
*------------------------------------------------------------------------------*/
static	void	ComFadeIn(void)
{
	//--- ���C���[�ԍ� --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay,  &Text[iCount]);	// ���C���[�ԍ�

	//--- ���� ----------------------------------------------------
	float	fTime;
	iCount += 1;								// �J���}��ǂݔ�΂�
	iCount += ParamGet(&fTime, &Text[iCount]);	// �e�N�X�`���[�ԍ��B���̉摜���\�������

	//--- �t�F�[�h�C�� --------------------------------------------
	int	iID             = (int)fLay;
	int	iTime           = (int)fTime;
	LayerWork[iID].fAlp = 0;
	//			�Ώەϐ�		  �I���l�@����  �x������
	MoveStart(&LayerWork[iID].fAlp, 255,  iTime, 0);
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : �t�F�[�h�A�E�g >>>			@fade_out ���C���[�ԍ�, ����
*------------------------------------------------------------------------------*/
static	void	ComFadeOut(void)
{
	//--- ���C���[�ԍ� --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay,  &Text[iCount]);	// ���C���[�ԍ�

	//--- ���� ----------------------------------------------------
	float	fTime;
	iCount += 1;								// �J���}��ǂݔ�΂�
	iCount += ParamGet(&fTime, &Text[iCount]);	// �e�N�X�`���[�ԍ��B���̉摜���\�������

	//--- �t�F�[�h�A�E�g ------------------------------------------
	int	iID             = (int)fLay;
	int	iTime           = (int)fTime;
	LayerWork[iID].fAlp = 255;
	//			�Ώەϐ�		  �I���l ����  �x������
	MoveStart(&LayerWork[iID].fAlp, 0,  iTime, 0);
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h : ���C���[�ړ� >>>			@move ���C���[�ԍ�, �w���W,�x���W,����
*------------------------------------------------------------------------------*/
static	void	ComMove(void)
{
	//--- ���C���[�ԍ� --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay,  &Text[iCount]);	// ���C���[�ԍ�

	//--- �w���W --------------------------------------------------
	float	fX;
	iCount += 1;								// �J���}��ǂݔ�΂�
	iCount += ParamGet(&fX, &Text[iCount]);	// �e�N�X�`���[�ԍ��B���̉摜���\�������

	//--- �x���W --------------------------------------------------
	float	fY;
	iCount += 1;								// �J���}��ǂݔ�΂�
	iCount += ParamGet(&fY, &Text[iCount]);	// �e�N�X�`���[�ԍ��B���̉摜���\�������

	//--- ���� ----------------------------------------------------
	float	fTime;
	iCount += 1;								// �J���}��ǂݔ�΂�
	iCount += ParamGet(&fTime, &Text[iCount]);	// �e�N�X�`���[�ԍ��B���̉摜���\�������

	//--- ���C���[�ړ� --------------------------------------------
	int		iID   = (int)fLay;
	int		iTime = (int)fTime;
	float	fW    = LayerWork[iID].vecP1.x - LayerWork[iID].vecP0.x;
	float	fH    = LayerWork[iID].vecP1.y - LayerWork[iID].vecP0.y;
	//			�Ώەϐ�               �I���l   ����  �x������
	MoveStart(&LayerWork[iID].vecP0.x, fX,      iTime, 0);
	MoveStart(&LayerWork[iID].vecP0.y, fY,      iTime, 0);
	MoveStart(&LayerWork[iID].vecP1.x, fX + fW, iTime, 0);
	MoveStart(&LayerWork[iID].vecP1.y, fY + fH, iTime, 0);
}



/*------------------------------------------------------------------------------*
| <<< �R�}���h�`�F�b�N >>>
|	�߂�l	true : ��x�ł��R�}���h�����s����
*------------------------------------------------------------------------------*/
static	bool	CommandCheck(void)
{
	bool	bRet = false;

	for(int i = 0;;i++)
	{
		if(sys.bError) return bRet;
		int	iLen = strlen(CommandTbl[i]);	// ��strlen = ������̒����𒲂ׂ�
		if(iLen == 0) break;				// ������̒������O�Ȃ�I�[�Ƃ݂Ȃ�
		if(strncmp(&Text[iCount], CommandTbl[i], iLen) == 0)
		{
			iCount += iLen;					// �R�}���h�̕�������i�߂�
			bRet    = true;
			sprintf(strErrorCommand, "%s �ɃG���[������܂��B", CommandTbl[i]);
			switch(i)
			{
			case 0:			// 00 : ���p�X�y�[�X
			case 1:			// 01 : �^�u
			case 2:			// 02 : �{���̉��s�R�[�h
				break;
			case 3:			// 03 : �V�K�̉��s			@n
				ComKaigyo(); break;
			case 4:			// 04 : �E�G�C�g			@wait
				ComWait(); return true;
			case 5:			// 05 : ���x��				#		���Ȃɂ��������Ȃ��ŃX�L�b�v����B
				ComLabel(); break;
			case 6:			// 06 : �������W�����v		@goto	���w�胉�x���܂ŃW�����v
				ComGoto(); break;
			case 7:			// 07 : �ϐ�				$
				ComVar();  break;
			case 8:			// 08 : ��������			@if
				Comif(); break;
			case 9:			// 09 : ���j���[			@menu
				ComMenu(); return true;
			case 10:		// 10 : �ϐ��𕶎���		%d, %f
				ComVarToStr(); break;
			case 11:		// 11 : �N���A�[			@clear
				ComClear(); break;
			case 12:		// 12 : �L�[�҂�			@key
				ComKeyWait();        return true;
			case 13:		// 13 : �L�[�҂�			@Key
				ComKeyWaitNoMark();  return true;
			case 14:		// 14 : �T�u���[�`���R�[��	@gosub	���w�胉�x�����Ăяo��
				ComGosub(); return true;
			case 15:		// 15 : ���^�[��			@return	��@gosub�ŌĂяo���ꂽ�̂�߂�
				ComReturn(); return true;
			case 16:		// 16 : �R�����g			//
				ComComment();   break;
			case 17:		// 17 : �e�N�X�`���[���[�h	@tex_load
				ComTexLoad();  break;
			case 18:		// 18 : �e�N�X�`���[�ԍ��ݒ�@tex_set
				ComTexSet();   break;
			case 19:		// 19 : �t�F�[�h�C��		@fade_in
				ComFadeIn();   break;
			case 20:		// 20 : �t�F�[�h�A�E�g		@fade_out
				ComFadeOut(); break;
			case 21:		// 21 : ���C���[�ړ�		@move
				ComMove();	  break;
			}
			i = -1;							// ������x�ACommandTbl�̍ŏ����璲�ׂ�
		}
	}
	//--- �s���ȃR�}���h���� --------------------------------------
	if(Text[iCount] == '@')
	{
		sprintf(strErrorCommand, "�s���ȃR�}���h������܂��B");
		script_error();
	}
	return bRet;
}



/*----------------------------------------------------------------------*
| <<< �X�N���v�g���C������ >>>			�����̊֐����ŕ`�揈���͍s���܂���
|	�߂�l	true : �X�N���v�g�����͏I������
*----------------------------------------------------------------------*/
bool	script_main(void)
{
	if(Text == NULL)  return false;			// �X�N���v�g�����[�h���ɂ͈ȉ����������Ȃ��B
	if(sys.bError)    return false;
	if(iBufCount > 0) return false;

	//--- ���j���[ ------------------------------------------------
	if(bMenu)
	{
		int a = MenuMain(&Menu);			// ���j���[�I�����A���j���[�\���I�t�AiAns �Ɉꎞ������ޔ�
		if(a >= 0){	Var[0].fDat = (float)a; bMenu = false;}
		return false;
	}
	//--- �E�G�C�g�`�F�b�N ------------------------------------
	if(iDelay > 0){ iDelay--; return false;}	// @wait ���̏����BiDelay ���O�ɂȂ�܂ňȉ����������Ȃ�
	
	//--- �L�[�E�G�C�g --------------------------------------------
	if(iDelay == 0 && bKeyWait)
	{
		if(bKeyMark)
		{
			char	a[] = {(char)0xf0, NULL};
			if((sys.iTimeCount % 60) > 30) a[0]++;
			float	x = (iX / 512.f) * 640.f;
			float	y = (iY / 512.f) * 480.f;
			Printf(iNextX + x, y, (float)d3.fontInfo[TEXT_FONT].Height * 0.8f, a);
		}
		if(pad.cl || pad.key[KEY_Z]) bKeyWait = false;
		else						 return false;
	}
	//--- �e�N�X�`���[�ɕ`��J�n ----------------------------------
	int		iLen;
											// ������̏I�[�Ȃ�Ώ������Ȃ�
	while(Text[iCount] != NULL)
	{
		if(sys.bError) return false;
		//--- �R�}���h�`�F�b�N ------------------------------------
		bool	bRet = CommandCheck();
		if(bRet) break;						// �R�}���h�����s���Ă����當���͕`�悵�Ȃ�

		//--- ���p�E�S�p�����`��Z�b�g ----------------------------
		if(ISKANJI(Text[iCount])) iLen = 2;	// �S�p
		else					  iLen = 1;	// ���p

		for(int i = 0; i < iLen; i++)
		{
			strBuf[iBufCount] = Text[iCount];
			iBufCount++;
			iCount++;
		}
		if(iBufCount + 2 > BUF_MAX) break;
	}

	//--- �X�N���v�g�I���`�F�b�N ----------------------------------
	if(Text == NULL) return true;			// �X�N���v�g�͏I������
	else			 return false;
}



/*----------------------------------------------------------------------*
| <<< ���C���[��`�� >>>
|	����	iID = ���C���[�ԍ�
*----------------------------------------------------------------------*/
static	void	BgDraw(int iID)
{
	LAYER_WORK	*w = &LayerWork[iID];

	if(w->fAlp <= 0) return;
/*
	D3_ZREAD( false);
	D3_ZWRITE(false);
	D3_TEXTURE(0, w->iTex);
	D3_2DDRAW(&w->vecP0,					// ����̍��W
			  &w->vecP1,					// �E���̍��W
			  w->fU0, w->fV0,
			  w->fU1, w->fV1,
			  D3_COL((int)w->fAlp, 255, 255, 255));
	D3_ZREAD( true);
	D3_ZWRITE(true);

//
return;//
*/
	
	int	type = D3_SHADER_TYPE_CHARAOUTLINE;

	if(d3_shader_set(type))
	{
		d3_mesh_draw_1st(type);
		uint	uiPasses;
		d3.eff->Begin(&uiPasses, D3DXFX_DONOTSAVESTATE);

		d3.eff->SetTexture("texDecale", d3.tex[w->iTex]);
		d3.eff->Pass(0);

		d3.eff->SetFloat("g_fWidth",  d3.texInfo[w->iTex].fWidth);
		d3.eff->SetFloat("g_fHeight", d3.texInfo[w->iTex].fHeight);

		D3_VTX_LX_	vt[] = {-1, -1, 0, D3_COL_NORMAL,  0, 1,
							-1,  1, 0, D3_COL_NORMAL,  0, 0,
							 1,  1, 0, D3_COL_NORMAL,  1, 0,
							 1, -1, 0, D3_COL_NORMAL,  1, 1};

		d3.dev->SetFVF(D3_FVF_LX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt, sizeof(D3_VTX_LX));

		d3.eff->End();
		d3.iPolyCount += 2;

		d3.eff->SetTexture("texDecale", NULL);
		d3_mesh_draw_3rd();
	}
}



/*----------------------------------------------------------------------*
| <<< �X�N���v�g�`�� >>>
|	����	iTex = �g�p����e�N�X�`���[�ԍ�
*----------------------------------------------------------------------*/
void	script_draw(int iTex)
{
//	if(sys.bFrameSkip) return;

	//--- ���j���[ ------------------------------------------------
	if(bMenu) MenuDraw(&Menu);

	//--- �����`�� ------------------------------------------------
	if(iBufCount > 0)
	{
		d3_offscreen_begin(iTex);
		d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3_COL(0, 0, 0, 0), 1.0f, 0);
		int	i;
		for(i = 0; i < BUF_MAX;)
		{
			int	iLen;
			if(ISKANJI(strBuf[i])) iLen = 2;	// �S�p
			else				   iLen = 1;	// ���p

			if(strBuf[i] == '\n')				// ���s
			{
				iX     = TEXT_X1;
				iNextX = 0;
				iY    += d3.fontInfo[TEXT_FONT].Height;
				i++;
				continue;
			}
			else if(strBuf[i] == '\b')			// BS�i��ʃN���A�[�j
			{
				iX     = TEXT_X1;
				iNextX = 0;
				iY     = 0;
//				d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOL(255, 128, 0, 0), 1.0f, 0);
				d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, d3.dwClearCol, 1.0f, 0);
				i++;
				continue;
			}

			iX += iNextX;
			if(iX >= TEXT_X2)					// �s���܂ł������H
			{									// �ȉ��� if ���ŋ֑�����
				if(!IsKinsokuCheck((uchar *)&strBuf[i]))
				{
					iX  = TEXT_X1;				// ���s
					iY += d3.fontInfo[TEXT_FONT].Height;
				}
			}
			RECT	r = {iX, iY, 0, 0};
			d3.font[TEXT_FONT]->DrawText(NULL, &strBuf[i], iLen, &r, DT_NOCLIP, dwColor);
			if(iLen == 1) iNextX = (int)((float)d3.fontInfo[TEXT_FONT].Width * 1.7f);
			else		  iNextX = (int)((float)d3.fontInfo[TEXT_FONT].Width * (float)iLen);

			i += iLen;
		}
		d3_offscreen_end(iTex);

		iBufCount -= i;
		memcpy(strBuf, &strBuf[i], sizeof(strBuf) - i);
	}
	//--- ���C���[ ------------------------------------------------
	D3_MIN_MAG_FILTER(0, D3DTEXF_LINEAR);	// ���̐ݒ�ŁA�������{�P�܂�
	D3_BLEND_NORMAL();						// �������ݒ�
	d3.dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	for(int i = LAYER_MAX - 1; i != -1; i--)
	{										// �e�L�X�g�ʂ݂̂��̐ݒ�
		if(LayerWork[i].iTex == iTex) d3.dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		else						  d3.dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		switch(LayerWork[i].iType)
		{
		case 0:
			BgDraw(i);
			break;
		}
	}
	d3.dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}



/*------------------------------------------------------------------------------*
| <<< �X�N���v�g�ŕ����\�� >>>
|	����	Format = printf�Ɠ����ł�
*------------------------------------------------------------------------------*/
void	script_printf(char *Format,...)
{

	script_free();

	//--- ������쐬 ------------------------------------------------
    char	strBuf[1024];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- �T�C�Y���擾���� ------------------------------------------
	int	iSize = strlen(strBuf);
		
	//--- �������[���m�ۂ��� ----------------------------------------
	iSize += 1;								// +1 ������NULL��ǉ�
	Text  = (char *)malloc(iSize);
	if(Text == NULL) sys_error("script_printf: %s �̕��͂Ń������[�̊m�ۂɎ��s���܂����B", strBuf);
	memset(Text, 0, iSize);					// �[���N���A�[
}



/*------------------------------------------------------------------------------*
| <<< �X�N���v�g�ǂݍ��� >>>
|	����	*strFile = �ǂݍ��݃t�@�C��
*------------------------------------------------------------------------------*/
bool	script_load(char *strFile)
{
	HANDLE	Handle;							// �ǂݍ��݃n���h��
	DWORD	rs;								// �_�~�[
	int		iSize;							// �ǂݍ��݃T�C�Y

	//--- �������[�J�� --------------------------------------------
	if(Text != NULL){ free(Text); Text = NULL;}

	//--- �t�@�C�����J�� --------------------------------------
	//char	str[MAX_PATH + 1];
	//sprintf(str, "%s%s", strPath, strFile);
	Handle = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if(Handle == INVALID_HANDLE_VALUE){ return sys_error("%s ���J���܂���B", strFile);}

	sprintf(sys.strName, "%s", strFile);

	//--- �T�C�Y���擾���� ------------------------------------
	iSize = GetFileSize(Handle, NULL);
	if(iSize == 0){ CloseHandle(Handle); return sys_error("%s �̃T�C�Y���O�o�C�g�ł��B", strFile);}
		
	//--- �������[���m�ۂ��� ----------------------------------
	iSize += 1;								// +1 ������NULL��ǉ�
	Text  = (char *)malloc(iSize);
	if(Text == NULL){ CloseHandle(Handle); return sys_error("%s ��ǂݍ��ݒ��������[�̊m�ۂɎ��s���܂����B", strFile);}
	memset(Text, 0, iSize);					// �[���N���A�[

	//--- �ǂݍ��� --------------------------------------------
	ReadFile(Handle, Text, iSize, &rs, NULL);

	//--- �t�@�C������� ------------------------------------
	CloseHandle(Handle);

	//--- �t�@�C�������΃p�X�œ��� ------------------------------
	sprintf(GosubWork[0].strName, "%s", strFile);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ϐ������ϐ��Ǘ��ԍ��𓾂� >>>
|	����	*strName = �ϐ����i�擪�� $ �͕s�v�ł��j
|	�߂�l			 =  -1 : ���̕ϐ��͓o�^����Ă��Ȃ��B
|				    !=  -1 : ���̕ϐ��̊Ǘ��ԍ�
*------------------------------------------------------------------------------*/
int		TextVarNumGet(char *strName)
{
	int	iNameLen = strlen(strName);
	int	i;
	for(i = 0;;i++)							// �ϐ���(name)���o�^����Ă��邩���ׂ�
	{										// �o�^�����������H
		if(strncmp(Var[i].strName, strName, iNameLen) == 0) break;
											// �o�^����Ă��Ȃ���� -1 ��Ԃ�
		if(strlen( Var[i].strName) == 0)					return -1;
	}
	return i;								// �߂�l�͕ϐ��̊Ǘ��ԍ�
}



/*------------------------------------------------------------------------------*
| <<< �ϐ�����ϐ��Ɋ��蓖�Ă� >>>
|	����	*strName = �ϐ����i�擪�� $ �͕s�v�ł��j
|	�߂�l			 =  -1 : �ϐ��͓o�^�ł��Ȃ������B�܂��͓����̕ϐ������݂���
|				    !=  -1 : ���̕ϐ��̊Ǘ��ԍ�
*------------------------------------------------------------------------------*/
int		TextVarNameSet(char *strName)
{
	//--- �����̕ϐ������邩���ׂ� --------------------------------
	int	iNameLen = strlen(strName);
	int	i;
	for(i = 0; i < VAR_MAX; i++)
	{
		if(strncmp(Var[i].strName, strName, iNameLen) == 0) break;
	}

	//--- �����̕ϐ������邩���ׂ� --------------------------------
	for(i = 0; i < VAR_MAX; i++)
	{
		if(Var[i].strName[0] == NULL)
		{
			strcpy(Var[i].strName, strName);
			return i;
		}
	}
	//--- �ϐ��͑S�ēo�^�ς� --------------------------------------
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< �Ǘ��ԍ����ϐ��̒l�𓾂� >>>
|	����	iID   = �ϐ��Ǘ��ԍ��iTextVarNumGet�œ����l�j
|	�o��	*fOut = �ϐ��̒l
|	�߂�l		  = true  : ����ɒl��Ԃ����B
|				  = false : ���̕ϐ��͖��o�^�B
*------------------------------------------------------------------------------*/
bool	TextVarGet(int iID, float *fOut)
{
	if(iID == -1)					return false;	// -1 �Ƃ����ϐ�ID�͂Ȃ�
	if(Var[iID].strName[0] == NULL) return false;	// �ϐ����`�F�b�N
													// �ꕶ���ڂ�NULL�Ȃ疢�o�^
	*fOut = Var[iID].fDat;							// fOut �ɒl���Z�b�g
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �Ǘ��ԍ����ϐ��̒l���Z�b�g >>>
|	����	iID  = �ϐ��Ǘ��ԍ��iTextVarNumGet�œ����l�j
|	�o��	fIn  = �ϐ��̒l
|	�߂�l		 = true  : ����ɒl���Z�b�g�����B
|				 = false : ���̕ϐ��͖��o�^�B
*------------------------------------------------------------------------------*/
bool	TextVarSet(int iID, float fIn)
{
	if(iID == -1)					return false;	// -1 �Ƃ����ϐ�ID�͂Ȃ�
	if(Var[iID].strName[0] == NULL) return false;	// �ϐ����`�F�b�N
													// �ꕶ���ڂ�NULL�Ȃ疢�o�^
	Var[iID].fDat = fIn;							// fOut �ɒl���Z�b�g
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �e�L�X�g�ʒu���J�n�ʒu�ɐݒ肷�� >>>
*------------------------------------------------------------------------------*/
void	TextCountClear(void)
{
	iCount = 0;
}
