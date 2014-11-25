//
//	�Փ˔���
//



#define	EPSIRON		0.00001f // �덷



/*------------------------------------------------------------------------------*
| <<< �_���ʂ̂ǂ��瑤�ɂ��邩���ׂ� >>>
|	����	pplane  = ��
|			pvecPos = �_
|	�߂�l	0  < ����
|			0  > �\��
|			0 == �ʂ̏�
*------------------------------------------------------------------------------*/
float	collision_plane_point(PLANE *pplane, VEC3 *pvecPos)
{

	// ���ʂ̖@���Ɠ_�x�N�g���Ƃ̓���
	return calc_vec3_dot(&VEC3(pplane->a, pplane->b, pplane->c), pvecPos);
}



/*------------------------------------------------------------------------------*
| <<< �_�������̂ǂ��瑤�ɂ��邩���ׂ� >>>
|	����	pvecPoint = ���ׂ������W
|			pvecLine0, pvecLine1 = ��
|	�߂�l	-1 : ����
|			 1 : �E��
|			 0 : ���̏�
*------------------------------------------------------------------------------*/
int		collision_line_point(VEC2 *pvecPoint, VEC2 *pvecLine0, VEC2 *pvecLine1)
{
					// ���O�ς̌v�Z�ł�
	float	fRet = ((pvecPoint->x - pvecLine0->x) * (pvecPoint->y - pvecLine1->y) - (pvecPoint->y - pvecLine0->y) * (pvecPoint->x - pvecLine1->x));
	if(fRet < 0){ return  1;}
	if(fRet > 0){ return -1;}
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< �����Ɛ����Ƃ̌������� >>>
|	����	pvecLine0 - pvecLine1 = ���ׂ�������
|			pvecRay0  - pvecRay1  = ����������������
|	�߂�l	TRUE	: �������Ă���
|			FALSE	: �������Ă��Ȃ�
*------------------------------------------------------------------------------*/
BOOL	collision_line_ray(VEC2 *pvecLine0, VEC2 *pvecLine1, VEC2 *pvecRay0, VEC2 *pvecRay1)
{

	// (x0,y0)-(x1,y1)�ɂ����āAx0,y0���������l�ɂȂ�悤�ɕ��בւ�
	// �ȉ��̋�`�`�F�b�N�Ɏg�����߁A���̌������ς���Ă��C�ɂ��Ȃ�
	float	fLineX0 = pvecLine0->x;
	float	fLineX1 = pvecLine1->x;
	if(fLineX0 > fLineX1){ SWAPF(fLineX0, fLineX1);}
	float	fLineY0 = pvecLine0->y;
	float	fLineY1 = pvecLine1->y;
	if(fLineY0 > fLineY1){ SWAPF(fLineY0, fLineY1);}

	float	fRayX0 = pvecRay0->x;
	float	fRayX1 = pvecRay1->x;
	if(fRayX0 > fRayX1){ SWAPF(fRayX0, fRayX1);}
	float	fRayY0 = pvecRay0->y;
	float	fRayY1 = pvecRay1->y;
	if(fRayY0 > fRayY1){ SWAPF(fRayY0, fRayY1);}

	//--- ���ׂ�K�v�̂Ȃ����̂͂����ł͂��� -----------------
	// ��`�����g�̊O�ɂ��邩���`�F�b�N
	if(fLineX0 < fRayX0 && fLineX1 < fRayX0){ return FALSE;}
	if(fLineX0 > fRayX1 && fLineX1 > fRayX1){ return FALSE;}
	if(fLineY0 < fRayY0 && fLineY1 < fRayY0){ return FALSE;}
	if(fLineY0 > fRayY1 && fLineY1 > fRayY1){ return FALSE;}

	//if(pvecLine0->x < pvecRay0->x && pvecLine1->x < pvecRay0->x){ return FALSE;}
	//if(pvecLine0->x > pvecRay1->x && pvecLine1->x > pvecRay1->x){ return FALSE;}
	//if(pvecLine0->y < pvecRay0->y && pvecLine1->y < pvecRay0->y){ return FALSE;}
	//if(pvecLine0->y > pvecRay1->y && pvecLine1->y > pvecRay1->y){ return FALSE;}

	//--- �O�ς𒲂ׂ� ---------------------------------------
	VEC2	v0 = *pvecRay1  - *pvecRay0;
	VEC2	v1 = *pvecLine0 - *pvecRay0;
	VEC2	v2 = *pvecLine1 - *pvecRay0;

	//float	a = D3DXVec2CCW(&v0, &v1);		// �߂�l�����Ȃ獶
	//float	b = D3DXVec2CCW(&v0, &v2);		// �߂�l�����Ȃ�E
	float	a = calc_vec2_cross(&v0, &v1);	// �߂�l�����Ȃ獶
	float	b = calc_vec2_cross(&v0, &v2);	// �߂�l�����Ȃ�E

	if(a == 0.f || b == 0.f){ return TRUE;}	// �����̏�
	if(a >  0.f && b <  0.f){ return TRUE;}	// ����
	if(a <  0.f && b >  0.f){ return TRUE;}	// ����
	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< �~�Ɠ_�̏Փ˔��� >>>
|	����	pvecCircle, fR = �~�̒��S���W�Ɣ��a
|			pvecPoint      = �_���W
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_circle_point(VEC2 *pvecCircle, float fR, VEC2 *pvecPoint)
{

	float	x = pvecCircle->x - pvecPoint->x;
	float	y = pvecCircle->y - pvecPoint->y;
	float	r = fR;
	return (x * x + y * y) < (r * r);
}



/*------------------------------------------------------------------------------*
| <<< �~�Ɛ��̏Փ˔��� >>>
|	����	pvecCircle, fR       = �~�̒��S���W�Ɣ��a
|			pvecLine0, pvecLine1 = �����̎n�_�ƏI�_
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_circle_line(VEC2 *pvecCircle, float fR, VEC2 *pvecLine0, VEC2 *pvecLine1)
{

	//--- �����̒[���~�͈͓̔��Ȃ�΃q�b�g --------------------------
	if(collision_circle_point(pvecCircle, fR, pvecLine0)){ return TRUE;}
	if(collision_circle_point(pvecCircle, fR, pvecLine1)){ return TRUE;}

	//--- �~�̒��_�ƒ����܂ł̋��������߂� --------------------------
	// �_���璼���ɐ����ȒP�ʃx�N�g��(t1)�����߂�B
	VEC2	s1, s2, s3, t1, t2, v;

	// �x�N�g�� line1 - line0
	s1.x = pvecLine1->x - pvecLine0->x;
	s1.y = pvecLine1->y - pvecLine0->y;

	// �x�N�g�� cir - line0
	s2.x = pvecCircle->x - pvecLine0->x;
	s2.y = pvecCircle->y - pvecLine0->y;

	// �x�N�g�� s1 �� s2 �̊O��(�Q������ԂȂ̂ły���ɕ��s�ȃx�N�g��)
	float	z = s1.x * s2.y - s1.y * s2.x;

	// �~�̒��S��������ɑ��݂���Ƃ��O�ς͂O�ɂȂ�
	if(z == 0)
	{
		v.x = pvecCircle->x;
		v.y = pvecCircle->y;
	}
	else
	{
		// �x�N�g�� s1 �� s3 �̊O��(line1 - line0 �̐������ɂȂ�)
		s3.x =  (s1.y * z);
		s3.y = -(s1.x * z);

		// �������x�N�g���̐��K��
		calc_vec2_normalize(&t1, &s3);

		// ���ς��狗�����Z�o
		t2.x = pvecLine0->x - pvecCircle->x;
		t2.y = pvecLine0->y - pvecCircle->y;
		float	len = calc_vec2_dot(&t1, &t2);

		// ��_�Ɖ~�̒��S�̋��������a�ȏ�Ȃ�Փ˂��Ă��Ȃ�
		if(len > fR){ return FALSE;}

		// �~�̒��S���� line1 - line0 �Ɉ������������̑��̍��W�����߂�
		v.x = pvecCircle->x + (t1.x * len);
		v.y = pvecCircle->y + (t1.y * len);
	}

	// v �����C���ԂɏՓ˂Ƃ݂Ȃ�
	if((pvecLine0->x < v.x) && (v.x < pvecLine1->x)){ return TRUE;}
	if((pvecLine1->x < v.x) && (v.x < pvecLine0->x)){ return TRUE;}
	if((pvecLine0->y < v.y) && (v.y < pvecLine1->y)){ return TRUE;}
	if((pvecLine1->y < v.y) && (v.y < pvecLine0->y)){ return TRUE;}

	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< �~�Ɖ~�̏Փ˔��� >>>
|	����	pvecCircleA, fRadiusA = �~�̒��S���W�Ɣ��a
|			pvecCircleB, fRadiusB = �~�̒��S���W�Ɣ��a
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_circle_circle(VEC2 *pvecCircleA, float fRadiusA, VEC2 *pvecCircleB, float fRadiusB)
{

	float	x = pvecCircleA->x - pvecCircleB->x;
	float	y = pvecCircleA->y - pvecCircleB->y;
	float	r = fRadiusA       + fRadiusB;
	return	(x * x + y * y) < (r * r);
}



/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ɖ~�̏Փ˔��� >>>	�����v�̔��Ύ���Œ�`�̂���
|	����	pvecBox0�`3 = �l�p�`�̎l���_
|			pCircle		= �~�̒��S���W
|			fR			= �~�̔��a
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_box_circle(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecCircle, float fR)
{

	// �O�ς��g���~�̒��S���l�p�`�Ɋ܂܂�Ă��邩���ׂ�
	if(collision_line_point(pvecCircle, pvecBox0, pvecBox1) == 1
	&& collision_line_point(pvecCircle, pvecBox1, pvecBox2) == 1
	&& collision_line_point(pvecCircle, pvecBox2, pvecBox3) == 1
	&& collision_line_point(pvecCircle, pvecBox3, pvecBox0) == 1)
	{
		return TRUE;
	}

	// �l�p�`�̕ӂƉ~�̌�������
	if(collision_circle_line(pvecCircle, fR, pvecBox0, pvecBox1)){ return TRUE;}
	if(collision_circle_line(pvecCircle, fR, pvecBox1, pvecBox2)){ return TRUE;}
	if(collision_circle_line(pvecCircle, fR, pvecBox2, pvecBox3)){ return TRUE;}
	if(collision_circle_line(pvecCircle, fR, pvecBox3, pvecBox0)){ return TRUE;}

	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ɠ_�̏Փ˔��� >>>	�����v�̔��Ύ���Œ�`�̂���
|	����	pvecBox0�`3 = �l�p�`�̎l���_
|			pPoint		= �~�̒��S���W
|			fR			= �~�̔��a
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_box_point(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecPoint)
{

	// �O�ς��g���~�̒��S���l�p�`�Ɋ܂܂�Ă��邩���ׂ�
	if(collision_line_point(pvecPoint, pvecBox0, pvecBox1) == 1
	&& collision_line_point(pvecPoint, pvecBox1, pvecBox2) == 1
	&& collision_line_point(pvecPoint, pvecBox2, pvecBox3) == 1
	&& collision_line_point(pvecPoint, pvecBox3, pvecBox0) == 1)
	{
		return TRUE;
	}
	
	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ǝl�p�`�̏Փ˔��� >>>
|	����	p0, p1 = �l�p�`�̍���ƉE��
|			p2, p3 = �l�p�`�̍���ƉE��
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_box_box(VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3)
{

	if(p0->x > p3->x){ return FALSE;}
	if(p1->x < p2->x){ return FALSE;}
	if(p0->y > p3->y){ return FALSE;}
	if(p1->y < p2->y){ return FALSE;}
	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ɛ��̏Փ˔��� >>>		�������Ɛ����Ƃ̌�������
|	����	pvecBox0 �` pvecBox3  = �l�p�`(���v���)
|			pvecLine0 - pvecLine1 = ����
|	�߂�l	TRUE or FALSE
*------------------------------------------------------------------------------*/
BOOL	collision_box_line(VEC2 *pvecBox0,  VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3,
					   VEC2 *pvecLine0, VEC2 *pvecLine1)
{

	//--- �����̗��[����`�̒��ɂ��邩���ׂ� ------------------------
	if(collision_box_point(pvecBox0, pvecBox1, pvecBox2, pvecBox3, pvecLine0)){ return TRUE;}
	if(collision_box_point(pvecBox0, pvecBox1, pvecBox2, pvecBox3, pvecLine1)){ return TRUE;}

	//--- �e�ӂƂ̌�������
	if(collision_line_ray(pvecBox0, pvecBox1, pvecLine0, pvecLine1)){ return TRUE;}
	if(collision_line_ray(pvecBox1, pvecBox2, pvecLine0, pvecLine1)){ return TRUE;}
	if(collision_line_ray(pvecBox2, pvecBox3, pvecLine0, pvecLine1)){ return TRUE;}
	if(collision_line_ray(pvecBox0, pvecBox3, pvecLine0, pvecLine1)){ return TRUE;}

	return	FALSE;
}



/*------------------------------------------------------------------------------*
| <<< ���Ƌ��̏Փ˔��� >>>
|	����	pvecSphere			= ��0 �̒��S���W
|			fRadius				= ��0 �̑傫��
|			pvecSphere			= ��1 �̒��S���W
|			fRadius				= ��1 �̑傫��
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_sphere_sphere(VEC3 *pvecSphere0, float fRadius0, VEC3 *pvecSphere1, float fRadius1)
{

	if(calc_vec3_length(&VEC3(*pvecSphere0 - *pvecSphere1)) < fRadius0 + fRadius1){ return true;}
	else																		  { return false;}
}



/*------------------------------------------------------------------------------*
| <<< ���Ɛ����̌������� >>>
|	����	pvecSphere			= ���̒��S���W
|			fRadius				= ���̑傫��
|			pvecLine0,pvecLine1 = �����̎n�_�ƏI�_ 
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
#if 0
BOOL	collision_sphere_line(VEC3 *pvecSphere, float fRadius, VEC3 *pvecLine0, VEC3 *pvecLine1)
{

	//--- �����̒[�����Ɋ܂܂�Ă��邩�`�F�b�N ----------------------
	float	r  = fRadius;
	float	x0 = pvecSphere->x - pvecLine0->x;
	float	y0 = pvecSphere->y - pvecLine0->y;
	float	z0 = pvecSphere->z - pvecLine0->z;
	float	x1 = pvecSphere->x - pvecLine1->x;
	float	y1 = pvecSphere->y - pvecLine1->y;
	float	z1 = pvecSphere->z - pvecLine1->z;
	if(((x0 * x0) + (y0 * y0) + (z0 * z0)) < (r * r)){ return TRUE;}
	if(((x1 * x1) + (y1 * y1) + (z1 * z1)) < (r * r)){ return TRUE;}

	//--- ���̒��_�ƒ����܂ł̋��������߂� --------------------------

	// �_���璼���ɐ����ȒP�ʃx�N�g��(t1)�����߂�B
	VEC3	s1, s2, s3, s4, t1, t2, v;

	// �x�N�g�� la - lb
	s1 = *pvecLine1 - *pvecLine0;

	// �x�N�g�� la - cir
	s2 = *pvecSphere - *pvecLine0;

	// s1, s2 �̊O�� s3
	calc_vec3_cross(&s3, &s1, &s2);

	// �~�̒��S��������ł͂Ȃ��H
	if(calc_vec3_length_sq(&s3) > 0.00001f)
	{
		// s1, s3 �̊O�� s4 = ������
		calc_vec3_cross(&s4, &s1, &s3);

		// ���K��
		calc_vec3_normalize(&t1, &s4);

		// ���ς��狗�����Z�o
		t2 = *pvecLine0 - *pvecSphere;

		float	fLen = calc_vec3_dot(&t1, &t2);

		// ��_�Ɖ~�̒��S�̋��������a�ȏ�Ȃ�m�f
		if(fLen > r){ return FALSE;}

		// ���̒��S(cir)���璼��(pLine->a, pLine->b)�Ɉ������������̑��̍��W(v)
		v = *pvecSphere + t1 * fLen;
	}
	// ���S��������
	else
	{
		v = *pvecSphere;
	}

	// v �� pvecLine0 �� pvecLine1 �̊Ԃɂ���΂n�j
	if((pvecLine0->x < v.x) && (v.x < pvecLine1->x)){ return TRUE;}
	if((pvecLine1->x < v.x) && (v.x < pvecLine0->x)){ return TRUE;}
	if((pvecLine0->y < v.y) && (v.y < pvecLine1->y)){ return TRUE;}
	if((pvecLine1->y < v.y) && (v.y < pvecLine0->y)){ return TRUE;}
	if((pvecLine0->z < v.z) && (v.z < pvecLine1->z)){ return TRUE;}
	if((pvecLine1->z < v.z) && (v.z < pvecLine0->z)){ return TRUE;}

	//if ( (pLine->Pt0.Px < v.x) && (v.x < pLine->Pt1.Px) ) return TRUE;
	//if ( (pLine->Pt1.Px < v.x) && (v.x < pLine->Pt0.Px) ) return TRUE;
	//if ( (pLine->Pt0.Py < v.y) && (v.y < pLine->Pt1.Py) ) return TRUE;
	//if ( (pLine->Pt1.Py < v.y) && (v.y < pLine->Pt0.Py) ) return TRUE;
	//if ( (pLine->Pt0.Pz < v.z) && (v.z < pLine->Pt1.Pz) ) return TRUE;
	//if ( (pLine->Pt1.Pz < v.z) && (v.z < pLine->Pt0.Pz) ) return TRUE;

	return FALSE;
}
#endif



/*------------------------------------------------------------------------------*
| <<< ���Ɛ����̌������� >>>
|	����	pvecSphere = ���̒��S���W
|			fRadius	   = ���̑傫��
|			pvecLine   = �����̎n�_
|			pvecDir	   = �����̌���
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_sphere_ray(VEC3 *pvecSphere, float fR, VEC3 *pvecLine, VEC3 *pvecDir)
{

	VEC3	e = *pvecSphere;
	VEC3	v;
	D3DXVec3Normalize(&v, pvecDir);
	
	VEC3	r = *pvecLine - *pvecSphere;
	VEC3	a = D3DXVec3Dot(    &e, &v) * v - e;
	VEC3	c = r - D3DXVec3Dot(&r, &v) * v;

	float	fAlpha = D3DXVec3LengthSq(&a);
	float	fBeta  = D3DXVec3Dot(     &a, &c);
	float	fOmega = D3DXVec3LengthSq(&c) - fR * fR;

	// �Փ˔���
	float	tmp = fBeta * fBeta - fAlpha * fOmega;

	// �Փ˂��Ȃ����~�܂��Ă���ꍇ�͕s���l��Ԃ�
	if(fabs(fAlpha) <= EPSIRON || tmp < 0)
	{
		// ���ɂ߂荞��ł���ꍇ�͏Փ˂�Ԃ�
		if(fOmega < 0.0f){ return TRUE;}
		return FALSE;
	}

	// ���݂̈ʒu�ł߂荞��ł��邩�`�F�b�N
	if(fOmega < 0.0f){ return TRUE;}

	// �w��̊Ԃł͏Փ˂��Ȃ�
	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< ���C�ƎO�p�`�̌������� >>>		�����ʂ� FALSE ��Ԃ��܂�
|	����	pvecPos						 = �J�n�ʒu
|			pvecDir						 = ��̕����x�N�g��
|			pvecTriA, pvecTriB, pvecTriC = �O�p�`�̔C�ӂ̒��_
|	�o��	pvecOut						 = �������W
|	�߂�l	TRUE : �Փ˂��Ă���
*------------------------------------------------------------------------------*/
BOOL	collision_triangle_ray(VEC3 *pvecOut, VEC3 *pvecPos, VEC3 *pvecDir, VEC3 *apvecTri)
{

	VEC3	vecP;							// *pvecDir �� avecEdge[1](apvecTri[2] - apvecTri[0]) �̊O��
	VEC3	vecQ;
	VEC3	vecT;							// *pvecPos �� avecTri[0] �����ԃx�N�g��
	float	fDet;							// avecEdge[0]((apvecTri[1] - apvecTri[0]) �� *pvecDir �̓���
	float	fU, fV;//, fT;

	//--- apvecEdge[0] �����L����Q�x�N�g�����Z�o -------------------
	VEC3	avecEdge[2];
	avecEdge[0] = apvecTri[1] - apvecTri[0];
	avecEdge[1] = apvecTri[2] - apvecTri[0];

	//--- U �p�����[�^�[���v�Z --------------------------------------
	calc_vec3_cross(&vecP, pvecDir, &avecEdge[1]);

    // ���ς��O�ɋ߂��ꍇ�͎O�p�`�̕��ʂɂ���
	fDet = calc_vec3_dot(&avecEdge[0], &vecP);
	if(fDet > 0)
	{
		vecT  = *pvecPos - apvecTri[0];
	}
	else
	{
		vecT  = apvecTri[0] - *pvecPos;
		fDet *= -1;
	}
	if(fDet < 0.0001f){ return FALSE;}

	// U �p�����[�^�[����уe�X�g���E���v�Z
	fU = calc_vec3_dot(&vecT, &vecP);
	if((fU < 0.0f) || (fU > fDet)){ return FALSE;}

	//--- V �p�����[�^�[���聕�v�Z ----------------------------------
	calc_vec3_cross(&vecQ, &vecT, &avecEdge[0]);

	// V �p�����[�^�[����уe�X�g���E���v�Z
	fV = calc_vec3_dot(pvecDir, &vecQ);
	if((fV < 0.0f) || ((fU + fV) > fDet)){ return FALSE;}

	//--- ��_�̌v�Z ------------------------------------------------
	if(pvecOut != NULL)
	{
		//fT				= calc_vec3_dot(&avecEdge[1], &vecQ);
		float	fDetInv = 1.0f / fDet;
		fU			   *= fDetInv;
		fV			   *= fDetInv;
		*pvecOut		= apvecTri[0] + (fU * (apvecTri[1] - apvecTri[0])) + (fV * (apvecTri[2] - apvecTri[0]));
	}

	//--- �\������ --------------------------------------------------
	// �������܂������Ă��邩���������
	PLANE	plane;
	calc_plane_create(&plane, &apvecTri[0], &apvecTri[1], &apvecTri[2]);
	VEC3	v = *pvecPos - apvecTri[0];
	float	a = collision_plane_point(&plane, &v);

	calc_vec3_normalize(&v, pvecDir);
	v		  = (*pvecPos + v * 10000.f) - apvecTri[0];
	float	b = collision_plane_point(&plane, &v);

	// �܂������Ă��Ȃ�
	if(a >= 0 || b < 0){ return FALSE;}

	return TRUE;
}




#if 0


/**************************************************************************************************
	[[[	�Փˊ֌W�֌W ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< ���ƒ����̂̌������� >>>
|	����	pStart - pEnd = ����
|			pV0, pV1, pV2 = �O�p�`���ꂼ��̒��_
|	�o��	pOut		  = ��_�o�͗p (NULL�Ȃ�o�͂��Ȃ�)
|	�߂�l	TRUE  : ��������
|			FALSE : �������Ă��Ȃ�
*------------------------------------------------------------------------------*/
BOOL	d3_collision_line_plane(VEC3 *pOut, VEC3 *pStart, VEC3 *pEnd, VEC3 *pV0, VEC3 *pV1, VEC3 *pV2)
{

	//D3_3DTRI_FILL(pP0, pP1, pP2, D3_COL(255, 255, 0, 255));

	VEC3		vecP;
	VEC3		vecQ;
	VEC3		vecT;
	float		fDet;
	float		fInvDet;
	float		fT, fU, fV;

	// �ʂ̍쐬
	D3DXPLANE	plane;
	D3DXPlaneFromPoints(&plane,	pV0, pV1, pV2);

	// �����̗��[�������ʂ��܂����ő��݂��Ă��邩���`�F�b�N
	float	fDist0 = (plane.a * pStart->x) + (plane.b * pStart->y) + (plane.c * pStart->z) + plane.d;
	float	fDist1 = (plane.a * pEnd->x  ) + (plane.b * pEnd->y  ) + (plane.c * pEnd->z  ) + plane.d;
	if((fDist0 * fDist1) >= 0.0f) return FALSE;

	//VEC3	Start = VEC3(pStart->x, pStart->y, pStart->z);
	//VEC3	Dir   = VEC3(pEnd->x,   pEnd->y,   pEnd->z  ) - Start;
	VEC3	vecDir = *pEnd - *pStart;
	//VEC3	Vx[3];
	//Vx[0] = *pP0;
	//Vx[1] = *pP1;
	//Vx[2] = *pP2;

	// Vx[0]�����L����Q�x�N�g�����Z�o
	//VEC3	Edge1 = Vx[1] - Vx[0];
	//VEC3	Edge2 = Vx[2] - Vx[0];
	VEC3	vecEdge0 = *pV1 - *pV0;
	VEC3	vecEdge1 = *pV2 - *pV0;

	// U�p�����[�^�[���v�Z
	D3DXVec3Cross(&vecP, &vecDir, &vecEdge1);

    // ���ς��O�ɋ߂��ꍇ�͎O�p�`�̕��ʂɂ���
	fDet = D3DXVec3Dot(&vecEdge0, &vecP);
	//if(fDet > 0) vecT = Start - Vx[0];
	if(fDet > 0) vecT = *pStart - *pV0;
	//else		 vecT = Vx[0] - Start, fDet = -fDet;
	else		 vecT = *pV0    - *pStart, fDet = -fDet;
	if(fDet < 0.0001f) return FALSE;

	// U�p�����[�^�[����уe�X�g���E���v�Z
	fU = D3DXVec3Dot(&vecT, &vecP);
	if((fU < 0.0f) || (fU > fDet)) return FALSE;

	// V�p�����[�^�[���e�X�g���鏀�������܂�
	D3DXVec3Cross(&vecQ, &vecT, &vecEdge0);

	// V�p�����[�^�[����уe�X�g���E���v�Z
	fV = D3DXVec3Dot(&vecDir, &vecQ);
	if((fV < 0.0f) || ((fU + fV) > fDet)) return FALSE;

	// �ʒu����ۑ�
	if(pOut != NULL)
	{
		// T���v�Z���A�X�P�[�����O
		fT      = D3DXVec3Dot(&vecEdge1, &vecQ);
		fInvDet = 1.0f / fDet;
		fT     *= fInvDet;
		fU     *= fInvDet;
		fV     *= fInvDet;
		//pOut->x = Vx[0].x + (fU * (Vx[1].x - Vx[0].x)) + (fV * (Vx[2].x - Vx[0].x));
		//pOut->y = Vx[0].y + (fU * (Vx[1].y - Vx[0].y)) + (fV * (Vx[2].y - Vx[0].y));
		//pOut->z = Vx[0].z + (fU * (Vx[1].z - Vx[0].z)) + (fV * (Vx[2].z - Vx[0].z));
		pOut->x = pV0->x + (fU * (pV1->x - pV0->x)) + (fV * (pV2->x - pV0->x));
		pOut->y = pV0->y + (fU * (pV1->y - pV0->y)) + (fV * (pV2->y - pV0->y));
		pOut->z = pV0->z + (fU * (pV1->z - pV0->z)) + (fV * (pV2->z - pV0->z));
	}
	return TRUE;
}


/*----------------------------------------------------------------------*
| <<< �_�ƒ����̂̌������� >>>
|	����	a[8] = �����̂̒��_(���_�̕��т͒����̂̉�����Q�Ƃ̂���)
|			b    = �_�̍��W
|	�߂�l	TRUE or FALSE
*----------------------------------------------------------------------*/
BOOL	d3_collision_point_cube(VEC3 *a, VEC3 *b)
{

	if(a[0].x <= b->x && a[1].x >= b->x
	&& a[0].y <= b->y && a[3].y >= b->y
	&& a[0].z <= b->z && a[4].z >= b->z)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



/*----------------------------------------------------------------------*
| <<< �����ƒ����̂̌������� >>>
|	����	a[8]   = �����̂̒��_(���_�̕��т͒����̂̉�����Q�Ƃ̂���)
|			b �` c = ���C���̍��W
|	�߂�l	TRUE or FALSE
*----------------------------------------------------------------------*/
BOOL	d3_collision_line_cube(VEC3 *a, VEC3 *b, VEC3 *c)
{

	BOOL	ret = FALSE;

	//--- �Ƃ肠��������Ă��钸�_�� TRUE �Ƃ��� ------------------
	if(a[0].x <= b->x && a[1].x >= b->x
	&& a[0].y <= b->y && a[3].y >= b->y
	&& a[0].z <= b->z && a[4].z >= b->z)
	{
		return TRUE;
	}
	if(a[0].x <= c->x && a[1].x >= c->x
	&& a[0].y <= c->y && a[3].y >= c->y
	&& a[0].z <= c->z && a[4].z >= c->z)
	{
		return TRUE;
	}

	//--- ���f���Ă��钸�_�𒲂ׂ� ----------------------------------
											// �E��(�����ĉE)
	if(d3_collision_line_plane(NULL, b, c, &a[1], &a[2], &a[6])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[6], &a[5], &a[1])) return TRUE;
											// ����(�����č�)
	if(d3_collision_line_plane(NULL, b, c, &a[0], &a[3], &a[7])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[7], &a[4], &a[0])) return TRUE;
											// ����
	if(d3_collision_line_plane(NULL, b, c, &a[4], &a[7], &a[6])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[6], &a[5], &a[4])) return TRUE;
											// ��O��
	if(d3_collision_line_plane(NULL, b, c, &a[0], &a[1], &a[2])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[2], &a[3], &a[0])) return TRUE;
											// ���
	if(d3_collision_line_plane(NULL, b, c, &a[3], &a[2], &a[6])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[6], &a[7], &a[3])) return TRUE;
											// ���
	if(d3_collision_line_plane(NULL, b, c, &a[0], &a[1], &a[5])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[5], &a[4], &a[0])) return TRUE;

	return FALSE;
}

#endif




/**************************************************************************************************
	[[[	������ ]]]
**************************************************************************************************/

#if 0
/*----------------------------------------------------------------------*
| <<< ������̏����� >>>
|	���J�����������Ȃ�΁A���������s�̂���
*----------------------------------------------------------------------*/
void	d3_frustum_init(void)
{
	MTX		mat;

	D3DXMatrixInverse(&mat,	NULL, &d3.mtxViewProjection);
	d3.vecFrustum[0] = VEC3(-1.0f, -1.0f, 0.0f);	// xyz
	d3.vecFrustum[1] = VEC3( 1.0f, -1.0f, 0.0f);	// Xyz
	d3.vecFrustum[2] = VEC3(-1.0f,  1.0f, 0.0f);	// xYz
	d3.vecFrustum[3] = VEC3( 1.0f,  1.0f, 0.0f);	// XYz
	d3.vecFrustum[4] = VEC3(-1.0f, -1.0f, 1.0f);	// xyZ
	d3.vecFrustum[5] = VEC3( 1.0f, -1.0f, 1.0f);	// XyZ
	d3.vecFrustum[6] = VEC3(-1.0f,  1.0f, 1.0f);	// xYZ
	d3.vecFrustum[7] = VEC3( 1.0f,  1.0f, 1.0f);	// XYZ

	for(int	i =	0; i < 8; i++){ D3DXVec3TransformCoord(&d3.vecFrustum[i], &d3.vecFrustum[i], &mat);}
											// ������p�̕��ʎ������
	D3DXPlaneFromPoints(&d3.planeFrustum[0], &d3.vecFrustum[0],	&d3.vecFrustum[1], &d3.vecFrustum[2]);	// Near
	D3DXPlaneFromPoints(&d3.planeFrustum[1], &d3.vecFrustum[6],	&d3.vecFrustum[7], &d3.vecFrustum[5]);	// Far
	D3DXPlaneFromPoints(&d3.planeFrustum[2], &d3.vecFrustum[2],	&d3.vecFrustum[6], &d3.vecFrustum[4]);	// Left
	D3DXPlaneFromPoints(&d3.planeFrustum[3], &d3.vecFrustum[7],	&d3.vecFrustum[3], &d3.vecFrustum[5]);	// Right
	D3DXPlaneFromPoints(&d3.planeFrustum[4], &d3.vecFrustum[2],	&d3.vecFrustum[3], &d3.vecFrustum[6]);	// Top
	D3DXPlaneFromPoints(&d3.planeFrustum[5], &d3.vecFrustum[1],	&d3.vecFrustum[0], &d3.vecFrustum[4]);	// Bottom
}



/*------------------------------------------------------------------------------*
| <<< ���b�V����������̒��ɂ��邩�`�F�b�N >>>
|	����	pHit   = ���b�V���̃q�b�g���[�N
|			mWorld = ���b�V���̃��[���h���W
|	�߂�l	0 :	������̒�
|			1 :	�ꕔ�����؂�Ă���
|			2 :	������̊O
*------------------------------------------------------------------------------*/
int		d3_frustum_hit_check(D3_HIT *pHit, MTX *mWorld)
{

	for(int	i =	0; i < 8; i++) D3DXVec3TransformCoord(&pHit->vecWorld[i], &pHit->vecLocal[i], mWorld);
	pHit->vecWorld[8].x	= mWorld->_41;		// �o�E���f�B���O�{�b�N�X�̒��S�_
	pHit->vecWorld[8].y	= mWorld->_42;
	pHit->vecWorld[8].z	= mWorld->_43;
	
	//--- �ʂ̍쐬 --------------------------------------------------
	D3DXPlaneFromPoints(&pHit->planeWorld[0], &pHit->vecWorld[0], &pHit->vecWorld[1], &pHit->vecWorld[2]);	// Near
	D3DXPlaneFromPoints(&pHit->planeWorld[1], &pHit->vecWorld[6], &pHit->vecWorld[7], &pHit->vecWorld[5]);	// Far
	D3DXPlaneFromPoints(&pHit->planeWorld[2], &pHit->vecWorld[2], &pHit->vecWorld[6], &pHit->vecWorld[4]);	// Left
	D3DXPlaneFromPoints(&pHit->planeWorld[3], &pHit->vecWorld[7], &pHit->vecWorld[3], &pHit->vecWorld[5]);	// Right
	D3DXPlaneFromPoints(&pHit->planeWorld[4], &pHit->vecWorld[2], &pHit->vecWorld[3], &pHit->vecWorld[6]);	// Top
	D3DXPlaneFromPoints(&pHit->planeWorld[5], &pHit->vecWorld[1], &pHit->vecWorld[0], &pHit->vecWorld[4]);	// Bottom

	//---------------------------------------------------------------
	WORD	bOutside[8];
	int		out	= 0;

	ZeroMemory(&bOutside, sizeof(bOutside));
	// bitfield�̒���6�̉~���䕽�ʁA����ь���(�P�̏ꍇ�A�O��)
	// ���ׂĂɑ΂���`�F�b�N���E���_
	float	aa[8];

	for(int	iPoint = 0;	iPoint < 8;	iPoint++)
	{
		for(int	iPlane = 0;	iPlane < 6;	iPlane++)
		{
			aa[iPoint] = 
			   d3.planeFrustum[iPlane].a * pHit->vecWorld[iPoint].x
			+  d3.planeFrustum[iPlane].b * pHit->vecWorld[iPoint].y
			+  d3.planeFrustum[iPlane].c * pHit->vecWorld[iPoint].z
			+  d3.planeFrustum[iPlane].d;

			if(d3.planeFrustum[iPlane].a * pHit->vecWorld[iPoint].x
			+  d3.planeFrustum[iPlane].b * pHit->vecWorld[iPoint].y
			+  d3.planeFrustum[iPlane].c * pHit->vecWorld[iPoint].z
			+  d3.planeFrustum[iPlane].d < 0)
			{
				bOutside[iPoint] |=	(1 << iPlane);
				out++;
			}
		}
	}
	if(out == 0){ return 0;}

	if((bOutside[0]	& bOutside[1] &	bOutside[2]	& bOutside[3]
	&	bOutside[4]	& bOutside[5] &	bOutside[6]	& bOutside[7]) != 0)
	{
		return 2;
	}
	return 1;
}



/*------------------------------------------------------------------------------*
| <<< �����̂̔����_��������̒��ɂ��邩�`�F�b�N >>>
|	����	apvecPos = �����̂̔����_
|			mtxWorld = ���b�V���̃��[���h���W
|	�߂�l	0 :	������̒�
|			1 :	�ꕔ�����؂�Ă���
|			2 :	������̊O
*------------------------------------------------------------------------------*/
int		d3_frustum_hit_check(VEC3 *apvecPos, MTX *mtxWorld)
{

	//--- �����̂̔����_���g�����X�t�H�[�� --------------------------
	VEC3	avecP[8];
	for(int	i =	0; i < 8; i++) D3DXVec3TransformCoord(&avecP[i], &apvecPos[i], mtxWorld);

	//--- ���̒�` --------------------------------------------------
	D3DXPLANE	planeWorld[6];
	D3DXPlaneFromPoints(&planeWorld[0], &avecP[0], &avecP[1], &avecP[2]);	// Near
	D3DXPlaneFromPoints(&planeWorld[1], &avecP[6], &avecP[7], &avecP[5]);	// Far
	D3DXPlaneFromPoints(&planeWorld[2], &avecP[2], &avecP[6], &avecP[4]);	// Left
	D3DXPlaneFromPoints(&planeWorld[3], &avecP[7], &avecP[3], &avecP[5]);	// Right
	D3DXPlaneFromPoints(&planeWorld[4], &avecP[2], &avecP[3], &avecP[6]);	// Top
	D3DXPlaneFromPoints(&planeWorld[5], &avecP[1], &avecP[0], &avecP[4]);	// Bottom

	//--- ���� ------------------------------------------------------
	WORD	adwOut[8];
	int		iOut = 0;

	ZeroMemory(&adwOut, sizeof(adwOut));

	// bitfield�̒���6�̉~���䕽�ʁA����ь���(�P�̏ꍇ�A�O��)
	// ���ׂĂɑ΂���`�F�b�N���E���_
	//float	aa[8];

	for(int	iPoint = 0;	iPoint < 8;	iPoint++)
	{
		for(int	iPlane = 0;	iPlane < 6;	iPlane++)
		{
	/*		aa[iPoint] = 
			   d3.planeFrustum[iPlane].a * avecP[iPoint].x
			+  d3.planeFrustum[iPlane].b * avecP[iPoint].y
			+  d3.planeFrustum[iPlane].c * avecP[iPoint].z
			+  d3.planeFrustum[iPlane].d;
*/
			if(d3.planeFrustum[iPlane].a * avecP[iPoint].x
			+  d3.planeFrustum[iPlane].b * avecP[iPoint].y
			+  d3.planeFrustum[iPlane].c * avecP[iPoint].z
			+  d3.planeFrustum[iPlane].d < 0)
			{
				adwOut[iPoint] |= (1 << iPlane);
				iOut++;
			}
		}
	}
	if(iOut == 0){ return 0;}
	if((adwOut[0] & adwOut[1] &	adwOut[2] & adwOut[3] & adwOut[4] & adwOut[5] &	adwOut[6] & adwOut[7]) != 0) { return 2;}

	return 1;
}

#endif

