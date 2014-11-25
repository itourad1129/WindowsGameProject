//
//	���낢��v�Z����
//


//#include	"Main.h"

#undef	PI
#define	PI	D3DX_PI



/*------------------------------------------------------------------------------*
| <<< �O�p�֐��e�[�u�����쐬 >>>
|	����	iDiv = ����\
*------------------------------------------------------------------------------*/
static	int		iSinCosDiv;
static	float	*pfSin;
static	float	fSinCosResolution;

void	calc_sincos_init(int iDiv)
{

	iSinCosDiv		  = iDiv;							// ������
	fSinCosResolution = (float)iSinCosDiv / (PI * 2);	// �X�e�b�v

	//pfSin			  = (float*)sys_malloc(sizeof(float) * (iDiv + 1));
	pfSin			  = (float*)malloc(sizeof(float) * (iDiv + 1));

	int	i;
	for(i = 0; i < iSinCosDiv; i++)
	{
		pfSin[i] = sin((R(360) / (float)iSinCosDiv) * (float)i);
	}
	pfSin[i] = 0;
}



/*------------------------------------------------------------------------------*
| <<< sin >>>
|	����	fRad = ���W�A��
*------------------------------------------------------------------------------*/
float	calc_sin_get(float fRad)
{

	float	fSign = 1;

	if(fRad < 0){ fRad *= -1, fSign = -1;}			// �}�C�i�X�̎�
	if(fRad >= PI * 2){ fRad = fmod(fRad, PI * 2);}	// R(360)�𒴂�����

	int	a = (int)(fRad * fSinCosResolution);
	return pfSin[a] * fSign;
}



/*------------------------------------------------------------------------------*
| <<< cos >>>
|	����	fRad = ���W�A��
*------------------------------------------------------------------------------*/
float	calc_cos_get(float fRad)
{
	return calc_sin_get(fRad + PI / 2);
}



/*------------------------------------------------------------------------------*
| <<< �Q�c�x�N�g�������W�ϊ� >>>
|	����	pvecIn  = �x�N�g��
|			pmtx    = �s��
|	�o��	pvecOut = �x�N�g��
*------------------------------------------------------------------------------*/
void	calc_vec2_transform_coord(VEC2 *pvecOut, VEC2 *pvecIn, MTX *pmtx)
{

	float	x  = (pvecIn->x * (*pmtx)(0, 0)) + (pvecIn->y * (*pmtx)(1, 0)) + (*pmtx)(3, 0);
	float	y  = (pvecIn->x * (*pmtx)(0, 1)) + (pvecIn->y * (*pmtx)(1, 1)) + (*pmtx)(3, 1);
	pvecOut->x = x;
	pvecOut->y = y;
}


/*------------------------------------------------------------------------------*
| <<< �R�c�x�N�g�������W�ϊ� >>>
|	����	pvecIn  = �x�N�g��
|			pmtx    = �s��
|	�o��	pvecOut = �x�N�g��
*------------------------------------------------------------------------------*/
void	calc_vec3_transform_coord(VEC3 *pvecOut, VEC3 *pvecIn, MTX *pmtx)
{

	float	x  = (pvecIn->x * (*pmtx)(0, 0)) + (pvecIn->y * (*pmtx)(1, 0)) + (pvecIn->z * (*pmtx)(2, 0)) + (*pmtx)(3, 0);
	float	y  = (pvecIn->x * (*pmtx)(0, 1)) + (pvecIn->y * (*pmtx)(1, 1)) + (pvecIn->z * (*pmtx)(2, 1)) + (*pmtx)(3, 1);
	float	z  = (pvecIn->x * (*pmtx)(0, 2)) + (pvecIn->y * (*pmtx)(1, 2)) + (pvecIn->z * (*pmtx)(2, 2)) + (*pmtx)(3, 2);
	pvecOut->x = x;
	pvecOut->y = y;
	pvecOut->z = z;
}



/*------------------------------------------------------------------------------*
| <<< �R�c���W���X�N���[�����W�֕ϊ� >>>	��D3DXVec3Project�Ɠ�����������܂�
|	����	pvecPos   = �R�c���W
|			pmtxWorld = ���[���h�s��
|	�o��	pvecOut	  = �Q�c���W
|	���r���[�|�[�g�̓J�b�g
*------------------------------------------------------------------------------*/
void	calc_vec3_project(VEC3 *pvecOut, VEC3 *pvecPos, MTX *pmtxWorld)
{

	MTX		mtx;
	D3DXMatrixTranslation(&mtx, pvecPos);

	// �ړ��s��Ƀr���[�s��Ǝˉe�s����|����
	if(pmtxWorld != NULL)
	{
		mtx = mtx * *pmtxWorld * d3.mtxView * d3.mtxProjection;
	}
	else
	{
		mtx = mtx * d3.mtxView * d3.mtxProjection;
	}

	// �r���[�|�[�g�s����쐬
	MTX		mtxViewPort;
	D3DXMatrixIdentity(&mtxViewPort);
	float	w = (float)d3.sRenderW / 2;
	float	h = (float)d3.sRenderH / 2;
	//float	w = (float)__SCR_W / 2;
	//float	h = (float)__SCR_H / 2;
	mtxViewPort._11 =  w;
	mtxViewPort._41 =  w;
	mtxViewPort._22 = -h;
	mtxViewPort._42 =  h;

	// �r���[�|�[�g�s����|���ăX�N���[�����W�֕ϊ�
	mtx *= mtxViewPort;

	#ifdef	__2D_RESIZE
		pvecOut->x = (mtx._41 / mtx._44 - d3.vecOfs.x) / d3.fZoom;
		pvecOut->y = (mtx._42 / mtx._44 - d3.vecOfs.y) / d3.fZoom;
	#else
		pvecOut->x = mtx._41 / mtx._44;
		pvecOut->y = mtx._42 / mtx._44;
	#endif

	pvecOut->z = 0;
}



/*------------------------------------------------------------------------------*
| <<< �Q�c�x�N�g���� Catmull-Rom �ŕ⊮ >>>
|	����	p0, p1, p2, p3 = �ʒu�x�N�g��
|			s			   = �W��(0.0f �` 1.0f)
|	�߂�l	pvecOut		   = ���Z����
*------------------------------------------------------------------------------*/
void	calc_vec2_catmullrom(VEC2 *pvecOut, VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3, float s)
{

	VEC2	v0 = (*p2 - *p0) * 0.5f;
	VEC2	v1 = (*p3 - *p1) * 0.5f;

	float	s2 = s  * s;
	float	s3 = s2 * s;

    *pvecOut = (2 * *p1 - 2 * *p2 + v0 + v1) * s3 + (-3 * *p1 + 3 * *p2 - 2 * v0 - v1) * s2 + v0 * s + *p1;
}



/*------------------------------------------------------------------------------*
| <<< �t�s����쐬 >>>				��D3DXMatrixInverse�Ƃقړ�����������܂�
|	����	pmtxIn  = �t�s��ɂ������s��
|	�o��	pmtxOut = �t�s��
*------------------------------------------------------------------------------*/
void	calc_mtx_inverse(MTX *pmtxOut, MTX *pmtxIn)
{

	float	buf;							// �ꎞ�I�ȃf�[�^��~����
	int		i, j, k;						// �J�E���^
	MTX		m = d3.mtxIdentity;

	// �|���o���@
	float	*a     = (float*)&pmtxIn->_11;
	float	*inv_a = (float*)&m._11;
	for(i = 0; i < 4; i++)
	{
		buf = 1 / a[i * 4 + i];
		for(j = 0; j < 4; j++)
		{
			a[    i * 4 + j] *= buf;
			inv_a[i * 4 + j] *= buf;
		}

		for(j = 0; j < 4; j++)
		{
			if(i == j){ continue;}

			buf = a[j * 4 + i];
			for(k = 0; k < 4; k++)
			{
				a[j * 4 + k]     -= a[i * 4 + k] * buf;
				inv_a[j * 4  + k] -= inv_a[i * 4 + k] * buf;
			}
		}
	}
	*pmtxOut = m;
}



/*------------------------------------------------------------------------------*
| <<< �s��̐��K�� >>>
|	����	pmtx = ���K���������s��
|	�o��	pmtx = ���K�����ꂽ�s��
*------------------------------------------------------------------------------*/
void	calc_mtx_normalize(MTX *pmtx)
{

	VEC4	v[4] =
	{
		VEC4(pmtx->_11, pmtx->_21, pmtx->_31, pmtx->_41),
		VEC4(pmtx->_12, pmtx->_22, pmtx->_32, pmtx->_42),
		VEC4(pmtx->_13, pmtx->_23, pmtx->_33, pmtx->_43),
		VEC4(pmtx->_14, pmtx->_24, pmtx->_34, pmtx->_44)
	};

	calc_vec4_normalize(&v[0], &v[0]);
	calc_vec4_normalize(&v[1], &v[1]);
	calc_vec4_normalize(&v[2], &v[2]);
	calc_vec4_normalize(&v[3], &v[3]);

	pmtx->_11 =	v[0].x;	pmtx->_21 =	v[0].y;	pmtx->_31 =	v[0].z;	pmtx->_41 =	v[0].w;
	pmtx->_12 =	v[1].x;	pmtx->_22 =	v[1].y;	pmtx->_32 =	v[1].z;	pmtx->_42 =	v[1].w;
	pmtx->_13 =	v[2].x;	pmtx->_23 =	v[2].y;	pmtx->_33 =	v[2].z;	pmtx->_43 =	v[2].w;
	pmtx->_14 =	v[3].x;	pmtx->_24 =	v[3].y;	pmtx->_34 =	v[3].z;	pmtx->_44 =	v[3].w;
}

	
	
/*------------------------------------------------------------------------------*
| <<< �R���W���ʂ��쐬���� >>>			�� D3DX �� D3DXPlaneFromPoints �Ɠ����ł��B
|	����	pvecA, pvecB, pvecC = ���_
|	�o��	pplaneOut = ��
*------------------------------------------------------------------------------*/
void	calc_plane_create(PLANE *pplaneOut, VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2)
{

	// a ���� b �̃x�N�g���� s�A a ���� c �x�N�g���� t �Ƃ���
	// ���ʓ��̃x�N�g�� s �ƃx�N�g�� t �̊O�� (s x t) �͂Q�̃x�N�g���ɒ��p�ƂȂ�x�N�g���ł���A
	// ���ʂ̖@���x�N�g���Ɠ��������ƂȂ�

	VEC3	s = *pvecP1 - *pvecP0;
	VEC3	t = *pvecP2 - *pvecP0;

	VEC3	vecNor;
	calc_vec3_cross(    &vecNor, &s, &t);
	calc_vec3_normalize(&vecNor, &vecNor);

	pplaneOut->a = vecNor.x;
	pplaneOut->b = vecNor.y;
	pplaneOut->c = vecNor.z;
	pplaneOut->d = -((pvecP0->x * vecNor.x) + (pvecP0->y * vecNor.y) + (pvecP0->z * vecNor.z));
}



/*------------------------------------------------------------------------------*
| <<< ���ʂƃx�N�g���̓��ς��v�Z >>>		�� D3DX ��D3DXPlaneDotNormal �Ɠ����ł��B
|	����	pplane = ����
|			pvec   = �x�N�g��
|	�߂�l	���ς̒l
*------------------------------------------------------------------------------*/
float	calc_plane_vec3_dot(PLANE *pplane, VEC3 *pvec)
{

	return pplane->a * pvec->x + pplane->b * pvec->y + pplane->c * pvec->z;// + pplane->d * 0;
}



#if 0


/**************************************************************************************************
	[[[	�Z�p�n ]]]
	�����O�ŎZ�p�n��p�ӂ��܂������AD3DX �̓A�Z���u���ŋL�q����Ă���Ƃ������̂ŁA
	  �Ȃ�ׂ� D3DX �̕����g���܂��傤
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< �x�N�g�����m�̊|���Z >>>				�� D3DXMatrixMultiply
|	����	a, b = �s��
|	�o��	pOut = a * b �̌���
|	�߂�l	pOut �̃A�h���X
*------------------------------------------------------------------------------*/
void	d3_matrix_multi(MTX *pOut, MTX *a, MTX *b)
{

    MTX	c = *a;			// pOut �� a �������ꍇ������̂ŁA��������l���R�s�[���Ă��܂�

    pOut->_11 = c._11 * b->_11 + c._12 * b->_21 + c._13 * b->_31 + c._14 * b->_41;
    pOut->_12 = c._11 * b->_12 + c._12 * b->_22 + c._13 * b->_32 + c._14 * b->_42;
    pOut->_13 = c._11 * b->_13 + c._12 * b->_23 + c._13 * b->_33 + c._14 * b->_43;
    pOut->_14 = c._11 * b->_14 + c._12 * b->_24 + c._13 * b->_34 + c._14 * b->_44;

	pOut->_21 = c._21 * b->_11 + c._22 * b->_21 + c._23 * b->_31 + c._24 * b->_41;
	pOut->_22 = c._21 * b->_12 + c._22 * b->_22 + c._23 * b->_32 + c._24 * b->_42;
	pOut->_23 = c._21 * b->_13 + c._22 * b->_23 + c._23 * b->_33 + c._24 * b->_43;
	pOut->_24 = c._21 * b->_14 + c._22 * b->_24 + c._23 * b->_34 + c._24 * b->_44;

	pOut->_31 = c._31 * b->_11 + c._32 * b->_21 + c._33 * b->_31 + c._34 * b->_41;
	pOut->_32 = c._31 * b->_12 + c._32 * b->_22 + c._33 * b->_32 + c._34 * b->_42;
	pOut->_33 = c._31 * b->_13 + c._32 * b->_23 + c._33 * b->_33 + c._34 * b->_43;
	pOut->_34 = c._31 * b->_14 + c._32 * b->_24 + c._33 * b->_34 + c._34 * b->_44;

	pOut->_41 = c._41 * b->_11 + c._42 * b->_21 + c._43 * b->_31 + c._44 * b->_41;
	pOut->_42 = c._41 * b->_12 + c._42 * b->_22 + c._43 * b->_32 + c._44 * b->_42;
	pOut->_43 = c._41 * b->_13 + c._42 * b->_23 + c._43 * b->_33 + c._44 * b->_43;
	pOut->_44 = c._41 * b->_14 + c._42 * b->_24 + c._43 * b->_34 + c._44 * b->_44;
}



/*------------------------------------------------------------------------------*
| <<< �x�N�g���̒��������߂� >>>			�� D3DXVec2Normalize
|	����	p0 = �����𒲂ׂ����x�N�g��
|	�߂�l	�x�N�g���̒���
*------------------------------------------------------------------------------*/
float	d3_calc_vec2_length(VEC2 *p0)
{
	//return	D3DXVec2Length(p0);	// for dx
	return	sqrtf((p0->x * p0->x) + (p0->y * p0->y));
}

float	d3_calc_vec3_length(VEC3 *p0)
{
	//return	D3DXVec3Length(p0);	// for dx
	return	sqrtf((p0->x * p0->x) + (p0->y * p0->y) + (p0->z * p0->z));
}

float	d3_calc_vec4_length(VEC4 *p0)
{
	//return	D3DXVec4Length(p0);	// for dx
	return	sqrtf((p0->x * p0->x) + (p0->y * p0->y) + (p0->z * p0->z));
}


/*------------------------------------------------------------------------------*
| <<< �x�N�g���̐��K�� >>>					�� D3DXVec2Normalize
|	����		p1 = ���K���������x�N�g��
|	�o��		p0 = ���K�����ꂽ�x�N�g��
*------------------------------------------------------------------------------*/
void	d3_calc_vec2_normalize(VEC2 *p0, VEC2 *p1)
{
	//D3DXVec2Normalize(p0, p1);

	float	q = sqrtf(p1->x * p1->x + p1->y * p1->y);

	if(q != 0.0f){ q = 1 / q;}

	p0->x = p1->x * q;
	p0->y = p1->y * q;
}
void	d3_calc_vec3_normalize(VEC3 *p0, VEC3 *p1)
{
	//D3DXVec3Normalize(p0, p1);

	float	q = sqrtf(p1->x * p1->x + p1->y * p1->y + p1->z * p1->z);

	if(q != 0.0f){ q = 1 / q;}
	
	p0->x = p1->x * q;
	p0->y = p1->y * q;
	p0->z = p1->z * q;
}
void	d3_calc_vec4_normalize(VEC4 *p0, VEC4 *p1)
{
	//D3DXVec4Normalize(p0, p1);

	float	q = sqrtf(p1->x * p1->x + p1->y * p1->y + p1->z * p1->z + p1->w * p1->w);

	if(q != 0.0f){ q = 1 / q;}
	
	p0->x = p1->x * q;
	p0->y = p1->y * q;
	p0->z = p1->z * q;
	p0->w = p1->w * q;
}



/*------------------------------------------------------------------------------*
| <<< �x�N�g���̈����Z >>>					�� D3DXVec2Subtract
|	�� pOut = p0 - p1
*------------------------------------------------------------------------------*/
void	d3_calc_vec2_sub(VEC2 *pOut, VEC2 *p0, VEC2 *p1)
{
	pOut->x = p0->x - p1->x;
	pOut->y = p0->y - p1->y;
}
void	d3_calc_vec3_sub(VEC3 *pOut, VEC3 *p0, VEC3 *p1)
{
	pOut->x = p0->x - p1->x;
	pOut->y = p0->y - p1->y;
	pOut->z = p0->z - p1->z;
}
void	d3_calc_vec4_sub(VEC4 *pOut, VEC4 *p0, VEC4 *p1)
{
	pOut->x = p0->x - p1->x;
	pOut->y = p0->y - p1->y;
	pOut->z = p0->z - p1->z;
	pOut->w = p0->w - p1->w;
}



/*------------------------------------------------------------------------------*
| <<< �x�N�g���̓��ς����߂� >>>			�� D3DXVec2Dot�AD3DXVec3Dot
|	�߂�l		���ς̒l
*------------------------------------------------------------------------------*/
float	d3_calc_vec2_dot(VEC2 *p0, VEC2 *p1)
{
	return (p0->x * p1->x) + (p0->y * p1->y);
}
float	d3_calc_vec3_dot(VEC3 *p0, VEC3 *p1)
{
	return (p0->x * p1->x) + (p0->y * p1->y) + (p0->z * p1->z);
}
float	d3_calc_vec4_dot(VEC4 *p0, VEC4 *p1)
{
	return (p0->x * p1->x) + (p0->y * p1->y) + (p0->z * p1->z) + (p0->w * p1->w);
}



/*------------------------------------------------------------------------------*
| <<< �Q�c�x�N�g���̊O�ς����߂� >>>		�� D3DXVec2CCW �Ɠ����̊֐�
|	����	p0, p1 = �Q�c�x�N�g��
|	�߂�l	0  > �E��
|			0  < ����
|			0 == �d�Ȃ��Ă���
*------------------------------------------------------------------------------*/
float	d3_calc_vec2_cross(VEC2 *p0, VEC2 *p1)
{
	return p0->x * p1->y - p0->y * p1->x;
}


/*------------------------------------------------------------------------------*
| <<< �x�N�g���̊O�ς����߂� >>>			�� D3DXVec3Cross �Ɠ����̊֐�
|	�o��	*pOut = �O��
*------------------------------------------------------------------------------*/
void	d3_calc_vec3_cross(VEC3 *pOut, VEC3 *p0, VEC3 *p1)
{
	pOut->x = p0->y * p1->z - p1->y * p0->z;
	pOut->y = p0->z * p1->x - p1->z * p0->x;
	pOut->z = p0->x * p1->y - p1->x * p0->y;
}
void	d3_calc_vec4_cross(VEC4 *pOut, VEC4 *p0, VEC4 *p1)
{
	pOut->x = p0->y * p1->z - p1->y * p0->z;
	pOut->y = p0->z * p1->x - p1->z * p0->x;
	pOut->z = p0->x * p1->y - p1->x * p0->y;
	pOut->w = 0;
}



/*------------------------------------------------------------------------------*
| <<< �x�N�g���ƍs��̊|���Z >>>
|	����	pvecIn   = �x�N�g��
|			mtxWorld = �s��
|	�o��	pvecOut  = ���Z����
*------------------------------------------------------------------------------*/
void	d3_calc_vec2_transformcoord(VEC2 *pvecOut, VEC2 *pvecIn, MTX *mtxWorld)
{
	pvecOut->x = (pvecIn->x * mtxWorld->_11) + (pvecIn->y * mtxWorld->_21) + mtxWorld->_41;
	pvecOut->y = (pvecIn->x * mtxWorld->_12) + (pvecIn->y * mtxWorld->_22) + mtxWorld->_42;
}

void	d3_calc_vec3_transformcoord(VEC3 *pvecOut, VEC3 *pvecIn, MTX *mtxWorld)
{
	pvecOut->x = (pvecIn->x * mtxWorld->_11) + (pvecIn->y * mtxWorld->_21) + (pvecIn->z * mtxWorld->_31) + mtxWorld->_41;
	pvecOut->y = (pvecIn->x * mtxWorld->_12) + (pvecIn->y * mtxWorld->_22) + (pvecIn->z * mtxWorld->_32) + mtxWorld->_42;
	pvecOut->z = (pvecIn->x * mtxWorld->_13) + (pvecIn->y * mtxWorld->_23) + (pvecIn->z * mtxWorld->_33) + mtxWorld->_43;
}

#endif



/*------------------------------------------------------------------------------*
| <<< ���O�Ŏˉe�s��̍쐬 >>>				�� D3DXMatrixPerspectiveFovLH
|	�� d3.mtxProjection �ɍ쐬���܂��B
*------------------------------------------------------------------------------*/
MTX	*calc_mtx_perspective(MTX *pOut, float fFov, float fZNear, float fZFar)
{

	//float	fAspect = (float)d3.sRenderH / (float)d3.sRenderW;
	//float	fAspect = (float)__SCR_H     / (float)__SCR_W;
	float	fSin, fCos;

	SINCOS(fFov / 2, &fSin, &fCos);

    float	q = fZFar / (fZFar - fZNear);

    memset(pOut, 0, sizeof(MTX));

	pOut->_11 = fFov * (fCos / fSin);		// fXScale
	pOut->_22 = 1.0f * (fCos / fSin);		// fYScale
	pOut->_33 = q;
	pOut->_34 = 1.0f;
	pOut->_43 = -fZNear * q;

	return pOut;
}



/*------------------------------------------------------------------------------*
| <<< ���O�Ńr���[�s��̍쐬 >>>				�� D3DXMatrixLookAtLH
|	�� d3.mtxView �ɍ쐬���܂�
*------------------------------------------------------------------------------*/
MTX	*calc_mtx_lookat(MTX *pOut, VEC3 *pCam, VEC3 *pAt, VEC3 *pUp)
{

	VEC3	x, y, z;
	/*calc_vec3_sub(      &z, pAt, pCam);
	calc_vec3_normalize(&z, &z);
	calc_vec3_cross(    &x, pUp, &z);
	calc_vec3_normalize(&x, &x);
	calc_vec3_cross(    &y, &z, &x);
	*/

	D3DXVec3Subtract( &z, pCam, pAt);
	D3DXVec3Normalize(&z, &z);
	D3DXVec3Cross(    &x, pUp, &z);
	D3DXVec3Normalize(&x, &x);
	D3DXVec3Cross(    &y, &z, &x);

	*pOut = d3.mtxIdentity;
	pOut->_11 = x.x;
	pOut->_12 = y.x;
	pOut->_13 = z.x;
	pOut->_14 = 0.0f;
	
	pOut->_21 = x.y;
	pOut->_22 = y.y;
	pOut->_23 = z.y;
	pOut->_24 = 0.0f;

	pOut->_31 = x.z;
	pOut->_32 = y.z;
	pOut->_33 = z.z;
	pOut->_34 = 0.0f;
	 
	pOut->_41 = -calc_vec3_dot(pCam, &x);
	pOut->_42 = -calc_vec3_dot(pCam, &y);
	pOut->_43 = -calc_vec3_dot(pCam, &z);
	pOut->_44 = 1.0f;

	return pOut;
}



/*------------------------------------------------------------------------------*
| <<< �R�c���W���X�N���[�����W�֕ϊ� >>>	��D3DXVec3Project�Ɠ�����������܂�
|	����	pvecPos        = �R�c���W
|			pmtxProjection = �ˉe�s��
|			pmtxView	   = �r���[�s��
|			pmtxWorld	   = ���[���h�s��
|	�o��	pvecOut		   = �Q�c���W
|	���r���[�|�[�g�̓J�b�g
*------------------------------------------------------------------------------*/
void	calc_vec3_projection(VEC3 *pvecOut, VEC3 *pvecPos, MTX *pmtxProjection, MTX *pmtxView, MTX *pmtxWorld)
{

	MTX		mtx, mtxTrans;
	D3DXMatrixTranslation(&mtxTrans, pvecPos);
	
	mtx = mtxTrans * *pmtxWorld * *pmtxView * *pmtxProjection;

	// �r���[�|�[�g�s����쐬����
	float	w = (float)d3.sRenderW / 2;
	float	h = (float)d3.sRenderH / 2;
	MTX		mtxPort;
	mtxPort = d3.mtxIdentity;
	mtxPort._11 = w;
	mtxPort._41 = w;
	mtxPort._22 = -h;
	mtxPort._42 = h;
	mtxPort._33 = 1;	//vp.MaxZ / (vp.MaxZ - vp.MinZ);
	mtxPort._43 = 0;	//-mv._33 * vp.MinZ;
	
	// �Q�c�֎ˉe���ꂽ�s��
	mtx *= mtxPort;

	// �������Z�b�g���ďI��
	pvecOut->x = mtx._41 / mtx._44;
	pvecOut->y = mtx._42 / mtx._44;
	pvecOut->z = mtx._43 / mtx._44;
}



/*------------------------------------------------------------------------------*
| <<< �Q�̃x�N�g�����p�x�𓾂� >>>
|	����	a, b = �x�N�g��
|	�߂�l	�p�x�i���W�A���j
*------------------------------------------------------------------------------*/
float	calc_vec3_vec_to_vec_angle(VEC3 *a, VEC3 *b)
{
	return (float)acosf(D3DXVec3Dot(a, b) /	(D3DXVec3Length(a) * D3DXVec3Length(b)));
}



/*------------------------------------------------------------------------------*
| <<< �R�x�N�g�����ʂ��쐬���� >>>			�� D3DXPlaneFromPoints
|	����	pvecA, pvecB, pvecC = ���_
|	�o��	pplaneOut = ��
*------------------------------------------------------------------------------*/
/*PLANE *calc_plane_create(PLANE *pOut, VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2)
{

	// a ���� b �̃x�N�g���� s�A a ���� c �x�N�g���� t �Ƃ���
	// ���ʓ��̃x�N�g�� s �ƃx�N�g�� t �̊O�� (s x t) �͂Q�̃x�N�g���ɒ��p�ƂȂ�x�N�g���ł���A
	// ���ʂ̖@���x�N�g���Ɠ��������ƂȂ�

	VEC3	s = *pvecP1 - *pvecP0;
	VEC3	t = *pvecP2 - *pvecP0;

	VEC3	normal;
	D3DXVec3Cross(    &normal, &s, &t);
	D3DXVec3Normalize(&normal, &normal);

	pOut->a = normal.x;
	pOut->b = normal.y;
	pOut->c = normal.z;
	pOut->d = -((pvecP0->x * normal.x) + (pvecP0->y * normal.y) + (pvecP0->z * normal.z));

	return pOut;
}*/



/*------------------------------------------------------------------------------*
| <<< �ʂƓ_�x�N�g���Ƃ̓��ς��v�Z >>>		�� D3DXPlaneDotNormal
|	����	pplaneA = ��
|			pvecB   = ���_�x�N�g��
|	�߂�l	�ʂƒ��_�x�N�g���̓���
*------------------------------------------------------------------------------*/
float	calc_plane_dot_normal(PLANE *pplaneA, VEC3 *pvecB)
{
	return D3DXVec3Dot(&VEC3(pplaneA->a, pplaneA->b, pplaneA->c), pvecB);
}



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
float	calc_plane_vec_distance(VEC3 *pvecNormal, VEC3 *pvecP0, VEC3 *pvecP1)
{
	VEC3	v = *pvecP1 - *pvecP0;
	return D3DXVec3Dot(pvecNormal, &v) / D3DXVec3Length(pvecNormal);
}



/*------------------------------------------------------------------------------*
| <<< �s�񂩂�I�C���[�p�𒊏o���� >>>
|	����	m = �s��
|	�߂�l	yaw pitch roll
*------------------------------------------------------------------------------*/
VEC3	calc_mtx_to_eular(MTX *m)
{

	VEC3	vecR;
	VEC3	y_axis, z_axis;

//	11	12	13	14		0	3	6
//	21	22	23	24		1	4	7
//	31	32	33	34		2	5	8
//	41	42	43	44

	z_axis.x = m->_31;	//m[2];
	z_axis.y = m->_32;	//m[5];
	z_axis.z = m->_33;	//m[8];

	y_axis.x = m->_21;	//m[1];
	y_axis.y = m->_22;	//m[4];
	y_axis.z = m->_24;	//m[7];

	float	yaw, pitch, roll;
	//yaw
	yaw = (float)atan2f(z_axis.x, z_axis.z);

	//pitch
	float  cos_yaw = (float)cosf(yaw);
	pitch = (float)atan2f(cos_yaw * z_axis.y, (float)fabs(z_axis.z));

	// roll
	float	sin_yaw   = (float)sinf(yaw);
	float	cos_pitch = (float)cosf(pitch);
	roll = (float)atan2f(cos_pitch * (sin_yaw * y_axis.z - cos_yaw * y_axis.x), y_axis.y);

	return VEC3(yaw, pitch, roll);
}



/*------------------------------------------------------------------------------*
| <<< �x�N�g������]�x�N�g�������߂� >>>
|	����	v0,	v1 = �x�N�g��
|	�o��	a	   = x,	y �ɉ�]�p�i���W�A���j		
*------------------------------------------------------------------------------*/
void	calc_vec3_vec_to_vec_rotation(VEC3 *a, VEC3 *v0, VEC3 *v1)
{
	MTX		m;
	VEC3	t =	*v1	- *v0;

	a->y = -(float)atan2f( t.x,	t.z);
	D3DXMatrixRotationY(   &m, a->y);
	D3DXVec3TransformCoord(&t, &t, &m);

	a->x = (float)atan2f(t.y, t.z);
	a->z = 0;
}



/*------------------------------------------------------------------------------*
| <<< �X�N���[�����W����R�c�̃x�N�g�����쐬 >>>
|	����	pvecPos	= 2D ���W			���ʏ�Apos.z �� 0 �œn������
|	�߂�l			  3D ���W
*------------------------------------------------------------------------------*/
VEC3	calc_vec3_screen_to_vector(VEC3 *pvecPos)
{

	//VEC3	out;
	//D3DXVec3Unproject(&out, pos, &d3.viewport, &d3.mtxProjection, &d3.mtxView, mtxWorld);
	//return out;

	MTX	a = d3.mtxIdentity;

	#ifdef	__2D_RESIZE
		float	fWidth  = __SCR_W * d3.fZoom;
		float	fHeight = __SCR_H * d3.fZoom;
		a._11 =  fWidth  / 2.0f;
		a._22 = -fHeight / 2.0f;
		a._41 =  fWidth  / 2.0f;
		a._42 =  fHeight / 2.0f;
	#else
		a._11 =  d3.sRenderW / 2.0f;
		a._22 = -d3.sRenderH / 2.0f;
		a._41 =  d3.sRenderW / 2.0f;
		a._42 =  d3.sRenderH / 2.0f;
	#endif

	MTX	mtxViewPortInv;
	D3DXMatrixInverse(&mtxViewPortInv, NULL, &a);

	// �t�ϊ�
	a = mtxViewPortInv * d3.mtxProjectionInverse * d3.mtxViewInverse;
	
	VEC3	vecOut;

	#ifdef	__2D_RESIZE
		VEC3	v(pvecPos->x * d3.fZoom, pvecPos->y * d3.fZoom, pvecPos->z);
		D3DXVec3TransformCoord(&vecOut, &v,	&a);
	#else
		D3DXVec3TransformCoord(&vecOut, pvecPos, &a);
	#endif

	return vecOut;
}

/*------------------------------------------------------------------------------*
| <<< �X�N���[�����W����R�c�̃x�N�g�����쐬 >>>
|	����	pvecPos	  = 2D ���W			���ʏ�Apos.z �� 0 �œn������
|			pmtxWorld = ���[���h�s��
|	�߂�l			    3D ���W
*------------------------------------------------------------------------------*/
VEC3	calc_vec3_screen_to_vector(VEC3 *pvecPos, MTX *pmtxWorld)
{

	MTX	a = *pmtxWorld;

	#ifdef	__2D_RESIZE
		float	fWidth  = __SCR_W * d3.fZoom;
		float	fHeight = __SCR_H * d3.fZoom;
		a._11 =  fWidth  / 2.0f;
		a._22 = -fHeight / 2.0f;
		a._41 =  fWidth  / 2.0f;
		a._42 =  fHeight / 2.0f;
	#else
		a._11 =  d3.sRenderW / 2.0f;
		a._22 = -d3.sRenderH / 2.0f;
		a._41 =  d3.sRenderW / 2.0f;
		a._42 =  d3.sRenderH / 2.0f;
	#endif

	MTX		mtxViewPortInv;
	D3DXMatrixInverse(&mtxViewPortInv, NULL, &a);

	// �t�ϊ�
	a = mtxViewPortInv * d3.mtxProjectionInverse * d3.mtxViewInverse;
	
	VEC3	vecOut;

	#ifdef	__2D_RESIZE
		VEC3	v(pvecPos->x * d3.fZoom, pvecPos->y * d3.fZoom, pvecPos->z);
		D3DXVec3TransformCoord(&vecOut, &v,	&a);
	#else
		D3DXVec3TransformCoord(&vecOut, pvecPos, &a);
	#endif

	return vecOut;
}



/*------------------------------------------------------------------------------*
| <<< �R�c�x�N�g������X�N���[�����W���쐬 >>>
|	����	vecPos	 = 3D ���W
|			mtxWorld = ���[���h�s��
|	�o��	vecOut	 = �ϊ���̃x�N�g���i 2D ���W �j
*------------------------------------------------------------------------------*/
void	calc_vec3_vector_to_screen(VEC3 *vecOut, VEC3 *vecPos, MTX *mtxWorld)
{
	D3DVIEWPORT9	vp;// =	{0,	0, (DWORD)(SCR_W * sys.fXZoom),	(DWORD)(SCR_H	* sys.fYZoom), 0.0f, 500.0f};
	d3.dev->GetViewport(&vp);
	D3DXVec3Project(vecOut,	vecPos,	&vp, &d3.mtxProjection,	&d3.mtxView, mtxWorld);
}

void	calc_vec3_vector_to_screen(VEC3 *vecOut, VEC3 *vecPos)
{
	calc_vec3_vector_to_screen(vecOut, vecPos, &d3.mtxIdentity);
}


// �X�v���C��
VEC2	*cacl_vec2_spline(VEC2 *pv0, const VEC2 *pv1, const VEC2 *pt1, const VEC2 *pv2, const VEC2 *pt2, float t)
{

	float	t2, t3;
	VEC2	v[4];
	t2 = t * t;
	t3 = t * t * t;

	v[0]  = *pv1 * (( 2.0f * t3) - (3.0f * t2) + 1.0f);
	v[1]  = *pt1 * ((        t3) - (2.0f * t2) + t   );
	v[2]  = *pt2 * ((        t3) - (       t2)       );
	v[3]  = *pv2 * ((-2.0f * t3) + (3.0f * t2)       );
	v[0] += v[1];
	v[0] += v[2];
	*pv0  = v[0] + v[3];
	return pv0;
}



/*------------------------------------------------------------------------------*
| <<< �ʖ@���̌v�Z >>>
|	����	v0,	v1,	v2 = �O�p�`�̊e���_
|	�߂�l	�@���x�N�g��
*------------------------------------------------------------------------------*/
/*VEC3	d3_CalcPlaneNormal(VEC3 *v0, VEC3 *v1, VEC3	*v2)
{
	VEC3	v3,	v4,	tmp;

	v3 = *v1 - *v0;
	v4 = *v2 - *v1;
	D3DXVec3Cross(	  &tmp,	&v3, &v4);
	D3DXVec3Normalize(&tmp,	&tmp);
	return	tmp;
}*/
