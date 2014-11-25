//
// �A�v���P�[�V�����̏�����
//
//

#define	SYSTEM_FILE			"system.ini"	// �E�B���h�E�̃T�C�Y�ۑ��p�t�@�C��
#define	DEBUG_FILE			"DEBUG.TXT"		// �f�o�b�O�p�̃��O�t�@�C��
#define	__URL				""				// �o�[�W�����_�C�A���O�� URL
											// ���ߐF
#define	__COLORKEY			D3DCOLOR_ARGB(255, 0, 0, 0)

#define	SYS_RESOURCE_MAX	8				// ���[�U�[���g�p�ł��郊�\�[�X��
#define	TIME_RATE_MAX		60				// FPS �̌v������t���[����
#define	FILE_BAK			8				// �t�@�C�����̋L����


//---- �ȉ��̒萔�͏��������s�� -------------------------------------------------------------------
#define	DIRECTINPUT_VERSION	0x0800			// DirectInput �̃o�[�W�����i�x���h�~�ׁ̈j

#if !WINVER
	#define	WINVER			0x040A			// Windows 98 &	Windows	2000�ȍ~�@��IME	���֎~����̂ɕK�v
#endif

#define	DEVICE_MAKE_TIME			2		// ���T�C�Y��A�f�o�C�X�쐬�܂ł̎���
#define	ID_MYTIMER					100		// �^�C�}�[�C�x���g
#define	TIMER_COUNT					1000	// �^�C���X�^���v�𒲂ׂ�Ԋu
#define	MONITORS_MAX				16		// �T�|�[�g����ő僂�j�^�[��

#define	_CRT_SECURE_NO_DEPRECATE	1		// VC8�΍��p
#pragma	warning(disable:4996)				// VC8�΍��p
#pragma warning(disable:4793)
#pragma warning(disable:4995)

#ifndef	__META
	#define		__META		__MQO			// ���^�Z�R�C���ύX
#endif


//--- �E�B���h�E�X�^�C�� --------------------------------------------------------------------------
#define	WINDOW_STYLE0		CS_DBLCLKS
							// CS_DBLCLKS      : �_�u���N���b�N����
							// CS_NOCLOSE      : ����{�^������

							// CS_HREDRAW	   : ���T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`�悷��
							// CS_VREDRAW	   : �c�T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`�悷��
							// CS_NOCLOSE	   : �V�X�e�����j���[�́m����n�R�}���h���g�p�s�ɂ���
							// CS_CLASSDC	   : ���̃N���X�̃E�C���h�E���m�łP�̃f�o�C�X�R���e�L�X�g�����L����
							// CS_OWNDC		   : �E�C���h�E���Ƀf�o�C�X�R���e�L�X�g���P�Â���
							// CS_PARENTDC     : �e�E�C���h�E�̃f�o�C�X�R���e�L�X�g���g�p����
							// CS_GLOBALCLASS  : ���̃N���X���쐬�����A�v���P�[�V���������s����Ă���΁C���̃A�v���P�[�V��������ł����̃N���X�𗘗p�ł���悤�ɂȂ�D��ʓI��DLL�ŃJ�X�^���R���g���[�����쐬����Ƃ��Ɏg���炵���D

#define	WINDOW_STYLE1		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
							// WS_OVERLAPPED   : �^�C�g���o�[�A���E�����I�[�o�[���b�v�E�C���h�E
							// WS_CAPTION      : �^�C�g���o�[(WS_DLGFRAME���ꏏ�ɂ��Ă���)
							// WS_SYSMENU      : �^�C�g���o�[�ɃV�X�e�����j���[(����{�^���ƃA�C�R��)����
							// WS_THICKFRAME   : �T�C�Y�ύX�ł��鋫�E��
							// WS_MINIMIZEBOX  : �V�X�e�����j���[�ɍŏ����{�^��������
							// WS_MAXIMIZEBOX  : �V�X�e�����j���[�ɍő剻�{�^��������
							// WS_VISIBLE      : ������Ԃŕ\�������

							// WS_CHILD		   : �q�E�C���h�E�DWS_POPUP�Ƃ͕��p�ł��Ȃ�
							// WS_POPUP		   : �|�b�v�A�b�v�E�C���h�E�DWS_CHILD�Ƃ͕��p�ł��Ȃ�
							// WS_CLIPCHILDREN : �e�̕`��Ŏq�E�C���h�E�̕������N���b�s���O
							// WS_CLIPSIBLINGS : �q�̕`��ŌZ��E�C���h�E�̕������N���b�s���O
							// WS_BORDER	   : �ׂ����E��
							// WS_DLGFRAME	   : �T�C�Y�ύX�ł��Ȃ����E��
							// WS_HSCROLL	   : horizon  scroll(���������X�N���[��)
							// WS_VSCROLL	   : vertical scroll(���������X�N���[��)
							// WS_DISABLED	   : ������Ԃ��g�p�s��
							// WS_MAXIMIZE	   : ������Ԃōő剻
							// WS_MINIMIZE	   : ������Ԃōŏ���
							// WS_TABSTOP	   : �_�C�A���O�̃R���g���[���ɂ����DTab�Ńt�H�[�J�X�𓖂Ă���悤�ɂ���D
							// WS_GROUP		   : ��Ƀ_�C�A���O�̃��W�I�{�^���̃O���[�v�ݒ�Ɏg��

//--- �C���N���[�h --------------------------------------------------------------------------------
#include	<stdlib.h>
#include	<stdio.h>

#include	<stddef.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<ctype.h>
#include	<stdarg.h>
#include	<string.h>
#include	<io.h>
#include	<fcntl.h>
#include	<direct.h>
#include	<windows.h>
#include	<windowsx.h>
#include	<time.h>
#include	<zmouse.h>						// WM_MOUSEWHEEL���t�b�N���邽�߂ɕK�v
#include	<imm.h>							// IME ���֎~����̂ɕK�v
#include	<commctrl.h>					// �R���g���[��
#include	<shlobj.h>
#include	<shlwapi.h>						// PathFileExists �p
#include	<process.h>						// �X���b�h

#ifdef	__MOVIE
	#include	<streams.h>
	#include	<atlbase.h>
#endif

#include	<d3d9.h>
#include	<d3dx9.h>

#define DWORD_PTR DWORD

#include	<dmusici.h>
#include	<dinput.h>
#include	<dshow.h>


//--- �u������ ------------------------------------------------------------------------------------
// �ϐ�
typedef	unsigned	int		ulong;
typedef	unsigned	int		uint;
typedef	unsigned	short	ushort;
typedef	unsigned	char	uchar;

typedef	D3DXVECTOR2		VEC2;				// �Q�c�x�N�g��
typedef	D3DXVECTOR3		VEC3;				// �R�c�x�N�g��
typedef	D3DXVECTOR4		VEC4;				// �S�c�x�N�g��
typedef	D3DXMATRIX		MTX;				// �s�� (D3DXMATRIX ��荂���ɓ��삷��)
typedef	D3DXQUATERNION	QTN;				// �N�H�[�^�j�I��
typedef	D3DXPLANE		PLANE;				// ��
typedef	D3DMATERIAL9	MATERIAL;			// �}�e���A��
typedef	D3DXCOLOR		COL4;				// �J���[
struct	COL									// �W�r�b�g�J���[�̍\����
{
	uchar	r, g, b, a;
};

//--- �萔 ----------------------------------------------------------------------------------------
// �r�b�g
#define	BIT0				1
#define	BIT1				2
#define	BIT2				4
#define	BIT3				8
#define	BIT4				16
#define	BIT5				32
#define	BIT6				64
#define	BIT7				128
#define	BIT8				256
#define	BIT9				512
#define	BIT10				1024
#define	BIT11				2048
#define	BIT12				4096
#define	BIT13				8192
#define	BIT14				16384
#define	BIT15				32768

//--- �X�C�b�` ------------------------------------------------------------------------------------
#ifdef	__FULLSCREEN_2D_RESIZE
	#define	__2D_RESIZE
#endif
#ifdef	__2D_RESIZE
	#undef	__RENDER_FIXATION
	#define	__RENDER_FIXATION
#endif

#ifdef	__COLLISION
	#undef	__CALC
	#define	__CALC
#endif


//--- �}�N�����낢�� ------------------------------------------------------------------------------
#undef	SAFE_RELEASE
#define	SAFE_RELEASE(x)	{if(x){x->Release();x=NULL;}}

#undef	SAFE_DELETE
#define	SAFE_DELETE(p)	{if(p){delete(p); (p)=NULL;}}

#undef	SAFE_DELETE_ARRAY
#define	SAFE_DELETE_ARRAY(p) {if(p){delete[](p); (p)=NULL;}}

// �S�p�E���p��
#define	ISKANJI(c)	((uchar)(c)>=0x81 && (uchar)(c)<=0x9F || (uchar)(c)>=0xE0 && (uchar)(c)<=0xFC)

// ����
#define	SWAP( __a_,	__b_)	{int   __c_; __c_ =	__a_; __a_ = __b_; __b_	= __c_;}
#define	SWAPF(__a_,	__b_)	{float __c_; __c_ =	__a_; __a_ = __b_; __b_	= __c_;}

// ����
#define	RAND(x)				(int)(	 rand()	% ((int)x))
#define	FRAND(x)			((float)(rand()	* (x)) / RAND_MAX)
#define	RAND2( a, b)		(((b) -	(a)) ==	0 ?	a :	(RAND((	b) - (a)) +	a))
#define	FRAND2(a, b)		(((b) -	(a)) ==	0 ?	a :	(FRAND((b) - (a)) +	a))

// a b �Ԃ̋����� c	�̎��Ԃňړ�����Ƃ��̑��x��Ԃ�
#define	VALUE_TO_SPEED(	____a, ____b, ____c)										\
		((____a) > (____b))	? (((____a)	- (____b)) / (____c)) :	(((____b) -	( ____a)) /	(____c))
		
// �l��ڕW�l�֋߂Â���}�N��(a �� b �� c �̑��x�ŋ߂Â���)
#define	VALUE_TO_TARGET(____a, ____b, ____c)														\
		if(		(____a)	< (____b)){	(____a)	+= (____c);	if((____a) > (____b)) (____a) =	(____b);}	\
		else if((____a)	> (____b)){	(____a)	-= (____c);	if((____a) < (____b)) (____a) =	(____b);}

// �Ώە��Ƃ̊p�x�𓾂�
// ����	p0		   = ��
//		p1		   = ��
// �o��	rho, pitch = �p�x
inline	void	GET_ANGLE(float *yaw, float *pitch, VEC3 *p1, VEC3 *p0)
{
	VEC3	tmp	= *p1 -	*p0;
	D3DXVec3Normalize(&tmp,	&tmp);
	*yaw   = atan2f(tmp.x, tmp.z);
	*pitch = -tmp.y	* D3DX_PI /	2;
}

//--- �V�X�e���p���[�N ----------------------------------------------------------------------------
struct	SYS_WORK
{
	char		strName[256];					// ���̃Q�[���̃L���v�V����
	char		strClass[32];					// �N���X��
	bool		bActive;						// �A�N�e�B�u�Ȃ� true
	bool		bMain;							// app_main	�����s���Ȃ�� true	�����j���[�Ń��C���������Ď�����̂Ɏg�p
	char		cWindowStyle;					// �E�B���h�E�X�^�C��(SIZE_MAXIMIZED or	SIZE_MINIMIZED or SIZE_RESTORED)
	char		cResizeCount;					// �E�B���h�E���T�C�Y�p�J�E���^�[
	bool		bPause;							// �V�X�e�����ꎞ��~
	bool		bStartUp;						// true : �V�X�e���̏��������ł���

	int			iTimeCount;						// �V�X�e���J�E���^�[
	DWORD		dwTimeLast;						// ���Ԍv���p
	DWORD		dwTimeMod;						// ���Ԍv���p	�]����J�E���g
	bool		b30Frame;						// true �Ȃ�30�t���[���ɂȂ�
	bool		bFrameSkip;						// true	�Ȃ�`��X�L�b�v����
	bool		bFramelateMessage;				// true	�Ȃ�t���[�����[�g���v���p�e�B�ɕ\���K

	#ifdef	__SHOW_FPS
		DWORD	dwTimeRate[TIME_RATE_MAX];
		int		iTimeRateCount;
	#endif

	HINSTANCE	hInst;
	HWND		hWnd;							// �E�B���h�E�̃n���h��
	HACCEL		hAccel;							// �A�N�Z�����[�^�[�e�[�v���̃n���h��
	HMODULE		hResource[SYS_RESOURCE_MAX];	// ���\�[�X
	HMENU		hMenu;							// ���j���[�̃n���h��
	bool		bMenu;							// true	: ���j���[���ڕ\�����Ă���
	bool		bScreenChange;					// �X�N���[���`�F���W�t���O
	bool		bTopMost;						// ���C�����[�v���A�N�e�B�u���ɂ����삳���A�X�N���[������ɑO�ʂɒu��
	bool		bAlwayExec;						// ���C�����[�v���A�N�e�B�u���ɂ����삳����
	uchar		ucFileWatchReload;				// 0 != �t�@�C���Ď��ɂ�郊���[�h����������	1 : �e�N�X�`���[�A2 : ���b�V��
	ushort		usFileWatchNum;					// �t�@�C���Ď��ɂ�郊���[�h�����������t�@�C���ԍ�

	bool		bMouseShow;						// �}�E�X�̏��
	int			iEscSkip;						// ESC �L�[���X�L�b�v�����

	char		strExeFolder[		 MAX_PATH +	1];	// �C���X�g�[���t�H���_�[
	char		strFile[			 MAX_PATH +	1];	// �v���W�F�N�g���i�t���p�X�j
	char		strSelFile[			 MAX_PATH +	1];	// �I�𒆂̃t�@�C����
	char		strFileBak[FILE_BAK][MAX_PATH +	1];	// �ߋ��̎g�p�t�@�C��

	bool		bAfterLoad;						// true : �����ǂݍ���

	//--- ���j�^�[�֌W ----------------------------------------------
	int			iWidth,	iHeight;				// �E�B���h�E�̃T�C�Y(���̃^�u���܂�)
	float		fXZoom,	fYZoom;					// �E�B���h�E�̃Y�[���l
	void		*funcResize;					// ���T�C�Y���ɌĂяo�����֐���o�^����Ƃ���
	RECT		rectWindow;						// �E�B���h�E�̃T�C�Y(���݂̒l�������Ă���)
	RECT		rectWindowRestore;				// �E�B���h�E�̃T�C�Y(�ő剻�O�̒l�������Ă��܂�)
	bool		bPrimary;						// true : �v���C�}���[��ʂɃE�B���h�E������Ă���

	//--- �R�}���h���C�� --------------------------------------------
	PSTR		szCmdLine;

	//--- �G���[�p --------------------------------------------------
	bool		bError;							// true	: �G���[����
	char		strErr[2048];					// �G���[���b�Z�[�W
	void		*funcError;						// �G���[���ɌĂяo�����֐���o�^����Ƃ���
};


//--- �O���Q�Ɛ錾 --------------------------------------------------------------------------------
	extern	SYS_WORK	sys;
	extern	bool		sys_error(char*	Format,...);	// �G���[���b�Z�[�W�\���֐�
	extern	void		sys_printf(char* Format,...);	// �V�X�e�����b�Z�[�W�\���֐�
	extern	bool		sys_file_select(int	sw,	char *s, char *f);
	extern	uint		sys_window_style_get(void);
	extern	void		sys_mouse_show(bool	bSW);
	extern	bool		sys_timestamp_get(FILETIME *pOut, char *strFile);
	extern	void		sys_file_watch_main(void);
	extern	void		sys_file_watch_init(void);
	extern	void		sys_file_watch_free(void);
	extern	int			sys_message_box(char *pstrMes, char *pstrCaption, UINT uiType);	// ���Ȃ炸�E�B���h�E�̒����ɕ\������郁�b�Z�[�W�{�b�N�X
	// �V�X�e���̈ꎞ�x�~�ݒ�
	extern	void		sys_pause(void);

//--- �C���N���[�h --------------------------------------------------------------------------------
	// ��������ړ������Ă͂����܂���
	#ifdef	__AFONT
		#include	"ex\\_afont.h"
	#endif

	#ifdef	__D3
		#include	"system\\_d3.h"
		#include	"system\\_d3_mesh.h"
	#else
		struct	D3_WORK
		{
			LPDIRECT3D9				d;						// Direct3D	�̌�
			LPDIRECT3DDEVICE9		dev;					// �f�o�C�X
			bool					bPaintPresent;			// WM_PAINT ���ɉ摜�̓]�����s��(�����ltrue);
			RECT					*rectRender;			// �����_�����O���̓]����͈͂������ϐ��̃|�C���^(NULL�Ȃ�S���)
			bool					bWindow;				// �E�B���h�E���[�h�Ȃ�	true
			bool					bWindowBak;				// __EXIT_MENU �L�����Ɏg�p����܂�
			short					sRenderW, sRenderH;		// �����_�����O�T�C�Y
			bool					bDeviceChange;			// true : �f�o�C�X�ύX��
		};

		extern	D3_WORK	d3;

		// �_�~�[
		inline	void		d3_screen_change(bool bStyle){;}

	#endif


//--- �E�B���h�E�̃L���v�V���� --------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< �E�B���h�E�̃L���v�V�����ݒ� >>>
	|	����	printf �Ɠ����ł�
	*------------------------------------------------------------------------------*/
	extern	void	sys_caption_printf(char *Format, ...);
	#define	SYS_CAPTION_PRINTF	sys_caption_printf


//--- ���j���[�̕\���Ǘ� --------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< �t���X�N���[���̃��j���[�\���Ǘ� >>>
	|	����	bSw = true  : �\���I��
	|				= false : �\���I�t
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_view_set(bool bSw);
	#define	MENU_VIEW	sys_menu_view_set

	//--- ���j���[�X�C�b�`�p�̃}�N�� -------------------------------------
	
	// ���`�F�b�N�^�C�v�̍��ڂ͂�����g������
	/*------------------------------------------------------------------------------*
	| <<< ���j���[���ׂāA�`�F�b�N������ >>>
	|	����	idm = ���j���[ ID
	|			a   = �`�F�b�N���Ɏ��s����}�N��
	|			b   = �`�F�b�N���O�����Ɏ��s����}�N��
	*------------------------------------------------------------------------------*/
	#define	MENU_CHECK(type, a,	b)		 												\
			{																			\
				if(GetMenuState(sys.hMenu, (type), MF_BYCOMMAND) & MF_CHECKED)			\
				{																		\
					CheckMenuItem(sys.hMenu, (type), MF_BYCOMMAND | MF_UNCHECKED);		\
					(b);																\
				}																		\
				else																	\
				{																		\
					CheckMenuItem(sys.hMenu, (type), MF_BYCOMMAND | MF_CHECKED);		\
					(a);																\
				}																		\
			}

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�Ƀ`�F�b�N������ >>>
	|	����	idm = ���j���[ ID
	*------------------------------------------------------------------------------*/
	#define	MENU_CHECKED(idm)		CheckMenuItem(sys.hMenu, (idm), MF_BYCOMMAND | MF_CHECKED);

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�̃`�F�b�N���͂��� >>>
	|	����	idm = ���j���[ ID
	*------------------------------------------------------------------------------*/
	#define	MENU_UNCHECKED(idm)	CheckMenuItem(sys.hMenu, (idm), MF_BYCOMMAND | MF_UNCHECKED);

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�Ƀ`�F�b�N��������O������ >>>
	|	����	idm = ���j���[ ID
	|			sw  = true  : �`�F�b�N������
	|				= false : �`�F�b�N���O��
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_set(uint uiIDM, bool bSw);
	#define	MENU_SET	sys_menu_set

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�𖳌��ɂ��� >>>
	|	����	idm = ���j���[ ID
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_disable(uint uiIDM);
	#define	MENU_DISABLE	sys_menu_disable

	/*------------------------------------------------------------------------------*
	| <<< ���j���[��L���ɂ��� >>>
	|	����	idm = ���j���[ ID
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_enable(uint uiIDM);
	#define	MENU_ENABLE	sys_menu_enable

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�̕������ύX���� >>>
	|	����	idm = ���j���[ ID
	|			str = �ύX������
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_str_chage(uint uiIDM, char *str);
	#define	MENU_STR_CHANGE	sys_menu_str_chage

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�ɃZ�p���[�^�[��ǉ� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_separator_add(void);
	#define	MENU_SEPARATOR_ADD	sys_menu_separator_add

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�ɍŋߎg�����t�@�C����ǉ� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_file_make(void);
	#define	MENU_FILE_MAKE	sys_menu_file_make

	/*------------------------------------------------------------------------------*
	| <<< ���j���[�ɍŋߎg�����t�@�C����ǉ� >>>
	|	����	__file = �t�@�C����
	|			b      = true  : �t�@�C����ǉ�
	|				   = false : �����ڂ��폜
	*------------------------------------------------------------------------------*/
	extern	bool	sys_menu_file_add(char *__file,	bool b);
	#define	MENU_FILE_ADD	sys_menu_file_add


//--- �E�G�C�g���� --------------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< �^�C�}�[���Z�b�g���� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_wait_reset(void);
	#define	SYS_WAIT_RESET	sys_wait_reset

	/*------------------------------------------------------------------------------*
	| <<< �^�C�}�[���� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_wait(void);
	#define	SYS_WAIT		sys_wait

	/*------------------------------------------------------------------------------*
	| <<< �X���[�v���� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_sleep(void);
	#define	SYS_SLEEP	sys_sleep


//--- ���p�啶���𔼊p�������� --------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< strlwr �Ɠ��� >>>
	|	����	src = �啶������������
	|	�o��	src = �ϊ�������
	*------------------------------------------------------------------------------*/
	extern	void	sys_strlwr(char *src);
	#define	STRLWR	sys_strlwr


//--- ALT �L�[�������Ƀ��j���[���I����ԂɂȂ邪�A��������� --------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< ALT �������� >>>
	*------------------------------------------------------------------------------*/
	inline	void	SYS_ALT_KEY_OFF(void){ SendMessage(sys.hWnd, WM_SYSKEYUP, 0, 0L);}


//--- ���b�Z�[�W�{�b�N�X --------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< �I�����j���[ >>>
	|	�߂�l	true or false
	*------------------------------------------------------------------------------*/
	extern	bool	sys_exit_menu(void);
	#define	SYS_EXIT_MENU	sys_exit_menu

	/*------------------------------------------------------------------------------*
	| <<< �͂��A��������₤ >>>
	|	����	str = ���b�Z�[�W
	|	�߂�l	true or false
	*------------------------------------------------------------------------------*/
	extern	bool	sys_yes_no(char	*str);
	#define	SYS_YES_NO	sys_yes_no(char	*str)

	/*------------------------------------------------------------------------------*
	| <<< �͂��A�������A�L�����Z����₤ >>>
	|	����	str = ���b�Z�[�W
	|	�߂�l	true or false
	*------------------------------------------------------------------------------*/
	extern	bool	sys_yes_no(char	*str);
	#define	SYS_YES_NO_CANCEL	sys_yes_no


//--- �f�X�N�g�b�v�̃T�C�Y��Ԃ� ------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< �^�X�N�o�[�������T�C�Y�ł��f�X�N�g�b�v�̂w�T�C�Y�𓾂� >>>
	*------------------------------------------------------------------------------*/
	extern	int		sys_desktop_xsize(void);
	#define	SYS_DESKTOP_XSIZE	sys_desktop_xsize

	/*------------------------------------------------------------------------------*
	| <<< �^�X�N�o�[�������T�C�Y�ł��f�X�N�g�b�v�̂x�T�C�Y�𓾂� >>>
	*------------------------------------------------------------------------------*/
	extern	int		sys_desktop_ysize(void);
	#define	SYS_DESKTOP_YSIZE	sys_desktop_xsize


//--- �E�B���h�E���� ------------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< �t���[���̂w�T�C�Y�𓾂� >>>
	|	����	hWnd = �E�B���h�E�n���h��
	*------------------------------------------------------------------------------*/
	extern	int		sys_xframe_get(void);
	extern	int		sys_xframe_get(HWND hWnd);
	#define	SYS_XFRAME_GET	sys_xframe_get

	/*------------------------------------------------------------------------------*
	| <<< �t���[���̂x�T�C�Y�𓾂� >>>		�����C���E�B���h�E����
	|	����	hWnd = �E�B���h�E�n���h��
	*------------------------------------------------------------------------------*/
	extern	int		sys_yframe_get(void);
	extern	int		sys_yframe_get(HWND hWnd);
	#define	SYS_YFRAME_GET	sys_yframe_get

	/*------------------------------------------------------------------------------*
	| <<< �t���[���̃g�b�v���A�G�b�W�A���j���[�A�L���v�V�������������l�𓾂� >>>
	*------------------------------------------------------------------------------*/
	extern	int		sys_topframe_get(void);
	#define	SYS_TOPFRAME_GET	sys_topframe_get

	/*------------------------------------------------------------------------------*
	| <<< �E�B���h�E�T�C�Y��ݒ肷�� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_window_size_set(void);
	#define	SYS_WINDOW_SIZE_SET	sys_window_size_set


//--- �őO�ʂ̏���	-------------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< sys.bTopMost = true ���A�v�����őO�ʂ��� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_window_topmost_set(void);
	#define	SYS_WINDOW_TOPMOST_SET	sys_window_topmost_set


//--- �t�H���_�[���� ------------------------------------------------------------------------------

	#define	__FOLDER_MAX	8					// ����q��
	#define	EXT_MAX			8					// �g���q�̑���(�����w��̏ꍇ�� 8 �܂ł̈Ӗ�)

	/*----------------------------------------------------------------------*
	| <<< �t�@�C�������p�X�ƃt�@�C�����ɕ������� >>>
	|	��	�g���q�������ƃt�H���_�[�����t�@�C�������ɂȂ�̂ŁA
	|		�g���q���Ȃ��ꍇ�́A������\\�𑫂��Ă��������B										
	|	����	*strFile		= �t���p�X�̃t�@�C����
	|	�o��	*strPathOut		= �p�X
	|			*strFileNameOut	= �t�@�C����
	|			*strExtOut		= �g���q��
	*----------------------------------------------------------------------*/
	extern	void	sys_div_path(char *strPathOut, char	*strFileNameOut, char *strExtOut, char *strFile);

	/*----------------------------------------------------------------------*
	| <<< ���݂̃t�H���_�[�ʒu��ۑ����� >>>
	*----------------------------------------------------------------------*/
	extern	bool	sys_folder_push(void);

	/*----------------------------------------------------------------------*
	| <<< SysMakePath ���s�O�̃p�X�֖߂� >>>
	*----------------------------------------------------------------------*/
	extern	bool	sys_folder_pop(void);

	/*------------------------------------------------------------------------------*
	| <<< �t�H���_�[�ʒu��ݒ� >>>
	|	����	*strDir	= �t�H���_�[��
	|			bErr	= true : �G���[���b�Z�[�W�\��
	|	�߂�l	false :	�t�H���_�[�ύX���s
	*------------------------------------------------------------------------------*/
	extern	bool	sys_folder_set(char	*strDir, bool bErr);
	extern	bool	sys_folder_set(char	*strDir = NULL, bool bErr = false);
	#define	FOLDER_SET	sys_folder_set

	/*------------------------------------------------------------------------------*
	| <<< ���݂̃t�H���_�[��ۑ� >>>
	*------------------------------------------------------------------------------*/
	#define	FOLDER_PUSH()	sys_folder_push()

	/*------------------------------------------------------------------------------*
	| <<< FOLDER_PUSH �ŕۑ���������߂� >>>
	*------------------------------------------------------------------------------*/
	#define	FOLDER_POP()	sys_folder_pop()

	/*------------------------------------------------------------------------------*
	| <<< �f�B���N�g���𓾂� >>>
	|	����	lpszFolder	 = ���ׂ�t�H���_�[���i��΃p�X�j
	|			lpszWildCard = ���C���h�J�[�h�i*.txt�Ƃ��j
	|			bSw			 = true	: �T�u�t�H���_�[�����ׂ�
	|	�o��	strOut		 = ��΃p�X��Ԃ�
	*------------------------------------------------------------------------------*/
	extern	void	sys_get_dir(char *strOut, LPCTSTR lpszFolder, LPCTSTR lpszWildCard,	bool bSw);

//--- �t�@�C���f�o�b�O���� ------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< �f�o�b�O�t�@�C���폜 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_debug_clear(void);
	#define	DEBUG_CLEAR	sys_debug_clear
	
	/*------------------------------------------------------------------------------*
	| <<< �f�o�b�O���o�� >>>
	|	����	printf �Ɠ����t�H�[�}�b�g
	*------------------------------------------------------------------------------*/
	extern	void	sys_debug_printf(char *Format,...);
	#undef	DEBUG
	#define	DEBUG	sys_debug_printf



//--- �I������ ------------------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< �����I�� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_end(char *Format, ...);
	#define	SYS_END	sys_end

	/*------------------------------------------------------------------------------*
	| <<< �A�v���ɏI����ʒm���� >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_exit(void);
	#define	SYS_EXIT	sys_exit


//--- �h���b�O���h���b�v --------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< �h���b�O���h���b�v�O���� >>>	��sys.strSelFile �Ƀt���p�X������܂��B
	|	����	wParam = �C�x���g���̈���
	|			strExt = �Ώۊg���q(.*�Ƃ���ΑS�Ẵt�@�C��)
	|			pProc  = �g������(�t�@�C���������̎��g�p�̂���)
	|				   = NULL(������)
	|	�o��	sys.strSelFile = �t���p�X������܂��B
	|	�߂�l	true : �h���b�v����
	*------------------------------------------------------------------------------*/
	extern	bool	sys_drag_drop_begin(WPARAM wParam, char *strExt, void *pProc);
	extern	bool	sys_drag_drop_begin(WPARAM wParam, char	*strExt);
	#define	DRAG_DROP_BEGIN	sys_drag_drop_begin


//--- �L�[���� ------------------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< �L�[���� >>>							��DirectInput�s�v
	*------------------------------------------------------------------------------*/
	extern	int	sys_key_get(int	iCode);


//--- �ȉ��� *.h ���C���N���[�h���悤�I	-----------------------------------------------------------

#include	"ctrl\\_ctrl.h"
#include	"ctrl\\_ctrl_d3.h"
#include	"ctrl\\_ctrl_debug.h"

#ifdef	__D3
	#include	"system\\_skin.h"
	#include	"system\\_shader.h"
#endif

#include	"system\\_pad.h"
#include	"system\\_snd.h"

#ifdef	__TSK
	#include	"ex\\_tsk.h"
	#include	"ex\\_tsk_move.h"
#endif
#ifdef	__NET
	#include	"ex\\_net.h"
#endif
#ifdef	__CAMERA
	#include	"ex\\_camera.h"
#endif
#ifdef	__MQO
	#include	"ex\\_mqo.h"
#endif
#ifdef	__MOVIE
	#include	"ex\\_movie_to_texture.h"
#endif
#ifdef	__PRINTF
	#include	"ex\\_printf.h"
#endif
#ifdef	__IME
	#include	"ex\\_ime.h"
#endif
#ifdef	__TABLET
	#include	"ex\\_tablet.h"
#endif
#ifdef	__CALC
	#include	"ex\\_calc.h"
#endif
#ifdef	__COLLISION
	#include	"ex\\_collision.h"
#endif
#ifdef	__MESH_BUF
	#include	"ex\\_mesh_buf.h"
#endif
#ifdef	__ASORT
	#include	"ex\\_asort.h"
#endif
#ifdef	__FONT_TO_TEXTURE
	#include	"ex\\_font_to_texture.h"
#endif
#ifdef	__FRUSTUM
	#include	"ex\\_frustum.h"
#endif

//--- ������ --------------------------------------------------------
#ifdef	__SCRIPT
	#include	"ex\\_script.h"
	#include	"ex\\_menu.h"
#endif
//-------------------------------------------------------------------

#ifdef	__VERSION
	#include	"ctrl\\_ctrl_version_dialog.h"
#endif

