//
//	�R����c�C���r�[�v���^
//

#include	"main.h"


//--- �ϐ���` ------------------------------------------------------------------------------------
GAME_WORK	game;


/*------------------------------------------------------------------------------*
| <<< �Q�[�������� >>>
*------------------------------------------------------------------------------*/
void	game_init(void)
{
	//--- �摜�ǂݍ��� ---------------------------------------------
	FOLDER_SET("�摜");
	d3_tex_load(TEX_MOJI,		"����.png");
	d3_tex_load(TEX_TITLE,		"�^�C�g��.png");
	d3_tex_load(TEX_INFO,		"����.png");
	d3_tex_load(TEX_OVER,		"�Q�[���I�[�o�[.png");
	d3_tex_load(TEX_PARTICLE,	"�p�[�e�B�N��.png");

	//--- �RD���f���ǂݍ��� ----------------------------------------
	FOLDER_SET("3D���f��");
	d3_xfile_load(X_PLAYER,	"�v���C���[.mqo");
	d3_xfile_load(X_METEO,	"覐�.mqo");
	d3_xfile_load(X_BELL,	"�{�[�i�X.mqo");
	d3_xfile_load(X_STAR,	"��.mqo");

	//--- ���y �� ���ʉ� -------------------------------------------
	FOLDER_SET("�T�E���h");
	snd_load(SND_DAMAGE, "�_���[�W.wav", false);	// ���S
	snd_load(SND_START,	 "�X�^�[�g.wav", false);	// �J�n
	snd_load(SND_BGM,	 "���y.wav",	 true);		// BGM(���[�v�Đ�����̂� true)
	snd_load(SND_BELL,	 "�x��.wav",	 false);	// �x���Q�b�g
	snd_load(SND_MOVE,	 "�ړ�.wav",	 false);	// �v���C���[�ړ�

	//--- �J�����ݒ� -----------------------------------------------
	//d3.vecCamera		= VEC3(0, 0.0f, -10.0f);
	//��
	d3.vecCamera	= VEC3(0,-10.0f, -5.0f);
	//--- �ȉ��A�p�[�e�B�N���̎��ɕK�v�ɂȂ�܂��B--------------------
	float	tmp;
	D3DXMatrixInverse(&d3.mtxViewInverse, &tmp, &d3.mtxView);
	d3.mtxBillboard = d3.mtxViewInverse;

	//--- ���낢�돉���� -------------------------------------------
	ParticleInit();									// �p�[�e�B�N��������
	StarInit();
	MeteoInit();
	BellInit();
	ParticleInit();
	AfterimageInit();
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
		Title(TEX_TITLE, &game.iMode, MODE_INFO, SND_START);
		break;

	//--- ���� ---------------------------------------------------------------
	case MODE_INFO:
		if(Title(TEX_INFO, &game.iMode, MODE_GAME, SND_BGM))
		{
			PlayerInit();			// �v���C���[������
			
		}
		break;

	//--- �Q�[�� -------------------------------------------------------------
	case MODE_GAME:
		PlayerMain();				// �v���C���[���C��
		PlayerDraw();				// �v���C���[�`��
		StarMain();
		StarDraw();
		MeteoMain();
		MeteoDraw();
		BellMain();
		BellDraw();
		ParticleMain();
		AfterimageMain();
		AfterimageDraw();
		CameraMoveInit();
		CameraMoveMain();
		
		if(player->m_iHp <= 0){ game.iMode = MODE_OVER; snd_stop(SND_BGM);}
		break;

	//--- �Q�[���I�[�o�[ -----------------------------------------------------
	case MODE_OVER:
		Title(TEX_OVER, &game.iMode, MODE_TITLE, SND_NONE_);
		break;
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


