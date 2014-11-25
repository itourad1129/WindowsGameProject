//
//	���ɔԐ��`
//

#include	"main.h"


//--- �萔���` ----------------------------------------------------------------------------------

//--- �Q�[���i�s�p�ϐ� ----------------------------------------------------------------------------
int	iMode;			// �Q�[�����[�h
int iRound;			//�@��
float	fTimeDat;	// ���Ԍv���p�ł�
//--- �ϐ����` ----------------------------------------------------------------------------------
#define	 TIME_MAX	(60 * 60 * 1)		// 60�łP�b�A���̂P�b60�{���ĂP��
//--- �֐��錾 ------------------------------------------------------------------------------------
static void		GameStart(void);
static void		Game(void);
static void		Draw(void);
static void		Over(void);
static void		ClearCheck(void);
static void		MItemSet(int	iNum);


/*------------------------------------------------------------------------------*
| <<< �Q�[�������� >>>
*------------------------------------------------------------------------------*/
void	game_init(void)
{
	MenuInit();
	//MenuAdd("�Q�[�����n�߂�");
	//MenuAdd("����������");

	MItemSet(0);				// �^�C�g���̃��j���[���Ăяo��
	
	FOLDER_SET("�摜");
	d3_tex_load(TEX_MOJI,		"����.png");
	//d3_tex_load(TEX_MAP,		"�}�b�v.bmp");
	//d3_tex_load(TEX_PLAYER,		"�L����.bmp");
	d3_tex_load(TEX_TITLE,		"�^�C�g��.png");
	d3_tex_load(TEX_CLEAR,		"���E���h�N���A�[.png");
	d3_tex_load(TEX_ALLCLEAR,	"�I�[���N���A�[.png");
	d3_tex_load(TEX_OVER,		"�Q�[���I�[�o�[.png");
	d3_tex_load(TEX_INFO,		"����.png");

	// 3D ���f���ǂݍ���
	FOLDER_SET("3D���f��");
	d3_xfile_load(X_WALL,		 "iceblock.mqo");
	d3_xfile_load(X_MOKUTEKICHI, "ring.mqo");
	d3_xfile_load(X_NIMOTU,		 "ball.mqo");
	d3_xfile_load(X_PLAYER,		 "phoca.mqo");

	FOLDER_SET("�T�E���h");
	snd_load(SND_WALK,			"���s.wav",				false);
	snd_load(SND_OUCH,			"�ǂɂԂ���.wav",		false);
	snd_load(SND_START,			"�X�^�[�g.wav",			false);
	snd_load(SND_ATTAINMENT,	"�ړI�n���B.wav",		false);
	snd_load(SND_CLEAR,			"�����̃N���A�[.wav",	false);
	snd_load(SND_UNDO,			"�A���h�D.wav",			false);
	snd_load(SND_TIMEUP,		"�^�C���A�b�v.wav",		false);
	snd_load(SND_GIVEUP,		"�M�u�A�b�v.wav",		false);
	snd_load(SND_ALLCLEAR,		"�I�[���N���A�[.wav",	false);

	// �J�������W
	d3.vecCamera = VEC3(0, 10, -0.01f);
	// ���Ӎ��W
	d3.vecEye	 = VEC3(0,	0,		0);
	
	
	player = new cPlayer;
	
}



/*------------------------------------------------------------------------------*
| <<< �Q�[�����C�� >>>
*------------------------------------------------------------------------------*/
void	game_main(void)
{
	
	d3_render_begin();
	PrintfInit();

	Camera();

	switch(iMode)
	{
	//--- �^�C�g�� --------------------------------------------------------------
	case MODE_TITLE:
		//		�e�N�X�`���[		���̃��[�h�@�T�E���h
		//Title(TEX_TITLE, &iMode, MODE_INFO, SND_START);
		if(TitleMenu(TEX_TITLE, -1, bMenuOk))
		{
			//--- ���j���[�̍��ڕʂɃW�����v
			switch(iMenuSel)
			{
			case 0:				// �Q�[��
				MapLoad(iRound);
				GameStart();
				iMode	 = MODE_GAME;
				fTimeDat = TIME_MAX;
				Undo(0);
				snd_play(SND_START);
				break;
			case 1:							//����
				iMode = MODE_INFO;
				MItemSet(1);
				snd_play(SND_OUCH);
				break;
			case 2:							//�I��
				SYS_EXIT();
				break;
			}
		}
			
		break;
	//--- ���� -----------------------------------------------------------------
	case MODE_INFO:
		//Title(TEX_TITLE, &iMode, MODE_INFO, SND_START);
		if(TitleMenu(TEX_TITLE, -1, bMenuOk))
		{
			//--- ���j���[�̍��ڕʂɃW�����v
			switch(iMenuSel)
			{
			case 0:				// �Q�[��
				MapLoad(iRound);
				GameStart();
				iMode	 = MODE_GAME;
				fTimeDat = TIME_MAX;
				Undo(0);
				snd_play(SND_START);
				break;
			case 1:							//����
				iMode = MODE_TITLE;
				MItemSet(-1);
				snd_play(SND_OUCH);
				break;
			}
		}
		break;
	//--- �Q�[���� --------------------------------------------------------------
	case MODE_GAME:
		Game();
		break;
	//--- �N���A�[ -------------------------------------------------------------
	case MODE_CLEAR:
		if(Title(TEX_CLEAR, &iMode, MODE_GAME, SND_NONE))
		{
			iRound++;				// ���̃��E���h�֐i��
			GameStart();
			

		}
		break;
	
	//--- �I�[���N���A�[ ------------------------------------------------------
	case MODE_ALLCLEAR:
		if(Title(TEX_ALLCLEAR, &iMode, MODE_GAME, SND_ALLCLEAR))
		{
			GameStart();
			iRound = 0;
			

		}
		break;
	//--- �I�[�o�[--------------------------------------------------------------
	case MODE_OVER:
		Title(TEX_TITLE, &iMode, MODE_TITLE, SND_START);
		break;
	}

	MenuMain(&VEC2(64, 300), 56);
	PrintfFlush();
	d3_render_end();
	
}

/*------------------------------------------------------------------------------*
| <<< ���j���[�̍��ڐݒ� >>>
|	����	iNum = 0 : �^�C�g���A1 : ���j���[
*-------------------------------------------------------------------------------*/
static	void	MItemSet(int iNum)
{
	switch(iNum)
	{
	//--- �^�C�g�� ------------------------------------------------------
	case 0:
		MenuAdd("�Q�[�����n�߂�");
		MenuAdd("����������");
		MenuAdd("�I������");
		break;
	//--- ���� ----------------------------------------------------------
	case 1:
		MenuAdd("�Q�[�����n�߂�");
		MenuAdd("�^�C�g���֖߂�");
		bMenuCancelEnable = true;			// �E�N���b�N�ŃL�����Z����
		break;
	}
}

/*------------------------------------------------------------------------------*
| <<< �Q�[���J�n���� >>>
*-------------------------------------------------------------------------------*/
static void GameStart(void)
{
	fTimeDat = TIME_MAX;		// ���ԏ�����
	
	player->Reset();
	
	MapLoad(iRound);

	Undo(UNDO_RESET);			// �A���h�D���Z�b�g

}

/*------------------------------------------------------------------------------*
| <<< �Q�[������ >>>
*------------------------------------------------------------------------------*/
static void Game(void)
{
	// �M�u�A�b�v
	if(pad.bKeyClick[KEY_ESC])
	{										// �L�[����������
		iMode = MODE_TITLE;					// �Q�[�����[�h��ύX����̂ŁA�^�C�g���֖߂邱�Ƃ��ł���
		snd_play(SND_GIVEUP);
		pad.bKeyClick[KEY_ESC] = false;		// ���̈�s���Ȃ��ƃQ�[�����I�����Ă��܂��܂�
		return;
	}

	fTimeDat -= 1.f;
	if(fTimeDat <= 0.f){ iMode = MODE_OVER; snd_play(SND_TIMEUP);}
	
	player->Main();		// �v���C���[���C��
	
	MapDraw();			// �}�b�v�`��

	player->Draw();		// �v���C���[�`��

	ClearCheck();		// �ʃN���A�[

	TimerDraw(fTimeDat, TIME_MAX);		// �^�C�}�[����
										// �M�u�A�b�v�̕\��
	Printf(		8, 460, 16, "ESC	:GIVE UP!");
	Printf(460, 460, 16, "ROUND%0d", iRound);
										// �A���h�D�ē�
	Printf(	8, 440, 16, "CTRL+Z:UNDO!");

	//�A���h�D
	if((pad.bKeyPush[KEY_LCTRL] || pad.bKeyPush[KEY_RCTRL]) && pad.bKeyClick[KEY_Z] && player->CheckStop())
	{
		Undo(UNDO_GET);
		snd_play(SND_UNDO);
	}

}

/*------------------------------------------------------------------------------*
| <<< �ʃN���A�`�F�b�N >>>
*------------------------------------------------------------------------------*/
static		void	ClearCheck(void)
{
	for(int y = 0; y < MAP_H; y++)			// �c�Ƀ��[�v
	{
		for(int x = 0; x < MAP_W; x++)		// ���Ƀ��[�v
		{
			// x ����������A�����ŋA��
			if(chMap[ADR(x,y)] == 'x'){ return;}
		}
	}
	// x �͉ו����^�ԏꏊ
	// �ו��̉^�ԏꏊ�ɉו�������΁@x�@�͌�����Ȃ�
	// �Ō�܂� x ��������Ȃ������䂦�ɃN���A�[
	if(iRound != ROUND_MAX - 1){ iMode = MODE_CLEAR,    snd_play(SND_CLEAR 	 );}
	else					   { iMode = MODE_ALLCLEAR, snd_play(SND_ALLCLEAR);}
}
/*------------------------------------------------------------------------------*
| <<< �Q�[���I�[�o�[ >>>
*------------------------------------------------------------------------------*/
void	Over(void)
{
}
/*------------------------------------------------------------------------------*
| <<< �Q�[���`�� >>>
*------------------------------------------------------------------------------*/
void	Draw(void)
{
}
/*------------------------------------------------------------------------------*
| <<< �Q�[���I�� >>>
*------------------------------------------------------------------------------*/
void	game_end(void)
{
}


