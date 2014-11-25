//
//	���e�I(覐�)
//

#include	"main.h"

//--- �萔���` ----------------------------------------------------
#define BELL_START		60.0f			// 覐΂̃X�^�[�g�ʒu
#define	BELL_END		-13.0f			// 覐΂̏I���ʒu
#define SPEED_MIN		0.1f			// 覐΂̍Œᑬ�x
#define SPEED_MAX		0.5f			// 覐΂̍ō����x
#define FADE_IN_SPEED	0.01f			// �o�^���̃t�F�[�h�C�����x
#define	DELAY_TIME		240				// 覐Γo�^���̒x������
#define DAMAGE_TIME		60				// �v���C���[���_���[�W���ɂ��邭���鎞��

//--- �ϐ����` -----------------------------------------------------------
static	int	iOperationMax;				// 覐΂̏�����
cBell	*bell;

/*--------------------------------------------------------------------*
|	<<< ������ >>>
|	����	iNum = �Ǘ��ԍ�
*--------------------------------------------------------------------*/
void	cBell::Init(int iNum)
{
	m_vecPos.x	= (iNum % 3) * 3.0f -3.0f; // �J�n�ʒu�ݒ�
	m_vecPos.y	= BELL_START;
	m_vecPos.z	= 0;
	m_fAlp		= 0;
	m_iDelay	= DELAY_TIME * (iNum + 1);	// �x�����Ԑݒ�
	m_iDelay	/= 4;						// �ŏ��͂����o�Ă���悤�Ɋ����Ă݂�
}

/*--------------------------------------------------------------------
| <<< ���C�� >>>
*------------------------------------------------------------------*/
void	cBell::Main(void)
{
	//--- �x�����Ԍv�Z -----------------------------------------
	if(m_iDelay > 0)
	{
		m_iDelay--;
	}
	//--- �ړ� -------------------------------------------------
	else
	{
		//--- ���������[�g�v�Z ---------------------------------
		// �ړ��̑O�ɏ������Ȃ��ƁA��ʒ[�Ō�����ď����ʒu�ɖ߂������ɏ����t�F�[�h�C�����Ă��܂�
		VALUE_TO_TARGET(m_fAlp, 1.0f, FADE_IN_SPEED);

		//--- �ړ����� -----------------------------------------
		// �X�^�[�g�ʒu���H�X�^�[�g�ʒu�Ȃ�Α��x���v�Z
		if(m_vecPos.y == BELL_START){ m_vecSpeed.y = FRAND2(SPEED_MIN, SPEED_MAX);}

		// �ړ�
		m_vecPos.y -= m_vecSpeed.y;

		// ��ʂ̉��[�Ō��؂ꂽ���H
		if(m_vecPos.y < BELL_END)
		{
			m_vecPos.y = BELL_START;		// �����ʒu��
			m_fAlp		= 0;				// �����ɂ���
		}
	
		//--- �����蔻�� -----------------------------------------------
		if(collision_sphere_sphere(
				&player->m_vecPos,	d3.hitWork[X_PLAYER].fRadius * 0.25f,
				&m_vecPos,					d3.hitWork[X_BELL ].fRadius * 0.5f))
		{
			player->m_iScore = player->m_iScore + 100;
			// �����Ńp�[�e�B�N�����Ăяo���Ă�������(������)
			//ParticleStart(&VEC3(m_vecPos.x,m_vecPos.y,m_vecPos.z));
			m_vecPos.y			= BELL_START;		// 覐΂������ʒu�֖߂�
			m_iDelay			= RAND(DELAY_TIME);	// �x�����Ԃ̐ݒ�
			snd_play(SND_DAMAGE);
			
		}
	}
		//--- �s��v�Z -------------------------------------------------
		MTX		mtxTrans,	mtxRot;
										// ���s�ړ�
		D3DXMatrixTranslation(&mtxTrans, &m_vecPos);
											// X��]
		D3DXMatrixRotationX(	&mtxRot,	R(m_vecPos.y * 10));
		m_mtxWorld = mtxRot * mtxTrans;			// �s��̊|�����킹
	
}

/*----------------------------------------------------*
| <<< �`�� >>>
*----------------------------------------------------*/
void	cBell::Draw(void)
{
if(m_iDelay == 0)
	{
		d3_3d_draw(X_BELL, &m_mtxWorld, m_fAlp);
	}
}

//--- �ȉ��A�ꊇ���� ------------------------------------------------------------------------
/*----------------------------------------------------------*
| <<< 覐Ώ����� >>>
*-----------------------------------------------------------*/
void	BellInit(void)
{
	// �������m��
	if(bell == NULL)
	{
		bell = new cBell[BELL_MAX];
	}

	// ������
	for(int i = 0; i < BELL_MAX; i++)
	{
		bell[i].Init(i);
		
	}
}

/*--------------------------------------------------------------------*
| <<< 覐΃��C�� >>>
*-------------------------------------------------------------------*/
void	BellMain(void)
{
	// 覐΂̏�����
	iOperationMax = 3;

	// 覐΂̐����A���C������
	for(int i = 0; i < iOperationMax; i++)
	{
		bell[i].Main();
	}
}

/*--------------------------------------------------------------*
| <<< 覐Ε`�� >>>
*---------------------------------------------------------------*/
void	BellDraw(void)
{

	d3_stat_alpha_set(true);				// �������I��
	for(int	i = 0; i < iOperationMax; i++)
	{
		bell[i].Draw();
	}
}