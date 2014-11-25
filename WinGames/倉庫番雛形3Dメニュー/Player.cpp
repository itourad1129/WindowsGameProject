//
//	�v���C���[
//

#include	"main.h"

//--- �萔��` ---------------------------------------------------------------------------------------
#define		MOVE_SPEED	4							// �ړ����x
#define		MOVE_TIME	(BLK_SIZE / MOVE_SPEED)		// �ړ����ԁi���x�� 4 �ɂȂ�΁A32 / 4 = 8 �ƂȂ�j

//--- �ϐ���` ---------------------------------------------------------------------------------------
cPlayer *player;									// �|�C���^�Ȃ̂� new �Ń��������m�ۂ̂���


/*----------------------------------------------------------------------------------------------------
| <<< �v���C���[���Z�b�g���� >>>
*----------------------------------------------------------------------------------------------------*/
void		cPlayer::Reset(void)
{
	m_iDir		= DIR_DOWN;				// �������ɂ���

	m_vecPos	= VEC2(0, 0);			// �ʒu
	m_iMoveTime	= 0;					// �ړ�����
	m_iNimotu = 0;						// �ו��I�t

	m_fAnim = 0;
	m_fRot  = 0;
}

/*----------------------------------------------------------------------------------------------------*
| <<< �v���C���[���� >>>
*-----------------------------------------------------------------------------------------------------*/
void	cPlayer::Main(void)
{
	//--- �L������]�� --------------------------------------------------------------------
	//
	float	fRotTbl[4] = {90, 270, 0, 180};//90  270  0  180
	//m_fRot = fRotTbl[m_iDir];

	if(m_fRot != fRotTbl[m_iDir])
	{
		float	a = fRotTbl[m_iDir];
		float	b = (float)m_fRot - a;
		if(b < -180){ a -= 360;}
		if(b >  180){ a += 360;}
		VALUE_TO_TARGET(m_fRot, a, 20.f);
		if(m_fRot == a){ m_fRot = fRotTbl[m_iDir];}
		return;
	}

	int iNext = ((int)m_vecSpeed.y / MOVE_SPEED) * MAP_W + (int)m_vecSpeed.x / MOVE_SPEED;
	
	// �A�j���[�V����
	m_fAnim += 0.1f;
	//--- �L�����ړ��`�F�b�N -----------------------------------
	if(m_iMoveTime == 0)
	{
		// �L�[�������ꂽ��S�̕����ֈړ�													���ړ�����			�ړ����x
		if(pad.bKeyClick[KEY_UP		]){m_vecSpeed = VEC2(0, -MOVE_SPEED),m_iDir = DIR_UP,m_iMoveTime = MOVE_TIME;}
		if(pad.bKeyClick[KEY_LEFT	]){m_vecSpeed = VEC2(-MOVE_SPEED, 0),m_iDir = DIR_LEFT,m_iMoveTime = MOVE_TIME;}
		if(pad.bKeyClick[KEY_RIGHT	]){m_vecSpeed = VEC2( MOVE_SPEED, 0),m_iDir = DIR_RIGHT,m_iMoveTime = MOVE_TIME;}
		if(pad.bKeyClick[KEY_DOWN	]){m_vecSpeed = VEC2(0,  MOVE_SPEED),m_iDir = DIR_DOWN,m_iMoveTime = MOVE_TIME;}

		// KEY_UP ���Q�l�Ɏc��R�������쐬�̂���
	

	// �ړ����鎞�̂ݕǔ���
	if(m_iMoveTime > 0)
		{
		snd_play(SND_WALK);			//���s��

		// �ړ�����}�X�ւ̉��Z�l(���̒l��m_iAir�𑫂������̂����̃A�h���X)
		int iNext = ((int)m_vecSpeed.y / MOVE_SPEED) * MAP_W + ((int)m_vecSpeed.x / MOVE_SPEED);
										// xy �v���C���[�̑���
		m_iAdr = POS_ADR(m_vecPos.x, m_vecPos.y);
		m_iAdr += iNext;				// xy �v���C���[�̈ړ���𑫂�
		
			if(chMap[m_iAdr] == '@')		// ��Q���ɓ��������灦�ꕶ���Ȃ̂ŃV���O���N�H�[�e�[�V����
			{
				m_iMoveTime = 0;			// �ړ����Ԃ��O�ɂ���
				snd_play(SND_OUCH);			// �ǏՓˉ���ENUM�Œ�`�������̂��g���܂��傤�B
			}

			int	iXY2 = m_iAdr + iNext;		// �Q����𒲂ׂ�
											// �ו��̐�ɏ�Q�����邩�H
			if(chMap[m_iAdr] == 'o' && (chMap[iXY2] == '@' || chMap[iXY2] == 'o'))
			{
				m_iMoveTime = 0;		// �ړ����Ԃ��O�ɂ���
				snd_play(SND_OUCH);		// �ǏՓˉ�
			}
				
			else if(chMap[m_iAdr ] == 'o')	// �ו��ɓ���������
			{
				Undo(UNDO_SET);
				//chMap[m_iAdr] = ' ';		// �R�����g�ɂ���
				//		���̂悤�ɏ���������
											// �ו��͏������ǁA�ړI�n�͎c��
				if(chMapBak[m_iAdr] == 'x'){ chMap[m_iAdr] = 'x';}
				else					   { chMap[m_iAdr] = ' ';}

				m_iNimotu		= m_iDir + 1;
			}
			else
			{
				Undo(UNDO_SET);		// ��}�X�������тɃA���h�D�Z�b�g
			}

		}
	

	}	

	

	//--- �L�����ړ��� ----------------------------
	if(m_iMoveTime > 0)
	{
		m_vecPos += m_vecSpeed;		// �����ňړ�
		m_iMoveTime--;				// �ړ����Ԍv�Z
	
		// �ړ����Ԃ��O�ɂȂ�����ו������낷
		if(m_iMoveTime == 0 && m_iNimotu > 0)
		{
			int iNext = ((int)m_vecSpeed.y / MOVE_SPEED) * MAP_W + (int)m_vecSpeed.x / MOVE_SPEED;
			int a	  = m_iAdr + iNext;		// a �͉ו��̃}�b�v�A�h���X
			chMap[a]  = 'o';				// �ו���u��
			m_iNimotu = 0;
		}
	}

	m_fAnim += 0.025f; 
	m_iAnimTime_a++;
	if(m_iAnimTime_a >= 30)
	{
		m_iAnimTime_b++;
		m_iAnimTime_a = 0;

		if(m_iAnimTime_b >= 4)
		{
			m_iAnimTime_b = 0;
		}
	}
	
}
/*----------------------------------------------------------------------------------------------------*
| <<< �v���C���[�Ɖו��`�� >>>
*-----------------------------------------------------------------------------------------------------*/
void		cPlayer::Draw(void)
{
	

	MTX		mtxWorld, mtxTrans, mtxRot;

	mtxWorld = mtxRot  * mtxTrans;

	VEC3	v;
	
	v.x =  (m_vecPos.x - __SCR_W / 2 + BLK_SIZE / 2.f) / 56.0f;
	v.z = -(m_vecPos.y - __SCR_H / 2 + BLK_SIZE / 2.f) / 56.0f;
	v.y = 0;
	D3DXMatrixTranslation(&mtxTrans, &v);			// �ړ��s��쐬
	D3DXMatrixRotationY(	&mtxRot,	R(m_fRot));	// ��]�p��쐬
	mtxWorld = mtxRot * mtxTrans;					// �s��̍���(��]�A�ړ��̏�)
	
	d3_3d_draw(X_PLAYER, &mtxWorld);

	
	//--- �}�b�v���W�A�}�b�v�A�h���X�\�� -------------------------------------------------
	int x = (int)m_vecPos.x / BLK_SIZE;
	int y = (int)m_vecPos.y / BLK_SIZE;
	Printf(0, 16, 16, "PLAYER:%d,%d", x, y);
	Printf(0, 32, 16, "ADR	  :%d",		ADR(x, y));

	//--- �����ɂȂɂ����邩�\�� --------------------------------------------------------
	char	b = chMap[ADR(x,y)];
	if(b == '@'){ d3_printf("��"		);}		// Printf �֐��͑S�p�������g���Ȃ��̂ŁAd3_printf ���g���Ă܂�
	if(b == ' '){ d3_printf("��"		);}
	if(b == 'x'){ d3_printf("�ړI�n"	);}
	if(b == 'o'){ d3_printf("�ו�"	);}
	
	
	
	RECT	r;

	// �v���C���[�`��
	/*if(m_iDir == DIR_UP){SetRect(&r, 0,  0,  BLK_SIZE, BLK_SIZE);	// �摜�̐؂�o����
	}
	if(m_iDir == DIR_DOWN){SetRect(&r, 0, BLK_SIZE, BLK_SIZE, BLK_SIZE * 2); //����
	}
	if(m_iDir == DIR_LEFT){SetRect(&r, 0, BLK_SIZE * 2, BLK_SIZE, BLK_SIZE * 3);// ��
	}
	if(m_iDir == DIR_RIGHT){SetRect(&r, 0, BLK_SIZE * 3, BLK_SIZE, BLK_SIZE * 4);//�E
	}*/
	//SetRect(&r, 0 + (int)m_fAnim % 3 * BLK_SIZE, m_iDir * BLK_SIZE, BLK_SIZE + (int)m_fAnim % 3 * BLK_SIZE, BLK_SIZE + m_iDir * BLK_SIZE);
	// 1��   2��   3��    4�E 
	//v.x = m_vecPos.x + BLK_SIZE / 2.f;		// �\�����W(BLK_SIZE / 2 �Ŕ��u���b�N���炵�Ă���j
	//v.y = m_vecPos.y + BLK_SIZE / 2.f;
	//v.z = 0;
	//d3_spr_draw(TEX_PLAYER, &r, &v);		// �`��
	


	// �ȉ��A�ו����^��ł���Ƃ��͉ו���`��
	if(m_iNimotu == 0){return;}				// �ו����^��ł��Ȃ��Ƃ��͂����ŏI��

	SetRect(&r, 128, 0, 160, 32);
	v.x += m_vecSpeed.x * MOVE_TIME;		// += �ł��B����
	v.y += m_vecSpeed.y * MOVE_TIME;
	d3_spr_draw(TEX_MAP, &r, &v);

	
}
