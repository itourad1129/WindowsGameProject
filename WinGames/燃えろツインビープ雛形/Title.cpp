
//
//�^�C�g���̏���	��������Q�[���I�[�o�[�ɂ��g���܂��B
//

#include	"main.h"


/*---------------------------------------------------------------------*
<<<�^�C�g��>>>
	����	iTex	= �\������e�N�X�`���[�ԍ�
			*pMode  = �l���Z�b�g�������ϐ��̃|�C���^
			iNext   = ���N���b�N���̎��̃��[�h		��pMode �̎��̒l
			iSound  = ���N���b�N���̌��ʉ�
	�߂�l	true	= ���[�h���ς�����u��		���L�[�������ꂽ�u��
*-----------------------------------------------------------------------*/

//--- �萔 ---------------------------------------------------------------
#define FADE_SPEED_		20.f			// �t�B�[�h���x

//--- �ϐ� ---------------------------------------------------------------
static	float	fCount = 0.0f;			// �^�C�g���t�B�[�h�p
static	int		iFade	= -1;			// -1 : �t�B�[�h�C��
										// 0  : �Ȃɂ����Ȃ�
										// +1 : �t�B�[�h�A�E�g

bool	Title(int iTex, int *pMode, int iNext, int iSound)
{
	bool	bRet = false;

	//--- �`�� ----------------------------------------------------------
	d3_tex_set(iTex);
	d3_2d_box_fill_tex_draw(&VEC2(0,0), &VEC2(__SCR_W, __SCR_H), D3_COL((int)fCount, 255, 255, 255));
	
	//---�L�[���̓`�F�b�N------------------------------------------------
	// �X�y�[�X�L�[���}�E�X�����N���b�N�Ń��[�h�؂�ւ�
	if((pad.bKeyClick[KEY_SPACE] || pad.bLeftClick) && iFade == 0)
	{
		iFade = 1;						//�����Ȃ胂�[�h��ς����ɁA�t�B�[�h�J�n
		snd_play(iSound);
		//bRet	= true;					//�R�����g
		//*pMode	= iNext;			//�R�����g
	}

	//--- �t�B�[�h -----------------------------------
	switch(iFade)
	{
	case -1:							// �C��
		fCount += FADE_SPEED_;
		if(fCount >= 255.0f){ fCount = 255.0f; iFade = 0;}
		break;
	case 1:
		fCount -= FADE_SPEED_;
		if(fCount <= 0.0f)
		{
			fCount = 0.0f;
			iFade  = -1;

			*pMode = iNext;				// �t�F�[�h�A�E�g������������
			bRet	= true;				// �I���Ƃ���
		}
		break;
	}
	return bRet;
}

/*-----------------------------------------------------------------------------*
| <<< ���j���[�g�p���̃^�C�g�� >>>
|	����	iTex	= �\������e�N�X�`���[�ԍ�
|			iSound	= ���N���b�N���̌��ʉ�
|			bSw		= true : �t�F�[�h�A�E�g�J�n
|	�߂�l	true	= ���[�h���ς����
*-------------------------------------------------------------------------------*/
bool	TitleMenu(int	iTex,	int	iSound,	bool bSw)
{
	bool	bRet	= false;

	//--- 640�~480 �h�b�g�̊G��\�� --------------------
	d3_tex_set(iTex);
	d3_2d_box_fill_tex_draw(&VEC2(0, 0), &VEC2(__SCR_W, __SCR_H), D3_COL((int)fCount, 255, 255, 255));

	//--- �L�[���̓`�F�b�N --------------------------------------
	if(bSw)						// �����ATitle�֐��ƈႤ
	{
		iFade = 1;
		snd_play(iSound);
	}
	//--- �t�B�[�h ------------------------------------
	switch(iFade)
	{
	case -1:		//�C��
		fCount += FADE_SPEED_;
		if(fCount >= 255.0f){ fCount = 255.0f; iFade = 0;}
		break;
	case 1:			//�A�E�g
		fCount -= FADE_SPEED_;
		if(fCount <= 0.0f)
		{
			fCount = 0.0f;
			iFade  = -1;
			
				bRet	= true;
		}
		break;
	}
	return bRet;
}