#define	TSK_NO_MOTHER	((TSK_WORK *)-1)

/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω������� >>>
|	����	ptskMother = �}�U�[�ƂȂ�^�X�N	��TSK_NO_MOTHER �Ƃ��Ďw��Ȃ�Ζ���
|			*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
extern	TSK_WORK	*tsk_move_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime);


/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( ������ 0 �`90 �x�𗘗p ) >>>
|	����	ptskMother = �}�U�[�ƂȂ�^�X�N	��TSK_NO_MOTHER �Ƃ��Ďw��Ȃ�Ζ���
|			*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
extern	TSK_WORK	*tsk_move_sin_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime);


/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( �]���� 0 �`90 �x�𗘗p ) >>>
|	����	ptskMother = �}�U�[�ƂȂ�^�X�N	��TSK_NO_MOTHER �Ƃ��Ďw��Ȃ�Ζ���
|			*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
extern	TSK_WORK	*tsk_move_cos_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime);

