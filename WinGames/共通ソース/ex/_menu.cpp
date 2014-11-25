//
//	���j���[����
//


//#include	"main.h"
//#include	"_text.h"
//#include	"_menu.h"
#if 0

//--- �萔 --------------------------------------------------------
#define	FONT		3						// �t�H���g�ԍ��i2 �` 7 �͈̔͂Łj
#define	TEX_FRAME	0
#define	TEX_MENU	1
#define	ADD_W		 4.0f
#define	ADD_H		-2.5f

//--- �ϐ� --------------------------------------------------------
static	int		iSelect;					// �I�����ڂ̔ԍ�
static	int		iBlinkTime;					// �_�Ŏ���
static	VEC3	vecX0Y0, vecX1Y1;			// ��`�̈�̕`��͈�



/*------------------------------------------------------------------------------*
| <<< 8 �~ 8 �̃p�[�c����ׂăE�B���h�E�̕`�� >>>
|	����	p0 = ����̍��W
|			p1 = �E���̍��W
*------------------------------------------------------------------------------*/
void	MenuFrameDraw(VEC3 *p0, VEC3 *p1)
{
	FOLDER_SET("�X�N���v�g");
	d3_tex_load(TEX_FRAME, "�g.png");
	D3_TEXTURE(0, TEX_FRAME);
	float	u0, v0, u1, v1;

	if(sys.bFrameSkip) return;
	
	for(float x = p0->x; x < p1->x; x += 8.f)
	{
		for(float y = p0->y; y < p1->y; y += 8.f)
		{
			if(     x == p0->x     && y == p0->y    ){ u0 =  0; v0 =  0; u1 =  8; v1 =  8;}	// ����
			else if(x >= p1->x - 8 && y == p0->y    ){ u0 = 16; v0 =  0; u1 = 24; v1 =  8;}	// �E��
			else if(x == p0->x     && y >= p1->y - 8){ u0 =  0; v0 = 16; u1 =  8; v1 = 24;}	// ����
			else if(x >= p1->x - 8 && y >= p1->y - 8){ u0 = 16; v0 = 16; u1 = 24; v1 = 24;}	// �E��
			else if(x == p0->x                      ){ u0 =  0; v0 =  8; u1 =  8; v1 = 16;}	// ��
			else if(x >= p1->x - 8                  ){ u0 = 16; v0 =  8; u1 = 24; v1 = 16;}	// �E
			else if(                  y == p0->y    ){ u0 =  8; v0 =  0; u1 = 16; v1 =  8;}	// ��
			else if(                  y >= p1->y - 8){ u0 =  8; v0 = 16; u1 = 16; v1 = 24;}	// ��
			else									 { u0 =  8; v0 =  8; u1 = 16; v1 = 16;}	// �^��
			D3_2DDRAW(&VEC3(x,     y,     0.02f),
					  &VEC3(x + 8, y + 8, 0.02f),
					  u0 / d3.texInfo[TEX_FRAME].fWidth,
					  v0 / d3.texInfo[TEX_FRAME].fHeight,
					  u1 / d3.texInfo[TEX_FRAME].fWidth,
					  v1 / d3.texInfo[TEX_FRAME].fHeight,
					  D3_COL(255, 255, 255, 255));
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< �}�E�X���I�����č��ڂ̔w�i��`�� >>>
|	����	w     = ���j���[���[�N
|			iLine = ���̕�����
|			iMax  = ���ڂ̐�
|	�߂�l	-1    : �Ȃɂ��I�����Ă��Ȃ�
|			 0 �` : �I���������ڂ̔ԍ�
*------------------------------------------------------------------------------*/
static	int		MenuBoxDraw(MENU_WORK *w, int iLine, int iMax)
{
	int		ret = -1;

	D3_BLEND_NORMAL();						// �������I��

	//--- ���ڑI�����ɓ_�� -------------------------------------
	if(iBlinkTime != 0)
	{
		iBlinkTime -= 8;
		float	r   = (iBlinkTime / 60.f) * 255.f;
		D3_2DBOX_FILL(&vecX0Y0, &vecX1Y1,  D3_COL(128, (int)r, 0, 0));
		if(iBlinkTime == 0) return iSelect;
		else				return -1;
	}
	//--- ���j���[�̍��ڂ�`�悵�N���b�N�`�F�b�N ----------------
	for(int i = 0; i < iLine; i++)
	{
		float	fW  = w->fX + iMax * (d3.fontInfo[FONT].Width  + ADD_W);
		float	fH0 = w->fY + i    * (d3.fontInfo[FONT].Height + ADD_H);
		float	fH1 = fH0   +         d3.fontInfo[FONT].Height;
		VEC3	p0(w->fX, fH0, 0.01f);
		VEC3	p1(fW,    fH1, 0.01f);
		if(pad.sX > p0.x && pad.sX < p1.x		// �}�E�X���w����Ă���͈͂�h��Ԃ�
		&& pad.sY > p0.y && pad.sY < p1.y)
		{
			D3_2DBOX_FILL(&p0, &p1, D3_COL(128, 255, 0, 0));
			if(pad.pl && iBlinkTime == 0)
			{
				vecX0Y0	   = p0;
				vecX1Y1	   = p1;
				iBlinkTime = 60 * 4;
				ret        = i;
				//snd_play(SND_OK);
				iSelect    = i;
				break;
			}
		}
	}
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< ���j���[�`�� >>>
|	����	w = ���j���[���[�N
*------------------------------------------------------------------------------*/
void	MenuDraw(MENU_WORK *w)
{
	//--- �����t�H���g���쐬���� ----------------------------------
	D3_ZWRITE(false);
	D3_ZREAD( false);
	float	fW = (float)(w->iStrMax * (d3.fontInfo[FONT].Width  + ADD_W));
	float	fH = (float)(w->iLine   * (d3.fontInfo[FONT].Height + ADD_H));
	MenuFrameDraw(&VEC3(w->fX      - 8, w->fY      - 8, 0),
				  &VEC3(w->fX + fW + 8, w->fY + fH + 8, 0));

	//--- �I�����ڂɘg��`�� --------------------------------------
	w->iAns = MenuBoxDraw(w, w->iLine, w->iStrMax);

	//--- ���j���[�̍��ڂ�`�� ------------------------------------
	D3_ZREAD( false);
	D3_ZWRITE(false);
	D3_TEXTURE(0, TEX_MENU);
	D3_2DDRAW(&VEC3(w->fX, w->fY, 0), &VEC3(w->fX + 640, w->fY + 480, 0), 0, 0, 1, 1, D3_COL(255, 255, 255, 255));
	D3_ZREAD( true);
	D3_ZWRITE(true);
}



/*------------------------------------------------------------------------------*
| <<< ���j���[���C�� >>>	�������ł̓��j���[�̊�{�������s���܂��B
|	����	w = ���j���[���[�N
|	�߂�l	-2 : �L�����Z��
|			-1 : �Ȃɂ��I�����Ă��Ȃ�
|			0�`: �I���������j���[�ԍ�
*------------------------------------------------------------------------------*/
int		MenuMain(MENU_WORK *w)
{

	if(!w->bFirst)
	{
		if(pad.pl) return -1;
		w->bFirst = true;
	}

	//--- �L�����Z�����ɂ� -2�A����ȊO�͎w��̒l��Ԃ� -----------
	if(pad.cr)
	{
		//snd_play(SND_CANCEL);
		return -2;
	}
	else
	{
		return w->iAns;
	}
}



/*------------------------------------------------------------------------------*
| <<< ���j���[�I�� >>>	�������ł͕����t�H���g���J�����Ă��܂��B
*------------------------------------------------------------------------------*/
void	MenuEnd(void)
{
	SAFE_RELEASE(d3.font[FONT]);
}



/*------------------------------------------------------------------------------*
| <<< ���j���[������ >>>
*------------------------------------------------------------------------------*/
void	MenuInit(void)
{
	d3_offscreen_create(TEX_MENU, 512, 512);
	//d3.texInfo[TEX_MENU].bRepair = true;		// ���̃X�C�b�`�� true �ɂ���ƃt���X�N���[���؂�ւ����ɉ摜���ێ������
}



/*------------------------------------------------------------------------------*
| <<< ���j���[��} >>>
|	����	str = ���j���[�̕�����
*------------------------------------------------------------------------------*/
void	MenuMake(char *str)
{
	//--- �����t�H���g���쐬���� ----------------------------------
	d3_font_create(FONT, 16, 40, "HG�ۺ޼��M-PRO", false);

	//--- ���j���[�̍��ڂ�`�� ------------------------------------
	d3_offscreen_begin(TEX_MENU);
	d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3_COL(0, 0, 0, 0), 1.0f, 0);

	RECT	r = {0, 0, 0, 0};
//	d3.font[FONT]->DrawText(NULL, str, -1, &r, DT_NOCLIP, D3DCOL(255, 255, 255, 255));

	d3_offscreen_end(TEX_MENU);
}


#endif