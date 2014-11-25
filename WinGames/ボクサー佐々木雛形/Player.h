#define PLAYER_PUNCH_MAX		2					// �v���C���[�̌��̐�
// �v���C���[�N���X
class		cPlayer
{
public:
	float	m_fHP;									// �̗�

	float	m_afPunchTime[PLAYER_PUNCH_MAX];		// �p���`�̃A�j���[�V�����p���E����B
	float	m_fGuardTime;							// �K�[�h����

	void	CalcEnemyDamage(int iID);				// �G�l�~�[�֗^����_���[�W�v�Z
	void	Guard(void);							// �K�[�h
	void	Punch(void);							// �p���`

	void	Draw(void);								// �`��
	void	DamageSet(float fDamage);				// �v���C���[�փ_���[�W��^����
};

extern	cPlayer *player;							// �v���C���[���[�N
extern	void	PlayerInit(void);
extern	void	PlayerMain(void);
extern	void	PlayerDraw(void);
