
#define	AFTERIMAGE_MAX		200				// �c���o�^��

// �c���N���X
class	cAfterimage
{
public:
	int		m_iMeshNum;		// �RD���f���ԍ�
	int		m_iTime;		// �\������
	MTX		m_mtxWorld;		// ���[���h�s��
	MTX		m_mtxUpDate;	// �X�V�s��

	void	Init(void);		// ������
	void	Main(void);		// ���C��
	void	Draw(void);		// �`��
};

extern void		AfterimageInit(void);
extern void		AfterimageMain(void);
extern void		AfterimageDraw(void);
extern void		AfterimageSet(int	iMeshNum,	MTX *world,	MTX *update);
