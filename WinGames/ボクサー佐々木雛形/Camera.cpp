//
// �J�珈��
//

#include	"main.h"

/*-------------------------------------------------------------------*
| <<< �J�����ݒ� >>>
*--------------------------------------------------------------------*/
void	Camera(void)
{
									//�ǂ�			�ǂ�����	�����͂ǂ��炩��
	D3DXMatrixLookAtLH(&d3.mtxView, &d3.vecCamera, &d3.vecEye, &d3.vecUp);
												//������p
	D3DXMatrixPerspectiveFovLH(&d3.mtxProjection, R(45), (float)__SCR_W / (float)__SCR_H, D3_VIEW_NEAR, D3_VIEW_FAR);
												// View �s��
	d3.dev->SetTransform(D3DTS_VIEW, &d3.mtxView);
												// �ˉe�s��
	d3.dev->SetTransform(D3DTS_PROJECTION,  &d3.mtxProjection);
												// View�ˉe�s��
	d3.mtxViewProjection = d3.mtxView * d3.mtxProjection;
												// View�ˉe�]�u�s��
	D3DXMatrixTranspose(&d3.mtxViewProjectionTranspose, &d3.mtxViewProjection);

	//--- �ȉ��A�p�[�e�B�N���̎��ɕK�v�ɂȂ�܂��B--------------------
	// View �t�s������߂Ă���
	float		tmp;
	D3DXMatrixInverse(&d3.mtxViewInverse, &tmp, &d3.mtxView);
	d3.mtxBillboard = d3.mtxViewInverse;
}