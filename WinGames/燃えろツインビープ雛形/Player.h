// �v���C���[�N���X
class	cPlayer
{
public:
	VEC3	m_vecPos;					// �ʒu
	VEC3	m_vecSpeed;					// �ړ����x
	MTX		m_mtxWorld;					// ���[���h�s��
	VEC3	m_vecBak;					// �O����W
	int		m_iDamageTime;				// �_���[�W���󂯂Ă��鎞��
	void	Init(void);					// ������
	void	Main(void);					// ���C��
	void	Draw(void);					// �`��
	void	Damage(void);				// �_���[�W���A�J�����O���O��
	int		m_iHp;						//�v���C���[HP
	int		m_iScore;					//�X�R�A
};

extern	cPlayer	*player;

extern	void	PlayerInit(void);
extern	void	PlayerMain(void);
extern	void	PlayerDraw(void);

