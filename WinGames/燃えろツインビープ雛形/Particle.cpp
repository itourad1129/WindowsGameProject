//
//	�p�[�e�B�N��
//

#include	"main.h"

//--- �萔��` -----------------------------------------------------------------------------
#undef	MAX_						// #undef �͈�x #define �Œ�`�������̂𖳌��ɂ�����̂ł��B
#undef	TEX_						// �w�b�_�[�t�@�C�����Ŏg�p����Ă���\�����������̂�\�ߖ����ɂ��Ă���
#undef	SIZE_
#undef	TIME_-1
#undef	U0
#undef	V0	
#undef	U1
#undef	V1

#define	MAX_			500			// �\���ő吔
#define	TEX_			TEX_PARTICLE// �e�N�X�`���[
#define	SIZE_			0.5f		// �\���T�C�Y
#define TIME_			100			// UV�l

#define U0				2.f  / d3.texInfo[TEX_].fWidth
#define V0				2.f  / d3.texInfo[TEX_].fHeight
#define U1				29.f / d3.texInfo[TEX_].fWidth
#define V1				29.f / d3.texInfo[TEX_].fHeight

//--- �ϐ���` -----------------------------------------------------------------------
static	D3_LPVTX_BUF	vb[		TIME_];			// �����ɒ��_�̏�񂪓���
static	VEC3			vecPos[	 MAX_ ];		// �\�����W
static	VEC3			vecSpeed[MAX_ ];		// �g�U���x
static	int				iTime[	 MAX_ ];		// �\������
static	VEC3			vecAccel[MAX_ ];		// �����x

/*--------------------------------------------------------------*
| <<< �p�[�e�B�N�����C�� >>>
*--------------------------------------------------------------*/
void	ParticleMain(void)
{
	// �g�p���̃p�[�e�B�N���̎��Ԃ��v�Z����
	for(int i = 0; i < MAX_; i++)
	{
		if(iTime[i] > 0){ iTime[i]--;}
		vecSpeed[i] += vecAccel[i];
		vecPos[	 i] += vecSpeed[i];
	}
}
/*-----------------------------------------------------------------------*
| <<< �p�[�e�B�N�������� >>>
*-----------------------------------------------------------------------*/
void	ParticleInit(void)
{
	int	i;

	for(i = 0; i < MAX_; i++)			// �p�[�e�B�N���̎��Ԃ��O�Ƃ���
	{									// ����������
		iTime[i] = 0;
	}

	if(vb[0] != NULL){ return;}		// ���łɃ|���S����o�^���Ă�����A��

	// ���O�Ń|���S���쐬
	for(i = 0; i < TIME_; i++)
	{
		float	fR	 = (SIZE_	/ (float)TIME_) * (float)(TIME_ -i) + SIZE_;
		float	fAlp = (float)i	/ (float)TIME_;
		DWORD	c	 = D3_COL((225.f * fAlp), 255, 255, 255);

		// ���_���
		D3_VTX_LX_	vt[] =				// �p�[�e�B�N���̒��_�f�[�^
		{								// �����ł��Ȃ��� UV �l���ݒ肳��Ȃ�
			-fR, -fR, 0, c, U0, V1,
			-fR,  fR, 0, c, U0, V0,
			 fR,  fR, 0, c, U1, V0,
			 fR, -fR, 0, c, U1, V1,
		};
		D3_VERTEX_CREATE(vt, vb[i], LX, 4);	// ���̃}�N���Œ��_����o�^
	}
}

/*------------------------------------------------------------------------*
| <<< �p�[�e�B�N���`�� >>>
*-------------------------------------------------------------------------*/
void	ParticleDraw(void)
{
	// �ݒ�ύX(�����I����Ɍ��ɖ߂��K�v����)
	d3_light_set(			false);					// ���C�g�I�t
	d3_stat_zread_set(		false);					// �[�x�o�b�t�@�ւ̏������݂𖳌��ɂ���
	d3_stat_culling_set(	false);					// ���ʂ̕\��������
	d3_stat_alpha_greate_set(10);					// �A���t�@�e�X�g ON(RGB�̍��v��10�����Ȃ�ΐF�𓧉�)
	d3_stat_blend_set(		D3_BLEND_ADD);			// ���Z����

	// �p�[�e�B�N���`��
	for(int  i = 0; i < MAX_; i++)					// MAX ���[�v
	{
		if(iTime[i] == 0){continue;}				// ���Ԃ�0�ȏ�̂��̂�\��
													// ���Ԃ��p�^�[�������߂�
		d3_stat_stream_source_set(vb[iTime[i]], sizeof(D3_VTX_LX));

		d3_billboard_set(&d3.mtxBillboard, &vecPos[i]);
													// �`��
		d3_primitive_draw(D3DPT_TRIANGLEFAN, 0, 2);
	}

	// �I�������i�ύX���ꂽ�ݒ�����ɖ߂�)
	d3_stat_blend_set(			D3_BLEND_NORMAL);	// �ʏ�̍����ɖ߂�
	d3_stat_culling_set(		true);				// ���ʂ̕\�������Ȃ�
	d3_stat_alpha_greate_set(0);					// �A���t�@�e�X�g OFF
	d3_light_set(			true);					// ���C�f�B���O����
	d3_stat_zread_set(		true);					// �[�x�o�b�t�@��L���ɂ���
}

/*------------------------------------------------------------------------*
| <<< �p�[�e�B�N���X�^�[�g >>>
|	����		pvecPos = �o�^���W
|				fPower	= �����l
*-------------------------------------------------------------------------*/
void	ParticleStart(VEC3	*pvecPos, float fPower)
{
	for(int i = 0; i < MAX_; i++)
	{
		if(iTime[i] == 0)						// ���Ԃ� 0 �̂��̂�T���ēo�^����
		{
			vecPos[  i] = *pvecPos;			// �ʒu�̐ݒ�
			iTime[	 i] = TIME_;					// �\�����Ԃ̐ݒ�
			vecSpeed[i] = VEC3(0, 0, 0);	// �g�U���x�̓����_���Ō��߂�

			// �����x�v�Z
			vecSpeed[i].x = FRAND2(-fPower, fPower);
			vecSpeed[i].y = FRAND2(-fPower, fPower);
			vecSpeed[i].z = FRAND2(-fPower, fPower);
			break;
		}
	}
}
