//
//	�}�b�v
//

#include	"main.h"

//--- �ϐ���` ----------------------------------------------------------
char		chMap[	 MAP_SIZE];			// ���z�}�b�v�̃f�[�^���R�s�[���Ďg�p����
char		chMapBak[MAP_SIZE];			// ���z�}�b�v�̃o�b�N�A�b�v�p
/*------------------------------------------------------------------------------*
| <<< �}�b�v�ǂݍ��� >>>
|	����	iStage = �ǂݍ��ݔԍ�
*-------------------------------------------------------------------------------*/
void	MapLoad(int iStage)
{
	// �ʃf�[�^���R�s�[
	memcpy(chMap,	&chStage[iStage][2], sizeof(chMap));

	// �}�b�v�̃o�b�N�A�b�v�쐬
	memcpy(chMapBak, &chStage[iStage][2], sizeof(chMap));

	// �v���C���[�������W
	player->SetPos((float)chStage[iStage][0] * BLK_SIZE, (float)chStage[iStage][1] * BLK_SIZE);
}

/*------------------------------------------------------------------------------*
| <<< �}�b�v�`�� >>>
*-------------------------------------------------------------------------------*/
void	MapDraw(void)
{
	//RECT	r;

	for(int y = 0; y < MAP_H; y++)		// �c�Ƀ��[�v
	{
		for(int x = 0; x < MAP_W; x++)	// ���Ƀ��[�v
		{
			int		a = 0;
			char	b = chMap[ADR(x, y)];	// �}�N�� ADR ���g���A�h���X�v�Z���A�}�b�v�`�b�v�𓾂�

			float	fX = (float)x * BLK_SIZE + BLK_SIZE / 2.f;
			float	fY = (float)y * BLK_SIZE + BLK_SIZE / 2.f;

			if(b == '@'){ a = X_WALL;}		// ��		�����p�A�b�g�}�[�N
			if(b == 'o'){ a = X_NIMOTU;}		//�ו�
			if(b == 'x'){ a = X_MOKUTEKICHI;}		//�ו����^�Ԉʒu
			if(b == '-'){continue;}		//�Ȃɂ��Ȃ��Ƃ���
			if(b == ' '){continue;}		//�q�ɂ̏�


			MTX		mtxWorld;
			D3DXMatrixTranslation(&mtxWorld, ( fX - __SCR_W / 2 + BLK_SIZE / 2.f) / 56.0f,		// x ���W
												0,												// y ���W
												-( fY - __SCR_H / 2 + BLK_SIZE / 2.f) / 56.0f);// z ���W(�}�C�i�X���t���܂�)
			d3_3d_draw(a, &mtxWorld);

			// �����ŉו��A���A�ړI�n���`�悳���悤�ɏ������Ă�������

			// �摜�؂�o��
			//SetRect(&r, a * BLK_SIZE, 0, a * BLK_SIZE + BLK_SIZE, BLK_SIZE);

			// �`��
			
			//d3_spr_draw(TEX_MAP, &r, &VEC3(fX, fY, 0));
		}
	}
}