
// �v���C���[�N���X
class	cPlayer
{
public:
	VEC3	m_vecPos;			// �v���C���[���W
	
	float	m_fDir;				// ����

	void	Main(void);			// ���C��
	
	void	Draw(void);			// �`�揈��
};

// �ϐ�
extern	cPlayer *player;		// �v���C���[

//---   �O������̌Ăяo���֐� -----------------------------------------------------
extern	void	PlayerInit(void);
extern	void	PlayerMain(void);
extern	void	PlayerDraw(void);
