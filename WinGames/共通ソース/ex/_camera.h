

#ifndef	__CAMERA_
#define	__CAMERA_

//
//	�J��������
//

//--- �\���̒�` ----------------------------------------------------------------------------------
struct	CAMERA_WORK
{
	float	fMoveScale;						// �ړ����̃X�P�[��
	float	fViewAngle;						// �J�����̎���p
	float	fTheta,	fRho;					// �J�����̉�]�p�x
											// �J�����̉�]�s��
	VEC3	vecRot;							// �}�b�v�̉�]�l�A�ړ��l
	float	fOrthoScale;					// 0 �ȏ�Ȃ�ΐ��ˉe�s��ɂȂ�
};


//--- �O���Q�Ɛ錾 --------------------------------------------------------------------------------
extern	CAMERA_WORK	camera;

extern	void	camera_init(void);			// �J����������
extern	void	camera_main(void);			// �J�������C������
extern	void	camera_move_main(void);		// �f�o�b�O�J�����ړ�


/*------------------------------------------------------------------------------*
| <<< �f�o�b�O�J�����A�ړ��X�P�[���Z�b�g >>>
|	����	fScale = �J�����̈ړ��X�P�[��
*------------------------------------------------------------------------------*/
inline	void	camera_move_scale_set(float fScale){ camera.fMoveScale = fScale;}



#endif			//	__CAMERA_


