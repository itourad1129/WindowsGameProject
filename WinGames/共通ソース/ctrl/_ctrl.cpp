//
//	�R���g���[���֌W�̃v���O����
//

/*
	�d�g�݂Ƃ��Ắ@CTRL_MOTHER	�� CTRL_WORK �̃��������m�ہA
	�_�C�A���O���쐬����B
	�_�C�A���O�̓��\�[�X���g���A���\�[�X�ɂ͊e�R���g���[����
	�\��t������A�v���O�����ō쐬�����肷��B
*/

//	[[[ �t�H���g ]]]
//	[[[ �}�U�[ ]]]
//	[[[ �f�o�b�N�p�G�f�B�b�g�{�b�N�X ]]]
//	[[[ �v�b�V���{�^�� ]]]
//	[[[ �G�f�B�b�g�{�b�N�X ]]]
//	[[[ �X�^�e�B�b�N�e�L�X�g ]]]
//	[[[ �X�^�e�B�b�N�s�N�`���[ ]]]
//	[[[ �R���{�{�b�N�X ]]]
//	[[[ �O���[�v�{�b�N�X ]]]
//	[[[ �c�[���`�b�v ]]]
//	[[[ �^�u�R���g���[�� ]]]
//	[[[ �X���C�_�[ ]]]
//	[[[ ���X�g�r���[ ]]]
//	[[[ �`�F�b�N�{�b�N�X ]]]
//	[[[ �t�H���_�[�I�� ]]]
//	[[[ �X�N���[�� ]]]
//	[[[ ���W�I�{�^�� ]]]
//	[[[ �Z�p���[�^�[ ]]]



/*************************************************************************************************
	[[[ �t�H���g ]]]
**************************************************************************************************/
static	HFONT	hFontDefault = NULL;

/*------------------------------------------------------------------------------*
| <<< �e�R���g���[���̃f�t�H���g�t�H���g�ݒ� >>>
*------------------------------------------------------------------------------*/
void	ctrl_font_create(HFONT hFont)
{
	hFontDefault = hFont;
}

HFONT	ctrl_font_create(int iSize, char *strFont)
{

	HFONT	hFont = CreateFont(iSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, strFont);
	hFontDefault  = hFont;
	return	hFont;
}



/*------------------------------------------------------------------------------*
| <<< �t�H���g��� >>>
*------------------------------------------------------------------------------*/
void	ctrl_font_free(HFONT hFont)
{
	if(hFont != NULL)
	{
		DeleteObject(hFont);
		hFontDefault = NULL;
	}
}



/*************************************************************************************************
	[[[ �}�U�[ ]]]
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< ����̃_�C�A���O���쐬 >>>
|	����	hWnd		= �e�̃E�B���h�E�n���h��
|			pProc       = ���荞�݊֐���
|			strResource = �_�C�A���O���\�[�X
*------------------------------------------------------------------------------*/
HWND	ctrl_create(HWND hMother, void *pProc, char *strResource)
{
	return CreateDialog(sys.hInst, strResource, hMother, (DLGPROC)pProc);
}


/*------------------------------------------------------------------------------*
| <<< ����̃_�C�A���O���쐬 >>>
|	����	pProc       = ���荞�݊֐���
|			strResource = �_�C�A���O���\�[�X
*------------------------------------------------------------------------------*/
HWND	ctrl_create(void *pProc, char *strResource)
{
	return CreateDialog(sys.hInst, strResource, sys.hWnd, (DLGPROC)pProc);
}


/*------------------------------------------------------------------------------*
| <<< �R���g���[�����[�N�m�� >>>		��������͉��������_�C�A���O���쐬���܂��B
|	����	pProc	  =	�C�x���g�֐���
|	���������m�ۂ��܂��Bctrl_free �ŊJ���̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_create(void *pProc)
{

	//CTRL_WORK	*w	  = (CTRL_WORK*)malloc(sizeof(CTRL_WORK));

	//memset(w, 0, sizeof(CTRL_WORK));
	return CreateDialog(sys.hInst, "DIALOG", sys.hWnd, (DLGPROC)pProc);
	//ShowWindow(w->hWnd,	SW_HIDE);									// ��\���ō쐬����܂��B
	//if(hFontDefault != NULL) CTRL_FONT_SET(w->hWnd, hFontDefault);	// �t�H���g�ݒ�
	//return w;
}



/*------------------------------------------------------------------------------*
| <<< �R���g���[�����[�N�J�� >>>
*------------------------------------------------------------------------------*/
void	ctrl_free(CTRL_MOTHER *w)
{

	//--- �e�R���g���[������� ------------------------------------
	for(int i = 0; i < CTRL_CONTROL_MAX; i++)
	{
		if(w->hCtrl[i] != NULL)
		{
			DestroyWindow(w->hCtrl[i]);
			w->hCtrl[i] = NULL;
		}
		if(w->hTips[i] != NULL)
		{
			DestroyWindow(w->hTips[i]);
			w->hTips[i] = NULL;
		}
	}
	//--- ��̂����� ------------------------------------------------
	if(w->hMother != NULL)
	{
		w->hMother = NULL;
		DestroyWindow(w->hMother);
	}
}



/*************************************************************************************************
	[[[ �f�o�b�N�p�G�f�B�b�g�{�b�N�X ]]]	�� ctrl_create ���Ăяo�����ƂȂ��g�p�ł��܂��B
**************************************************************************************************/
static	HFONT	hDebugFont;				// �f�o�b�N�p�t�H���g
static	HWND	hDebugEdit;				// �f�o�b�N�p�̃G�f�B�b�g�{�b�N�X

void	ctrl_debug_create(void)
{
	hDebugEdit = CreateWindow("EDIT",						//�G�f�C�b�g�R���g���[��
							"",								// �ŏ��ɕ\������镶����
							WS_CHILD | WS_VISIBLE| WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | ES_WANTRETURN,
							0, 0, 0, 0,						// �\���ʒu�A���A����
							sys.hWnd,						// �e�E�B���h�E�̃n���h��
							(HMENU)1,						// �G�W�b�g�R���g���[���̂h�c
							sys.hInst,						// �C���X�^���X�E�n���h��
							NULL);							// �ʏ�m�t�k�k�ł�

	hDebugFont = CreateFont(12, 0, 0, 0,
							FW_NORMAL, FALSE, FALSE, 0, SHIFTJIS_CHARSET,
							OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "�l�r �S�V�b�N");

	SendMessage(hDebugEdit, WM_SETFONT, (WPARAM)hDebugFont, (LPARAM)MAKELPARAM(FALSE, 0));
}

void	ctrl_debug_free(void)
{
	DeleteObject( hDebugFont);
	DestroyWindow(hDebugEdit);
}

void	ctrl_debug_view(bool bSw)
{
	if(bSw) ShowWindow(hDebugEdit, SW_SHOW);
	else	ShowWindow(hDebugEdit, SW_HIDE);
}

void	ctrl_debug_resize(int iX0, int iY0, int iX1, int iY1)
{
	MoveWindow(hDebugEdit, iX0, iY0, iX1 - iX0, iY1 - iY0, TRUE);
}



/*------------------------------------------------------------------------------*
| <<< ���b�Z�[�W�\���p >>>				���������256�����ȓ��̂���
*------------------------------------------------------------------------------*/
#define	__EDIT_SIZE	30000
void	ctrl_debug_printf(char* Format,...)
{

    char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- �����ɉ��s���Ȃ��ꍇ�͂��� ------------------------------
	int	a = strlen(strBuf);
	if(strncmp(&strBuf[a - 2], "\r\n", 2) != 0) strcat(strBuf, "\r\n");

	//--- �G�f�B�b�g�{�b�N�X�ɕ�����]�� ----------------------------
	char	*strPool = (char*)malloc(__EDIT_SIZE + 512);
	char	*strText = strPool;
	GetWindowText(hDebugEdit, strText, __EDIT_SIZE);
	strcat(strText, strBuf);

	a = strlen(strText);					// �����̃I�[�o�[�`�F�b�N
	while(strlen(strText) >= __EDIT_SIZE)
	{
		strText = strstr(strText, "\r\n") + 2;
	}

	SetWindowText(hDebugEdit, strText);
	free(strPool);
	
	//--- �ŉ��s�փX�N���[�� ----------------------------------------
	a = SendMessage(hDebugEdit, EM_GETLINECOUNT, 0, 0);
	SendMessage(    hDebugEdit, EM_LINESCROLL, 0, a - 1);
}



/*************************************************************************************************
	[[[ �v�b�V���{�^�� ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �v�b�V���{�^���̍쐬 >>>
|	����	hWnd	 = �E�B���h�D�n���h��
|			menuID   = ���j���[��ID
|			str		 = �\�����镶����
|	�߂�l	�{�^���̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_push_button_create(HWND hWnd, uint menuID, char* Format,...)
{
	HWND	hwnd;
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	hwnd = CreateWindow("BUTTON", buf, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					0, 0, 0, 0, hWnd, (HMENU)menuID, sys.hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hwnd, hFontDefault);	// �t�H���g�ݒ�
	return	hwnd;
}



/*------------------------------------------------------------------------------*
| <<< �v�b�V���{�^���̃��b�Z�[�W�ύX >>>
|	����	hWnd	 = �E�B���h�D�n���h��
|			str		 = �\�����镶����
*------------------------------------------------------------------------------*/
void	ctrl_push_button_printf(HWND hWnd, char* Format,...)
{
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	SetWindowText(hWnd, buf);
}



/*************************************************************************************************
	[[[ �G�f�B�b�g�{�b�N�X ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �G�f�B�b�g�{�b�N�X�̍쐬 >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			dwFlag = �t���O
|			menuID = ���j���[��ID
|	�߂�l	�G�f�B�b�g�̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_edit_create(HWND hWnd, DWORD dwFlag, HMENU hMenu)
{
	HWND		hOut;
	HINSTANCE	hInst;

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	hOut  = CreateWindow("EDIT",				//�G�f�C�b�g�R���g���[��
				"",								// �ŏ��ɕ\������镶����
				WS_CHILD | WS_VISIBLE | /*WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | /*ES_READONLY |*/ ES_WANTRETURN | WS_BORDER | dwFlag,
				0, 0, 0, 0,						// �\���ʒu�A���A����
				hWnd,							// �e�E�B���h�E�̃n���h��
				hMenu,							// �G�f�B�b�g�R���g���[���̂h�c
				hInst,							// �C���X�^���X�E�n���h��
				NULL);							//�ʏ�m�t�k�k�ł�

	if(hFontDefault != NULL){ CTRL_FONT_SET(hOut, hFontDefault);}	// �t�H���g�ݒ�
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< �G�f�B�b�g�{�b�N�X�̍쐬 >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			dwFlag = �t���O
|	�߂�l	�G�f�B�b�g�̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_edit_create(HWND hWnd, DWORD dwFlag)
{
	return ctrl_edit_create(hWnd, dwFlag, (HMENU)1);
}



/*------------------------------------------------------------------------------*
| <<< ���b�Z�[�W�\���p >>>				���������256�����ȓ��̂���
|	����	hWnd	 = �n���h��
|			Format	 = printf�Ɠ���
*------------------------------------------------------------------------------*/
void	ctrl_edit_printf(HWND hWnd, char* Format,...)
{

    char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- �����ɉ��s���Ȃ��ꍇ�͂��� ------------------------------
	int	a = strlen(strBuf);
	if(strncmp(&strBuf[a - 2], "\r\n", 2) != 0){ strcat(strBuf, "\r\n");}

	//--- �G�f�B�b�g�{�b�N�X�ɕ�����]�� ----------------------------
	char	*strPool = (char*)malloc(__EDIT_SIZE + 512);
	char	*strText = strPool;
	GetWindowText(hWnd, strText, __EDIT_SIZE);
	strcat(strText, strBuf);

	a = strlen(strText);					// �����̃I�[�o�[�`�F�b�N
	while(strlen(strText) >= __EDIT_SIZE)
	{
		strText = strstr(strText, "\r\n") + 2;
	}
	SetWindowText(hWnd, strText);
	free(strPool);

	//--- �ŉ��s�փX�N���[�� ----------------------------------------
//	a = SendMessage(hDebugEdit, EM_GETLINECOUNT, 0, 0);
//	SendMessage(hDebugEdit, EM_LINESCROLL, 0, a - 1);
	a = SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
	SendMessage(    hWnd, EM_LINESCROLL,   0, a - 1);
}



/*------------------------------------------------------------------------------*
| <<< ���b�Z�[�W�𓾂� >>>
|	����	hWnd	 = �n���h��
|			*strOut	 = printf�Ɠ���
*------------------------------------------------------------------------------*/
void	ctrl_edit_get(HWND hWnd, char *strOut)
{
	GetWindowText(hWnd, strOut, __EDIT_SIZE);
}



/*************************************************************************************************
	[[[ �X�^�e�B�b�N�e�L�X�g ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �X�^�e�B�b�N�e�L�X�g�̍쐬 >>>
|	����	hWnd	 = �E�B���h�D�n���h��
|			menuID   = ���j���[��ID
|			x,y, w,h = �{�^���̈ʒu�ƃT�C�Y
|			dwFlag	 = �E�B���h�E�t���O(���ڂ݂Ȃǂ͂����ł��Ă�������)
|			str		 = �\�����镶����
|	�߂�l	�{�^���̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_static_text_create(HWND hWnd, uint menuID, DWORD dwFlag, char* Format,...)
{
	if(hWnd	== NULL) return NULL;

	HWND	hwnd;
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
																// �c�[���`�b�v�Ŏg���ʒm
																//       ��			���ڂ�
	hwnd = CreateWindow(TEXT("STATIC"), buf, WS_CHILD | WS_VISIBLE | SS_NOTIFY /*| SS_SUNKEN*/ | dwFlag,
					0, 0, 0, 0, hWnd, (HMENU)menuID, sys.hInst, NULL);

	if(hFontDefault != NULL) CTRL_FONT_SET(hwnd, hFontDefault);	// �t�H���g�ݒ�
	return	hwnd;
}



/*------------------------------------------------------------------------------*
| <<< �X�^�e�B�b�N�e�L�X�g�̍쐬 >>>		���c�[���`�b�v��\������Ƃ��͏�̊֐����g�p�̂���
|	����	hWnd	 = �n���h��
|			x,y, w,h = �{�^���̈ʒu�ƃT�C�Y
|			dwFlag	 = �E�B���h�E�t���O
|			str		 = �\�����镶����
|	�߂�l	�{�^���̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_static_text_create(HWND hWnd, DWORD dwFlag, char* Format,...)
{
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	HWND	hwnd;
																// �c�[���`�b�v�Ŏg���ʒm
																//       ��			���ڂ�
	hwnd = CreateWindow(TEXT("STATIC"), buf, WS_CHILD | WS_VISIBLE | SS_NOTIFY /*| SS_SUNKEN*/ | dwFlag,
					0, 0, 10, 10, hWnd, (HMENU)1, sys.hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hwnd, hFontDefault);	// �t�H���g�ݒ�
	return	hwnd;
}



/*------------------------------------------------------------------------------*
| <<< �X�^�e�B�b�N�e�L�X�g�Ƀ��b�Z�[�W�𑗂� >>>
|	����	hWnd	= �E�B���h�D�n���h��
|			Format	= ������
*------------------------------------------------------------------------------*/
void	ctrl_static_text_printf(HWND hWnd, char* Format,...)
{
	if(hWnd	== NULL){ return;}
	char	buf[512];
	vsprintf(buf, Format, (char	*)(&Format + 1));
	if(strlen(buf) == 0){ memset(buf, ' ', sizeof(buf)),	buf[511] = NULL;}
	SetWindowText(hWnd, buf);
}



/*************************************************************************************************
	[[[ �X�^�e�B�b�N�s�N�`���[ ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �s�N�`���[�{�b�N�X�̍쐬 >>>
|	����	hWnd   = �}�U�[�ƂȂ�n���h��
|			menuID = ���j���[��ID
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
|	   �X�^�e�B�b�N�̃C�x���g�� STN_CLICKED �� STN_DBLCLK �����ʒm����Ȃ��̂Œ��ӁB
|								�}�E�X�N���b�N�ƃ_�u���N���b�N
*------------------------------------------------------------------------------*/
HWND	ctrl_static_picture_create(HWND hWnd, uint menuID)
{
	HWND		hOut;
	HINSTANCE	hInst;

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hOut = CreateWindow(
				TEXT("static"),	NULL,
				WS_CHILD | WS_VISIBLE |	WS_BORDER | SS_NOTIFY,
				0, 0, 0, 0,
				hWnd, (HMENU)menuID, hInst,	NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// �t�H���g�ݒ�
	return hOut;
}
/*�@�ȉ��̂悤�ȃC�x���g�ɂȂ�܂��B
	//ctrl_static_picture_create(hWnd, ID_STATIC);�ō쐬��
	case WM_COMMAND:
		if(LOWORD(wParam) == ID_STATIC)
		{
			switch(HIWORD(wParam))
			{
			case STN_CLICKED:
				// �N���b�N���ꂽ�Ƃ��̏���
				break ;

			case STN_DBLCLK:
				// �_�u���N���b�N���ꂽ�Ƃ��̏���
				break ;
			}
		}
*/



/*------------------------------------------------------------------------------*
| <<< �s�N�`���[�{�b�N�X�̍쐬 >>>
|	����	hWnd = �}�U�[�ƂȂ�n���h��
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_static_picture_create(HWND hWnd)
{
	return	ctrl_static_picture_create(hWnd, 1);
}



/*************************************************************************************************
	[[[ �R���{�{�b�N�X ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�쐬 >>>
|	����	hWnd = �}�U�[�ƂȂ�n���h��
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_combo_create(HWND hWnd, uint hMenu)
{

	if(hWnd == NULL) return NULL;

	HWND		hOut;
	HINSTANCE	hInst;
	
	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hOut  = CreateWindow(
				TEXT("COMBOBOX"), NULL, 
				WS_CHILD | WS_VISIBLE | CBS_SIMPLE | CBS_DROPDOWN,
				0, 0, 0, 0,
				hWnd, (HMENU)hMenu, hInst,	NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// �t�H���g�ݒ�
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�쐬 >>>
|	����	hWnd = �}�U�[�ƂȂ�n���h��
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_combo_create(HWND hWnd)
{
	if(hWnd == NULL) return NULL;
	return	ctrl_combo_create(hWnd, 1);
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�N���A�[ >>>
|	����	hWnd = �n���h��
*------------------------------------------------------------------------------*/
void	ctrl_combo_clear(HWND hWnd)
{
	if(hWnd == NULL) return;
	ComboBox_ResetContent(hWnd);
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�ɍ��ڂ�ǉ� >>>
|	����	hWnd   = �n���h��
|			Format = ������
*------------------------------------------------------------------------------*/
void	ctrl_combo_str_add(HWND	hWnd, char* Format,...)
{
	if(hWnd == NULL) return;
    char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	SendMessage(hWnd, CB_ADDSTRING,	0, (LPARAM)strBuf);
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�̎w��̍��ڂ̕����𓾂� >>>
|	����	hWnd = �n���h��
|			iNum = ���ڔԍ�
|	�o��	strOut : ������
*------------------------------------------------------------------------------*/
void	ctrl_combo_str_get(char *strOut, HWND hWnd, int iSel)
{
	if(hWnd == NULL) return;
	SendMessage(hWnd, CB_GETLBTEXT,	iSel, (LPARAM)strOut);
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�̉��Ԗڂ�\�����邩�w�� >>>
|	����	hWnd = �n���h��
|			iSel = ���ڔԍ�
*------------------------------------------------------------------------------*/
void	ctrl_combo_sel_set(HWND	hWnd, int iSel)
{
	if(hWnd == NULL) return;
	SendMessage(hWnd, CB_SETCURSEL,	iSel, 0);
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�̑I�𒆂̔ԍ��𓾂� >>>
|	����	hWnd = �n���h��
*------------------------------------------------------------------------------*/
int		ctrl_combo_sel_get(HWND	hWnd)
{
	return (int)SendMessage(hWnd, CB_GETCURSEL,	0L,	0L);
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�̍��ڂƕ�������r���� >>>
|	����	hWnd = �n���h��
*------------------------------------------------------------------------------*/
bool	ctrl_combo_strcmp(HWND hWnd, int iSel, char* Format,...)
{
    char	strBuf0[512], strBuf1[512];

	va_list	ap;
	va_start(ap,	  Format);
	vsprintf(strBuf0, Format, ap);
	va_end(ap);

	ctrl_combo_str_get(strBuf1, hWnd, iSel);
	if(strcmp(strBuf0, strBuf1) == 0) return true;
	return false;
}



/*------------------------------------------------------------------------------*
| <<< �R���{�{�b�N�X�̍��ڐ���Ԃ� >>>
|	����	hWnd = �n���h��
*------------------------------------------------------------------------------*/
int		ctrl_combo_max_get(HWND	hWnd)
{
	return (int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
}



/*************************************************************************************************
	[[[ �O���[�v�{�b�N�X ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �O���[�v�{�b�N�X�̍쐬 >>>
|	����	hWnd		  =	�E�B���h�D�n���h��
|			Format... 	  =	���b�Z�[�W
*------------------------------------------------------------------------------*/
HWND	ctrl_group_box_create(HWND hWnd, char* Format,...)
{
	HWND		hOut;
	HINSTANCE	hInst;
	char		buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hOut = CreateWindow(
				TEXT("BUTTON"),	TEXT(buf),
				WS_CHILD | WS_VISIBLE |	BS_GROUPBOX,
				0, 0, 0, 0,
				hWnd, (HMENU)1,	hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// �t�H���g�ݒ�
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< �O���[�v�{�b�N�X�̃e�L�X�g�ύX >>>
|	����	hWnd		  =	�E�B���h�D�n���h��
|			Format... 	  =	���b�Z�[�W
*------------------------------------------------------------------------------*/
void	ctrl_group_box_printf(HWND hWnd, char* Format,...)
{
	if(hWnd == NULL) return;

	char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	SetWindowText(hWnd, strBuf);
}



/*************************************************************************************************
	[[[ �c�[���`�b�v ]]]
**************************************************************************************************/
// �c�[���`�b�v�̊��m�̃o�O�Ƃ��Ă͈ȉ��̂悤�Ȃ��̂�����܂�
// �Z�J���h�f�B�X�v���C�Ńc�[���`�b�v���\�����ꂽ�u�ԂɁA
// DirectX �ʂ��^�����ɂȂ�B
// �v���C�}���[�ł͂��̂悤�Ȍ��ۂ͋N���Ȃ�(2009_10_15)�B

/*------------------------------------------------------------------------------*
| <<< �c�[���`�b�v�̍쐬 >>>
|	������Ń��T�C�Y���ɂ�����܂�
|	����	hMother  = ��̂ƂȂ�E�B���h�E
|			hWnd	 = �E�B���h�D�n���h��
|			str		 = �\�����镶����
*------------------------------------------------------------------------------*/
HWND	ctrl_tool_tips_create(HWND hMother, HWND hWnd, char* Format,...)
{

	HWND		hTool;
	TOOLINFO	ti;
	char		buf[256];


	vsprintf(buf, Format, (char	*)(&Format + 1));

	//--- �c�[���`�b�v����� ----------------------------------------
	ti.hwnd	 = hWnd;
	ti.hinst = sys.hInst;
	hTool	 = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								hMother, NULL, sys.hInst, NULL);

	GetClientRect(ti.hwnd, &ti.rect);
	ti.cbSize	= sizeof(TOOLINFO);
	ti.uFlags	= TTF_SUBCLASS;
	ti.uId		= 1;
	ti.lpszText	= TEXT(buf);
	SendMessage(hTool, TTM_ADDTOOL,	0, (LPARAM)&ti);
	//SendMessage(hTool, TB_SETTOOLTIPS, (WPARAM)&ti, 0); 

	if(hFontDefault != NULL){ CTRL_FONT_SET(hTool, hFontDefault);}	// �t�H���g�ݒ�
	
	return	hTool;
}



/*------------------------------------------------------------------------------*
| <<< �c�[���`�b�v�̍쐬 >>>
|	����	hMother   = ��̂ƂȂ�E�B���h�E
|			uiControl = �R���g���[��ID
|			str		  = �\�����镶����
*------------------------------------------------------------------------------*/
HWND	ctrl_tool_tips_create(HWND hMother, uint uiControl, char* Format,...)
{

	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	return	ctrl_tool_tips_create(hMother, GetDlgItem(hMother, uiControl), buf);
}



/*------------------------------------------------------------------------------*
| <<< �c�[���`�b�v�̕����ݒ� >>>
|	����	hWnd	 = �E�B���h�D�n���h��
|			str		 = �\�����镶����
*------------------------------------------------------------------------------*/
void	ctrl_tool_tips_update_text(HWND	*hWnd, HWND hTarget, HWND hMother, char* Format,...)
{

/*	�ȉ��A���܂����삵�܂���
	TOOLINFO	ti;
	char	buf[256];
	vsprintf(buf, Format, (char	*)(&Format + 1));
	ti.lpszText	= TEXT(buf);
	SendMessage(hWnd, TTM_UPDATETIPTEXT, 0,	(LPARAM)&ti);
	*/

	char	buf[256];
	vsprintf(buf, Format, (char	*)(&Format + 1));

	if(*hWnd){ DestroyWindow(*hWnd);}
	*hWnd = ctrl_tool_tips_create(hMother, hTarget, buf);
}



/*************************************************************************************************
	[[[ �^�u�R���g���[�� ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �^�u�R���g���[���ɍ��ڒǉ� >>>
|	����	hWnd = �E�B���h�D�n���h��
|			iID	 = ���ʔԍ�
|			str	 = �^�O�̍���
*------------------------------------------------------------------------------*/
void	ctrl_tab_add(HWND hWnd,	uint uiID, char* Format,...)
{
	TC_ITEM	tcItem;
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	tcItem.mask	   = TCIF_TEXT;
	tcItem.pszText = TEXT(buf);
	SendMessage(hWnd, TCM_INSERTITEM, (WPARAM)uiID, (LPARAM)&tcItem);
}



/*------------------------------------------------------------------------------*
| <<< �^�u�R���g���[���̍쐬 >>>
|	����	hWnd = �}�U�[�ƂȂ�n���h��
|			uiID = �Ǘ��ԍ�
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_tab_create(HWND hWnd, uint uiID)
{

	HWND	hTab;							// �n���h��

	hTab  =	CreateWindowEx(
			0,										// �g���X�^�C���Ȃ�
			WC_TABCONTROL,							// �N���X�l�[��
			NULL,									// �E�B���h�E�l�[��
			WS_CHILD | WS_VISIBLE |	WS_CLIPSIBLINGS,// �E�B���h�E�X�^�C��
			0, 0, 0, 0,								// �ʒu�A�傫���i������Ԃł͋ɒ[�ɏ��������Ă����j
			hWnd,									// �e�E�B���h�E�̃n���h��
			(HMENU)ID_TABCTRL,						// �q���E�B���h�E���ʎq
			sys.hInst,								// �C���X�^���X�n���h��
			NULL);									// WM_CREATE�̃p�����[�^�[�Ȃ�
	if(hFontDefault != NULL) CTRL_FONT_SET(hTab, hFontDefault);	// �t�H���g�ݒ�
	return	hTab;
}



/*------------------------------------------------------------------------------*
| <<< �^�u�R���g���[���̍쐬 >>>
|	����	hWnd = �}�U�[�ƂȂ�n���h��
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_tab_create(HWND hWnd)
{
	return ctrl_tab_create(hWnd, ID_TABCTRL);
}



/*------------------------------------------------------------------------------*
| <<< �^�u�Ɋi�[�����_�C�A���O�̃T�C�Y�ύX >>>
|	����	hMother     = �_�C�A���O�̃n���h��
|			iXOfs iYOfs = �I�t�Z�b�g�ʒu
*------------------------------------------------------------------------------*/
void	ctrl_tab_dailog_resize(HWND hMother, int iXOfs, int iYOfs)
{

	RECT	r;
	GetClientRect(sys.hWnd, &r);			// �E�B���h�E�̃T�C�Y�𓾂�

	/*int	w = 4, h0 = 22, h1 = 10;
	CTRL_RESIZE(hMother, r.right + iXOfs - (__SCR_W - __RENDER_X1) + SYS_XFRAME_GET() / 2 + w  - 2, 
						 r.top   + iYOfs + h0,
						 r.right  -  w + 1, 
						 r.bottom - h1 + 5);
	*/
	int	w = 3;
	int	b = CTRL_TAB_HEIGHT;				// �^�u�̍��ڂ̍���(�ǂ�����ĎZ�o���邩�s���Ȃ̂Œ��ŏ����Ă܂�)
	CTRL_RESIZE(hMother, r.right + iXOfs - (__SCR_W - __RENDER_X1) + SYS_XFRAME_GET() / 2 + w, 
						 r.top   + iYOfs						   + b,
						 r.right  - w * 2 + 1, 
						 r.bottom - 4);
}



/*------------------------------------------------------------------------------*
| <<< �^�u�őI������Ă��鍀�ڔԍ���Ԃ� >>>
|	����	hMother = �_�C�A���O�̃n���h��
|	�߂�l	���ڔԍ�
*------------------------------------------------------------------------------*/
int		ctrl_tab_get(HWND hMother)
{
	return	TabCtrl_GetCurSel(hMother);		// �I���^�u�̔ԍ��𓾂�
}



/*------------------------------------------------------------------------------*
| <<< �^�u�̍��ڂ�I������ >>>
|	����	hMother = �_�C�A���O�̃n���h��
|			iNum    = ���ڔԍ�
*------------------------------------------------------------------------------*/
void	ctrl_tab_set(HWND hMother, int iNum)
{
	TabCtrl_SetCurSel(hMother, iNum);
}



/*************************************************************************************************
	[[[ �X���C�_�[ ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �X���C�_�[�̍쐬 >>>
|	����	hWnd   = �}�U�[�ƂȂ�n���h��
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_slider_create(HWND hWnd, DWORD dwFlag)
{
	HWND		hOut;
	HINSTANCE	hInst;

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	hOut = CreateWindowEx( 
				0,															// no extended styles
				TRACKBAR_CLASS,												// class name
				"Trackbar Control",											// title (caption)
				WS_CHILD | WS_VISIBLE | /*TBS_AUTOTICKS |*/ TBS_ENABLESELRANGE,	// style
										// ���������������ŕ\������X�C�b�`
				0, 0, 0, 0,													// position & size
				hWnd,														// parent window
				(HMENU)ID_SLIDER,											// control identifier
				hInst,														// instance
				NULL);														// no WM_CREATE parameter
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// �t�H���g�ݒ�
	return hOut;
};


HWND	ctrl_slider_create(HWND hWnd)
{
	return ctrl_slider_create(hWnd, 0);
}



/*------------------------------------------------------------------------------*
| <<< �X���C�_�[�̃����W�ݒ� >>>
|	����	hWnd = �X���C�_�[�̃n���h��
*------------------------------------------------------------------------------*/
void	ctrl_slider_range_set(HWND hWnd, int iMin, int iMax)
{
	SendMessage(hWnd, TBM_SETRANGE,	0, MAKELONG(iMin, iMax));
}



/*------------------------------------------------------------------------------*
| <<< �X���C�_�[�̒l��ݒ� >>>
|	����	hWnd = �X���C�_�[�̃n���h��
*------------------------------------------------------------------------------*/
void	ctrl_slider_set(HWND hWnd, int iPos)
{
	SendMessage(hWnd, TBM_SETPOS, TRUE, iPos);
}


/*------------------------------------------------------------------------------*
| <<< �X���C�_�[�̒l�𓾂� >>>
|	����	hWnd = �X���C�_�[�̃n���h��
|	�߂�l	�X���C�_�[�̒l
*------------------------------------------------------------------------------*/
int		ctrl_slider_get(HWND hWnd)
{
	return SendMessage(hWnd, TBM_GETPOS, 0, 0);
}



/*************************************************************************************************
	[[[ ���X�g�r���[ ]]]
**************************************************************************************************/



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�̕�����`�� >>>
|	���t�H�[�J�X�����ֈڂ�ƁA�I�����ڂ�������Ȃ��Ȃ�̂ŁA�������Ă��܂�
*------------------------------------------------------------------------------*/
static	void	ListViewTextDraw(HDC hdc, char *Text, RECT *rc,	int	fmt)
{
	
	DRAWTEXTPARAMS	para;
	int				Justify;
	
	// �ׂ荇�������񂪂������Ȃ��悤�ɍ��E�Ƀ}�[�W��������B
	// �������A�E�}�[�W���͕����̔z�u���E�񂹂ɂ����Ƃ������t����B
	para.cbSize		   = sizeof(para);
	para.iLeftMargin   = 2;
	para.iRightMargin  = 0;
	para.iTabLength	   = 0;
	para.uiLengthDrawn = 0;

	// �T�u�A�C�e���̕����̔z�u�𒲂ׂāADrawTextEx�p�̃X�^�C���ɒ����B
	if(		(fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT )
	{
		Justify	= DT_LEFT;
	}
	else if((fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
	{
		Justify			  =	DT_RIGHT;
		para.iRightMargin =	6;
	}
	else
	{
		Justify	= DT_CENTER;
	}

	// ������̕\���@���E�}�[�W��������̂�DrawTextEx���g���Ă���B
	DrawTextEx(hdc,	Text, lstrlen(Text), rc, Justify | DT_VCENTER |	DT_SINGLELINE |	DT_END_ELLIPSIS, &para);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�A�C�e����`�悷�� >>>
*------------------------------------------------------------------------------*/
void	ctrl_list_view_draw(LPDRAWITEMSTRUCT lpDraw)
{

	HWND		hList =	lpDraw->hwndItem;	// ���X�g�r���[�̃n���h��
	HDC			hDC	  =	lpDraw->hDC;		// �f�o�C�X�R���e�L�X�g
	RECT		rc,	rcAll, rcClient;
	HBRUSH		hBrush;						// �w�i�`��p�̃u���V�n���h��
	COLORREF	Color;
	char		Text[256];
	int			SubItem;
	LVCOLUMN	LvColumn;					// �񍀖ڎ擾�p�̍\����
	LVITEM		LvItem;						// �A�C�e�����擾�p�̍\����
	HIMAGELIST	hImage;						// �C���[�W���X�g�̃n���h��
	int			SubItemNum;
	
	
	// �f�o�C�X�R���e�L�X�g��ۑ�
	SaveDC(hDC);
	
	// �����̔w�i�̕\�����[�h��񓧉߂ɐݒ肷��B
	SetBkMode(hDC, OPAQUE);

	// ODS_SELECTED�t���O������Ƃ��ɂ́A�����\������B
	if(lpDraw->itemState & ODS_SELECTED)
	{
		// �w�i�F�̎擾
		hBrush = CreateSolidBrush (GetSysColor(COLOR_HIGHLIGHT));
		SetBkColor(	 hDC, GetSysColor(COLOR_HIGHLIGHT));
		SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
	{
		Color =	ListView_GetTextBkColor(hList);
		
		// ListView_SetTextBkColor����x������ĂȂ��Ƃ��ɂ́Aff000000�Ƃ����F�ɂȂ�̂�
		// ���̎��ɂ́A�V�X�e���̐F���g���B��������Ȃ��ƍ��ɂȂ��Ă��܂��B
		if(Color & 0xff000000) Color = GetSysColor(COLOR_WINDOW);
		hBrush = CreateSolidBrush(Color);
	}

	// �񌩏o���̃t�H�[�}�b�g�����擾����B
	ZeroMemory(&LvColumn, sizeof(LvColumn));
	LvColumn.mask =	LVCF_FMT;
	ListView_GetColumn(hList, 0, &LvColumn);

	// �w�i�𖄂߂�
	ListView_GetItemRect(hList,	lpDraw->itemID,	&rcAll,	LVIR_BOUNDS);
	ListView_GetItemRect(hList,	lpDraw->itemID,	&rc,	LVIR_LABEL);
	rcAll.left = rc.left;

	GetClientRect(hList, &rcClient);

	// �A�C�e���̏����擾���܂��B�\������C���[�W�̃C���f�b�N�X������̂�ListView_GetItem���g���B
	ZeroMemory(&LvItem,	sizeof(LvItem));
	LvItem.iItem      = lpDraw->itemID;
	LvItem.iSubItem   = 0;
	LvItem.mask       = LVIF_TEXT |	LVIF_IMAGE;
	LvItem.pszText    = Text;
	LvItem.cchTextMax = sizeof(Text);
	ListView_GetItem(hList, &LvItem);

	// �C���[�W���X�g�̃n���h�����擾����
	hImage = ListView_GetImageList(hList, LVSIL_SMALL);

	// �C���[�W���X�g���o�^����Ă���Ƃ������A�C���[�W��\������B
	// �C���[�W�ԍ��́AiImage�����o�ɂ���B
	if(hImage)
	{
		ListView_GetItemRect(hList, lpDraw->itemID, &rc, LVIR_ICON);
																	// �C���[�W�������\�����邩�ǂ�����ݒ肷��B
		ImageList_Draw(hImage, LvItem.iImage, hDC, rc.left, rc.top, ((lpDraw->itemState & ODS_SELECTED) ? ILD_FOCUS : ILD_NORMAL) | ILD_TRANSPARENT);
	}

	// �������\������BListView_GetItemRect���g���ƕ\������̈悪������B
	ListView_GetItemRect(hList, lpDraw->itemID, &rc, LVIR_LABEL);
	ListViewTextDraw(hDC, Text, &rc, LvColumn.fmt);

	// �T�u�A�C�e���̐����擾����
	SubItemNum = Header_GetItemCount(ListView_GetHeader(hList)) - 1;

	// �T�u�A�C�e�������l�ɕ\������B
	for(SubItem = 1; SubItem <= SubItemNum; SubItem++)
	{
		// �w�i�̃N���A
		ListView_GetSubItemRect(hList, lpDraw->itemID, SubItem, LVIR_BOUNDS, &rc);
		FillRect(hDC, &rc, hBrush);

		// �񍀖ڂ̃t�H�[�}�b�g�𓾂܂��B�\���̂͂������ݒ肵���̂ł��̂܂܎g���B
		ListView_GetColumn(     hList, SubItem, &LvColumn);
		ListView_GetSubItemRect(hList, lpDraw->itemID, SubItem, LVIR_LABEL, &rc);

		// �A�C�e���̏����擾���܂��B�����񂾂��Ȃ̂ō��x��ListView_GetItemText���g���܂��B
		ListView_GetItemText(hList, lpDraw->itemID, SubItem, Text, sizeof(Text));
		ListViewTextDraw(hDC, Text,	&rc, LvColumn.fmt);
	}

	if(lpDraw->itemState & ODS_FOCUS)
	{
		//DrawFocusRect(hDC, &rcAll);				// �����b��I�ɃR�����g�ɂ��Ă��܂�
	}
	
	// ������u���V�̍폜
	DeleteObject(hBrush);

	// �f�o�C�X�R���e�L�X�g�̕���
	RestoreDC(hDC, -1);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�쐬 >>>
|	����	hWnd = ����̃n���h��
|			bSw0 = true	: �����I����
|			bSw1 = true	: �`�F�b�N�{�^���L
|	�߂�l	�쐬�������X�g�r���[�̃n���h��
|	�����X�g�r���[���g���ꍇ�́A�e�E�B���h�E�̃C�x���g�� 
|			case WM_DRAWITEM:
|				ctrl_list_view_draw((LPDRAWITEMSTRUCT)lParam);
|	  �Ƃ��Ă��������B
*------------------------------------------------------------------------------*/
HWND	ctrl_list_view_create(HWND hWnd, bool bSw0,	bool bSw1)
{

	HWND		hList;
	DWORD		dwStyle;
	HINSTANCE	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);


	if(bSw0) dwStyle = WS_CHILD	| WS_VISIBLE | LVS_REPORT;					// �����I����
	else	 dwStyle = WS_CHILD	| WS_VISIBLE | LVS_REPORT |	LVS_SINGLESEL;	// �P���I����

	hList =	CreateWindowEx(
					0,
					WC_LISTVIEW, "", 
					WS_BORDER | dwStyle | LVS_OWNERDRAWFIXED,
					0, 0, 0, 0,	hWnd, (HMENU)ID_LISTVIEW, sys.hInst, NULL);
	dwStyle	 = ListView_GetExtendedListViewStyle(hList);

	if(bSw1) dwStyle |=	LVS_EX_CHECKBOXES |	LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES	| LVS_EX_HEADERDRAGDROP;
	else	 dwStyle |=						LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES	| LVS_EX_HEADERDRAGDROP ;

	ListView_SetExtendedListViewStyle(hList, dwStyle);

	if(hFontDefault != NULL) CTRL_FONT_SET(hList, hFontDefault);	// �t�H���g�ݒ�

	return hList;
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�T�C�Y�ύX >>>
|	����	hMother = ��̂ƂȂ�_�C�A���O�̃n���h��
|			hList   = ���X�g�r���[�̃n���h��
*------------------------------------------------------------------------------*/
void	ctrl_list_view_resize(HWND hMother, HWND hList)
{
	RECT	r0, r1;
	GetWindowRect(hMother, &r0);
	GetWindowRect(hList,   &r1);
	const	int		iOfs = - 8;
	MoveWindow(   hList,   r1.left - r0.left, r1.top - r0.top, r1.right - r1.left, r0.bottom - r0.top - (r1.top - r0.top) + iOfs, TRUE);
}



/*------------------------------------------------------------------------------*
| <<< �R������ǉ����� >>>
|	����	hWnd	  =	�E�B���h�D�n���h��
|			iInsPoint =	�ǉ�����ʒu
|			str		  =	�ǉ������̕�����
|			iSize	  =	�R�����̃T�C�Y
*------------------------------------------------------------------------------*/
void	ctrl_list_view_column_add(HWND hWnd, int iInsPoint,	char *str, int iSize)
{
	LV_COLUMN	lvcol;
	lvcol.mask	   = LVCF_FMT |	LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt	   = LVCFMT_LEFT;
	lvcol.cx	   = iSize;
	lvcol.pszText  = str;
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hWnd,	iInsPoint, &lvcol);
}



/*------------------------------------------------------------------------------*
| <<< �R�����̕����������� >>>
|	����	hWnd = �E�B���h�D�n���h��
|			iNum = �R�����ԍ�
*------------------------------------------------------------------------------*/
void	ctrl_list_view_column_resize(HWND hWnd,	int	iNum)
{
	ListView_SetColumnWidth(hWnd, iNum,	LVSCW_AUTOSIZE_USEHEADER);
}



/*------------------------------------------------------------------------------*
| <<< �R�����̕��𓾂� >>>
|	����	hWnd = �E�B���h�D�n���h��
|			iNum = �R�����ԍ�
*------------------------------------------------------------------------------*/
int	ctrl_list_view_column_size_get(HWND hWnd, int iNum)
{
	return ListView_GetColumnWidth(hWnd, iNum);
}



/*------------------------------------------------------------------------------*
| <<< �A�C�e����ǉ� >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			iSub   = �T�u�A�C�e���ԍ�( 0 �ō��ڂ�ǉ����Ă����܂�)
|			iItem  = �A�C�e���ԍ�
|			Format = �ǉ������̕�����
*------------------------------------------------------------------------------*/
void	ctrl_list_view_add(HWND	hWnd, int iSub, int iItem, char* Format,...)
{

	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	if(iSub	== 0)
	{
		LVITEM	item;
		//item.mask	  =	LVIF_TEXT;
		item.mask	  =	LVIF_TEXT |	LVIF_PARAM;
 		item.iItem	  =	iItem;
		item.iSubItem =	iSub;
		item.pszText  =	buf;
		item.lParam	  =	0;
		ListView_InsertItem(hWnd, &item);
	}
	else
	{
		ListView_SetItemText(hWnd, iItem, iSub,	buf);
	}
}



/*------------------------------------------------------------------------------*
| <<< �w��̍��ڂ��폜 >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			iSub   = �T�u�A�C�e���ԍ�( 0 �ō��ڂ�ǉ����Ă����܂�)
|			iItem  = �A�C�e���ԍ�
|	�߂�l	true : �J������
*------------------------------------------------------------------------------*/
bool	ctrl_list_view_dec(HWND hWnd, int iItem)
{
	BOOL	bRet = ListView_DeleteItem(hWnd, iItem);
	if(bRet) return true;
	else	 return false;
}



/*------------------------------------------------------------------------------*
| <<< ���X�g���當���𓾂� >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			iSub   = �T�u�A�C�e���ԍ�
|			iItem  = �A�C�e���ԍ�
|			str	   = �ǉ������̕�����
|			iSize  = ������̃T�C�Y
|	�o��	strOut = 
*------------------------------------------------------------------------------*/
void	ctrl_list_view_str_get(char *strOut, HWND hWnd, int iSub, int iItem, int iSize)
{
	ListView_GetItemText(hWnd, iItem, iSub,	strOut,	iSize);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�ɕ������Z�b�g >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			iSub   = �T�u�A�C�e���ԍ�( 0 �ō��ڂ�ǉ����Ă����܂�)
|			iItem  = �A�C�e���ԍ�
|			Format = �ǉ������̕�����
*------------------------------------------------------------------------------*/
void	ctrl_list_view_str_set(HWND	hWnd, int iSub, int iItem, char* Format,...)
{

	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	ListView_SetItemText(hWnd, iItem, iSub,	buf);
}



/*------------------------------------------------------------------------------*
| <<< �w��̍��ڂ����� >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			a, b   = a �� b ������
*------------------------------------------------------------------------------*/
void	ctrl_list_view_swap(HWND hWnd, int a, int b)
{

	char	str0[256], str1[256];

	for(int i = 0; i < 64; i++)
	{
		str0[0] = NULL;
		str1[0] = NULL;
		ctrl_list_view_str_get(str0, hWnd, i, a, sizeof(str0));
		if(str0[0] == NULL) break;
		ctrl_list_view_str_get(str1, hWnd, i, b, sizeof(str1));
		if(str1[0] == NULL) break;
		ctrl_list_view_str_set(      hWnd, i, a, str1);
		ctrl_list_view_str_set(      hWnd, i, b, str0);
	}
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�I�����A�`�F�b�N�̕����������ꂽ�����ׂ� >>>
|	����	hWnd   = �n���h��
|	�߂�l	true : �`�F�b�N������������
*------------------------------------------------------------------------------*/
bool	ctrl_list_view_check_click_check(HWND hWnd)
{
	LVHITTESTINFO	lvHitInfo;

	GetCursorPos(&lvHitInfo.pt);
	ScreenToClient(hWnd, &lvHitInfo.pt);

	if(lvHitInfo.pt.x < 18)	return true;	// �`�F�b�N�̕�����I������
	return false;
}



/*------------------------------------------------------------------------------*
| <<< �A�C�e���̃`�F�b�N�X�C�b�`�I���E�I�t >>>
|	����	hWnd  =	�E�B���h�D�n���h��
|			iItem =	�A�C�e���ԍ�
|			bSw	  =	�`�F�b�N�X�C�b�`�̏��
*------------------------------------------------------------------------------*/
#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif
void	ctrl_list_view_check_set(HWND hWnd,	int	iItem, bool	bSw)
{
//	ListView_SetItemState(hWnd,	iItem, INDEXTOOVERLAYMASK((int)bSw), LVIS_OVERLAYMASK);
	ListView_SetCheckState(hWnd, iItem, (int)bSw);
}



/*------------------------------------------------------------------------------*
| <<< �A�C�e���̃`�F�b�N�X�C�b�`�̏�Ԃ𓾂� >>>
|	����	hWnd  =	�E�B���h�D�n���h��
|			iItem =	�A�C�e���ԍ�
|	�߂�l	TRUE  :	�I��
|			FALSE :	�I�t
*------------------------------------------------------------------------------*/
bool	ctrl_list_view_check_get(HWND hWnd,	int	iItem)
{
	return (ListView_GetCheckState(hWnd, iItem)	== TRUE);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�̓��e��S���� >>>
|	����	hWnd = �E�B���h�D�n���h��
*------------------------------------------------------------------------------*/
void	ctrl_list_view_reset(HWND hWnd)
{
	ListView_DeleteAllItems(hWnd);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�̃A�C�e�����𒲂ׂ� >>>
|	����	hWnd = �E�B���h�D�n���h��
|	�߂�l	�S�s�����Ԃ�
*------------------------------------------------------------------------------*/
int		ctrl_list_view_total_count_get(HWND	hWnd)
{
	return ListView_GetItemCount(hWnd);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�̃t�H�[�J�X��Ԃ� >>>			���I����Ԃɂ���
|	����	hWnd = �E�B���h�D�n���h��
|	�߂�l	-1	 = �����I������Ă��Ȃ�
|			-1	!= �I��ԍ�
*------------------------------------------------------------------------------*/
int		ctrl_list_view_focus_get(HWND hWnd)
{
	for(int	i =	0; i < ctrl_list_view_total_count_get(hWnd); i++)
	{
		UINT	uiState	= ListView_GetItemState(hWnd, i, LVIS_FOCUSED);
		//UINT	uiState	= ListView_GetItemState(hWnd, i, LVIS_SELECTED | LVIS_FOCUSED);
		/*if(uiState & LVIS_SELECTED)
		{
			// �I������Ă���
		}
		else*/
		if(uiState & LVIS_FOCUSED) return i;
	}
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�̃t�H�[�J�X���Z�b�g���� >>>	���I������Ă���ԍ���Ԃ�
|	����	hWnd = �E�B���h�D�n���h��
|			iSel = �t�H�[�J�X�����郊�X�g�ԍ�
*------------------------------------------------------------------------------*/
void	ctrl_list_view_focus_set(HWND hWnd, int iSel)
{
	//ListView_SetItemState(hWnd,	-1,	  0,				LVIS_DROPHILITED | LVIS_FOCUSED	| LVIS_SELECTED	| LVIS_CUT);
	//ListView_SetItemState(hWnd,	iSel, LVIS_DROPHILITED,	LVIS_DROPHILITED);
	//SetFocus(hWnd);

	ListView_SetItemState(hWnd, iSel, LVIS_SELECTED, LVIS_SELECTED);
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�̃t�H�[�J�X���������� >>>
|	����	hWnd = �E�B���h�D�n���h��
|			iSel = �t�H�[�J�X���������郊�X�g�ԍ�
*------------------------------------------------------------------------------*/
void	ctrl_list_view_focus_release(HWND hWnd, int iSel)
{
	//ListView_SetItemState(hWnd,	-1,	0,  LVIS_FOCUSED);	// -1�Ԗڂ�I����Ԃɂ���Ƃ������@�ŉ��
	ListView_SetItemState(hWnd,	iSel,	0,  LVIS_SELECTED);	// -1�Ԗڂ�I����Ԃɂ���Ƃ������@�ŉ��
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�̃}�E�X�őI�������ԍ��𓾂� >>>			���h���b�O�����Ƃ��ȂǂɎg��
|	����	hWnd = �E�B���h�D�n���h��
|			bSw  = true : �����ɂȂɂ��Z�b�g����ĂȂ��Ă��I��ԍ���Ԃ�
|	�߂�l	-1	 = �����I������Ă��Ȃ�
|			-1	!= �I��ԍ�
*------------------------------------------------------------------------------*/
int		ctlr_list_view_sel_get(HWND hListView, bool bSw)
{
	LVHITTESTINFO	lvHitInfo;

	GetCursorPos(					   &lvHitInfo.pt);
	ScreenToClient(         hListView, &lvHitInfo.pt);
	ListView_SubItemHitTest(hListView, &lvHitInfo);

	char	str[MAX_PATH];
	ctrl_list_view_str_get(str, hListView, 0, (int)lvHitInfo.iItem, MAX_PATH);

	//--- ��Βl�𓾂� ----------------------------------------------
	if(bSw) return (lvHitInfo.pt.y - 16) / 14;

	//--- ���ʂɒl��Ԃ� --------------------------------------------
	if(str[0] == NULL) return -1;
	else			   return lvHitInfo.iItem;
}

int		ctlr_list_view_sel_get(HWND hListView)
{
	return ctlr_list_view_sel_get(hListView, false);
}



/*------------------------------------------------------------------------------*
| <<< �A�C�e���������I������Ă���ꍇ�A���ɑI������Ă���ԍ���Ԃ� >>>
|	����	hWnd  = �E�B���h�D�n���h��
|			iItem = �A�C�e���ԍ�
*------------------------------------------------------------------------------*/
int		ctrl_list_view_next_select_get(HWND hWnd, int iItem)
{

	return	ListView_GetNextItem(hWnd, iItem, LVNI_SELECTED);

	//�����̂悤�Ɏg������
	//int	iItem = -1;
	//while((iItem = ctrl_list_next_select_get(Ctrl.pWork[0]->hListView[0], iItem)) != -1)
	//{
	//		�I������Ă��鍀�ڂ��ׂĂ�������ʂ�
	//}
}




/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�h���b�O�J�n(�h���b�O�C���[�W�쐬) >>>
|	����	hList      = ���X�g�r���[�̃n���h��
|			hMother    = �}�U�[�ƂȂ�E�B���h�E�̃n���h��
|			pNM      �@= ���X�g�r���[�̕\���ʒm���b�Z�[�W
|	�o��	phimageOut = �h���b�O�C���[�W�̃n���h��
|	�߂�l	�I���������ڔԍ�
*------------------------------------------------------------------------------*/
int		ctrl_list_view_drag_begin(HIMAGELIST *phimageOut, HWND hList, HWND hMother, NMLISTVIEW *pNM)
{

	// �h���b�O�C���[�W�擾
	POINT	pointPos;
	*phimageOut = ListView_CreateDragImage(hList, pNM->iItem, &pointPos);

	// ���X�g�r���[���h���b�O�ʒu���v�Z���āA�J�n
	POINT	pointCursor;
	GetCursorPos(  &pointCursor);
	ScreenToClient(hMother, &pointCursor);

	int	iX = pointCursor.x - pointPos.x;
	int	iY = pointCursor.y - pointPos.y;
	ImageList_BeginDrag(*phimageOut, 0, iX, iY);
	ImageList_DragEnter(hMother,    0, 0);

	// �}�E�X�L���v�`��
	SetCapture(hMother);

	return pNM->iItem;
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�h���b�O��(�h���b�O�C���[�W�̈ړ�) >>>
|	����	hList   = ���X�g�r���[�̃n���h��
|			hMother = �}�U�[�ƂȂ�E�B���h�E�̃n���h��
|			iX, iY  = �}�E�X���W
*------------------------------------------------------------------------------*/
void	ctrl_list_view_drag(HWND hList, HWND hMother, int iX, int iY)
{

	POINT	pointPos = {iX, iY};
	ClientToScreen(hMother, &pointPos);

	RECT	r0, r1;
	GetWindowRect(hMother, &r0);
	GetWindowRect(hList,   &r1);
	ImageList_DragMove(pointPos.x - r0.left + (r1.left - r0.left), pointPos.y - r0.top + (r1.top - r0.top));
}



/*------------------------------------------------------------------------------*
| <<< ���X�g�r���[�h���b�O�I��(�h���b�O�C���[�W�̉��) >>>
|	����	hMother    = �}�U�[�ƂȂ�E�B���h�E�̃n���h��
|			phimageOut = �h���b�O�C���[�W�̃n���h��
*------------------------------------------------------------------------------*/
void	ctrl_list_view_drag_end(HWND hMother, HIMAGELIST *phimageListView)
{

	ImageList_DragLeave(hMother);
	ImageList_EndDrag();

	// �h���b�O�C���[�W�̉��
	ImageList_Destroy(*phimageListView);
	*phimageListView = NULL;

	// �}�E�X�L���v�`������
	ReleaseCapture();
}



/*------------------------------------------------------------------------------*
| <<< �}�E�X���w���Ă���A���X�g�r���[�̔ԍ��𓾂� >>>
|	����	hList   = ���X�g�r���[�̃n���h��
|	�߂�l	-1 == �����ȂƂ���
|			-1 != ���ڔԍ�
*------------------------------------------------------------------------------*/
int		ctrl_list_view_pos_get(HWND hList)
{
	// �J�[�\���ʒu���擾
	POINT	pointCursor;
	GetCursorPos(&pointCursor);

	// �N���b�N���ꂽ�ʒu�̃A�C�e���̃C���f�b�N�X���擾
	LVHITTESTINFO	lvHit;
	lvHit.pt.x = pointCursor.x;
	lvHit.pt.y = pointCursor.y;
	ScreenToClient(hList, &lvHit.pt);

	return ListView_HitTest(hList, &lvHit);
}



/*************************************************************************************************
	[[[ �`�F�b�N�{�b�N�X ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �`�F�b�N�{�b�N�X�̍쐬 >>>
|	����	hWnd   = �}�U�[�ƂȂ�n���h��
|	�� �傫����	CTRL_RESIZE	�Őݒ�̂���
*------------------------------------------------------------------------------*/
HWND	ctrl_check_box_create(HWND hWnd, char* Format,...)
{
	if(hWnd == NULL) return NULL;

	HWND		hOut;
	HINSTANCE	hInst;
	char		buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	hOut = CreateWindow(
				TEXT("BUTTON") , TEXT(buf),
				WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,// | BS_LEFTTEXT,
				0, 0, 0, 0,
				hWnd, (HMENU)1, hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// �t�H���g�ݒ�
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< �`�F�b�N�{�b�N�X�ɒl��ݒ� >>>
|	����	hWnd = �R���g���[���̃n���h��
|			bSw  = true or false;
*------------------------------------------------------------------------------*/
void	ctrl_check_box_set(HWND hWnd, bool bSw)
{
	if(hWnd == NULL) return;
	if(bSw) SendMessage(hWnd, BM_SETCHECK, BST_CHECKED,   0);
	else    SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
}



/*------------------------------------------------------------------------------*
| <<< �`�F�b�N�{�b�N�X�̏�Ԃ𓾂� >>>
|	����	hWnd = �R���g���[���̃n���h��
*------------------------------------------------------------------------------*/
bool	ctrl_check_box_get(HWND hWnd)
{
	if(hWnd == NULL) return false;
	return	(SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED);
}



/*************************************************************************************************
	[[[ �t�H���_�[�I�� ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< SHBrowseForFolder()�p�R�[���o�b�N	>>>
*------------------------------------------------------------------------------*/
static	int	CALLBACK SHBrowseProc(HWND hWnd, UINT uMsg,	LPARAM lParam, LPARAM lpData)
{
	if(uMsg	== BFFM_INITIALIZED	&& lpData)
	{										// �f�t�H���g�őI��������p�X�̎w��
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< �V�X�e�����m�ۂ���ITEMIDLIST���J�����Ȃ���΂Ȃ�Ȃ� >>>
*------------------------------------------------------------------------------*/
static	void	_SHFree(ITEMIDLIST *pidl)
{
	IMalloc	*pMalloc;
	SHGetMalloc(&pMalloc);
	if(pMalloc)
	{
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
}



/*------------------------------------------------------------------------------*
| <<< �t�H���_�I���_�C�A���O�̋N�� >>>
|	����	hWnd			  =	�E�B���h�E�n���h��
|			lpszDefaultFolder =	�f�t�H���g�t�H���_�[
|			lpszBuffer		  =	�I�������t�H���_�[��
|			strMessage		  =	���b�Z�[�W
*------------------------------------------------------------------------------*/
static	UINT	GetOpenFolderName(HWND hWnd, LPCTSTR lpszDefaultFolder,	LPTSTR lpszBuffer, char	*strMessage)
{
	BROWSEINFO	bi;
	ITEMIDLIST	*pidl;
	char		szSelectedFolder[MAX_PATH];
											// �t�b�N���|����
//	hookFolderHandle = SetWindowsHookEx(WH_CBT, ProcFolderHook, NULL, GetCurrentThreadId()); 
//	return NULL;//  MessageBox(sys.hWnd, str1, str2, uiType);

	ZeroMemory(&bi,	sizeof(BROWSEINFO));
	bi.hwndOwner = hWnd;
	bi.lpfn		 = SHBrowseProc;				// �R�[���o�b�N�֐����w��
	bi.lParam	 = (LPARAM)lpszDefaultFolder;	// �f�t�H���g�őI�������Ă����t�H���_���w��
	bi.lpszTitle = strMessage;					// �^�C�g���̎w��
	pidl		 = SHBrowseForFolder(&bi);		// �t�H���_�_�C�A���O�̋N��
	if(pidl)
	{											// �I�����ꂽ�t�H���_�����擾
		SHGetPathFromIDList(pidl, szSelectedFolder);
		_SHFree(pidl);
		if((DWORD)lstrlen(szSelectedFolder)	< MAX_PATH)	lstrcpy(lpszBuffer,	szSelectedFolder);
		return IDOK;							// �t�H���_���I�����ꂽ
	}
	return IDCANCEL;							// �t�H���_�͑I������Ȃ�����
}



/*------------------------------------------------------------------------------*
| <<< �t�H���_�[�I�� >>>
|	����	hWnd			 = �E�B���h�E�n���h��
|			strDefaultFolder = �f�t�H���g�t�H���_�[��
|			strMessage		 = ���b�Z�[�W
|	�o��	sys.strSelFile	= �t�H���_�[��
|	�߂�l	true			 = �t�H���_�[�I������
*------------------------------------------------------------------------------*/
bool	ctrl_folder_select(HWND	hWnd, char *strDefaultFolder, char *strMessage)
{
	char	dir[MAX_PATH];
	char	str[500];

	//--- �f�t�H���g�t�H���_�[�𒲂ׂ� ----------------------------
	if(strlen(strDefaultFolder)	== 0)		// �w�肪�����ꍇ��	c:\	��
	{
		sprintf(dir, "c:\\");
	}
	else									// �t�@�C�����͐؂藣��
	{
		sys_div_path(dir, NULL,	NULL, strDefaultFolder);
	}
	//--- ���b�Z�[�W�ݒ� ------------------------------------------
	if(strMessage == NULL)
	{
		strcpy(str,	"�t�H���_��I�����Ă�������");
	}
	else
	{
		strcpy(str,	strMessage);
	}
	
	if(GetOpenFolderName(hWnd, dir,	sys.strSelFile,	str) ==	IDOK)
	{
		//MessageBox(hWnd, szFolder, "�I�����ꂽ�t�H���_", MB_OK);
		return true;
	}
	return false;
}



/*------------------------------------------------------------------------------*
| <<< �t�H���_�[�I�� >>>
|	����	hWnd			 = �E�B���h�E�n���h��
|			strDefaultFolder = �f�t�H���g�t�H���_�[��
|	�o��	sys.strSelFile	 = �t�H���_�[��
|	�߂�l	true			 = �t�H���_�[�I������
*------------------------------------------------------------------------------*/
bool	ctrl_folder_select(HWND	hWnd, char *strDefaultFolder)
{
	if(strDefaultFolder == NULL) return ctrl_folder_select(hWnd, sys.strExeFolder, NULL); 
	else						 return ctrl_folder_select(hWnd, strDefaultFolder, NULL);
}



/*************************************************************************************************
	[[[ �X�N���[�� ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �X�N���[���o�[�̍쐬 >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			dwFlag = �E�B���h�E�t���O		SBS_VERT(�c) or SBS_HORZ(��)
|			sc     = �X�N���[���o�[���
|	�߂�l	�X�N���[���o�[�̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_scrollbar_create(HWND hWnd, DWORD dwFlag, SCROLLINFO *sc)
{

	HINSTANCE	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	HWND	h =  CreateWindow(TEXT("SCROLLBAR"), TEXT(""), WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 0, 0, hWnd, (HMENU)1, hInst, NULL);

	SetScrollInfo(h, SB_CTL, sc, true);
	sc->fMask = SIF_POS;

	return	h;
}



/*------------------------------------------------------------------------------*
| <<< �X�N���[���o�[�̃C�x���g���� >>>
|	����	hWnd   = �E�B���h�D�n���h��
|			dwFlag	 = �E�B���h�E�t���O		SBS_VERT(�c) or SBS_HORZ(��)
|			sc     = �X�N���[���o�[���
|	�߂�l	true : �X�V����
*------------------------------------------------------------------------------*/
bool	ctrl_scrollbar_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, SCROLLINFO *sc)
{

	bool	bRet = false;

	switch(msg)
	{
	case WM_HSCROLL:
		switch(LOWORD(wp))
		{
		case SB_LEFT:			sc->nPos = sc->nMin;					  bRet = true; break;
		case SB_RIGHT:			sc->nPos = sc->nMax;					  bRet = true; break;
		case SB_LINELEFT:		if(sc->nPos               ){ sc->nPos--;} bRet = true; break;
		case SB_LINERIGHT:		if(sc->nPos < sc->nMax - 9){ sc->nPos++;} bRet = true; break;
		case SB_PAGELEFT:		sc->nPos -= sc->nPage;					  bRet = true; break;
		case SB_PAGERIGHT:		sc->nPos += sc->nPage;					  bRet = true; break;
		case SB_THUMBPOSITION:	sc->nPos  = HIWORD(wp);					  bRet = true; break;
		}
		SetScrollInfo(hWnd, SB_CTL, sc, true);
		break;
	}

	return	bRet;
}



/*************************************************************************************************
	[[[ ���W�I�{�^�� ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< ���W�I�{�^���̍쐬 >>>
|	����	hWnd		= �E�B���h�D�n���h��
|			str			= �{�^���ɕ\�����镶����
|			bFirstGroup = �O���[�v�̐擪���� true �̂���
|	�߂�l	�X�N���[���o�[�̃n���h��
*------------------------------------------------------------------------------*/
HWND	ctrl_radio_create(HWND hWnd, char *str, bool bFirstGroup)
{

	HINSTANCE	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	DWORD	d = 0;
	if(bFirstGroup) d = WS_GROUP;

	HWND	h =  CreateWindow(TEXT("BUTTON"), TEXT(str), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | d, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);

	if(hFontDefault != NULL) CTRL_FONT_SET(h, hFontDefault);	// �t�H���g�ݒ�
	return	h;
}



/*------------------------------------------------------------------------------*
| <<< ���W�I�{�^���̏�Ԃ𓾂� >>>
|	����	hWnd   = �E�B���h�D�n���h��
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	ctrl_radio_get(HWND hWnd)
{
	if(SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED) return true;
	else													return false;
}



/*------------------------------------------------------------------------------*
| <<< ���W�I�{�^����ݒ肷�� >>>
|	����	hWnd   = �E�B���h�D�n���h��
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
void	ctrl_radio_set(HWND hWnd, bool bSw)
{

	// BST_CHECKED 			�{�^���̃`�F�b�N���I���ɂ��܂��B
	// BST_INDETERMINATE 	�{�^�����O���C�\���i �s�m��j�̏�Ԃɂ��܂��B�{�^�����ABS_3STATE �X�^�C���܂��� BS_AUTO3STATE �X�^�C�������Ƃ��ɂ����g�p���Ă��������B
	// BST_UNCHECKED		�{�^���̃`�F�b�N���I�t�ɂ��܂��B

	uint	 d = BST_CHECKED;
	if(!bSw) d = BST_UNCHECKED;
	SendMessage(hWnd, BM_SETCHECK, d, 0);
}



/*************************************************************************************************
	[[[ �Z�p���[�^�[ ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �Z�p���[�^�[�̃v���V�[�W�� >>>
*------------------------------------------------------------------------------*/
static	CTRL_MOTHER	*ctrlMother;			// ctrl �}�U�[�̃|�C���^
static	void		*pCtrlSetFunc;			// CtrlSet�֐��̓o�^�p
static	bool		bCtrlSeparatorDrag;		// �Z�p���[�^�[���h���b�O����̂Ɏg�p
static	LRESULT	CALLBACK	CtrlSeparatorProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uiMsg)
	{
	//--- �J�[�\���ύX ----------------------------------------------
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		return 0;

	//--- �}�E�X���� ------------------------------------------------
	// �����Ńh���b�N�����Ă���
	case WM_LBUTTONDOWN:
		bCtrlSeparatorDrag = true;
		SetCapture(hWnd);					// �E�B���h�E�O�ł��}�E�X�̒l���E����悤�ɃL���v�`���[
		break;

	case WM_MOUSEMOVE:
		if(bCtrlSeparatorDrag)
		{
			POINT	a;						// �}�E�X�̃X�N���[�����W
			GetCursorPos(&a);
			
			RECT	r;
			GetWindowRect(sys.hWnd, &r);
											// iY �Ƀf�o�b�N�E�B�C���h�[�̕`��̈� Y ���W
			int	iLeft = r.left + GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE);
			//int	iRight = r.right - (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE));

			int	w = a.x - iLeft;
			//if(w <         256) break;		// ���
			//if(w > SCR_W - 256) break;		// ����

			if(w <              256){ break;}		// ���
			if(w > sys.iWidth - 256){ break;}		// ����
			
			ctrlMother->usSeparator[0] = (ushort)(a.x				  - iLeft);
			ctrlMother->usSeparator[1] = (ushort)(sys.iWidth		  - ctrlMother->usSeparator[0]);
			d3.sRenderW			       = (ushort)(a.x				  - iLeft);
			d3.rectRender->right       = (ushort)(d3.rectRender->left + d3.sRenderW);
			//CtrlSet();
			void( *func)(void);
			func = (void(*)())pCtrlSetFunc;
			(*func)();
		}
		break;

	case WM_LBUTTONUP:
		sys.cResizeCount   = 0;				// ���̃J�E���^�[�̒l�����̒l�ɂȂ�܂Ńf�o�C�X���쐬���Ȃ�
		bCtrlSeparatorDrag = false;
		ReleaseCapture();					// �L���v�`���[�����}�E�X���J��
		break;
	}
	return 	CallWindowProc(ctrlMother->procSeparator, hWnd, uiMsg, wParam, lParam);
}



/*------------------------------------------------------------------------------*
| <<< �Z�p���[�^�[�쐬 >>>
|	����	*r = �E�B���h�E�͈̔�
*------------------------------------------------------------------------------*/
void	ctrl_separator(CTRL_MOTHER *c, void *pCtrlSet, RECT *r)
{

	if(c->hSeparator == NULL)
	{
		ctrlMother                = c;
		pCtrlSetFunc			  = pCtrlSet;
		ctrlMother->hSeparator	  = ctrl_static_text_create(sys.hWnd, NULL, "");
		ctrlMother->procSeparator = CTRL_PROC_CHANGE(ctrlMother->hSeparator, CtrlSeparatorProc);
	}

	ctrlMother->usSeparator[0] = sys.iWidth - ctrlMother->usSeparator[1];
	CTRL_RESIZE(ctrlMother->hSeparator, ctrlMother->usSeparator[0], r->top, ctrlMother->usSeparator[0] + 2, r->bottom);
	r->right += 4;
}



/*************************************************************************************************
	[[[ �c�[���o�[ ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �c�[���o�[�쐬 >>>
|	����	iResourceNum = �摜�̃��\�[�X�ԍ�(�摜�͘A�Ԃɂ��邱��)
|			iW, iH       = �{�^���̃T�C�Y(�ЂƂ�����̃T�C�Y�ł�)
|			tbb			 = 
|			iCount		 = �{�^���̐�
*------------------------------------------------------------------------------*/
#define		ID_TOOLBAR		500

HWND	ctrl_toolbar_create(int iResourceNum, int iW, int iH, TBBUTTON *tbb, /*TBBUTTON *tb,*/ int iCount)
{

	HWND	hTool = CreateToolbarEx(
						sys.hWnd,							// �e�E�B���h�E
															// �E�B���h�E�X�^�C��
						WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
						ID_TOOLBAR,							// �R���g���[��ID
						iCount,								// �C���[�W�̐�
						sys.hInst,
						iResourceNum,
						tbb,
						iCount,								// �{�^���̐�
						0,									// �{�^���̕�
						0,									// �{�^���̍���
						iW,									// �C���[�W�̕�
						iH,									// �C���[�W�̍���
						sizeof(TBBUTTON));
	return hTool;
}



/*------------------------------------------------------------------------------*
| <<< �c�[���o�[�ɃX�y�[�X��}�� >>>
|	����	hMother = �c�[���o�[�n���h��
|			iCount  = �X�y�[�X������Ƃ���
*------------------------------------------------------------------------------*/
void	ctrl_toolbar_space_insert(HWND hMother, int iCount)
{

	TBBUTTON	tbSpace = {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0}; 

	SendMessage(hMother, TB_INSERTBUTTON, iCount, (LPARAM)&tbSpace);
}



/*------------------------------------------------------------------------------*
| <<< �c�[���o�[�̃{�^���̏�Ԃ�ݒ� >>>
|	����	hMother = �c�[���o�[�n���h��
|			tb      = �c�[���o�[�̃e�[�u��
|			iNum    = �Ώۂ̃{�^���ԍ�(���͒[���O)
|			bSw     = true or false
*------------------------------------------------------------------------------*/
void	ctrl_toolbar_set(HWND hMother, TBBUTTON *tb, int iNum, bool bSw)
{

	SendMessage(hMother, TB_CHECKBUTTON, tb[iNum].idCommand, MAKELONG(bSw, 0));
}



/*------------------------------------------------------------------------------*
| <<< �c�[���o�[�̍����𓾂� >>>
|	����	hMother = �c�[���o�[�n���h��
*------------------------------------------------------------------------------*/
/*uint	ctrl_toolbar_height_get(HWND hMother)
{
	return CTRL_TOOLBAR_H + 16;
}*/



/*------------------------------------------------------------------------------*
| <<< �c�[���o�[�̍����𓾂� >>>
|	����	uiHeight = �c�[���o�[�̊G�̏c�T�C�Y
*------------------------------------------------------------------------------*/
uint	ctrl_toolbar_height_get(uint uiHeight)
{
	return uiHeight + 12;
}


