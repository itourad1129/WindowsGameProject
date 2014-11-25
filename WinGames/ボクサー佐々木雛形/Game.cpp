//
//	�{�N�T�[���X�ؐ��^
//

#include	"main.h"


//--- �ϐ����` ----------------------------------------------------------------------------------

GAME_WORK	game;

/*------------------------------------------------------------------------------*
| <<< �Q�[�������� >>>
*------------------------------------------------------------------------------*/
void	game_init(void)
{
	//--- �e�N�X�`���[�ǂݍ��� ----------------------------------------
	FOLDER_SET("�摜");
	d3_tex_load(TEX_MOJI, "����.png");

	//--- X �t�@�C���ǂݍ��� ----------------------------------------------
	FOLDER_SET("3D���f��");
	skin_load(	X_ENEMY, "enemy.x");
	d3_xfile_load(X_LEFT,	"punch.mqo");
	d3_xfile_load(X_RIGHT,	"punch.mqo");

	//--- �e�N�X�`���[�ǂݍ��� ---------------------------------------------
	FOLDER_SET("�摜");
	d3_tex_load(TEX_BACK,	"�����O.jpg");
	d3_tex_load(TEX_KO_BAR,	"ko.png");
	d3_tex_load(TEX_TITLE,	"�^�C�g��.jpg");
	d3_tex_load(TEX_INFO,	"����.jpg");
	d3_tex_load(TEX_WIN,	"����.jpg");
	d3_tex_load(TEX_LOSE,	"�s�k.jpg");

	//--- ���ʉ��ǂݍ��� ---------------------------------------------------
	FOLDER_SET("�T�E���h");
	snd_load(SND_PUNCH,		"�p���`.wav",				false);
	snd_load(SND_GUARD,		"�K�[�h.wav",				false);
	snd_load(SND_PLY_DAMAGE,"�v���C���[�_���[�W.wav",	false);
	snd_load(SND_ENE_DAMAGE,"�G�l�~�[�_���[�W.wav",		false);
	snd_load(SND_WIN_LOSE,	"�����I��.wav",				false);
	snd_load(SND_OK,		"�S���O.wav",				false);

	//--- �J�����ʒu -------------------------------------------------------
	d3.vecCamera = VEC3(0, 15, -10);
	d3.vecEye	 = VEC3(0, 15,   0);

	d3.light.Direction.x = 0;
	d3.light.Direction.y = 0;
	d3.light.Direction.z = -d3.vecCamera.z;
}

/*-----------------------------------------------------------------*
| <<< �e�X�g >>>
*-----------------------------------------------------------------*/
void	Test(void)
{
	//--- �A�j���[�V���� ------------------------------------------
	skin_frame_set(X_ENEMY, 1);			//1 �͍Đ����x

	//--- �A�j���[�V�����؂�ւ� ----------------------------------
	if(pad.bKeyClick[KEY_NUM0]){ skin_anim_set(X_ENEMY, 0, 30);}
	if(pad.bKeyClick[KEY_NUM1]){ skin_anim_set(X_ENEMY, 1, 30);}
	if(pad.bKeyClick[KEY_NUM2]){ skin_anim_set(X_ENEMY, 2, 30);}
	if(pad.bKeyClick[KEY_NUM3]){ skin_anim_set(X_ENEMY, 3, 30);}
	if(pad.bKeyClick[KEY_NUM4]){ skin_anim_set(X_ENEMY, 4, 30);}
	if(pad.bKeyClick[KEY_NUM5]){ skin_anim_set(X_ENEMY, 5, 30);}

	//--- �`�� ----------------------------------------------------
	MTX		mtxWorld, mtxScale, mtxRot, mtxTrans;

	D3DXMatrixScaling(		&mtxScale, 25);		// �g�嗦�͊e���Őݒ�̂���
												// �K�v�Ȃł�΁A���W���ݒ�̂���
	D3DXMatrixTranslation(&mtxTrans, &VEC3(0, 0, 55));
	mtxWorld = mtxScale * mtxTrans;
	skin_draw(X_ENEMY, &mtxWorld);
}

/*-----------------------------------------------------------------*
| <<< �q�b�g�|�C���g�\�� >>>
|	����	iNum = 0 : �v���C���[�A 1 : �G�l�~�[
|			fLen = �o�[�̒���(0.0f �` 1.0f )
*-------------------------------------------------------------------*/
void	HitPointDraw(int	iNum, float fLen)
{
	d3_stat_zwrite_set(false);
	d3_stat_zread_set( false);

	//--- HP�`�� ----------------------------------------------------
	VEC2	p0, p1;
	float	a = 264.f;
	if(iNum == 0){ p0 = VEC2(  8, 8); p1 = VEC2(  8 + a, 40);}
	else		 { p0 = VEC2(368, 8); p1 = VEC2(368 + a, 40);}
	d3_2d_box_fill_draw(&p0, &p1, D3_COL(255, 255, 255, 0));// ���n
	p0.x += 1, p1.x += -1;									// ���菬��������
	p0.y += 1, p1.y += -1;
	if(iNum == 0){ p0.x += (a - a * fLen);}					// �v���C���[
	else		 { p1.x -= (a - a * fLen);}					// �G�l�~�[
	d3_2d_box_fill_draw(&p0, &p1, D3_COL(255, 255, 255, 0)); // ���C�t�̓{�b�N�X��

	//--- KO�`�� -----------------------------------------------------
	if(iNum == 1)						// �G�l�~�[�̎��̂�KO�`��
	{
		d3_spr_draw(TEX_KO_BAR, NULL, &VEC3(320, 24, 0));
	}
	d3_stat_zread_set( true);
	d3_stat_zwrite_set(true);
}



/*------------------------------------------------------------------------------*
| <<< �Q�[�����C�� >>>
*------------------------------------------------------------------------------*/
void	game_main(void)
{
	d3_render_begin();
	PrintfInit();			// Printf �N���A�[
	Camera();				// �J����

	switch(game.iMode)
	{
	//--- �^�C�g�� -------------------------------------------------------------
	case MODE_TITLE:
		Title(TEX_TITLE, &game.iMode, MODE_INFO, SND_OK);
		break;

	//--- ���� ---------------------------------------------------------------
	case MODE_INFO:
		if(Title(TEX_INFO, &game.iMode, MODE_GAME, SND_OK))
		{
			PlayerInit();			// �v���C���[������
			EnemyInit();
			
		}
		break;

	//--- �Q�[�� -------------------------------------------------------------
	case MODE_GAME:
		PlayerMain();				// �v���C���[���C��
		PlayerDraw();				// �v���C���[�`��
		EnemyMain();
		EnemyDraw();
		HitPointDraw(0, player->m_fHP / HP_MAX);
		HitPointDraw(1, enemy->m_fHP / HP_MAX);
		break;

	//--- �Q�[���I�[�o�[ -----------------------------------------------------
	case MODE_WIN:
		Title(TEX_WIN, &game.iMode, MODE_TITLE, SND_WIN_LOSE);
		break;

	//case MODE_LOSE:
		//Title(TEX_LOSE, &game.iMode, MODE_TITLE, SND_WIN_LOSE);
		//break;
	}
	PrintfFlush();							// Printf �`��
	d3_render_end();
}

/*------------------------------------------------------------------------------*
| <<< �Q�[���I�� >>>
*------------------------------------------------------------------------------*/
void	game_end(void)
{
}


