//
//	�f�B�X�v���C�ύX�_�C�A���O�֌W�̃v���O����
//
//


//#include	"main.h"


//--- �萔��` ------------------------------------------------------
#undef	WIDTH_
#define	WIDTH_		435						// �E�B���h�E�̃T�C�Y
#undef	HEIGHT_
#define	HEIGHT_		(285 - 56)


static	void	ChangeSet(void);



/**************************************************************************************************
	[[[	�_�C�A���O�֌W ]]]
**************************************************************************************************/

static	char	*strDisplay[] =				// fmtDisplay �𕶎�����������
{
	"D3DFMT_X8R8G8B8", "D3DFMT_R5G6B5"
};
											// ���_�v�Z�ݒ�p
static	char	*strVP[] = {"HARDWARE_VP", "SOFTWARE_VP"};

static	D3_WORK		d3_bak;					// �L�����Z�����̃o�b�N�A�b�v�p
static	HWND		hDialog;				// �f�o�C�X�ύX�_�C�A���O�̃n���h��



/*------------------------------------------------------------------------------*
| <<< ��ʃ��[�h���R���{�{�b�N�X�ɗ񋓂��� >>>
|	����	iAdapter = �A�_�v�^�[�ԍ��i�O���f�t�H���g�j
|			fmt		 = �ݒ肵�����f�B�X�v���C�t�H�[�}�b�g
|	�߂�l	�t���X�N���[���̉𑜓x���w���ԍ�
*------------------------------------------------------------------------------*/
static	int		EnumMode(int iAdapter, D3DFORMAT fmt)
{
#if IDC_RESOLUTION_COMBO
	D3DDISPLAYMODE	mode;
	int				iRet	   = 0;
	HWND			hResoCombo = GetDlgItem(hDialog, IDC_RESOLUTION_COMBO);


	//SendMessage(hWnd, CB_RESETCONTENT, 0, 0);	// ����
	ctrl_combo_clear(hResoCombo);

	int	iMax   = d3.d->GetAdapterModeCount(iAdapter, fmt);
	int	iCount = 0;
	for(int	i =	0; i < iMax; i++)
	{
		if(d3.d->EnumAdapterModes(iAdapter,	fmt, i,	&mode) == D3D_OK)
		{
			if(mode.RefreshRate	!= D3_HZ    ) continue;	// ���t���b�V�����[�g��	60MHz�Œ�
			if(mode.Height		<  D3_REZO_H) continue;	// �𑜓x���c 480 �ȉ��͖���
			//SendMessage(hWnd,	CB_ADDSTRING, 0, (LPARAM)buf);
			//#ifndef	__RENDER_FIXATION
			//	if(mode.Width == __SCR_W && mode.Height == __SCR_H)
			//#endif
			ctrl_combo_str_add(hResoCombo, "%d x %d", mode.Width, mode.Height);

			if(mode.Width  == (ushort)d3.sFullScreenRenderW
			&& mode.Height == (ushort)d3.sFullScreenRenderH) 
			{
				iRet = iCount;
			}
			iCount++;
		}
	}
	return iRet;
#else
	return 0;
#endif
}



/*------------------------------------------------------------------------------*
| <<< �_�C�A���O���̃A�_�v�^�[�̐ݒ��ύX���� >>>
|	����	bWindow	= true	: �E�B���h�E�ŋN��
*------------------------------------------------------------------------------*/
static	void	CtrlSet(bool bWindow)
{
#ifdef	IDC_ADAPTER_COMBO
	int		i, j;
	D3_WORK	*w		   = &d3;
	HWND	hAdpCombo  = GetDlgItem(hDialog, IDC_ADAPTER_COMBO);
	HWND	hAdpFCombo = GetDlgItem(hDialog, IDC_ADAPTERFORMAT_COMBO);
	HWND	hResoCombo = GetDlgItem(hDialog, IDC_RESOLUTION_COMBO);

	//--- �A�_�v�^�[ ------------------------------------------------
	ctrl_combo_clear(hAdpCombo);
	for(i =	0; i < (int)d3.d->GetAdapterCount(); i++)
	{
		D3DADAPTER_IDENTIFIER9	Adapter;
		d3.d->GetAdapterIdentifier(i, 0, &Adapter);
		ctrl_combo_str_add(hAdpCombo, Adapter.Description);
	}
	ctrl_combo_sel_set(hAdpCombo, w->ucAdapter);

	//--- �E�B���h�E ------------------------------------------------
	if(bWindow)
	{										// �E�B���h�E�փX�C�b�`������
		CheckRadioButton(hDialog, IDC_WINDOW, IDC_FULLSCREEN, IDC_WINDOW);
		ctrl_combo_clear(hAdpFCombo);		// �f�B�X�v���C�t�H�[�}�b�g
		for(i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			if(w->fmtDisplay ==	d3_display_format_tbl[i])
			{
				ctrl_combo_str_add(hAdpFCombo, strDisplay[i]);
				ctrl_combo_sel_set(hAdpFCombo, 0);
				break;
			}
		}
		EnableWindow(hAdpFCombo, false);	// �E�B���h�E�̎��̓t�H�[�}�b�g�̕ύX�֎~
											// �𑜓x
		ctrl_combo_sel_set(hResoCombo, EnumMode(w->ucAdapter, w->fmtDisplay));
		EnableWindow(      hResoCombo, false);
	}
	//--- �t���X�N���[�� --------------------------------------------
	else
	{										// �t���X�N���[���փX�C�b�`������
		CheckRadioButton(hDialog, IDC_WINDOW, IDC_FULLSCREEN, IDC_FULLSCREEN);
		ctrl_combo_clear(hAdpFCombo);		// �f�B�X�v���C�t�H�[�}�b�g
		int	k =	0, l = 0;							// R5G6B5 �� �o�b�N�o�b�t�@�� X8R8G8B8 �Ƃ����̂����肦��̂ł������Ă���
		for(i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			bool	flag = false;
			for(j =	0; j < D3_FMT_DISPLAY_MAX; j++)
			{
				if(SUCCEEDED(d3.d->CheckDeviceType(w->ucAdapter, w->deviceType,	d3_display_format_tbl[i], d3_display_format_tbl[j],	false))){ flag = true;}
			}
			if(w->fmtDisplay ==	d3_display_format_tbl[i]){ l = k;}
			if(flag){ ctrl_combo_str_add(hAdpFCombo, strDisplay[i]), k++;}
		}
		EnableWindow(      hAdpFCombo, true);	// �t���X�N���[�����̓t�H�[�}�b�g�̕ύX����
		ctrl_combo_sel_set(hAdpFCombo, l);
												// �𑜓x
		ctrl_combo_sel_set(hResoCombo, EnumMode(w->ucAdapter, w->fmtDisplay));
		EnableWindow(      hResoCombo, true);
	}
	//--- ���_�̌v�Z���n�[�h�ł�邩�\�t�g�ł�邩 ------------------
	/*ctrl_combo_clear(hVertCombo);
	j =	0;
	if((d3.Caps.DevCaps	& D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
	{
		if(d3.Caps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE
		|| d3.Caps.TextureCaps & D3DPTEXTURECAPS_ALPHA)
		{									// D3DCREATE_HARDWARE_VERTEXPROCESSING ��ǉ�
			ctrl_combo_str_add(hVertCombo, strVP[0]);
			j++;
		}
	}										// D3DCREATE_SOFTWARE_VERTEXPROCESSING ��ǉ� MIX �͖���
	ctrl_combo_str_add(hVertCombo, strVP[1]);
	if(j !=	0)								// ���ڂ��ӂ�����ꍇ�͌��ݑI�����Ă�����̂Ƃ���B
	{
		if(d3.ucVertexProcessing ==	D3DCREATE_HARDWARE_VERTEXPROCESSING) j = 0;
		else															 j = 1;
	}
	ctrl_combo_sel_set(hVertCombo, j);
	*/
#endif
}



/*------------------------------------------------------------------------------*
| <<< �w��̃X�C�b�`�A�R���{�{�b�N�X�̏�Ԃ� d3�ɕԂ�>>>
*------------------------------------------------------------------------------*/
static	void	ComboSet(int iID)
{
#ifdef	IDC_ADAPTER_COMBO
	int		i, sel;
	HWND	h = GetDlgItem(hDialog, iID);

	switch(iID)
	{
	case IDC_ADAPTER_COMBO:					// �A�_�v�^�[
		if(d3.ucAdapter	!= ctrl_combo_sel_get(h))
		{
			d3.ucAdapter = ctrl_combo_sel_get(h);
											// �A�_�v�^�[�̏�Ԃ𓾂�
			d3.d->GetDeviceCaps(		d3.ucAdapter, d3.deviceType, &d3.caps);
											// �f�B�X�v���C�̏�Ԃ𓾂�
			d3.d->GetAdapterDisplayMode(d3.ucAdapter, &d3.displayMode);
		}
		break;
	case IDC_ADAPTERFORMAT_COMBO:			// �A�_�v�^�[�̉摜�t�H�[�}�b�g
		sel	= ctrl_combo_sel_get(h);
		for(i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			if(ctrl_combo_strcmp(GetDlgItem(hDialog, iID), sel,	strDisplay[i]))
			{
				d3.fmtDisplay	 = d3_display_format_tbl[i];
				d3.fmtBackBuffer = d3_display_format_tbl[i];
				break;
			}
		}
		break;
	case IDC_RESOLUTION_COMBO:				// �`��T�C�Y
		{
			char	str[256];
			ctrl_combo_str_get(str, h, ctrl_combo_sel_get(h));
			sscanf(str,	"%d x %d", &d3.sFullScreenRenderW, &d3.sFullScreenRenderH);
		}
		break;
	case IDC_WINDOW:
		d3.bWindow    = IsDlgButtonChecked(hDialog, iID) == BST_CHECKED;
		break;
	}
#endif
}



/*------------------------------------------------------------------------------*
| <<< �_�C�A���O�̃C�x���g���� >>>
*------------------------------------------------------------------------------*/
static	BOOL	CALLBACK	D3CtrlProc(HWND	hWnd, UINT msg,	WPARAM wParam, LPARAM lParam)
{
#ifdef	IDC_ADAPTER_COMBO
	switch(msg)
	{
	case WM_INITDIALOG:
		//static	bool	isLocked = false;		// ���������ɂ̓X�C�b�`�̓����}��
		//isLocked = true;
		CTRL_WINDOW_RECT_SET(hWnd);					// �E�B���h�E�̒�����
		hDialog	   = hWnd;
		d3.bWindow = true;							// �N�����͋������ɃE�B���h�E�Ƃ���
		CtrlSet(d3.bWindow);
		//isLocked	 = false;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ADAPTER_COMBO:				// �A�_�v�^�[
		case IDC_ADAPTERFORMAT_COMBO:		// �A�_�v�^�[�̉摜�t�H�[�}�b�g
		case IDC_RESOLUTION_COMBO:			// �`��T�C�Y
		//case IDC_VERTEXPROCESSING_COMBO:	// ���_�̌v�Z���@
			if(CBN_SELCHANGE ==	HIWORD(wParam))
			{
				ComboSet(LOWORD(wParam));	
				CtrlSet(IsDlgButtonChecked(hWnd, IDC_WINDOW) ==	BST_CHECKED);
			}
			break;
		case IDC_WINDOW:
			d3.fmtBackBuffer = d3.fmtDisplay = d3.deskTopDisplayMode.Format;
			ComboSet(LOWORD(wParam));
			CtrlSet(true);
			EnableWindow(GetDlgItem(hWnd, IDC_ADAPTERFORMAT_COMBO),	false);
			CheckRadioButton(hWnd, IDC_WINDOW, IDC_FULLSCREEN, IDC_WINDOW);
			break;
		case IDC_FULLSCREEN:
			d3.fmtBackBuffer = d3.fmtDisplay = d3.deskTopDisplayMode.Format;
			ComboSet(LOWORD(wParam));
			CtrlSet(false);
			EnableWindow(GetDlgItem(hWnd, IDC_ADAPTERFORMAT_COMBO),	true);
			CheckRadioButton(hWnd, IDC_WINDOW, IDC_FULLSCREEN, IDC_FULLSCREEN);
			break;
		case IDOK:							// OK �{�^����������
			ChangeSet();
			SendMessage(hDialog, WM_CLOSE, 0, 0L);
			break;
		case IDCANCEL:						// CANCEL ��������
			memcpy(&d3,	&d3_bak, sizeof(D3_WORK));
			SendMessage(hDialog, WM_CLOSE, 0, 0L);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDialog, IDOK);
		hDialog	= NULL;
		break;
	default:
		return FALSE;
	}
#endif
	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< �_�C�A���O�I�[�v�� >>>
|	����	bSw	= true	: �_�C�A���O���J����	 d3	��ݒ聨�A�_�v�^�[������
|				  false	: �_�C�A���O���J���Ȃ��� d3	��ݒ聨�A�_�v�^�[������
*------------------------------------------------------------------------------*/
void	ctrl_d3_open(void)
{
	memcpy(&d3_bak,	&d3, sizeof(D3_WORK));	// �L�����Z�����̃o�b�N�A�b�v

	DialogBox(sys.hInst, "CHANGE_DEVICE", sys.hWnd,	D3CtrlProc);

	//--- �G�X�P�[�v���������܂ő҂�
	while(1)
	{
		if(!sys_key_get(VK_ESCAPE)){ break;}
	}
}



/*------------------------------------------------------------------------------*
| <<< �ݒ���X�V >>>
*------------------------------------------------------------------------------*/
void	ChangeSet(void)
{
#ifdef	IDC_ADAPTER_COMBO
	ComboSet(IDC_ADAPTER_COMBO);			// �A�_�v�^�[
	//ComboSet(IDC_VERTEXPROCESSING_COMBO);	// ���_�̌v�Z���@
	ComboSet(IDC_WINDOW);					// �E�B���h�E
											// �n�j���AHAL��REF�AHARDWARE��SOFTWARE	�̐؂�ւ��̓f�o�C�X�V�K�쐬
	if(d3.ucAdapter			 !=	d3_bak.ucAdapter
	|| d3.bWindow			 !=	d3_bak.bWindow
	|| d3.ucVertexProcessing !=	d3_bak.ucVertexProcessing)
	{
		while(sys.bMain);
		sys.cResizeCount = 0;
	}
#endif
}
