#define	METEO_MAX			100					// 覐΂̐��i���������قǓ�Փx��������j

// 覐΃N���X
class	cMeteo
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

extern	cMeteo	*meteo;

extern	void	MeteoInit(void);
extern	void	MeteoMain(void);
extern	void	MeteoDraw(void);

