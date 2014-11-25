// �G�l�~�[�N���X
class	cEnemy
{
public:
	float	m_fAttackTime;				// �U������
	int		m_iAnimCount;				// �A�j���[�V�����J�E���^�[
	int		m_iRenda;					// �p���`�̘A�Ő�
	int		m_iMode;					// ���[�h
	float	m_fHP;						// �̗�
	float	m_fLevel;					// ��Փx

	void	Reset(BOOL bMotBlend);		// ���Z�b�g
	void	Wait(void);					// �҂�
	void	Attack(void);				// �U��
	void	Damage(void);				// �_���[�W
	void	KnockOut(void);				// �m�b�N�A�E�g
	void	Groggy(void);				// �O���b�L�[
	void	Main(void);					// ���C��
	void	Draw(void);					// �`��
	void	DamageSet(float fDamage);	// �_���[�W��^����
	void	GroggyStart(void);			// �O���b�L�[�J�n
};

enum	ENE_MODE				// �G�l�~�[�̍U�����[�h �G���t���C�i�̃A�j���[�V�������������������߁A����ւ��܂��B
{
	ENE_DAMAGE,					// �_���[�W
	ENE_KNOCKOUT,				// �m�b�N�A�E�g
	ENE_GROGGY,					// �O���b�L�[
	ENE_LEFT_PUNCH,				// ���p���`
	ENE_RIGHT_PUNCH,			// �E�p���`
	ENE_WAIT,					// �҂�
};

extern cEnemy	*enemy;			// �G�l�~�[���[�N
extern	void	EnemyInit(void);
extern	void	EnemyMain(void);
extern	void	EnemyDraw(void);