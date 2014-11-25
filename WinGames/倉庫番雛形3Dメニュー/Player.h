//--- �v���C���[�N���X -----------------------------------------------------------------
class	cPlayer
{
private:							// pivate�i�v���C�x�[�g�͂��̃N���X�O����Q�Ƃł��Ȃ��j
	VEC2	m_vecPos;				// �\�����W
	VEC2	m_vecSpeed;				// �ړ����x
	int		m_iMoveTime;			// �ړ��񐔁A32�h�b�g�� 8 ��ɕ����Đi��
	int		m_iDir;					// ����
	int		m_iAdr;					// �����̃}�b�v�A�h���X
	int		m_iNimotu;				// 0 != �ו����^��ł���
	float	m_fAnim;
	int		m_iAnimTime_a;
	int		m_iAnimTime_b;
	float	m_fRot;					// ��]�l

public:							// public(�p�u���b�N�̓N���X�O����Q�Ɖ�)
	void	SetDir(int iDir)			// ������ݒ�
	{
		m_iDir = iDir;
	}

	int		GetDir(void)				// �����𓾂�
	{
		return m_iDir;
	}
	
	void	SetPos(float fX, float fY)	// ���W��ݒ�
	{
		m_vecPos = VEC2(fX, fY);		// ���̂悤�ɏ������邱�ƂŁA
	}									// �N���X���� private �̕ϐ��ɏ������߂�
	
	void	Reset(void);			// ���Z�b�g
	void	Main(void);				// ���C��
	void	Draw(void);				// �`��

	float	GetXPos(void)			// �v���C���[�� X ���W�𓾂�
	{
		return m_vecPos.x;
	}

	float	GetYPos(void)			// �v���C���[�� Y ���W�𓾂�
	{
		return m_vecPos.y;
	}

	BOOL	CheckStop(void)			// ��~���Ă���Ƃ��� true ��Ԃ�
	{
		return (m_iMoveTime == 0);
	}

	
};

extern	cPlayer *player;