//
//	�o�[�W����
//


//#include	"..\\main.h"

//--- �ϐ���` ------------------------------------------------------
static	WNDPROC	Org_MyAboutStatic;
static	char	strText_[2][64];



/*------------------------------------------------------------------------------*
| <<< URL �̃R�[���o�b�N >>>
*------------------------------------------------------------------------------*/
static	LRESULT	CALLBACK	MyAboutStaticProc(HWND	hWnd, UINT msg,	WPARAM wp, LPARAM lp)
{

	switch(msg)
	{
	//--- �J�[�\���ύX ----------------------------------------------
	case WM_SETCURSOR:
		SetCursor(LoadCursor(sys.hInst,	"YUBI"));
		return 0;

	//--- IE �N�� ---------------------------------------------------
	case WM_LBUTTONDOWN:
		ShellExecute(hWnd, NULL, strText_[1], NULL, NULL, SW_SHOWNORMAL);
		return 0;

	//--- �����A�����ɂ��� ----------------------------------------
	case WM_PAINT:
		{
			HFONT		hFont;
			LOGFONT		lf;
			PAINTSTRUCT	ps;
			RECT		r;
			HDC			hdc	= BeginPaint(hWnd, &ps);

			memset(&lf,	0, sizeof(LOGFONT));	// �����t�H���g�ݒ�
			strcpy(lf.lfFaceName , "system");
			lf.lfUnderline = TRUE;
			lf.lfHeight	   = 14;
			lf.lfWidth	   = 6;
			GetClientRect(hWnd,	&r);
			SetTextAlign(hdc, TA_CENTER);
			hFont =	CreateFontIndirect(&lf);
			SelectObject(hdc, (HGDIOBJ)hFont);
			SetBkColor(	 hdc, GetSysColor(COLOR_MENU));
			SetTextColor(hdc, RGB(0, 0,	255));
			SetBkMode(	 hdc, TRANSPARENT);
												// �����`��
			TextOut(	 hdc, r.right /	2, 0, strText_[1], strlen(__URL));
			DeleteObject(hFont);
			EndPaint(hWnd, &ps);
			return 0;
		}
	}
	return CallWindowProc(Org_MyAboutStatic, hWnd, msg,	wp,	lp);
}



/*------------------------------------------------------------------------------*
| <<< �_�C�A���O�̃C�x���g���� >>>
*------------------------------------------------------------------------------*/
static	BOOL	CALLBACK	DlgProc(HWND hWnd, UINT	msg, WPARAM	wParam,	LPARAM lParam)
{
	static	HWND	hMyStatic;

	switch(msg)
	{
	case WM_INITDIALOG:
		//--- �_�C�A���O�����A�E�B���h�E�̒����� --------------------
		CTRL_WINDOW_RECT_SET(hWnd, CTRL_VER_WIDTH, CTRL_VER_HEIGHT);
			
		//--- URL �̃R���g���[���𓾂� ------------------------------
		hMyStatic =	GetDlgItem(hWnd, IDC_VER_URL);
		//if(!hMyStatic) MessageBox(NULL,	"Error", "OK", MB_OK);
		if(!hMyStatic){ sys_message_box("�o�[�W�����E�B���h�E���J���܂���", "OK", MB_OK);}
		Org_MyAboutStatic =	(WNDPROC)GetWindowLong(hMyStatic, GWL_WNDPROC);
		SetWindowLong(hMyStatic, GWL_WNDPROC, (LONG)MyAboutStaticProc);

		//--- ���b�Z�[�W���M ----------------------------------------
		//SetDlgItemText(hWnd, IDC_VER_MESSAGE,	strText_[1]);
		SetDlgItemText(hWnd, IDC_VER_TITLE,	strText_[0]);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:							// OK �{�^��
			SendMessage(hWnd, WM_CLOSE,	0, 0L);
			break;
		}
		break;
	case WM_CLOSE:
		//if(SetWindowLong(hMyStatic,	GWL_WNDPROC, (LONG)Org_MyAboutStatic) == 0)	MessageBox(hWnd, "Error", "OK",	MB_OK);
		if(SetWindowLong(hMyStatic,	GWL_WNDPROC, (LONG)Org_MyAboutStatic) == 0){ sys_message_box("�o�[�W�����E�B���h�E������܂���", "OK", MB_OK);}
		EndDialog(hWnd,	IDOK);
		d3.bRender = true;
		return TRUE;
	default:
		return FALSE;
	}
	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< �o�[�W�������I�[�v�� >>>
|	����	strTitle = �A�v����
|			strUrl	 = �z���y�̃A�h���X
*------------------------------------------------------------------------------*/
void	ctrl_version_dialog_open(char *strTitle, char *strUrl)
{
	strcpy(strText_[0], strTitle);
	strcpy(strText_[1], strUrl);
	DialogBox(sys.hInst, "VER",	sys.hWnd, DlgProc);
}
