//
//	�f�o�b�O�p
//
//		���Z�p���[�^�[�̎d�g�݂́A�ׂ��u�����X�^�e�B�b�N�̃C�x���g�ł���
//



#define	DEBUG_MONITOR_MAX	100				// ���j�^�[�\����
#define	DEBUG_MONITOR_W		400				// ����
#define	DEBUG_LIST_H		320				// ���X�g�r���[�̍���
#define	INTERVAL_TIME		60				// �\���̍X�V�Ԋu
#define	CONFIG_NAME			"debug.ini"

#define	SEPARATOR_H	4						// �Z�p���[�^�[�̃T�C�Y
#define	OFFSET_X	6						// ���X�g�A�G�f�B�b�g�̗]��
#define	OFFSET_Y	2


typedef	struct
{
	HWND	hwnd;
	BOOL	bEnd;
}PARAM_, *PPARAM_;

typedef	struct
{
	uchar	ucType;							// 0 : �I�t
											// 1 : ����
											// 2 : char
											// 3 : uchar
											// 4 : short
											// 5 : ushort
											// 6 : int
											// 7 : uint
											// 8 : float
											// 9 : double
	bool	bHex;							// true : 16 �i���ŕ\��
	void	*p;								// �f�[�^�̊i�[��
}DEBUG_WORK_;


static	DEBUG_WORK_	debug[DEBUG_MONITOR_MAX];


static	HWND	hDlgWnd;
static	HWND	hListView;					// �\���p�̃��X�g�r���[
static	HWND	hEdit;						// �G�f�B�b�g�{�b�N�X
static	HWND	hSeparator;					// �T�C�Y�ύX�p�̃Z�p���[�^�[
static	WNDPROC	procSeparator;				// �Z�p���[�^�[�̃C�x���g
static	int		iDebugMonitorCount;
static	PARAM_	data;
											// ���X�g�r���[�̍���(���̒l�ő��̃T�C�Y���ύX�����)
static	int		iListViewH	  = DEBUG_LIST_H;
static	float	fListEditRate = 0.5f;
static	int		iSeparatorY;				// �Z�p���[�^�[�̂x���W
static	bool	bDrag;						// �Z�p���[�^�[���h���b�O����̂Ɏg�p
static	RECT	rectDebug;					// �f�o�b�O�E�B���h�E�̃T�C�Y

//--- �֐��錾 ------------------------------------------------------
static	void	ConfigSave(void);
static	bool	ConfigLoad(void);
static	void	DebugMonitor(void);
static	void	DebugResize(float fRate);
static	BOOL	CALLBACK	DebugProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
static	LRESULT	CALLBACK	SeparatorProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);



/*------------------------------------------------------------------------------*
| <<< �������Z�[�u >>>
*------------------------------------------------------------------------------*/
static	void	ConfigSave(void)
{

	HANDLE	handle;						// �ǂݍ��݃n���h��
	DWORD	wr;							// �_�~�[
	char	str[256];

	GetWindowRect(hDlgWnd, &rectDebug);
	FOLDER_PUSH();
	FOLDER_SET();						// �f�B���N�g�����d�w�d�ʒu��
	remove(CONFIG_NAME);
	handle = CreateFile(CONFIG_NAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	FOLDER_POP();
	if(handle == INVALID_HANDLE_VALUE) return;

	sprintf(str, "%04d,%04d,%04d,%04d,%.4f\r\n",
			rectDebug.top,	  rectDebug.left,		// �E��
			rectDebug.bottom, rectDebug.right,		// ����
			fListEditRate
			);
	WriteFile(handle, str, strlen(str),	&wr, NULL);
	CloseHandle(handle);
}



/*------------------------------------------------------------------------------*
| <<< �V�X�e���t�@�C�������[�h >>>	���V�X�e���t�@�C���ɂ̓E�B���h�E�ʒu���ۑ�����Ă���
*------------------------------------------------------------------------------*/
static	bool	ConfigLoad(void)
{

	HANDLE	handle;						// �ǂݍ��݃n���h��
	DWORD	rs;							// �_�~�[
	int		size;
	char	str[256];

	FOLDER_PUSH();
	FOLDER_SET();
	handle = CreateFile(CONFIG_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	FOLDER_POP();
	if(handle == INVALID_HANDLE_VALUE)			return false;
	if((size = GetFileSize(handle, NULL)) == 0)	return false;
	memset(str, 0, sizeof(str));
	ReadFile(handle, str, size,	&rs, NULL);
	CloseHandle(handle);
	sscanf(str,	"%04d,%04d,%04d,%04d,%f\r\n",
				&rectDebug.top,	   &rectDebug.left,		// �E��
				&rectDebug.bottom, &rectDebug.right,	// ����
				&fListEditRate
				);

	//--- ��ʊO�̃E�B���h�E��␳���� ------------------------------
	if(rectDebug.left  < 0) rectDebug.right  -= rectDebug.left, rectDebug.left = 0;
	if(rectDebug.top   < 0) rectDebug.bottom -= rectDebug.top,  rectDebug.top = 0;
	if(rectDebug.right > SYS_DESKTOP_XSIZE())
	{
		int	a =	rectDebug.right - SYS_DESKTOP_XSIZE();
		if((rectDebug.left -= a) < 0) rectDebug.left = 0;
		rectDebug.right = SYS_DESKTOP_XSIZE();
	}
	int	y =	SYS_DESKTOP_YSIZE() + GetSystemMetrics(SM_CYCAPTION);
	if(rectDebug.bottom > y)
	{
		int	a =	rectDebug.bottom  - y;
		if((rectDebug.top -= a) < 0) rectDebug.top = 0;
		rectDebug.bottom = y;
	}
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ϐ����j�^�[�`�� >>>
*------------------------------------------------------------------------------*/
static	void	DebugMonitor(void)
{

	DEBUG_WORK_	*d = debug;

	for(int i = 0; i < iDebugMonitorCount; i++, d++)
	{
		switch(d->ucType)
		{
		//--- str ---------------------------------------------------
		case 1:
			ctrl_list_view_str_set(hListView, 1, i, (char*)d->p);
			break;
		//--- char --------------------------------------------------
		case 2:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(char)     %d", *(char*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(char)   0x%x", *(char*)d->p);
			break;
		//--- uchar -------------------------------------------------
		case 3:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(uchar)    %d", *(uchar*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(uchar)  0x%x", *(uchar*)d->p);
			break;
		//--- short -------------------------------------------------
		case 4:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(short)    %d", *(short*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(short)  0x%x", *(short*)d->p);
			break;
		//--- ushort ------------------------------------------------
		case 5:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(ushort)   %d", *(ushort*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(ushort) 0x%x", *(ushort*)d->p);
			break;
		//--- int ---------------------------------------------------
		case 6:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(int)      %d", *(int*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(int)    0x%x", *(int*)d->p);
			break;
		//--- uint --------------------------------------------------
		case 7:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(uint)     %d", *(uint*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(uint)   0x%x", *(uint*)d->p);
			break;
		//--- float -------------------------------------------------
		case 8:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(float)    %f", *(float*)d->p);
			break;
		//--- double ------------------------------------------------
		case 9:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(double)   %l", *(double*)d->p);
			break;
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�v���V�[�W�� >>>
*------------------------------------------------------------------------------*/
static	BOOL	CALLBACK	DebugProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	switch(iMsg)
	{
	//--- ���T�C�Y ------------------------------------------------------------
	case WM_SIZE:
		DebugResize(fListEditRate);
//		SetActiveWindow(sys.hWnd);
		break;
	//--- �ő厞�A�ŏ����̐��� ------------------------------------------------
	case WM_GETMINMAXINFO:
		{
			int	x = SYS_XFRAME_GET();
			int	y = SYS_YFRAME_GET();
			if(x >= 0 && sys.cWindowStyle == SIZE_RESTORED)
			{
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = 300 + x;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = 300 + y;
			}
		}
		break;
	//--- �ϐ��Ď��̂��߂̃^�C�}�[�C�x���g ------------------------------------
	case WM_TIMER:
		DebugMonitor();
//		if(b1st) SetActiveWindow(sys.hWnd), b1st = false;
		break;
	//--- �_�C�A���O�{�b�N�X��������Ƃ� ----------------------------------
	case WM_CLOSE:
		ConfigSave();
		DestroyWindow(hDlgWnd);
		return TRUE;
	}
	return FALSE;  // DefWindowProc()�ł͂Ȃ��AFALSE��Ԃ����ƁI
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�̃��C�A�E�g�ύX >>>
|	����	fRate = ���X�g�r���[�ƃG�f�B�b�g�{�b�N�X�̔䗦
|	���@iListViewH �̒l�őS�Č��܂�܂�
*------------------------------------------------------------------------------*/
static	void	DebugResize(float fRate)
{

	RECT	r;

	GetWindowRect(hDlgWnd, &r);
											// �t���[���̃T�C�Y������
	r.top    += (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE)) + GetSystemMetrics(SM_CYCAPTION);
	r.bottom -= (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE));
	r.right  -= (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE));
	r.left   += (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE));

	r.right  -= r.left;						// bottom�Aright �̓T�C�Y��
	r.bottom -= r.top;

	//--- ���X�g�r���[ --------------------------------------
	int	y = OFFSET_Y;						// �������։�����
	int	h = (int)((float)r.bottom * fRate) - SEPARATOR_H / 2 - OFFSET_Y;

	CTRL_RESIZE(hListView, OFFSET_X, y, r.right - OFFSET_X, h);
	
	//--- �G�f�B�b�g ----------------------------------------
	y = (int)((float)r.bottom * fRate) + SEPARATOR_H;
	h = (int)((float)r.bottom * (1.0f - fRate)) - SEPARATOR_H / 2 - OFFSET_Y;
	CTRL_RESIZE(hEdit,     OFFSET_X, y, r.right - OFFSET_X, y + h);
	
	//--- �Z�p���[�^�[ --------------------------------------
	h		    = SEPARATOR_H;
	y		    = (int)((float)r.bottom * fRate) - SEPARATOR_H / 2;
	iSeparatorY = y;
	CTRL_RESIZE(hSeparator, 0, y, r.right, y + h);
}



/*------------------------------------------------------------------------------*
| <<< �Z�p���[�^�[�̃v���V�[�W�� >>>
*------------------------------------------------------------------------------*/
static	LRESULT	CALLBACK	SeparatorProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uiMsg)
	{
	//--- �J�[�\���ύX ----------------------------------------------
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return 0;

	//--- �}�E�X���� ------------------------------------------------
	// �����Ńh���b�N�����Ă���
	// iSeparatorY �̒l���s���m�Ȃ��߁A�����̃Y���𐶂��Ă���
	case WM_LBUTTONDOWN:
		bDrag = true;
		SetCapture(hWnd);					// �E�B���h�E�O�ł��}�E�X�̒l���E����悤�ɃL���v�`���[
		break;

	case WM_MOUSEMOVE:
		if(bDrag)
		{
			POINT	a;						// �}�E�X�̃X�N���[�����W
			GetCursorPos(&a);
			
			RECT	r;
			GetWindowRect(hDlgWnd, &r);
											// iY �Ƀf�o�b�N�E�B�C���h�[�̕`��̈� Y ���W
			int	iTop    = r.top    +  GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYCAPTION);
			int	iBottom = r.bottom - (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE));

			if(a.y < iTop    + 100) break;	// ���
			if(a.y > iBottom - 100) break;	// ����
											// ���[�g
			fListEditRate = (float)(a.y - iTop) / (float)(iBottom - iTop);
			DebugResize(fListEditRate);
		}
		break;

	case WM_LBUTTONUP:
		bDrag = false;
		ReleaseCapture();							// �L���v�`���[�����}�E�X���J��
		break;

	
	}
	return 	CallWindowProc(procSeparator, hWnd, uiMsg, wParam, lParam);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�I�[�v�� >>>
*------------------------------------------------------------------------------*/
void	ctrl_debug_dialog_open(void)
{

	if(hDlgWnd != NULL) return;
	hDlgWnd = CreateDialog(sys.hInst, "DEBUG", sys.hWnd, DebugProc);

	//--- �E�B���h�E�̌`��ݒ肷�� ----------------------------------
	if(ConfigLoad())
	{
		MoveWindow(hDlgWnd, rectDebug.left, rectDebug.top, rectDebug.right - rectDebug.left, rectDebug.bottom - rectDebug.top, TRUE); 
	}
	else
	{
		RECT	r;
		GetWindowRect(sys.hWnd,	&r);
		MoveWindow(hDlgWnd, r.right, r.top, DEBUG_MONITOR_W, r.bottom - r.top, TRUE);
	}

	ShowWindow(  hDlgWnd, SW_SHOW);
	UpdateWindow(hDlgWnd);
											// ���X�g�r���[
	hListView     = ctrl_list_view_create(hDlgWnd, false, false);
											// ���b�Z�[�W�\���p�G�f�B�b�g�{�b�N�X
	hEdit         = ctrl_edit_create(hDlgWnd, WS_VSCROLL |  ES_MULTILINE | WS_HSCROLL | ES_READONLY | ES_AUTOVSCROLL);
			 								// �Z�p���[�^�[
	hSeparator	  = ctrl_static_text_create(hDlgWnd, NULL, "");
	procSeparator = CTRL_PROC_CHANGE(hSeparator, SeparatorProc);
											// ���ڂ̒ǉ�
	ctrl_list_view_column_add(hListView, 0, "�ϐ���", 150);
	ctrl_list_view_column_add(hListView, 1, "�l",     232);

	//--- �Ď��p�̃^�C�}�[�N�� --------------------------------------
	// �^�C�}�[���g���A�ēx���C���E�B���h�E���A�N�e�B�u�ɂ��邱�ƂŁA�p�b�g������ɓ���
	SetTimer(hDlgWnd, ID_MYTIMER, INTERVAL_TIME, NULL);

	SendMessage(hDlgWnd, WM_SIZE, 0, 0L);

//	b1st = true;
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O���� >>>
*------------------------------------------------------------------------------*/
void	ctrl_debug_dialog_close(void)
{
	ConfigSave();
	SendMessage(hDlgWnd, WM_CLOSE, 0, 0L);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	��������
*------------------------------------------------------------------------------*/
void	ctrl_debug_str_monitor(char *strVar, char *str)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 1;
		debug[iID].p	  = (void*)str;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, str);
		iDebugMonitorCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� char �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, char *cVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 2;
		debug[iID].p	  = (void*)cVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, char *cVar)
{
	ctrl_debug_monitor(strVar, cVar, false);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� uchar �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, uchar *ucVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 3;
		debug[iID].p	  = (void*)ucVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, uchar *ucVar)
{
	ctrl_debug_monitor(strVar, ucVar, false);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� short �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, short *sVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 4;
		debug[iID].p	  = (void*)sVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, short *sVar)
{
	ctrl_debug_monitor(strVar, sVar, false);
}

	
	
/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� ushort �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, ushort *usVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 5;
		debug[iID].p	  = (void*)usVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, ushort *usVar)
{
	ctrl_debug_monitor(strVar, usVar, false);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� int �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, int *iVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 6;
		debug[iID].p	  = (void*)iVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, int *iVar)
{
	ctrl_debug_monitor(strVar, iVar, false);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� uint �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, uint *uiVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 7;
		debug[iID].p	  = (void*)uiVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, uint *uiVar)
{
	ctrl_debug_monitor(strVar, uiVar, false);
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� float �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, float *fVar)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 7;
		debug[iID].p	  = (void*)fVar;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�ϐ����j�^�[�ݒ� >>>	�� double �^
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, double *dVar)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 8;
		debug[iID].p	  = (void*)dVar;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�_�C�A���O�A�G�f�B�b�g�{�b�N�X�ɕ�����𑗂� >>>
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor_printf(char* Format,...)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hEdit != NULL)
	{
		char	strBuf[512];

		va_list	ap;
		va_start(ap,	 Format);
		vsprintf(strBuf, Format, ap);
		va_end(ap);
		ctrl_edit_printf(hEdit, strBuf);
	}
}