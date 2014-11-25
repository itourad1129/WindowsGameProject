
#define STAR_MAX		100			// ���̐�

// ���N���X
class	cStar
{
public:
	VEC3	m_vecPos;				// �ʒu
	VEC3	m_vecSpeed;				// �ړ����x
	int		m_iDelay;				// �x������
	MTX		m_mtxWorld;				// ���[���h
	float	m_fAlp;					// ���������[�g

	void	Init(void);				// ������
	void	Main(void);				// ���C��
	void	Draw(void);				// �`��
};

extern	cStar	*star;

extern	void	StarInit(void);
extern	void	StarMain(void);
extern	void	StarDraw(void);