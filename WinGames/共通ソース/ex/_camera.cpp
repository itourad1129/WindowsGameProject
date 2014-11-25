//
//	�J��������
//


//--- �ϐ���` ------------------------------------------------------------------------------------
						// ���J�����̎���p(VC6.0�ł͂Ȃ����G���[��)
CAMERA_WORK		camera;// = {R(45)};		// �X�P�[���������l�ݒ�
static	BOOL	bInit     = false;


//--- �f�o�b�O�J�����ړ��p�֐� --------------------------------------------------------------------

static	BOOL	bMoveCam1st = false;		// �f�o�b�O�J�����������t���O
static	VEC3	vecMoveCamRot;				// �f�o�b�O�J�����̉�]�l
static	VEC3	vecDebugCamPos;				// �f�o�b�O�J�����̒������W
static	VEC3	vecDebugCamOfs;				// �f�o�b�O�J�����̒������W�̃I�t�Z�b�g
static	float	fMoveCamDist;				// �f�o�b�O�J�����̒������W�܂ł̋���



/*------------------------------------------------------------------------------*
| <<< �J���������� >>>
*------------------------------------------------------------------------------*/
void	camera_init(void)
{

	camera.fMoveScale = 0.1f;				// �f�o�b�O�J�����̈ړ��l
	camera.fViewAngle = R(45);				// �A���O����������
	bInit			  = true;
}



/*------------------------------------------------------------------------------*
| <<< �J�����ݒ� >>>
|	����	bDefault = true	: �f�t�H���g�̃J�������g�p
*------------------------------------------------------------------------------*/
void	camera_main(void)
{

	//--- ���������K�v�ȏꍇ --------------------------------------
	if(!bInit){ camera_init();}

	//--- �����s��A�ˉe�s��̍쐬 --------------------------------
#ifdef	__RIGHT_HAND
	D3DXMatrixLookAtRH(&d3.mtxView,	&d3.vecCamera, &d3.vecEye, &d3.vecUp);
	if(camera.fOrthoScale > 0)
	{
		D3DXMatrixOrthoRH(&d3.mtxProjection, d3.sRenderW / camera.fOrthoScale, d3.sRenderH / camera.fOrthoScale, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
	else
	{
		D3DXMatrixPerspectiveFovRH(&d3.mtxProjection, camera.fViewAngle, (float)d3.sRenderW / (float)d3.sRenderH, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
#else
	D3DXMatrixLookAtLH(&d3.mtxView,	&d3.vecCamera, &d3.vecEye, &d3.vecUp);
	if(camera.fOrthoScale > 0)
	{
		D3DXMatrixOrthoLH(&d3.mtxProjection, d3.sRenderW / camera.fOrthoScale, d3.sRenderH / camera.fOrthoScale, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
	else
	{
		D3DXMatrixPerspectiveFovLH(&d3.mtxProjection, camera.fViewAngle, (float)d3.viewport.Width / (float)d3.viewport.Height, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
#endif
											// �e�s���ݒ�
	d3.dev->SetTransform(D3DTS_VIEW,	   &d3.mtxView);
	d3.dev->SetTransform(D3DTS_PROJECTION, &d3.mtxProjection);
											// ���_�V�F�[�_�[�p�Ɍv�Z���Ă���
	d3.mtxViewProjection = d3.mtxView *	d3.mtxProjection;
											// d3d_view_proj_mat �̓]�u�s��
	D3DXMatrixTranspose(&d3.mtxViewProjectionTranspose,	&d3.mtxViewProjection);

	//--- �t�s��̍쐬 ----------------------------------------------
	D3DXMatrixInverse(&d3.mtxViewInverse,       NULL, &d3.mtxView);
	D3DXMatrixInverse(&d3.mtxProjectionInverse, NULL, &d3.mtxProjection);

	//--- �ȉ��A�p�[�e�B�N���̎��ɕK�v�ɂȂ�܂� ------------------
	float	tmp;
	D3DXMatrixInverse(&d3.mtxViewInverse, &tmp,	&d3.mtxView);
	d3.mtxBillboard	= d3.mtxViewInverse;
}



/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�J�����ړ� >>>
|	���J�����͈ړ��̂݁A��]�͑ΏۃI�u�W�F�N�g�����[�J���ŉ�
*------------------------------------------------------------------------------*/
void	camera_move_main(void)
{

	//--- ���������� ------------------------------------------------
	if(!bMoveCam1st)
	{
		vecDebugCamPos  = d3.vecEye;		// �������W�̃I�t�Z�b�g

		// �p�x�𓾂�
		GET_ANGLE(&vecMoveCamRot.y, &vecMoveCamRot.x, &d3.vecCamera, &d3.vecEye);
		vecMoveCamRot.z = 0;

		// �����𓾂�
		fMoveCamDist    = D3DXVec3Length(&(d3.vecEye - d3.vecCamera));
		bMoveCam1st     = true;
	}

	//--- �E�h���b�O�ŃJ������]�A�z�C�[���ŃY�[�� ------------------
	if(pad.bRightPush && (pad.sMoveX != 0 || pad.sMoveY != 0 || pad.sMoveZ != 0))
	{
		fMoveCamDist -= (float)pad.sMoveZ * camera.fMoveScale;

		#ifdef	__RIGHT_HAND
			vecMoveCamRot -= VEC3(R(-pad.sMoveY), R(pad.sMoveX), 0);
		#else
			vecMoveCamRot += VEC3(R(-pad.sMoveY), R(pad.sMoveX), 0);
		#endif
	}

	//--- �z�C�[���h���b�O�ňړ� ------------------------------------
	if(pad.bCenterPush && (pad.sMoveX != 0 || pad.sMoveY != 0) && camera.fMoveScale > 0)
	{
		float	fPitch, fYaw;
		GET_ANGLE(&fYaw, &fPitch, &d3.vecEye, &d3.vecCamera);

		MTX	m;
		D3DXMatrixRotationYawPitchRoll(&m, fYaw, fPitch, 0);

		VEC3	a((float)pad.sMoveX, -(float)pad.sMoveY, 0);
		D3DXVec3TransformCoord(&a, &a, &m);
		vecDebugCamOfs -= a * camera.fMoveScale;
	}

	//--- �J�������W�v�Z --------------------------------------------
	MTX		m;
	D3DXMatrixRotationYawPitchRoll(&m, &vecMoveCamRot);

	VEC3	a(0, 0, fMoveCamDist);
	D3DXVec3TransformCoord(        &a, &a, &m);
	
	d3.vecCamera = vecDebugCamPos + vecDebugCamOfs + a;
	d3.vecEye    = vecDebugCamPos + vecDebugCamOfs;
}

