
#ifndef	__CTRL_
#define	__CTRL_


#define	CTRL_DIALOG_MAX		64					// �_�C�A���O�̍ő吔
#define	CTRL_CONTROL_MAX	64					// �e�R���g���[����
#define	CTRL_TAB_MAX		16					// �^�u�̐�
#define	CTRL_FONT_MAX		8					// �t�H���g�̐�

#define	ID_TABCTRL			2000				// �^�u�̎��ʎq
#define	ID_LISTVIEW			4100				// ���X�g�r���[�������̊��荞�ݔԍ�
#define	ID_STATIC			5200				// �X�^�e�B�b�N�̊��荞�ݔԍ�

#define	ID_SLIDER			2020				// �X���C�_�[


//--- �R���g���[�����[�N --------------------------------------------
#if 1
	struct	CTRL_WORK
	{
		HWND		hWnd;								// �n���h��(���ꂪ�}�U�[�ƂȂ�)
		HWND		hCtrl[  CTRL_CONTROL_MAX];			// �e�R���g���[���̃n���h��
		WNDPROC		procOri[CTRL_CONTROL_MAX];			// ���X�̃X�^�e�B�b�N�̃v���V�[�W��
	};
#endif

//--- �R���g���[���}�U�[ --------------------------------------------
// ���̃��[�N�͕K���ЂƂ���ACTRL_WORK �͑S�Ă��̃��[�N�ŊǗ������
struct	CTRL_MOTHER
{
	HWND		hSeparator;					// �Z�p���[�^�[�n���h��
	WNDPROC		procSeparator;				// �Z�p���[�^�[�̊��荞�ݗp
	ushort		usSeparator[4];				// �Z�p���[�^�[�̈ʒu(�E�B���h�E�㉺���E�̏�)
	HWND		hCtrl[  CTRL_CONTROL_MAX];	// �e�R���g���[���̃n���h��
	HWND		hTips[  CTRL_CONTROL_MAX];	// �e�R���g���[���̃c�[���`�b�v
	WNDPROC		procOri[CTRL_CONTROL_MAX];	// ���X�̃X�^�e�B�b�N�̃v���V�[�W��
	HWND		hMother;
};

//--- �c���[�r���[ --------------------------------------------------
#define	CTRL_TREE_STR_MAX	256				// �c���[�r���[�̕�����
#define	CTRL_TREE_EXT_MAX	8				// �g���q�̑���(�����w��̏ꍇ�� 8 �܂ł̈Ӗ�)
struct	CTRL_TREE_WORK
{
	HTREEITEM	hTree;						// �c���[�̃A�C�e���n���h��
	int			iObj[CTRL_TREE_EXT_MAX];		// �A�C�R���̉摜�ԍ�
	char		str[ CTRL_TREE_STR_MAX];		// ������̊i�[�p
};

//--- �O���Q�Ɛ錾 --------------------------------------------------
// �t�H���g
	extern	void		ctrl_font_create(HFONT hFont);
	extern	HFONT		ctrl_font_create(int iSize, char *strFont);
	extern	void		ctrl_font_free(HFONT hFont);
// �}�U�[
	extern	HWND		ctrl_create(HWND hMother, void *pProc, char *strResource);
	extern	HWND		ctrl_create(void *pProc, char *strResource);
	extern	HWND		ctrl_create(char *strDialog, void *pProc);
	extern	void		ctrl_debug_free(void);
	extern	HWND		ctrl_create(void *pProc);
	extern	void		ctrl_free(CTRL_MOTHER *w);
// �f�o�b�N�p
	extern	void		ctrl_debug_create(void);
	extern	void		ctrl_debug_view(bool bSw);
	extern	void		ctrl_debug_resize(int iX, int iY, int iW, int iH);
	extern	void		ctrl_debug_printf(char* Format,...);
// �v�b�V���{�^��
	extern	HWND		ctrl_push_button_create(HWND hWnd, uint menuID, char* Format,...);
	extern	void		ctrl_push_button_printf(HWND hWnd, char* Format,...);
// �G�f�B�b�g�{�b�N�X
	extern	HWND		ctrl_edit_create(HWND hWnd, DWORD dwFlag, HMENU hMenu);
	extern	HWND		ctrl_edit_create(HWND hWnd, DWORD dwFlag);
	extern	void		ctrl_edit_printf(HWND hWnd, char* Format,...);
	extern	void		ctrl_edit_get(HWND hWnd, char *strOut);
// �X�^�e�B�b�N�e�L�X�g
	extern	HWND		ctrl_static_text_create(HWND hWnd, uint menuID, DWORD dwFlag, char* Format,...);
	extern	HWND		ctrl_static_text_create(HWND hWnd,              DWORD dwFlag, char* Format,...);
	extern	void		ctrl_static_text_printf(HWND hWnd, char* Format,...);
// �X�^�e�B�b�N�s�N�`���[
	extern	HWND		ctrl_static_picture_create(HWND hWnd);
	extern	HWND		ctrl_static_picture_create(HWND hWnd, uint menuID);
// �R���{�{�b�N�X
	extern	HWND		ctrl_combo_create(HWND hWnd, uint hMenu);
	extern	HWND		ctrl_combo_create(HWND hWnd);
	extern	void		ctrl_combo_clear(HWND hWnd);
	extern	void		ctrl_combo_str_add(HWND	hWnd, char* Format,...);
	extern	void		ctrl_combo_str_get(char *strOut, HWND hWnd, int iSel);
	extern	void		ctrl_combo_sel_set(HWND	hWnd, int iSel);
	extern	int			ctrl_combo_sel_get(HWND	hWnd);
	extern	bool		ctrl_combo_strcmp(HWND hWnd, int iSel, char* Format,...);
	extern	int			ctrl_combo_max_get(HWND	hWnd);
// �O���[�v�{�b�N�X
	extern	HWND		ctrl_group_box_create(HWND hWnd, char* Format,...);
	extern	void		ctrl_group_box_printf(HWND hWnd, char* Format,...);
// �c�[���`�b�v
	extern	HWND		ctrl_tool_tips_create(HWND hMother, HWND hWnd, char* Format,...);
	extern	HWND		ctrl_tool_tips_create(HWND hMother, uint uiControl, char* Format,...);
	//extern	void		ctrl_tool_tips_update_text(HWND	hWnd, char* Format,...);
	extern	void		ctrl_tool_tips_update_text(HWND	*hWnd, HWND hTarget, HWND hMother, char* Format,...);
// �X���C�_�[
	extern	HWND		ctrl_slider_create(HWND hWnd, DWORD dwFlag);
	extern	HWND		ctrl_slider_create(HWND hWnd);
	extern	void		ctrl_slider_range_set(HWND hWnd, int iMin, int iMax);
	extern	void		ctrl_slider_set(HWND hWnd, int iPos);
	extern	int			ctrl_slider_get(HWND hWnd);
// �^�u
	#define	CTRL_TAB_HEIGHT		(22)						// �^�u�̍��ڍ���
	#define	CTRL_TAB_RESIZE(       hWnd, x0, y0, x1, y1)	CTRL_RESIZE(hWnd, x0 + GetSystemMetrics(SM_CXEDGE), y0, x1, y1)
	#define	CTRL_TAB_DIALOG_RESIZE(hWnd, x0, y0, x1, y1)	CTRL_RESIZE(hWnd, x0 + GetSystemMetrics(SM_CXEDGE) + 2, y0 + 22, x1 - 2, y1 - 2)
	#define	CTRL_TAB_DIALOG_RESIZE_(hWnd, r,  w,  h)		CTRL_RESIZE(hWnd, (r.right - w) + GetSystemMetrics(SM_CXEDGE) + 2, r.top + 22, r.right - GetSystemMetrics(SM_CXEDGE) * 2 - 2, r.bottom - 2)
	#define	CTRL_TAB_SELECT_GET(h)	TabCtrl_GetCurSel(h)		// �I���^�u�̔ԍ��𓾂�
	extern	void		ctrl_tab_add(HWND hWnd,	uint uiID, char* Format,...);
	extern	HWND		ctrl_tab_create(HWND hWnd, uint uiID);
	extern	HWND		ctrl_tab_create(HWND hWnd);
	extern	void		ctrl_tab_dailog_resize(HWND hMother, int iXOfs, int iYOfs);
	extern	int			ctrl_tab_get(HWND hMother);				// �I���^�u�̔ԍ��𓾂�
	extern	void		ctrl_tab_set(HWND hMother, int iNum);	// �^�u�̍��ڂ�I������

// ���X�g�r���[
	//#define	CTRL_LIST_VIEW_RESIZE(hWnd, x0, y0, x1, y1)	CTRL_RESIZE(hWnd, (x0) + 6, (y0) + 6, (x1) - 12, (y1) - 32);
	extern	void		ctrl_list_view_draw(LPDRAWITEMSTRUCT lpDraw);
	extern	HWND		ctrl_list_view_create(HWND hWnd, bool bSw0,	bool bSw1);
	extern	void		ctrl_list_view_resize(HWND hMother, HWND hList);
	extern	void		ctrl_list_view_column_add(HWND hWnd, int iInsPoint,	char *str, int iSize);
	extern	void		ctrl_list_view_column_resize(HWND hWnd,	int	iNum);
	extern	int			ctrl_list_view_column_size_get(HWND hWnd, int iNum);
	extern	void		ctrl_list_view_reset(HWND hWnd);
	extern	void		ctrl_list_view_add(HWND	hWnd, int iSub, int iItem, char* Format,...);
	extern	bool		ctrl_list_view_dec(HWND hWnd, int iItem);
	extern	void		ctrl_list_view_str_get(char *strOut, HWND hWnd, int iSub, int iItem, int iSize);
	extern	void		ctrl_list_view_str_get(char *strOut, HWND hWnd, int iSub, int iItem);
	extern	void		ctrl_list_view_str_set(HWND hWnd, int iSub, int iItem, char* Format,...);
	extern	void		ctrl_list_view_swap(HWND hWnd, int a, int b);				// �h���b�O���A���ڂ̓���ւ�
	extern	int			ctrl_list_view_total_count_get(HWND	hWnd);
	extern	bool		ctrl_list_view_check_click_check(HWND hWnd);				// ���X�g�r���[�I�����A�`�F�b�N�̕����������ꂽ�����ׂ�
	extern	void		ctrl_list_view_check_set(HWND hWnd,	int	iItem, bool	bSw);	// ���X�g�r���[�̃`�F�b�N�{�b�N�X�Ƀ`�F�b�N��t����
	extern	bool		ctrl_list_view_check_get(HWND hWnd,	int	iItem);				// ���X�g�r���[�̃`�F�b�N�{�b�N�X�Ƀ`�F�b�N�̏�Ԃ𓾂�
	extern	int			ctrl_list_view_focus_get(HWND hWnd);
	extern	void		ctrl_list_view_focus_set(HWND hWnd, int iSel);				// �t�H�[�J�X�𓖂Ă�(�I����Ԃɂ���)
	extern	void		ctrl_list_view_focus_release(HWND hWnd, int iSel);			// �t�H�[�J�X����������
	extern	int			ctlr_list_view_sel_get(HWND hListView, bool bSw);			// ���X�g�r���[�̃}�E�X�őI�������ԍ��𓾂�
	extern	int			ctlr_list_view_sel_get(HWND hListView);
	extern	int			ctrl_list_view_next_select_get(HWND hWnd, int iItem);
																					// ���X�g�r���[�h���b�O�J�n(�h���b�O�C���[�W�쐬)
	extern	int			ctrl_list_view_drag_begin(HIMAGELIST *phimageOut, HWND hList, HWND hMother, NMLISTVIEW *pNM);
																					// ���X�g�r���[�h���b�O��(�h���b�O�C���[�W�̈ړ�)
	extern	void		ctrl_list_view_drag(HWND hList, HWND hMother, int iX, int iY);
																					// ���X�g�r���[�h���b�O�I��(�h���b�O�C���[�W�̉��)
	extern	void		ctrl_list_view_drag_end(HWND hMother, HIMAGELIST *phimageListView);
	extern	int			ctrl_list_view_pos_get(HWND hList);							// �}�E�X���w���Ă���A���X�g�r���[�̔ԍ��𓾂�

// �`�F�b�N�{�b�N�X
	extern	HWND		ctrl_check_box_create(HWND hWnd, char* Format,...);
	extern	void		ctrl_check_box_set(HWND hWnd, bool bSw);
	extern	bool		ctrl_check_box_get(HWND hWnd);
// �t�H���_�[�I��
	extern	bool		ctrl_folder_select(HWND	hWnd, char *strDefaultFolder, char *strCaption);
	extern	bool		ctrl_folder_select(HWND	hWnd, char *strDefaultFolder);
// �X�N���[���o�[
	extern	HWND		ctrl_scrollbar_create(HWND hWnd, DWORD dwFlag, SCROLLINFO *sc);
	extern	bool		ctrl_scrollbar_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, SCROLLINFO *sc);
// ���W�I�{�^��
	extern	HWND		ctrl_radio_create(HWND hWnd, char *str, bool bFirstGroup);
	extern	bool		ctrl_radio_get(HWND hWnd);
	extern	void		ctrl_radio_set(HWND hWnd, bool bSw);
// �Z�p���[�^�[
	extern	void		ctrl_separator(RECT *r);
// �c�[���o�[
	//#define	CTRL_TOOLBAR_HEIGHT_GET(h)	(h + 22)
	extern	HWND		ctrl_toolbar_create(int iResourceNum, int iW, int iH, TBBUTTON *tbb, int iCount);
	extern	void		ctrl_toolbar_space_insert(HWND hMother, int iCount);
	extern	void		ctrl_toolbar_set(HWND hMother, TBBUTTON *tb, int iNum, bool bSw);
	extern	uint		ctrl_toolbar_height_get(HWND hMother);
	extern	uint		ctrl_toolbar_height_get(uint uiHeight);
// �c���[�r���[
	extern	HTREEITEM	ctrl_tree_add(HWND hWnd, HTREEITEM hItem, HTREEITEM hNext, int iImage, char* Format,...);
	extern	HTREEITEM	ctrl_tree_add(HWND hWnd, HTREEITEM hItem, int iImage, char* Format,...);
	extern	void		ctrl_tree_reset(HWND hWnd);
	extern	void		ctrl_tree_dir_all_view(HWND hWnd, CTRL_TREE_WORK *tree);
	extern	int			ctrl_tree_dir_sel_get(HWND hWnd, CTRL_TREE_WORK *tree);
	extern	void		ctrl_tree_dir_get(CTRL_TREE_WORK *treeOut, HWND hWnd, int iCount, HTREEITEM *hParent, LPCTSTR lpszFolder, LPCTSTR lpszWildCard, bool bSw);
	extern	void		ctrl_tree_dir_get(CTRL_TREE_WORK *treeOut, HWND hWnd, int iMax, char *strFolder, char *strWildCard, bool bSw);


//--- �֗��ȃ}�N������ ----------------------------------------------------------------------------

//	�_�C�A���Oor�R���g�[���̕\���A��\��
	inline	void	CTRL_SHOW(HWND hWnd, bool bSw)
	{
		if(bSw){ ShowWindow(hWnd, SW_SHOW);}
		else   { ShowWindow(hWnd, SW_HIDE);}
	}

//	�R���g���[���̃T�C�Y�ύX�v��
	inline	void	CTRL_RESIZE(HWND hWnd){		SendMessage(hWnd, WM_SIZE, 0, 0);}

//	�R���g���[���̃T�C�Y�ύX
//		����	�E�B���h�E�̎l�_���w��̂���
	inline	void	CTRL_RESIZE(HWND hWnd, uint x0, uint y0, uint x1, uint y1)
	{
		MoveWindow(hWnd, x0, y0, (x1) -	(x0), (y1) - (y0), TRUE);
	}

// �_�C�A���O�����A�E�B���h�E�̒�����
	inline	void	CTRL_WINDOW_RECT_SET(HWND hWnd)
	{
		RECT	r;
		GetWindowRect(hWnd,	&r);
		int		iWidth  = (r.right  - r.left);		// �_�C�A���O�̃T�C�Y�𓾂�
		int		iHeight = (r.bottom - r.top );

		GetWindowRect(sys.hWnd,	&r);
		int		w  = (r.right  - r.left) / 2;
		int		h  = (r.bottom - r.top ) / 2;
		int		x0 =  r.left + w - iWidth  / 2;
		int		y0 =  r.top	 + h - iHeight / 2;
		MoveWindow(hWnd, x0, y0, iWidth, iHeight, TRUE);
	}

// �_�C�A���O�����A�E�B���h�E�̒�����
	inline	void	CTRL_WINDOW_RECT_SET(HWND hWnd, int iWidth, int iHeight)
	{
		RECT	r;
		GetWindowRect(sys.hWnd,	&r);
		int		w  = (r.right  - r.left) / 2;
		int		h  = (r.bottom - r.top ) / 2;
		int		x0 =  r.left + w - iWidth  / 2;
		int		y0 =  r.top	 + h - iHeight / 2;
		MoveWindow(hWnd, x0, y0, iWidth, iHeight, TRUE);
	}

// �e�R���g���[���̃t�H���g�ύX
	inline	void	CTRL_FONT_SET(HWND hWnd, HFONT hFont)
	{
		SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)); 
	}

// ���荞�݃C�x���g������������
	inline	WNDPROC	CTRL_PROC_CHANGE(HWND hWnd, WNDPROC newProc)
	{
												// ���X�̃��b�V�����X�g�r���[�̃v���V�[�W����ۑ����C�x���g���̂��Ƃ�
		WNDPROC	proc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)newProc);

		return proc;
	}

// ���X�g�r���[�̃_�~�[�C�x���g
//	�����X�g�r���[�O�ŁA�}�E�X�̃z�C�[�����܂킵�Ă����X�g���X�N���[�������Ȃ�����
	#define	CTRL_LIST_VIEW_DUMMY_PROC(FuncName, OriProc)									\
		static	LRESULT	CALLBACK	FuncName(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)		\
		{																					\
			switch(msg)																		\
			{																				\
			case WM_KEYDOWN: return NULL; /* �L�[�𖳌��ɂ��� */							\
			case WM_MOUSEMOVE:																\
			case WM_MOUSEWHEEL:																\
			if(pad_mosue_render_range_check()){ return NULL;}								\
				break;																		\
			}																				\
			return CallWindowProc(OriProc, hWnd, msg, wp, lp);								\
		}

// ���X�g�r���[�h���b�O�C�x���g
	#define	CTRL_LIST_VIEW_DRAG_EVENT(Num, FuncName, hListWnd, FuncCall)						\
		static	void		FuncCall(bool bCancel);												\
		static	ushort		usSel_##Num;														\
		static	HANDLE		hCapture_##Num;														\
		static	HIMAGELIST	hImage_##Num;														\
		void	FuncName(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)					\
		{																						\
			switch(uiMsg)																		\
			{																					\
			case WM_LBUTTONUP:				/* ���X�g�r���[�h���b�O�I��			*/				\
			case IDCANCEL:																		\
				if(hCapture_##Num  != NULL)														\
				{																				\
					if(ctrl_list_view_drag_end(&hCapture_##Num, &hImage_##Num, hWnd, hListWnd))	\
					{						/* LBUTTONUP �̂Ƃ��͔z�u���� */					\
						FuncCall(uiMsg == IDCANCEL);											\
					}																			\
				}																				\
				break;																			\
			case WM_MOUSEMOVE:																	\
				if(hCapture_##Num != NULL)														\
				{																				\
					ctrl_list_view_drag_on(hWnd, hListWnd, LOWORD(lParam), HIWORD(lParam));		\
				}																				\
				break;																			\
			}																					\
		}

// ���X�g�r���[�h���b�O�J�n		��CTRL_LIST_VIEW_DRAG_EVENT �ƑΉ�����ԍ��ŌĂяo���̂���
	#define	CTRL_LIST_VIEW_DRAG_BEGIN(Num, hMotherWnd, hListWnd, FuncCall)												\
		{																												\
			if(ctlr_list_view_sel_get(hListWnd) != -1)																	\
			{																											\
				FuncCall;																								\
				ctrl_list_view_drag_begin(&hCapture_##Num, &hImage_##Num, hMotherWnd, hListWnd, (NMLISTVIEW*)lParam);	\
			}																											\
		}

#endif										// end of __CTRL_

