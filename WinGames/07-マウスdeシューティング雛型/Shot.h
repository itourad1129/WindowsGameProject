#define SHOT_MAX		20			//�e�̍ő吔

// �e�N���X
class
{
public:
	BOOL	m_bUse;					// TRUE : �g�p��
	VEC3	m_vecPos;				// ���W
	float	m_fSpeed;				// �ړ����x
	float	m_fDist;				// �ړ���������
	float	m_fDir;					// ����

	void	Main(void);				// ���C��
	void	Draw(void);				// �`��
};

// �ϐ�
extern cShot	*shot[];			// �e���[�N

// �ȉ��A�ꊇ�����p�̊֐�
extern	void	ShotInit(void);
extern	void	ShotMain(void);
extern	void	ShotDraw(void);
extern	void	ShotStart(VEC3 *pvecPos, float fDir);

