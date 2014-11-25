//
//	�A���`�G�C���A�X�t�H���g�w�b�_
//

#ifndef	__AFONT_H

#define	__AFONT_H


#define	AFONT_MAX		16					// �A���`�t�H���g����



//--- �A���`�t�H���g�p ----------------------------------------------------------------------------
struct	AFONT_WORK							// �A���`�t�H���g�̃��[�N
{
	bool			bView;						// �`��X�C�b�`
	bool			bUse;						// �g�p�t���O(�����_�����O�ʂ�������̔���Ɏg�p)
	bool			bInit;						// �������t���O
	bool			bKey;						// �L�[�E�G�C�g�҂�
	char			cFontNum;					// �g�p�t�H���g�ԍ�
	ushort			usX0,  usY0;				// �����̏����o���������W
	ushort			usX1,  usY1;

	ushort			usX,   usY;					// ���݂̕����`��ʒu

	short			sOfsW, sOfsH;				// ��������l(�������悤)
	GLYPHMETRICS	gmAnt;
	VEC2			vecP0, vecP1;				// �\�����W
	DWORD			dwCol;						// �����F
	uchar			*pBuf;						// �t�H���g�i�[�p�̃o�b�t�@
	char			*pOriStr;					// ������i�[�p
	char			*pStr;						// pOriStr �̃��[�N�悤�B������̕`��͂�����ōs��
	float			fCount;						// �����`��̃J�E���^�[
	float			fSpeed;						// ������̕`�摬�x(0.0f�Ȃ�Α���)

	short			sScroll;					// �����̃X�N���[���p
	short			sSnd;						// �����\�����̌��ʉ�(SND_NULL�Ŗ���)
	float			fA, fR, fG, fB;				// �����F
};


//--- �O���Q�Ɛ錾 --------------------------------------------------------------------------------
extern	void	afont_free(int iID);
extern	void	afont_free(void);

extern	void	afont_printf(int iID, int iX, int iY, char* Format,...);					// �A���`�t�H���g�ŕ����\��
extern	void	afont_printf(int iID,                 char* Format,...);
extern	void	afont_printf(                         char* Format,...);

extern	void	afont_render(int iTex, int iID, int iX, int iY, int iLen, DWORD dwCol, char* Format,...);
extern	void	afont_draw(float fX, float fY, float fAlp, bool bZoom);
extern	void	afont_draw(float fX, float fY, bool bZoom);
extern	void	afont_draw(bool fZoom);
extern	void	afont_draw(void);
extern	void	afont_draw(float fX, float fY);

extern	void	afont_create(int iID, int iFontNum, int iRenderW, int iRenderH);			// �A���`�t�H���g������
extern	void	afont_create(		 int iFontNum, int iRenderW, int iRenderH);
extern	void	afont_speed_set(int iID, float fSpeed);
extern	void	afont_rect_set(int iID, RECT *r);

/*------------------------------------------------------------------------------*
| <<< ��������T�C�Y�ݒ� >>>	���t�H���g�̃T�C�Y�ł͂Ȃ��A��������ς���
|	����	iID    = �Ǘ��ԍ�
|			iW, iH = ��������l(�����̊Ԋu�����p)
*------------------------------------------------------------------------------*/
extern	void	afont_font_size_set(int iID, int iW, int iH);

extern	void	afont_sound_set(int iID, int iSound);
extern	bool	afont_print_check(int iID);
extern	void	afont_print_sw(int iID, bool bSw);
extern	void	afont_print_sw(bool bSw);


#endif					// end _afont.h


