//
//	�Փ˔���w�b�_
//

#ifndef	__COLLISION_H

#define	__COLLISION_H


/*------------------------------------------------------------------------------*
| <<< �_���ʂ̂ǂ��瑤�ɂ��邩���ׂ� >>>
|	����	pplane  = ��
|			pvecPos = �_
|	�߂�l	0  < ����
|			0  > �\��
|			0 == �ʂ̏�
*------------------------------------------------------------------------------*/
extern	float	collision_plane_point(PLANE *pplane, VEC3 *pvecPos);



/*------------------------------------------------------------------------------*
| <<< �_�������̂ǂ��瑤�ɂ��邩���ׂ� >>>
|	����	pvecPoint = ���ׂ������W
|			pvecLine0, pvecLine1 = ��
|	�߂�l	-1 : ����
|			 1 : �E��
|			 0 : ���̏�
*------------------------------------------------------------------------------*/
extern	int		collision_line_point(VEC2 *pvecPoint, VEC2 *pvecLine0, VEC2 *pvecLine1);

/*------------------------------------------------------------------------------*
| <<< �����Ɛ����Ƃ̌������� >>>
|	����	pvecLine0 - pvecLine1 = ���ׂ�������
|			pvecRay0  - pvecRay1  = ����������������	�� �������Ȍ��ɂ��邽�߂� p0 �K������Ap1 �͉E���ƂȂ�悤�ɂ���
|	�߂�l	TRUE	: �������Ă���
|			FALSE	: �������Ă��Ȃ�
*------------------------------------------------------------------------------*/
extern	BOOL	collision_line_ray(VEC2 *pvecLine0, VEC2 *pvecLine1, VEC2 *pvecRay0, VEC2 *pvecRay1);


/*------------------------------------------------------------------------------*
| <<< �~�Ɠ_�̏Փ˔��� >>>
|	����	pvecCircle, fR = �~�̒��S���W�Ɣ��a
|			pvecPoint      = �_���W
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_circle_point(VEC2 *pvecCircle, float fR, VEC2 *pvecPoint);

/*------------------------------------------------------------------------------*
| <<< �~�Ɛ��̏Փ˔��� >>>
|	����	pvecCircle, fR       = �~�̒��S���W�Ɣ��a
|			pvecLine0, pvecLine1 = �����̎n�_�ƏI�_
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_circle_line(VEC2 *pvecCircle, float fR, VEC2 *pvecLine0, VEC2 *pvecLine1);

/*------------------------------------------------------------------------------*
| <<< �~�Ɖ~�̏Փ˔��� >>>
|	����	pvecCircleA, fRadiusA = �~�̒��S���W�Ɣ��a
|			pvecCircleB, fRadiusB = �~�̒��S���W�Ɣ��a
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_circle_circle(VEC2 *pvecCircleA, float fRadiusA, VEC2 *pvecCircleB, float fRadiusB);



/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ɖ~�̏Փ˔��� >>>
|	����	pvecBox0, pvecBox01 = �l�p�`�̍���A�E��
|			pvecCircle			= �~�̒��S���W
|			fR					= �~�̔��a
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_circle(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecCircle, float fR);

/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ɠ_�̏Փ˔��� >>>	�����v�̔��Ύ���Œ�`�̂���
|	����	pvecBox0�`3 = �l�p�`�̎l���_
|			pvecPoint	= �_�̍��W
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_point(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecPoint);

/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ǝl�p�`�̏Փ˔��� >>>
|	����	p0, p1 = �l�p�`�̍���ƉE��
|			p2, p3 = �l�p�`�̍���ƉE��
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_box(VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3);

/*------------------------------------------------------------------------------*
| <<< �l�p�`�Ɛ��̏Փ˔��� >>>		�������Ɛ����Ƃ̌�������
|	����	pvecBox0 �` pvecBox3  = �l�p�`(���v���)
|			pvecLine0 - pvecLine1 = ����
|	�߂�l	TRUE or FALSE
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_line(VEC2 *pvecBox0,  VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecLine0, VEC2 *pvecLine1);



/*------------------------------------------------------------------------------*
| <<< ���Ƌ��̏Փ˔��� >>>
|	����	pvecSphere			= ��0 �̒��S���W
|			fRadius				= ��0 �̑傫��
|			pvecSphere			= ��1 �̒��S���W
|			fRadius				= ��1 �̑傫��
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_sphere_sphere(VEC3 *pvecSphere0, float fRadius0, VEC3 *pvecSphere1, float fRadius1);

/*------------------------------------------------------------------------------*
| <<< ���Ɛ����̌������� >>>
|	����	pvecSphere			= ���̒��S���W
|			fRadius				= ���̑傫��
|			pvecLine0,pvecLine1 = �����̎n�_�ƏI�_ 
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
#if 0
extern	BOOL	collision_sphere_line(VEC3 *pvecSphere, float fRadius, VEC3 *pvecLine0, VEC3 *pvecLine1);
#endif

/*------------------------------------------------------------------------------*
| <<< ���Ɛ����̌������� >>>
|	����	pvecSphere			= ���̒��S���W
|			fRadius				= ���̑傫��
|			pvecLine0,pvecLine1 = �����̎n�_�ƏI�_ 
|	�߂�l	TRUE : �q�b�g���Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_sphere_ray(VEC3 *pvecPos, float fR, VEC3 *pvecLine, VEC3 *pvecDir);



/*------------------------------------------------------------------------------*
| <<< �����ƎO�p�`�̌������� >>>		�����ʂ� FALSE ��Ԃ��܂�
|	����	pvecA, pvecB				 = ����
|			pvecTriA, pvecTriB, pvecTriC = �O�p�`�̔C�ӂ̒��_
|	�o��	pvecOut						 = �������W
|	�߂�l	TRUE : �Փ˂��Ă���
*------------------------------------------------------------------------------*/
extern	BOOL	collision_triangle_ray(VEC3 *pvecOut, VEC3 *pvecPos, VEC3 *pvecDir, VEC3 *apvecTri);


#endif					// end _collision.h


