//�G�l�~�[�N���X
class	cEnemy
{
public:
	BOOL	m_bUse;					// TRUE : �g�p��
	BOOL	m_bDead;				// TRUE : ���S
	int		m_iType;				// �G�l�~�[�̎��
	VEC3	m_vecPos;				// �G�l�~�[�̈ʒu
	float	m_fSpeed;				// ���x
	float	m_fDist;				// �ړI�n�܂ł̋���
	float	m_fRot;					// ��]�l
	float	m_fAlp;					// �A���t�@�l

	void	Main(void);				// ���C��
	void	Draw(void);				// �`��
};

//	�ϐ�
extern	cEnemy	*enemy[];

// �ȉ��A�ꊇ�����̊֐�
extern	void	EnemyInit(void);
extern	void	EnemyMain(void);
extern	void	EnemyDraw(void);
