//
//	�J����
//

#include	"main.h"

//--- �\���̒�` ----------------------------------------------
struct	CAMERA_MOVE				// �J�����̍\����
{
	int		fTime;				// �J�����̈ړ�����
	float	fX, fY, fZ;			// �J�����̈ړ��ʒu
};

//--- �J�����ړ��p�ϐ� ---------------------------------------------------
static	int		iTime;			// �J�����̈ړ����Ԍv���p
static	int		iCount;			// ���̃J�����e�[�u���̃J�E���g�p
static	VEC3	vecSpeed;		// �J�����̈ړ����x

static	CAMERA_MOVE acamera[] =		// �J�����̈ړ��p�e�[�u��
{//TIME		X		Y		Z
	{ 500,	0,		-8.5f,	-3.0f},	// TIME��60��1�b�ƂȂ�܂��B500�Ȃ�8�b���炢
	{ 300,	0,		-20.0f,	-22.0f},	//
	{-1},
};

/*-------------------------------------------------------------------*
| <<< �J�����ړ������� >>>
*-------------------------------------------------------------------*/
void	CameraMoveInit(void)
{
	iCount		= 0;
	iTime		= acamera[iCount].fTime; // �J�����̕ω�����
	vecSpeed	= VEC3(0, 0, 0);		// �J�����̕ω����x
	d3.vecCamera.x = acamera[iCount].fX;
	d3.vecCamera.y = acamera[iCount].fY;
	d3.vecCamera.z = acamera[iCount].fZ;
	d3.light.Direction = d3.vecCamera * -1;
}

/*------------------------------------------------------*
| <<< �J�����ړ����C�� >>>
*-------------------------------------------------------*/
void	CameraMoveMain(void)
{
	//---- ���Ԃ�0�ɂȂ�܂ňړ��������� ------------------------
	if(iTime > 0)
	{
		iTime--;
		d3.vecCamera	+= vecSpeed;		// �J�������W����S

		d3.light.Direction = d3.vecCamera * -1;	// �Â��Ȃ�Ȃ��悤�Ƀ��C�g���ݒ�
	}
	//--- ���Ԃ�0�̎��͎��̍��W�܂ł̑��x���v�Z ---------------------------------
	else
	{
		CAMERA_MOVE *w = &acamera[iCount];
		iTime			= w->fTime;

		vecSpeed		= (VEC3(w->fX, w->fY, w->fZ) - d3.vecCamera) / (float)iTime;
		iCount++;

		if(acamera[iCount].fTime == -1){iCount = 0;}
	}
	// �f�o�b�N�p�ɃJ�������W�\��
	Printf(0, 468, 12, "CAMERA %.2f, %.2f, %.2f", d3.vecCamera.x, d3.vecCamera.y, d3.vecCamera.z);
}