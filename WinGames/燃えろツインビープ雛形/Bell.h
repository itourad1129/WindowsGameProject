#define	BELL_MAX			30					// 覐΂̐��i���������قǓ�Փx��������j

// 覐΃N���X
class	cBell
{
public:
	VEC3	m_vecPos;			// �ʒu
	VEC3	m_vecSpeed;			// �ړ����x
	int		m_iDelay;			// �x������
	MTX		m_mtxWorld;			// ���[���h
	float	m_fRot;				// ��]�l
	float	m_fAlp;				// �`��

	void	Init(int	iNum);	// ������
	void	Main(void);			// ���C��
	void	Draw(void);			// �`��
};

extern	cBell	*bell;

extern	void	BellInit(void);
extern	void	BellMain(void);
extern	void	BellDraw(void);
