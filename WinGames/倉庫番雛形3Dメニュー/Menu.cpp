//
//	���j���[����
//

#include	"main.h"

//--- �萔 ------------------------------------------------------------------------------------------------

#define	CENTER_		1			//0 : �������悹�A1 : ��������

#define FONT_ID_	3			// �t�H���g�ԍ�(2 ~ 7 �͈̔͂�)
#define TEX_W_		512			// �e�N�X�`���[�̉��T�C�Y(2,4,8,16,32,64,128,256,512,1024�̒���)
#define TEX_H_		32			// �e�N�X�`���[�̏c�T�C�Y(2,4,8,16,32,64,128,256,512,1024�̒���)
#define FONT_SIZE_	42			// �����̃T�C�Y
#define	INTERVAL_TIME_	30	

//--- �ϐ� ------------------------------------------------------------------------------------------------

int		iMenuMax;				//���j���[�̍��ڐ�

int		iMenuSel;				// ���j���[�����܂ǂ����w���Ă��邩
bool	bMenuCancel;	// ���j���[�̃{�^�����
bool	bMenuOk;
bool	bMenuCancelEnable;		// true : �L�����Z������

static	int	iIntervalTime  = 0;		//�_�Ŏ���(0 �ȊO�̎�)


/*---------------------------------------------------------------------------------*
| <<< �}�E�X�͈̔̓`�F�b�N >>>				�����j���[�̍��ڃ`�F�b�N�Ɏg��
|	����	p0, p1 = �͈͂̍���ƉE��
|	�߂�l	true : �͈͓�
*----------------------------------------------------------------------------------*/
static	BOOL	CheckBox(VEC2 *p0, VEC2 *p1)
{
	if((int)p0->x <= pad.sX && p1->x >pad.sX
	&& (int)p0->y <= pad.sY && p1->y >pad.sY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*--------------------------------------------------------------------------*
| <<< ���j���[������ >>>
*--------------------------------------------------------------------------*/
void	MenuInit(void)
{
	FOLDER_SET("�摜");
	d3_tex_load(TEX_MENU_FRAME0, "menubar.png", true);		// ��I����
	d3_tex_load(TEX_MENU_FRAME1, "menubar2.png", true);		// �I����

	//--- �����t�H���g���쐬���� ------------------------------------
	//	"HG�ۃS�V�b�NM-PRO" �̓t�H�g�V���b�v���C���X�g�[�����邱�ƂŎg�p�ł���t�H���g�ł��B
	//	���C���X�g���ɂ́uMS�S�V�b�N�v�ɂȂ�܂��B
	d3_font_create(FONT_ID_, FONT_SIZE_ / 2, FONT_SIZE_, "HG�ۺ޼��M-PRO", false, 300);
}

/*-----------------------------------------------------------------------*
| <<< ���j���[�`�恕���� >>>
|	����	p		  = �`����W
|			HInterval = �c�̔z�u
*------------------------------------------------------------------------*/
void	MenuMain(VEC2 *p, float fHInterval)
{

	if(iIntervalTime == 0 && bMenuOk){ return;}

	//--- ���j���[�̍��ڂ�`�� ----------------------------------
	d3_stat_zread_set( false);					// �d�Ȃ�𖳎�����
	d3_stat_zwrite_set(false);
	d3_stat_alpha_set( true);					// �����I��
	d3_stat_filter_set(D3_FILTER_LINEAR);		// �ڂ�������

	//--- �J�[�\������ ------------------------------------------
	if(pad.bKeyClick[KEY_UP])
	{
		iMenuSel--;
		if(iMenuSel < 0){ iMenuSel = iMenuMax -1;}
	}
	if(pad.bKeyClick[KEY_DOWN])
	{
		iMenuSel++;
		if(iMenuSel >= iMenuMax){ iMenuSel = 0;}
	}

	//--- �}�E�X�𓮂�������ǂ����w���Ă��邩�̕ϐ������Z�b���� --
	if((pad.sMoveX || pad.sMoveY) && iIntervalTime == 0)
	{
		iMenuSel = -1;
	}

	// p0 ��������W�Ƃ���
	VEC2	p0 = *p;

	for(int i = 0; i < iMenuMax; i++)
	{
		//--- �`��X�C�b�` ------------------------------------------
		bool	bDraw = false;
		if(iIntervalTime == 0				// iIntervaltime �� 0�̎��̓��j���[���I���̎�
		||(iIntervalTime % 20) > 5 && iMenuSel == i)
		{
			bDraw = true;
		}

		//--- �g -----------------------------------------------------
		int		iTex  = TEX_MENU_FRAME0;				// �g�̃e�N�X�`���[�ԍ�
		D3_INFO *info = &d3.texInfo[iTex];
		// p0 �͉E�����W		���I���W�i���摜�̃T�C�Y�𓾂Ă���
		VEC2	p1 = p0 + VEC2(info->fOriWidth, info->fOriHeight);

		// �}�E�X�Ƃ̓����蔻��
		//if(CheckBox(&p0, &p1))
		if((pad.sMoveX || pad.sMoveY) && CheckBox(&p0, &p1) || iMenuSel == i)
		{
			d3_tex_set(iTex + 1);
			if(iIntervalTime == 0){ iMenuSel = i;}
		}
		else
		{
			d3_tex_set(iTex);
		}
		// �`��
		if(bDraw){ d3_2d_box_fill_tex_draw(&p0, &p1, 0, 0, info->fU, info->fV, D3_COL(255, 255, 255, 255));}

		//--- ���j���[�̕�����`�� ----------------------------------------------
		info = &d3.texInfo[TEX_MENU0];

		#if CENTER_ == 1
			float	fX = (__SCR_W - info->fWidth / 4) / 2;
		#else
			float	fX = p0.x;
		#endif

		d3_tex_set(				TEX_MENU0 + i);
		if(bDraw){ d3_2d_box_fill_tex_draw(&VEC2(fX, p0.y),&VEC2(fX + info->fWidth / 2, p0.y + info->fHeight / 2));}

		//--- ���s --------------------------------------------------------------
		p0.y += fHInterval;
	}
	//--- �L�����Z�����ɂ� -2�A����ȊO�͎w��̒l��Ԃ� ---------------------------
	if((pad.bRightClick || pad.bKeyClick[KEY_X]) && bMenuCancelEnable)
	{
		iMenuMax	=0;
		bMenuCancel = true;
		iMenuSel	= -2;
	}
	else if((pad.bLeftClick || pad.bKeyClick[KEY_Z]) && iMenuSel >= 0)
	{
		iIntervalTime = INTERVAL_TIME_;
	}
	//--- �_�ŏ��� ----------------------------------------------
	if(iIntervalTime > 0)
	{
		iIntervalTime--;
		if(iIntervalTime == 0)
		{
			iMenuMax = 0;
			bMenuOk  = true;
		}
	}

	//--- �I������ ---------------------------------------------------------------
	d3_stat_zread_set( true);				// �d�Ȃ��L���ɂ���
	d3_stat_zwrite_set(true);
}

/*----------------------------------------------------------------------*
| <<< ���j���[���ڂ̒ǉ� >>>		�������_�����O�ʂɕ�������}���Ă���
|	����	str	= ���ڂ̕�����
*----------------------------------------------------------------------*/
void	MenuAdd(char *str)
{
	int		iTex = TEX_MENU0 + iMenuMax;	// iTex �͍�}����e�N�X�`���[�ԍ�

	//--- �����_�����O�ʍ쐬 ----------------------------------
	d3_offscreen_free(		iTex);
	d3_offscreen_create(	iTex, TEX_W_ * 2, TEX_H_ * 2);
	d3.texInfo[iTex].bRepair = true;		// ���̃X�C�b�`��true �ɂ���ƃt���X�N���[���؂�ւ����ɉ摜���ۑ������
	
	//--- ���j���[�̍��ڂ�`�� --------------------------------
	d3_offscreen_begin(iTex);
	d3_clear(D3_COL(0, 0, 0, 0));

	RECT	r = {32, 10, TEX_W_-32, TEX_H_};
	//�e
	r.left -= 2;
	r.top  += 2;
	d3_font_draw(FONT_ID_, &r, CENTER_, D3_COL(255,	0,	0,	0), str);
	// ����
	r.left -= 2;
	r.top  -= 2;
	d3_font_draw(FONT_ID_, &r, CENTER_, D3_COL(255,	255, 255, 255), str);

	d3_offscreen_mem_save(iTex);
	d3_offscreen_end(iTex);

	//--- ���j���[�̑I�����ڂ�ݒ� ----------------------------------------
	iMenuMax++;

	iMenuSel		= -1;
	bMenuOk			= false;
	bMenuCancel		= false;
	bMenuCancelEnable = false;
}