
#ifndef	__TABLET_
#define	__TABLET_

//
//	�^�u���b�g����
//


#include	<wintab.h>


#define	TABLET_PRESSURE_RESOLUTION	1024			// �M���̕���\

struct	TABLET_WORK						// �^�u���b�g���[�N
{
	bool	bPush;							// true : �^�u���b�g�Ƀy�������Ă���
	int		iX, iY;							// �^�u���b�g�̂w�x���W
	int		iPressure;						// �M��

	// �ȉ��A�J�b�g
	//bool	bClick;							// true : �^�u���b�g�Ƀy���������u��
	//bool	bRelease;						// true : �^�u���b�g����y���𗣂����u��
	//bool	bTabletBak;						// bTabletClick �v�Z�p
	//int	iTabletMoveX, iTabletMoveY;		// �y���̈ړ���
	//int	iTabletXBak,  iTabletYBak;		// iTabletMoveX, iTabletMoveY �v�Z�p
};


extern	TABLET_WORK	tablet;


/*------------------------------------------------------------------------------*
| <<< �^�u���b�g������ >>>
|	�߂�l	�^�u���b�g�R���e�N�X�g�̃n���h��
*------------------------------------------------------------------------------*/
extern	HCTX	TabletInit(void);



/*------------------------------------------------------------------------------*
| <<< �^�u���b�g�̃C�x���g >>>
|	����	hWnd		   = �E�B���h�E�n���h��
|			uiMsg		   = ���b�Z�[�W
|			wParam, lParam = �p�����[�^�[
*------------------------------------------------------------------------------*/
extern	void	TabletProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);



//extern	HCTX	TabletInit2(void);
//extern	void	TabletReceive(WPARAM wParam, LPARAM lParam);
//extern	void	TabletMouse(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);


#endif	// __TABLET_

