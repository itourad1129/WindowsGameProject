
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
bool	Title(int iTex, int *pMode, int iNext, int iSound)
{
	bool	bRet = false;

	//--- �`�� ----------------------------------------------------------
	d3_tex_set(iTex);
	d3_2d_box_fill_tex_draw(&VEC2(0,0), &VEC2(__SCR_W, __SCR_H));
	
	//---�L�[���̓`�F�b�N------------------------------------------------
	// �X�y�[�X�L�[���}�E�X�����N���b�N�Ń��[�h�؂�ւ�
	if((pad.bKeyClick[KEY_SPACE] || pad.bLeftClick))
	{
		snd_play(iSound);
		bRet	= true;
		*pMode	= iNext;			// �|�C���^���g�����[�h�؂�ւ�
	}

	return bRet;
}

