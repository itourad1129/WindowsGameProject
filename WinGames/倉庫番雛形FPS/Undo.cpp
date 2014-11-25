//
//	�A���h�D����
//

#include	"main.h"


#define	UNDO_MAX	100				// �A���h�D�̍ő勉
#define UNDO_SIZE	(MAP_SIZE + 3)

//--- �A���h�D�p ------------------------------------------------------------------

static	char	cUndoDat[UNDO_MAX][UNDO_SIZE];


/*----------------------------------------------------------------------*
| <<< �A���h�D�[���� >>>
|	����	iSw = UNDO_RESET  : ����������
|				= UNDO_SET	  : UNDO �f�[�^�����Z�b�g����
|				= UNDO_GET	  : ���ۂ� UNDO ����
*-----------------------------------------------------------------------*/
void	Undo(int	iSw)
{
	switch(iSw)
	{
		//--- ���������� �ʂ̍ŏ��ɂ��
	case UNDO_RESET:
		memset(cUndoDat, 0, sizeof(cUndoDat));
		break;

	//--- ����i�ނ��Ƃɂ��
	case UNDO_SET:
		// �f�[�^�̈�ԌÂ������������
		memmove(&cUndoDat[1][0], &cUndoDat[0][0], UNDO_SIZE * (UNDO_MAX - 1));

		// UNDO �p�Ƀf�[�^�ݒ�
		cUndoDat[0][0] = (char)(player->GetXPos() / BLK_SIZE);
		cUndoDat[0][1] = (char)(player->GetYPos() / BLK_SIZE);
		cUndoDat[0][2] = (char)player->GetDir();
		memcpy(&cUndoDat[0][3], chMap, MAP_SIZE);
		break;

	//--- �O�̃f�[�^�����o�������Ƃ��Ɏ��s
	case UNDO_GET:
		if(cUndoDat [0][0] == 0){break;}
		player->SetPos((float)(cUndoDat[0][0] * BLK_SIZE), (float)(cUndoDat[0][1] * BLK_SIZE));
		player->SetDir(cUndoDat[0][2]);

		memcpy(chMap, &cUndoDat[0][3], MAP_SIZE);
		memcpy(&cUndoDat[0][0], &cUndoDat[1][0], UNDO_SIZE * (UNDO_MAX -1));
		memset(&cUndoDat[UNDO_MAX - 1][0], 0,	UNDO_SIZE);
		break;
	}
}