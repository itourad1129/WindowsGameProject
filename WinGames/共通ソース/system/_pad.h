//
// DirectInput �֌W
//


#ifndef	__PAD_H
#define	__PAD_H

//	�W���C�p�b�g�̓ǂݍ��ݗ�
//
//			�p�b�g�ԍ�		   �{�^�� ID
//				��				��
//		pad.joy[0].bKeyClick[JOY_01]



#define	JOYPAD_MAX			16				// �W���C�p�b�g�̍ő吔

enum										// �W���C�p�b�g�̃{�^����
{
	JOY_UP,
	JOY_DOWN,
	JOY_LEFT,
	JOY_RIGHT,
	JOY_01,
	JOY_02,
	JOY_03,
	JOY_04,
	JOY_05,
	JOY_06,
	JOY_07,
	JOY_08,
	JOY_09,
	JOY_10,
	JOY_11,
	JOY_12,
	JOY_13,
	JOY_14,
	JOY_15,
	JOY_16,
	JOY_MAX
};

enum										// �L�[���`
{
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_NUM0,
	KEY_NUM1,
	KEY_NUM2,
	KEY_NUM3,
	KEY_NUM4,
	KEY_NUM5,
	KEY_NUM6,
	KEY_NUM7,
	KEY_NUM8,
	KEY_NUM9,
	KEY_NUMPERIOD,		// �e���L�[��.
	KEY_NUMPLUS,		// �e���L�[�̃v���X
	KEY_NUMMINUS,		// �e���L�[�̃}�C�i�X
	KEY_NUMDIV,			// �e���L�[�́�
	KEY_NUMMULTI,		// �e���L�[�́~
	KEY_MINUS,			// �|
	KEY_BEKI,			// ^
	KEY_YEN,			// ��
	KEY_AT,				// @
	KEY_LBRACKET,		// [
	KEY_SEMICOLON,		// ;
	KEY_COLON,			// :
	KEY_RBRACKET,		// ]
	KEY_COMMA,			// ,
	KEY_PERIOD,			// .
	KEY_SLASH,			// /
	KEY_BACKSLASH,		// ���L�[�{�[�h����
	KEY_SPACE,
	KEY_NUMENTER,		// �e���L�[�̃G���^�[
	KEY_ENTER,
	KEY_UNDERBAR,		// �A���_�[�o�[

	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_INS,
	KEY_DEL,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_HOME,
	KEY_END,
	KEY_TAB,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_ESC,
	KEY_LCTRL,
	KEY_RCTRL,
	KEY_LALT,			// �� ALT
	KEY_RALT,			// �E ALT
	KEY_BACKSPACE,
	KEY_MAX
};


//--- �}�N�� --------------------------------------------------------------------------------------
#define	PAD_UP_CHECK()			(pad.bKeyPush[KEY_UP   ] || pad.joy[0].bKeyClick[JOY_UP   ])
#define	PAD_DOWN_CHECK()		(pad.bKeyPush[KEY_DOWN ] || pad.joy[0].bKeyClick[JOY_DOWN ])
#define	PAD_LEFT_CHECK()		(pad.bKeyPush[KEY_LEFT ] || pad.joy[0].bKeyClick[JOY_LEFT ])
#define	PAD_RIGHT_CHECK()		(pad.bKeyPush[KEY_RIGHT] || pad.joy[0].bKeyClick[JOY_RIGHT])

#define	PAD_UP_REP_CHECK()		(PAD_KEY_REP(KEY_UP)	|| PAD_KEY_REP(PAD_UP   ))
#define	PAD_DOWN_REP_CHECK()	(PAD_KEY_REP(KEY_DOWN)	|| PAD_KEY_REP(PAD_DOWN ))
#define	PAD_LEFT_REP_CHECK()	(PAD_KEY_REP(KEY_LEFT)	|| PAD_KEY_REP(PAD_LEFT ))
#define	PAD_RIGHT_REP_CHECK()	(PAD_KEY_REP(KEY_RIGHT)	|| PAD_KEY_REP(PAD_RIGHT))

#define	PAD_SHIFT()				(pad.bKeyPush[KEY_LSHIFT] || pad.bKeyPush[KEY_RSHIFT])
#define	PAD_CTRL()				(pad.bKeyPush[KEY_LCTRL ] || pad.bKeyPush[KEY_RCTRL ])
#define	PAD_ALT()				(pad.bKeyPush[KEY_LALT  ] || pad.bKeyPush[KEY_RALT  ])

											// �L�[���s�[�g�`�F�b�N�̃}�N��
#define	PAD_KEY_REP(a)			(pad.bKeyRep[a] == 1    || pad.bKeyClick[a])
											// �}�E�X�̍��W��ݒ�
#define	PAD_MOUSE_XY_SET(x,	y)	SetCursorPos(x,	y)

//--- �\���̒�` ----------------------------------------------------------------------------------
struct	JOY_WORK
{
	LPDIRECTINPUTDEVICE8	dev;					// NULL	�ȊO�Ȃ�΃f�o�C�X�L��
	char					*strName;				// ���O
	char					*strProduct;			// �v���_�N�g��
	bool					bForceFeedback;			// true	: �t�H�[�X�t�B�[�h�o�b�N�Ή�
	bool					bPolling;				// 
	bool					bKeyPush[   JOY_MAX];	// �㉺�ƃ{�^��16�̏��
	uchar					bKeyClick[  JOY_MAX];	// 1 �Ȃ�΂��̃{�^�����������u��
	uchar					bKeyRelease[JOY_MAX];	// �L�[�������ꂽ�u��
	char					cKeyRepTime[JOY_MAX];
	bool					bKeyRep[    JOY_MAX];	// ���s�[�g���
	bool					bKeyBak[    JOY_MAX];	// �O��̃{�^���̒l
	short					sLx, sLy, sLz;			// �A�i���O�X�e�B�b�N�̏��(��)
	short					sRx, sRy, sRz;			// �A�i���O�X�e�B�b�N�̏��(�E)
	DIJOYSTATE2				di;
};

struct	PAD_WORK
{
	short					sXOri, sYOri;			// __2D_RESIZE �X�C�b�`�L������pad.sX,sY�����T�C�Y�����̂ŁA�I���W�i���T�C�Y�ۑ��p
	short					sX,	sY,	sZ;				// �}�E�X�̍��W�i��ʂ̃T�C�Y�v�Z�ς݁j
	short					sXBak,	sYBak,	sZBak;	// �}�E�X�̍��W�O��l	
	short					sMoveX,	sMoveY,	sMoveZ;	// �}�E�X�̈ړ���
	// ���{�^��
	bool					bLeftPush;
	bool					bLeftClick;
	bool					bLeftRelease;
	bool					bLeftDblClick;			// �_�u���N���b�N���
	bool					bLeftRep;				// �L�[���s�[�g���
	char					cLeftRepTime;			// �L�[���s�[�g����
	// �E�{�^��
	bool					bRightPush;
	bool					bRightClick;
	bool					bRightRelease;
	bool					bRightDblClick;
	bool					bRightRep;
	char					cRightRepTime;
	// �^�񒆃{�^��
	bool					bCenterPush;
	bool					bCenterClick;
	bool					bCenterRelease;
	bool					bCenterDblClick;
	bool					bCenterRep;
	char					cCenterRepTime;

	JOY_WORK				joy[JOYPAD_MAX];	// �W���C�X�e�B�b�N�̃��[�N	
	int						iJoyCount;			// �W���C�p�b�g�̐�
	LPDIRECTINPUT8			dev;				// DirectInput �f�o�C�X
	LPDIRECTINPUTDEVICE8	lpMouse;			// �}�E�X
	LPDIRECTINPUTDEVICE8	lpKey;				// �L�[�{�[�h
	DIDEVCAPS				caps;				// �W���C�X�e�B�b�N�̔\��

	uchar					bKeyPush[   KEY_MAX];
	uchar					bKeyBak[    KEY_MAX];
	uchar					bKeyClick[  KEY_MAX];
	uchar					bKeyRep[    KEY_MAX];
	uchar					bKeyRelease[KEY_MAX];	// �L�[�������ꂽ���
	char					cKeyRepTime[KEY_MAX];	// �L�[���s�[�g����
};


//--- �O���Q�Ɛ錾 --------------------------------------------------------------------------------
extern	PAD_WORK	pad;
extern	void		pad_check(void);
extern	void		pad_clear(void);
extern	bool		pad_init(void);
extern	bool		pad_end(void);
extern	void		pad_main(void);
extern	bool		pad_any_key_check(void);
extern	bool		pad_mosue_render_range_check(void);				// �}�E�X�̐�Βl���}�E�X�ʒu�������_�����O�͈͓������ׂ�
extern	void		pad_mosue_pos_get(short *pX, short *pY);


//--- �ȉ��͂R�c�p --------------------------------------------------
extern	void			pad_mouse_ray_get(VEC3 *vecPos, VEC3 *vecDir, MTX *mtxWorld);
extern	void			pad_mouse_pos_get(VEC3 *vecPos, VEC3 *vecDir, MTX *mtxWorld);
extern	bool			pad_mouse_sphere_intersection_check(float fRadius, MTX *mtx);					// ���ƃ}�E�X�̌�������
extern	bool			pad_mouse_mesh_intersection_check(VEC3 *pOut, int iID, int iMat, MTX *mtx);

#ifdef	__D3
	extern	D3_VTX_DEFAULT	*pad_mouse_mesh_intersection_triangle(void);
#endif

#endif			//	__PAD_H
