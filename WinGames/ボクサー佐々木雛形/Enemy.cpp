//
//	�G�l�~�[
//

#include	"main.h"

//--- �萔��` --------------------------------------------------------------------------

#define MOTION_BLEND_TIME	5					// ���[�V�����̐؂�ւ�����(5/60�b)

#define ENE_X				0					// �G�l�~�[ X ���W
#define ENE_Y				-30					//			 Y ���W
#define ENE_Z				55					//			 Z ���W
#define ENE_SCLAE			30.0f				// �G�l�~�[�̑傫��
#define WAIT_TIME			RAND(10, 70)		// �����őҋ@���Ԃ�ݒ肷��
#define ATACK_TIME			(120  /	enemy->m_fLevel)	// �U������
#define ATTACK_SPEED		(0.5f * enemy->m_fLevel)	// �U�����x

#define DAMAGE_TIME			30						// �_���[�W����
#define KNOCKOUT_TIME		120						// �m�b�N�A�E�g����(�R�b)
#define KNOCKOUT_PLAY_SPEED 4						// �m�b�N�A�E�g���̍Đ����x

//--- �ϐ����` ---------------------------------------------------------------------------
cEnemy	*enemy;

/*---------------------------------------------------------------------------*
| <<< ���샊�Z�b�g >>>		���G�l�~�[�̃��[�h�؂�ւ��̍ۂɂ͕K���Ăяo������
|	����	bBlend = TRUE : �u�����h����
*--------------------------------------------------------------------------*/
void	cEnemy::Reset(BOOL bBlend)
{
	m_iMode		= ENE_WAIT;			// �ŏ��͑҂�����n�߂�
	m_iRenda	= 0;				// �G�l�~�[�A�p���`�̘A�Ő������Z�b�g

	float	fBlendTime;				// ���[�V�����u�����h���Ԍv�Z
	if(bBlend){ fBlendTime = MOTION_BLEND_TIME;}
	else      { fBlendTime = 0;}
									// ���[�V�����u�����h�BfBlendTime �̓��[�V�����؂�ւ��̎���
	skin_anim_set(X_ENEMY, m_iMode, fBlendTime);
	m_iAnimCount = WAIT_TIME;		// �A�j���[�V�����^�C���ݒ�
}
/*-----------------------------------------------------------------*
| <<< �ҋ@ >>>
*------------------------------------------------------------------*/
void	cEnemy::Wait(void)
{
	// �A�j���[�V������i�߂�
	//skin_anim_set(X_ENEMY, 5, 30);
	skin_frame_set(X_ENEMY, 1);

	// �A�j���[�V������0�Ȃ烂�[�h��ς���
	if(m_iAnimCount <= 0)
	{
		// 1/3 �̊m���ŁA���p���`�A�E�p���`�A�҂��̂ǂꂩ�ɂ���
		switch(RAND(3))
		{
		//--- ���p���` ----------------------------------------------------
		case 0:
			m_iMode		= ENE_LEFT_PUNCH;
			m_iRenda	= RAND2(1, 5);		// �p���`�̘A�Ő�
			m_iAnimCount = (int)ATACK_TIME; // �A�j���[�V�����^�C��
			break;
		//--- �ҋ@ -------------------------------------------------------
		case 2:
			m_iMode		= ENE_WAIT;
			m_iAnimCount = WAIT_TIME;		// �A�j���[�V�����^�C�������Ō��߂�
			break;
		}									// �A�j���[�V�����؂�ւ��B
		skin_anim_set(X_ENEMY, m_iMode, MOTION_BLEND_TIME);
	}

}
/*----------------------------------------------------------------------------------------------*
| <<< �G�l�~�[�U�� >>>
*------------------------------------------------------------------------------------------------*/
void	cEnemy::Attack(void)
{
	// �A�j���[�V������i�߂�
	skin_frame_set(X_ENEMY, ATTACK_SPEED);

	// �A�j���[�V������0�Ȃ烂�[�h��ς���
	if(m_iAnimCount <= 0)
	{
		m_iRenda--;
		if(m_iRenda > 0)			// �A�Ő����`�F�b�N����
		{
			if(RAND(2) == 0){ m_iMode = ENE_LEFT_PUNCH ;}
			else			{ m_iMode = ENE_RIGHT_PUNCH;}
			m_iAnimCount = (int)ATACK_TIME; // �A�j���[�V������
			skin_anim_set(X_ENEMY, m_iMode, MOTION_BLEND_TIME);
			snd_play(SND_PUNCH);
		}
		else						// �A�Ő��� 0 �Ȃ� MODE_WAIT ��
		{
			Reset(TRUE);			// �������� MODE_WAIT �ɂȂ�܂��B
		}
	}
}
/*--------------------------------------------------------------------*
| <<< �_���[�W��^���� >>>
| ����		fDamage = �_���[�W�l
*----------------------------------------------------------------------*/
void	cEnemy::DamageSet(float fDamage)
{
	//--- �_���[�W�v�Z -----------------------------------------------
	m_fHP		-= fDamage;
	m_iMode		 = ENE_DAMAGE;
	m_iAnimCount = DAMAGE_TIME;

	// ���[�V�������_���[�W�ɐ؂�ւ�
	skin_anim_set(	X_ENEMY, m_iMode, MOTION_BLEND_TIME);
	snd_play(		SND_ENE_DAMAGE);

	//--- �m�b�N�A�E�g�`�F�b�N -----------------------------------
	if(m_fHP <= 0)
	{
		m_iAnimCount = KNOCKOUT_TIME * KNOCKOUT_PLAY_SPEED;
		m_iMode		 = ENE_KNOCKOUT;

		// ���[�V�������m�b�N�A�E�g�ɐ؂�ւ�				�������̊|���Z�Ȃ��Ă��悢�ł�
		skin_anim_set(	X_ENEMY, m_iMode, MOTION_BLEND_TIME * (float)KNOCKOUT_PLAY_SPEED);
		snd_play(SND_ENE_DAMAGE);
	}
}

/*----------------------------------------------------------------------*
| <<< �_���[�W >>>						���G�l�~�[�_���[�W�́~�Q�{�ōĐ������
*-----------------------------------------------------------------------*/
void	cEnemy::Damage(void)
{
	skin_frame_set(X_ENEMY, 2);		// �A�j���[�V������i�߂�
	if(m_iAnimCount <= 0)			// �A�j���[�V�������O�Ȃ烂�[�h��ς���
	{
		Reset(TRUE);				// ��������MODE_WAIT�ɂȂ�܂��B
	}
}

/*---------------------------------------------------------------------*
| <<< �G�l�~�[ >>>
*---------------------------------------------------------------------*/
void	cEnemy::Main(void)
{
	//--- �A�j���[�V���� --------------------------------------------
	m_iAnimCount--;
	
	//--- ���[�h�ʂɏ����𕪊� ---------------------------------------
	switch(m_iMode)
	{
	case ENE_LEFT_PUNCH:		// ���p���`
	case ENE_RIGHT_PUNCH:		// �E�p���`
		Attack();
		break;
	case ENE_WAIT:				// �҂�
		Wait();
		break;
	case ENE_DAMAGE:			// �_���[�W
		Damage();
		break;
	}
}

/*--------------------------------------------------------------------*
| <<< �`�� >>>
*-------------------------------------------------------------------*/
void	cEnemy::Draw(void)
{
	MTX		mtxWorld, mtxScale, mtxRot, mtxTrans;

	D3DXMatrixScaling(		&mtxScale, ENE_SCLAE);		// �g�嗦�͊e���Őݒ�̂���
														// ���W���e���Őݒ�̂���
	D3DXMatrixTranslation(&mtxTrans, &VEC3(ENE_X, ENE_Y, ENE_Z));
	mtxWorld = mtxScale * mtxTrans;
	skin_draw(X_ENEMY, &mtxWorld);
}

//--- �ꊇ���� ----------------------------------------------------------------------------------

/*--------------------------------------------------------------------------*
| <<< �G�l�~�[���샊�Z�b�g >>> ���G�l�~�[�̃��[�h�ؑւ̍ۂɂ͕K���Ăяo������
*---------------------------------------------------------------------------*/
void	EnemyReset(void)
{
	enemy->Reset(FALSE);
	enemy->m_fHP = HP_MAX;
}
/*-------------------------------------------------------------------------------------------
| <<< ������ >>>
*------------------------------------------------------------------------------------------*/
void	EnemyInit(void)
{
	// �������擾
	if(enemy == NULL){ enemy = new cEnemy;}

	enemy->Reset(FALSE);
	enemy->m_fHP		= HP_MAX;				// �G�l�~�[�̗̑�
	enemy->m_fLevel		= 1;					// �G�l�~�[�� AI ���x���ݒ�(1, 2, 4, 8(��))
	enemy->m_fAttackTime = ATACK_TIME;
}

/*------------------------------------------------------------------------------------------*
| <<< �G�l�~�[���C�� >>>
*------------------------------------------------------------------------------------------*/
void	EnemyMain(void)
{
	enemy->Main();
}

/*----------------------------------------------------------------------------------------------*
| <<< �G�l�~�[�`�� >>>
*------------------------------------------------------------------------------------------------*/
void	EnemyDraw(void)
{
	enemy->Draw();
}



