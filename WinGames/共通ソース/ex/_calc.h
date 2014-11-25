//
//	�F�X�Ȍv�Z���}�N�������Ă��܂�
//

#ifndef	__CALC_H

#define	__CALC_H


/*------------------------------------------------------------------------------*
| <<< sin cos �𓯎��ɓ��� >>>
*------------------------------------------------------------------------------*/
//	sin	�� cos		���o�͂� float �^
__inline	void	__cdecl	SINCOS(float *pfSinOut, float *pfCosOut, float fR)
{
	__asm
	{
		fld	fR
		fsincos
		mov	eax, pfCosOut
		mov	edx, pfSinOut
		fstp	dword ptr [eax]
		fstp	dword ptr [edx]
	}
}

/*------------------------------------------------------------------------------*
| <<< �O�p�֐��e�[�u�����쐬 >>>
|	����	iDiv ������
*------------------------------------------------------------------------------*/
extern	void	calc_sincos_init(int iDiv);

/*------------------------------------------------------------------------------*
| <<< sin >>>
|	����	fRad = ���W�A��
*------------------------------------------------------------------------------*/
extern	float	calc_sin_get(float fRad);

/*------------------------------------------------------------------------------*
| <<< cos >>>
|	����	fRad = ���W�A��
*------------------------------------------------------------------------------*/
extern	float	calc_cos_get(float fRad);

/*------------------------------------------------------------------------------*
| <<< �Q�c�x�N�g���̊O�ς����߂� >>>		�� D3DXVec2CCW �Ɠ����̊֐�
|	����	p0, p1 = �Q�c�x�N�g��
|	�߂�l	0  > �E��
|			0  < ����
|			0 == �d�Ȃ��Ă���
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_cross(VEC2 *p0, VEC2 *p1)
{
	return p0->x * p1->y - p0->y * p1->x;
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̊O�ς��v�Z >>>
|	����	pvecA, pvecB = �x�N�g��
|	�o��	pvecOut = �O��
*------------------------------------------------------------------------------*/
inline	void	calc_vec3_cross(VEC3 *pvecOut, VEC3 *pvecA, VEC3 *pvecB)
{
	#ifndef	__WII
		D3DXVec3Cross(pvecOut, pvecA, pvecB);
	#else
		VECCrossProduct((Vec*)pvecA, (Vec*)pvecB, (Vec*)pvecOut);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���𐳋K������ >>>
|	����	pvecA   = �x�N�g��
|	�o��	pvecOut = ���K�����ꂽ�x�N�g��
*------------------------------------------------------------------------------*/
inline	void	calc_vec4_normalize(VEC4 *pvecOut, VEC4 *pvecA)
{

	D3DXVec4Normalize(pvecOut, pvecA);
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���𐳋K������ >>>
|	����	pvecA   = �x�N�g��
|	�o��	pvecOut = ���K�����ꂽ�x�N�g��
*------------------------------------------------------------------------------*/
inline	void	calc_vec3_normalize(VEC3 *pvecOut, VEC3 *pvecA)
{
	#ifndef	__WII
		D3DXVec3Normalize(pvecOut, pvecA);
	#else
		//VECNormalize((Vec*)pvecA, (Vec*)pvecOut);
		nw4r::math::VEC3Normalize(pvecOut, pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �Q�c�x�N�g���𐳋K������ >>>
|	����	pvecA   = �x�N�g��
|	�o��	pvecOut = ���K�����ꂽ�x�N�g��
*------------------------------------------------------------------------------*/
inline	void	calc_vec2_normalize(VEC2 *pvecOut, VEC2 *pvecA)
{
	#ifndef	__WII
		D3DXVec2Normalize(pvecOut, pvecA);
	#else
		nw4r::math::VEC2Normalize(pvecOut, pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̓��ς��v�Z >>>
|	����	pvecA, pvecB = �x�N�g��
|	�߂�l	���ς̒l
*------------------------------------------------------------------------------*/
inline	float	calc_vec3_dot(VEC3 *pvecA, VEC3 *pvecB)
{
	#ifndef	__WII
		return D3DXVec3Dot(pvecA, pvecB);
	#else
		return VECDotProduct((Vec*)pvecA, (Vec*)pvecB);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̓��ς��v�Z >>>
|	����	pvecA, pvecB = �x�N�g��
|	�߂�l	���ς̒l
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_dot(VEC2 *pvecA, VEC2 *pvecB)
{
	#ifndef	__WII
		return D3DXVec2Dot(pvecA, pvecB);
	#else
		return (pvecA->x * pvecB->x) + (pvecA->y * pvecB->y);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̒������v�Z >>>
|	����	pvecA = �x�N�g��
|	�߂�l	����
*------------------------------------------------------------------------------*/
inline	float	calc_vec3_length(VEC3 *pvecA)
{

	//return (float)sqrt((x * x) + (y * y) + (z * z));

	#ifndef	__WII
		return D3DXVec3Length(pvecA);
	#else
		return nw4r::math::VEC3Len(pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̒����̓���Ԃ� >>>	
|	����	pvecA = �x�N�g��
|	�߂�l	����
*------------------------------------------------------------------------------*/
inline	float	calc_vec3_length_sq(VEC3 *pvecA)
{
	
	// �ȉ��̂悤�� sqrt ���g��Ȃ��̂ō���
	// return (x * x) + (y * y) + (z * z);

	#ifndef	__WII
		return D3DXVec3LengthSq(pvecA);
	#else
		return nw4r::math::VEC3Len(pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̒������v�Z >>>
|	����	pvecA = �x�N�g��
|	�߂�l	����
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_length(VEC2 *pvecA)
{

	//return (float)sqrt((x * x) + (y * y));

	#ifndef	__WII
		return D3DXVec2Length(pvecA);
	#else
		return nw4r::math::VEC2Len(pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x�N�g���̒����̓���Ԃ� >>>	
|	����	pvecA = �x�N�g��
|	�߂�l	����
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_length_sq(VEC2 *pvecA)
{
	
	// �ȉ��̂悤�� sqrt ���g��Ȃ��̂ō���
	// return ((x * x) + (y * y);

	#ifndef	__WII
		return D3DXVec2LengthSq(pvecA);
	#else
		return nw4r::math::VEC2Len(pvecA);
	#endif
}


/*------------------------------------------------------------------------------*
|	<<< �s�񓯎m�̊|���Z >>>
|	����	pmtxA, pmtxB = �|�����킹��s��
|	�o��	pmtxOut = ���s�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_multi(MTX *pmtxOut, MTX *pmtxA, MTX *pmtxB)
{
	#ifndef	__WII
		D3DXMatrixMultiply(pmtxOut, pmtxA, pmtxB);
	#else
		//nw4r::math::MTX34Mult(pmtxOut, pmtxA, pmtxB);
		MTXConcat(*pmtxB, *pmtxA, *pmtxOut);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ���s�ړ��s����쐬 >>>
|	����	fX, fY, fZ = ���s�ړ��p�����[�^
|	�o��	pmtxOut    = ���s�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_trans(MTX *pmtxOut, float fX, float fY, float fZ)
{
	#ifndef	__WII
		D3DXMatrixTranslation(pmtxOut, fX, fY, fZ);
	#else
		MTXTrans(*pmtxOut, fX, fY, fZ);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ���s�ړ��s����쐬 >>>
|	����	pvecVec = ���s�ړ��p�����[�^
|	�o��	pmtxOut = ���s�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_trans(MTX *pmtxOut, VEC3 *pvecTrans)
{
	#ifndef	__WII
		D3DXMatrixTranslation(pmtxOut, pvecTrans);
	#else
		//nw4r::math::MTX34RotXYZRad(pmtxOut, pvecRot->x, pvecRot->y, pvecRot->z);
		MTXTrans(*pmtxOut, pvecTrans->x, pvecTrans->y, pvecTrans->z);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �R���̉�]�s����쐬 >>>
|	����	pvecRot = ��]�x�N�g��
|	�o��	pmtxOut = ��]�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_yaw_pitch_roll(MTX *pmtxOut, VEC3 *pvecRot)
{
	#ifndef	__WII
		D3DXMatrixRotationYawPitchRoll(pmtxOut, pvecRot);
	#else
		nw4r::math::MTX34RotXYZRad(pmtxOut, pvecRot->x, pvecRot->y, pvecRot->z);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �R���̉�]�s����쐬 >>>
|	����	fYaw, fPitch, fRoll = �e���̉�]�l
|	�o��	pmtxOut             = ��]�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_yaw_pitch_roll(MTX *pmtxOut, float fYaw, float fPitch, float fRoll)
{
	#ifndef	__WII
		D3DXMatrixRotationYawPitchRoll(pmtxOut, fYaw, fPitch, fRoll);
		//MTX	mtxX, mtxY, mtxZ;
		//D3DXMatrixRotationX(&mtxX, fPitch);
		//D3DXMatrixRotationY(&mtxY, fYaw);
		//D3DXMatrixRotationZ(&mtxZ, fRoll);
		//*pmtxOut = mtxZ * mtxX * mtxY;
	#else
		// ���̏ꍇ���ƁA�s��̊|���鏇�Ԃ��Ⴄ�̂��v���悤�Ȓl���A���Ă��Ȃ�
		#if 0
			OSInitFastCast();
			nw4r::math::MTX34RotXYZRad(pmtxOut, -fPitch, -fYaw, -fRoll);
		#else
			MTX	mtxX, mtxY, mtxZ;
			MTXRotRad((MtxPtr)mtxX, 'x', -fPitch);
			MTXRotRad((MtxPtr)mtxY, 'y', -fYaw);
			MTXRotRad((MtxPtr)mtxZ, 'z', -fRoll);
			MTXConcat(mtxZ, mtxX, mtxZ);
			MTXConcat(mtxZ, mtxY, *pmtxOut);
		#endif
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �w���̉�]�s����쐬 >>>
|	����	fRotX   = ��]�l
|	�o��	pmtxOut = ��]�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_rot_x(MTX *pmtxOut, float fRotX)
{
	#ifndef	__WII
		D3DXMatrixRotationX(pmtxOut, fRotX);
	#else
		MTXRotRad((MtxPtr)pmtxOut, 'x', fRotX);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �x���̉�]�s����쐬 >>>
|	����	fRotY   = ��]�l
|	�o��	pmtxOut = ��]�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_rot_y(MTX *pmtxOut, float fRotY)
{
	#ifndef	__WII
		D3DXMatrixRotationY(pmtxOut, fRotY);
	#else
		MTXRotRad((MtxPtr)pmtxOut, 'y', fRotY);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �y���̉�]�s����쐬 >>>
|	����	fRotZ   = ��]�l
|	�o��	pmtxOut = ��]�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_rot_z(MTX *pmtxOut, float fRotZ)
{
	#ifndef	__WII
		D3DXMatrixRotationZ(pmtxOut, fRotZ);
	#else
		MTXRotRad((MtxPtr)pmtxOut, 'z', fRotZ);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< �g��s����쐬 >>>
|	����	fScale  = �g��l
|	�o��	pmtxOut = ��]�s��
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_scale(MTX *pmtxOut, float fScale)
{
	#ifndef	__WII
		D3DXMatrixScaling(pmtxOut, fScale);
	#else
		MTXScale((MtxPtr)pmtxOut, fScale, fScale, fScale);
	#endif
}
inline	void	calc_mtx_scale(MTX *pmtxOut, float fXScale, float fYScale, float fZScale)
{
	#ifndef	__WII
		D3DXMatrixScaling(pmtxOut, fXScale, fYScale, fZScale);
	#else
		MTXScale((MtxPtr)pmtxOut, fXScale, fYScale, fZScale);
	#endif
}

/*------------------------------------------------------------------------------*
| <<< �Q�c�x�N�g�������W�ϊ� >>>
|	����	pvecIn  = �x�N�g��
|			pmtx    = �s��
|	�o��	pvecOut = �x�N�g��
*------------------------------------------------------------------------------*/
extern	void	calc_vec2_transform_coord(VEC2 *pvecOut, VEC2 *pvecIn, MTX *pmtx);

/*------------------------------------------------------------------------------*
| <<< �R�c�x�N�g�������W�ϊ� >>>
|	����	pvecIn  = �x�N�g��
|			pmtx    = �s��
|	�o��	pvecOut = �x�N�g��
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_transform_coord(VEC3 *pvecOut, VEC3 *pvecIn, MTX *pmtx);

/*------------------------------------------------------------------------------*
| <<< �R�c���W���X�N���[�����W�֕ϊ� >>>	��D3DXVec3Project�Ɠ�����������܂�
|	����	pvecPos   = �R�c���W
|			pmtxWorld = ���[���h�s��
|	�o��	pvecOut	  = �Q�c���W
|	���r���[�|�[�g�̓J�b�g
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_project(VEC3 *pvecOut, VEC3 *pvecPos, MTX *pmtxWorld);

/*------------------------------------------------------------------------------*
| <<< �Q�c�x�N�g���� Catmull-Rom �ŕ⊮ >>>
|	����	p0, p1, p2, p3 = �ʒu�x�N�g��
|			s			   = �W��(0.0f �` 1.0f)
|	�߂�l	pvecOut		   = ���Z����
*------------------------------------------------------------------------------*/
extern	void	calc_vec2_catmullrom(VEC2 *pvecOut, VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3, float s);

/*------------------------------------------------------------------------------*
| <<< �t�s����쐬 >>>				��D3DXMatrixInverse�Ƃقړ�����������܂�
|	����	pmtxIn  = �t�s��ɂ������s��
|	�o��	pmtxOut = �t�s��
*------------------------------------------------------------------------------*/
extern	void	calc_mtx_inverse(MTX *pmtxOut, MTX *pmtxIn);




/*------------------------------------------------------------------------------*
| <<< ���ʂ��x�N�g�������o�� >>>
|	����	pplane  = ����
|	�o��	pvecOut = �x�N�g��
*------------------------------------------------------------------------------*/
inline	void	calc_plane_to_vec(VEC3 *pvecOut, PLANE *pplane)
{
	#ifndef	__WII
		//VEC3	vecPlane(plane.a,   plane.b,   plane.c);
		*pvecOut = VEC3(pplane->a, pplane->b, pplane->c);
	#else
		//VEC3	vecPlane(plane.N.x, plane.N.y, plane.N.z);
		*pvecOute = VEC3(pplane->N.x, pplane->N.y, pplane->N.z);
	#endif
}

/*------------------------------------------------------------------------------*
| <<< �R���W��蕽�ʂ��쐬���� >>>			�� D3DX �� D3DXPlaneFromPoints �Ɠ����ł��B
|	����	pvecA, pvecB, pvecC = ���_
|	�o��	pplaneOut = ��
*------------------------------------------------------------------------------*/
extern	void	calc_plane_create(PLANE *pplaneOut, VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2);

/*------------------------------------------------------------------------------*
| <<< ���ʂƃx�N�g���̓��ς��v�Z >>>		�� D3DX ��D3DXPlaneDotNormal �Ɠ����ł��B
|	����	pplane = ����
|			pvec   = �x�N�g��
|	�߂�l	���ς̒l
*------------------------------------------------------------------------------*/
extern	float	calc_plane_vec3_dot(PLANE *pplane, VEC3 *pvec);




/*------------------------------------------------------------------------------*
| <<< ���O�Ŏˉe�s��̍쐬 >>>				�� D3DXMatrixPerspectiveFovLH
|	�� d3.mtxProjection �ɍ쐬���܂��B
*------------------------------------------------------------------------------*/
extern	MTX	*calc_mtx_perspective(MTX *pOut, float fFov, float fZNear, float fZFar);

/*------------------------------------------------------------------------------*
| <<< ���O�Ńr���[�s��̍쐬 >>>				�� D3DXMatrixLookAtLH
|	�� d3.mtxView �ɍ쐬���܂�
*------------------------------------------------------------------------------*/
extern	MTX	*calc_mtx_lookat(MTX *pOut, VEC3 *pCam, VEC3 *pAt, VEC3 *pUp);

/*------------------------------------------------------------------------------*
| <<< �R�c���W���X�N���[�����W�֕ϊ� >>>	��D3DXVec3Project�Ɠ�����������܂�
|	����	pvecPos        = �R�c���W
|			pmtxProjection = �ˉe�s��
|			pmtxView	   = �r���[�s��
|			pmtxWorld	   = ���[���h�s��
|	�o��	pvecOut		   = �Q�c���W
|	���r���[�|�[�g�̓J�b�g
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_projection(VEC3 *pvecOut, VEC3 *pvecPos, MTX *pmtxProjection, MTX *pmtxView, MTX *pmtxWorld);

/*------------------------------------------------------------------------------*
| <<< �Q�̃x�N�g�����p�x�𓾂� >>>
|	����	a, b = �x�N�g��
|	�߂�l	�p�x�i���W�A���j
*------------------------------------------------------------------------------*/
extern	float	calc_vec3_vec_to_vec_angle(VEC3 *a, VEC3 *b);

/*------------------------------------------------------------------------------*
| <<< �ʂƓ_�x�N�g���Ƃ̓��ς��v�Z >>>		�� D3DXPlaneDotNormal
|	����	pplaneA = ��
|			pvecB   = ���_�x�N�g��
|	�߂�l	�ʂƒ��_�x�N�g���̓���
*------------------------------------------------------------------------------*/
extern	float	calc_plane_dot_normal(PLANE *pplaneA, VEC3 *pvecB);

/*------------------------------------------------------------------------------*
| <<< ���ʂƓ_�Ƃ̋��������߂� >>>
|	����	pvecNormal = �ʂ̖@��
|			pvecP0     = �ʏ�̍��W
|			pvecP1     = �ʏ�ɂ͖������W
|	�߂�l	�ʂ� p1 �Ƃ̋���
|
|	�@���x�N�g�� n �ƃx�N�g�� p - p0 �Ƃ̂Ȃ��p���ƂƂ���ƁA
|	n �� p - p0 �Ƃ̓��ς� n�E(p - p0) = |n||p-p0|cos�Ƃƕ\�����Ƃ��ł���B
|	�����ŁA���ʂƓ_�Ƃ̋��� h ��
|	�Ƃ�p���ā@h = |p-p0| cos�Ƃƕ\�����Ƃ��ł��邩��A
|	�����̎��𐮗�����ƁAh = n�E(p - p0) / |n| �ƂȂ�A
|	���̎����畽�ʂƓ_�Ƃ̋��������܂�B
*------------------------------------------------------------------------------*/
extern	float	calc_plane_vec_distance(VEC3 *pvecNormal, VEC3 *pvecP0, VEC3 *pvecP1);

/*------------------------------------------------------------------------------*
| <<< �s�񂩂�I�C���[�p�𒊏o���� >>>
|	����	m = �s��
|	�߂�l	yaw pitch roll
*------------------------------------------------------------------------------*/
extern	VEC3	calc_mtx_to_eular(MTX *m);

/*------------------------------------------------------------------------------*
| <<< �x�N�g������]�x�N�g�������߂� >>>
|	����	v0,	v1 = �x�N�g��
|	�o��	a	   = x,	y �ɉ�]�p�i���W�A���j		
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_vec_to_vec_rotation(VEC3 *a, VEC3 *v0, VEC3 *v1);

/*------------------------------------------------------------------------------*
| <<< �X�N���[�����W����R�c�̃x�N�g�����쐬 >>>
|	����	pos		 = 2D ���W			���ʏ�Apos.z �� 0 �œn������
|			mtxWorld = ���[���h�s��
|	�߂�l			   3D ���W
*------------------------------------------------------------------------------*/
extern	VEC3	calc_vec3_screen_to_vector(VEC3 *pos, MTX *mtxWorld);
extern	VEC3	calc_vec3_screen_to_vector(VEC3 *pos);

/*------------------------------------------------------------------------------*
| <<< �R�c�x�N�g������X�N���[�����W���쐬 >>>
|	����	vecPos	 = 3D ���W
|			mtxWorld = ���[���h�s��
|	�o��	vecOut	 = �ϊ���̃x�N�g���i 2D ���W �j
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_vector_to_screen(VEC3 *vecOut, VEC3 *vecPos, MTX *mtxWorld);
extern	void	calc_vec3_vector_to_screen(VEC3 *vecOut, VEC3 *vecPos);


// �X�v���C��
extern	VEC2	*cacl_vec2_spline(VEC2 *pv0, const VEC2 *pv1, const VEC2 *pt1, const VEC2 *pv2, const VEC2 *pt2, float t);



#endif					/// end calc.h

