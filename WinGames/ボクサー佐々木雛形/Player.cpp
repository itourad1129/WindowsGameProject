//
//	�v���C���[
//

#include	"main.h"

//--- �萔��` ---------------------------------------------------------------------------------
#define PUNCH_TIME				180.f				// �p���`�̕\������
#define PUNCH_SPEED				20.f				// �p���`�̑��x
#define PUNCH_Z					40.f				// �p���`��Z�ڐA�l
#define PUNCH_Y					12.f				// �p���`��Y���W
#define PUNCH_X					4.f					// �p���`��X�ڐA�l
#define PUNCH_XOFS				-12.f				// �p���`�̊p�x
#define PUNCH_ANGLE				R(30)				// �p���`�̊p�x

#define COUNTER_START_TIME		5			// �J�E���^�[�� enemy->m_iAnimCount �� COUNTER_START_TIME
#define COUNTER_END_TIME		(enemy->m_fAttackTime - 5)
#define COUNTER_A_POWER			5					// �J�E���^�[���̍U����

//--- �ϐ� -------------------------------------------------------------------------------------
cPlayer *player;									// �v���C���[���[�N

/*--------------------------------------------------------------------------------*
| <<< �p���` >>>
*---------------------------------------------------------------------------------*/
void	cPlayer::Punch(void)
{
	for(int i = 0; i < PLAYER_PUNCH_MAX; i++)
	{//���� ��
												//���� �E
		//--- �p���`���o������ -----------------------------------------
		//
		int	aiKey[] = {KEY_LCTRL, KEY_RCTRL};

		//�@���L�[���͂�����A			���p���`���ĂȂ��A			���h�䂵�ĂȂ����Ƀp���`���łĂ�
		if(pad.bKeyClick[aiKey[i]] && m_afPunchTime[i] == 0 && m_fGuardTime == 0)
		{
			m_afPunchTime[i] = PUNCH_TIME;		// �p���`���Ԃɒl����ꂽ��J�n
			snd_play(SND_PUNCH);
		}

		//--- �p���`��߂����� ------------------------------------------
		VALUE_TO_TARGET(m_afPunchTime[i], 0, PUNCH_SPEED);
		//--- �_���[�W�v�Z ----------------------------------------------
		if(m_afPunchTime[i] > 0)				// �p���`�̃A�j���[�V������i�߂�BPUNCH_TIME �� 0 �ɂȂ�܂ŁB
		{										// 20�͑��x(�傫������Ƒ����Ȃ�)
			CalcEnemyDamage(i);					// �����Ń_���[�W�v�Z
		}
	}
}

/*-----------------------------------------------------------------------------------------*
| <<< �G�l�~�[�֗^����_���[�W�l�v�Z >>>
|	����	iID = 0 : ���p���`
|				= 1 : �E�p���`
*-----------------------------------------------------------------------------------------*/
void	cPlayer::CalcEnemyDamage(int iID)
{
	//						�������ɂ͎����Œ��ׂ��G�l�~�[�Ƀp���`��������l������Ă��������B
	if(m_afPunchTime[iID] != 120){ return;}		// �p���`�� �w��̒l�̎��̂ݔ���

	bool	bCounter = false;					// �J�E���^�[�t���O

	switch(enemy->m_iMode)
	{
	//--- �p���` ----------------------------------------------------------------------
	case ENE_RIGHT_PUNCH:
		if(iID == 0){ bCounter = true;}			// �G�l�~�[�E�p���`�ɂ̓v���C���[��
		break;
	case ENE_LEFT_PUNCH:						// �G�l�~�[���p���`�ɂ̓v���C���[�E
		if(iID == 1){ bCounter = true;}			// �ŃJ�E���^�[����
		break;
	}

	//--- �J�E���^�[�̏ꍇ�A���Ԃ��v�����ăJ�E���^�[�����邩���ׂ� --
	if(bCounter && enemy->m_iAnimCount >= COUNTER_START_TIME  && enemy->m_iAnimCount <= COUNTER_END_TIME)		// �L���Ƃ���
	{
		enemy->DamageSet(COUNTER_A_POWER);			// �G�l�~�[�Ƀ_���[�W��^����
	}
}








/*---------------------------------------------------------------------------------*
| <<< �`�� >>>
*----------------------------------------------------------------------------------*/
void	cPlayer::Draw(void)
{
	d3_stat_alpha_set(true);			// �������I��

	for(int i = 0; i < PLAYER_PUNCH_MAX; i++) // �p���`�̓��[�v���g��
	{
		//--- �O���[�u�`�� ------------------------------------------
		float	z =				 sinf(R(m_afPunchTime[i])) * PUNCH_Z;
		float	x =	PUNCH_XOFS + sinf(R(m_fGuardTime	)) * PUNCH_X;
		float	r = PUNCH_ANGLE;
		if(i == 1){ x *= -1, r *= -1;}		// �E�p���`�͍��E���]

		MTX		mtxWorld, mtxRot, mtxTrans;
		D3DXMatrixRotationY(	&mtxRot,	r);
		D3DXMatrixTranslation(&mtxTrans, &VEC3(x, PUNCH_Y, z));
		mtxWorld = mtxRot * mtxTrans;
		d3_3d_draw(X_LEFT + i, &mtxWorld, 0.8f);// �O���[�u�͔������ɂ���
	}
}
//--- �ꊇ���� -------------------------------------------------------------------------------------
/*--------------------------------------------------------------------------------*
| <<< �v���C���[������ >>>
*---------------------------------------------------------------------------------*/
void	PlayerInit(void)
{
	// ����������
	if(player == NULL){ player = new cPlayer;}

	// ������
	for(int i = 0; i < PLAYER_PUNCH_MAX; i++)
	{
		player->m_afPunchTime[i] = 0;		// �p���`�̃A�j���[�V�������ԃN���A
	}
	player->m_fGuardTime = 0;				// �K�[�h����
	player->m_fHP		 = HP_MAX;			// �̗�
}

/*-------------------------------------------------------------------------------*
| <<< �v���C���[���C�� >>>
*--------------------------------------------------------------------------------*/
void	PlayerMain(void)
{
	player->Punch();			// �p���`
}

/*-------------------------------------------------------------------------------*
| <<< �v���C���[�`�� >>>
*--------------------------------------------------------------------------------*/
void	PlayerDraw(void)
{
	player->Draw();
}

