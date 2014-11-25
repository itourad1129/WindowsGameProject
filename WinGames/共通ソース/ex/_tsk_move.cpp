//
//	�ړ��^�X�N
//


//#include	"main.h"


//--- �\���̒�` ----------------------------------------------------------------------------------
struct	MOVE_WORK
{
	int			iMode;
	TSK_WORK	*ptskMother;
	float		*pfStart, fStartBak;
	float		fStart, fEnd, fSpeed;
	float		fLength;
	float		fCount;
	int			iTime, iTimeMax;
};


//--- �֐��錾 ------------------------------------------------------------------------------------
static	void	Move(TSK_WORK *t);



/*------------------------------------------------------------------------------*
| <<< pfStart �̒��� fEnd �ɂ���^�X�N >>>
*------------------------------------------------------------------------------*/
static	void	Move(TSK_WORK *t)
{

	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	//--- �ŏ��̈��̂݃X�s�[�h�̌v�Z������ ------------------------
	switch(t->ucMode)
	{
	case TSK_INIT:
		w->fSpeed = (w->fEnd - *w->pfStart)	/ (float)w->iTime;
		t->ucMode = TSK_MAIN;
		break;

	case TSK_END:
		tsk_end(t);
		break;
	}
	//--- mother ��	-1 �Ȃ��̂��`�F�b�N���Ȃ�	---------------------
	if(w->ptskMother !=	TSK_NO_MOTHER)
	{
		//--- ��̂̃^�X�N������ł����炱���ŏI�� ------------------
		if(w->ptskMother->uiAdrs ==	NULL)
		{
			tsk_end(t);
			return;
		}
	}
	//--- �ȉ����Ԃ��v�����ړ�---------------------------------------
	w->iTime--;
	*w->pfStart += w->fSpeed;
	if(w->iTime <= 0)
	{
		*w->pfStart = w->fEnd;
		tsk_end(t);
	}
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω������� >>>
|	����	ptskMother = �}�U�[�ƂȂ�^�X�N	��TSK_NO_MOTHER �Ƃ��Ďw��Ȃ�Ζ���
|			*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay)
{

	//--- ptskMother ��	-1 �Ȃ��̂��`�F�b�N���Ȃ�	-----------------
	if(ptskMother != TSK_NO_MOTHER)
	{
		//--- ��̂̃^�X�N������ł����炱���ŏI�� ------------------
		if(ptskMother->uiAdrs == NULL){ return NULL;}
	}
	//--- ���Ԃ� -1	�Ȃ瑦�����s ------------------------------------
	if(iTime == -1)
	{
		*pfStart = fEnd;
		return NULL;
	}
	//--- ���łɏI���ʒu�ɂ����� ------------------------------------
	if(*pfStart == fEnd)
	{
		return NULL;
	}
	//--- �ȉ��A�ړ��^�X�N���N�����Đ��l��ݒ� ----------------------
	TSK_WORK	*t = tsk_start(TSK_MOVE, Move,	0, 0);
	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	w->ptskMother = ptskMother;
	w->pfStart	  = pfStart;
	w->fEnd		  = fEnd;
	w->iTimeMax	  =
	w->iTime	  = iTime;
	w->fStartBak  = *pfStart;

	if(iDelay == -1){ tsk_quick_call();}
	else			{ t->sDelay = iDelay;}

	return t;
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω������� >>>
|	����	*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime, int iDelay)
{
	return	tsk_move_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, iDelay);
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω������� >>>
|	����	*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime)
{
	return	tsk_move_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, -1);
}



/*------------------------------------------------------------------------------*
| <<< sin ���g���� pfStart �̒��� fEnd �ɂ���^�X�N >>>	�����������A�I���x��
*------------------------------------------------------------------------------*/
static	void	MoveSin(TSK_WORK *t)
{

	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	//--- �ŏ��̈��̂݃X�s�[�h�̌v�Z������ ----------------
	switch(t->ucMode)
	{
	case TSK_INIT:
		w->fSpeed  = R(90) / (float)w->iTime;
		w->fLength = w->fEnd - *w->pfStart;
		t->ucMode  = TSK_MAIN;
		break;

	case TSK_END:
		tsk_end(t);
		break;
	}
	//--- mother ��	-1 �Ȃ��̂��`�F�b�N���Ȃ�	---------------------
	if(w->ptskMother !=	TSK_NO_MOTHER)
	{
		//--- ��̂̃^�X�N������ł����炱���ŏI�� ------------------
		if(w->ptskMother->uiAdrs ==	NULL)
		{
			tsk_end(t);
			return;
		}
	}

	//--- �ȉ����Ԃ��v�����ړ�---------------------------------------
	w->iTime--;
	w->fCount  += w->fSpeed;
	*w->pfStart = w->fStart + w->fLength * sinf(w->fCount);
	//d3_printf("sin:%d,%.4f,%.4f,%.4f", w->iTime, ( sinf(w->fCount)), w->fLength, *w->pfStart);

	if(w->iTime <= 0)
	{
		*w->pfStart = w->fEnd;
		tsk_end(t);
	}
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( ������ 0 �`90 �x�𗘗p ) >>>
|	����	ptskMother = �}�U�[�ƂȂ�^�X�N	��TSK_NO_MOTHER �Ƃ��Ďw��Ȃ�Ζ���
|			*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_sin_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay)
{

	//--- ptskMother ��	-1 �Ȃ��̂��`�F�b�N���Ȃ�	-----------------
	if(ptskMother != TSK_NO_MOTHER)
	{
		//--- ��̂̃^�X�N������ł����炱���ŏI�� ------------------
		if(ptskMother->uiAdrs == NULL){ return NULL;}
	}
	//--- ���Ԃ� -1	�Ȃ瑦�����s ------------------------------------
	if(iTime == -1)
	{
		*pfStart = fEnd;
		return NULL;
	}
	//--- ���łɏI���ʒu�ɂ����� ------------------------------------
	if(*pfStart == fEnd)
	{
		return NULL;
	}
	//--- �ȉ��A�ړ��^�X�N���N�����Đ��l��ݒ� ----------------------
	TSK_WORK	*t = tsk_start(TSK_MOVE, MoveSin, 0, 0);
	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	w->ptskMother = ptskMother;
	w->pfStart	  = pfStart;
	w->fStart	  = *pfStart;
	w->fEnd		  = fEnd;
	w->iTimeMax	  =
	w->iTime	  = iTime;
	w->fStartBak  = *pfStart;

	if(iDelay == -1){ tsk_quick_call();}
	else			{ t->sDelay = iDelay;}

	return t;
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( ������ 0 �`90 �x�𗘗p ) >>>
|	����	*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime, int iDelay)
{
	return	tsk_move_sin_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, iDelay);
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( ������ 0 �`90 �x�𗘗p ) >>>
|	����	*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime)
{
	return	tsk_move_sin_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, -1);
}



/*------------------------------------------------------------------------------*
| <<< cos ���g���� pfStart �̒��� fEnd �ɂ���^�X�N >>>	�������x���A�I������
*------------------------------------------------------------------------------*/
static	void	MoveCos(TSK_WORK *t)
{

	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	//--- �ŏ��̈��̂݃X�s�[�h�̌v�Z������ ----------------
	switch(t->ucMode)
	{
	case TSK_INIT:
		w->fSpeed  = R(90) / (float)w->iTime;
		w->fLength = w->fEnd - *w->pfStart;
		t->ucMode  = TSK_MAIN;
		break;

	case TSK_END:
		tsk_end(t);
		break;
	}
	//--- mother ��	-1 �Ȃ��̂��`�F�b�N���Ȃ�	---------------------
	if(w->ptskMother !=	TSK_NO_MOTHER)
	{
		//--- ��̂̃^�X�N������ł����炱���ŏI�� ------------------
		if(w->ptskMother->uiAdrs ==	NULL)
		{
			tsk_end(t);
			return;
		}
	}

	//--- �ȉ����Ԃ��v�����ړ�---------------------------------------
	w->iTime--;
	w->fCount  += w->fSpeed;
	*w->pfStart = w->fStart + w->fLength * (1.0f - cosf(w->fCount));
	//d3_printf("cos:%d,%.4f,%.4f,%.4f", w->iTime, ( cos(w->fCount)), w->fLength, *w->pfStart);

	if(w->iTime <= 0)
	{
		*w->pfStart = w->fEnd;
		tsk_end(t);
	}
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( �]���� 0 �`90 �x�𗘗p ) >>>
|	����	ptskMother = �}�U�[�ƂȂ�^�X�N	��TSK_NO_MOTHER �Ƃ��Ďw��Ȃ�Ζ���
|			*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_cos_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay)
{

	//--- ptskMother ��	-1 �Ȃ��̂��`�F�b�N���Ȃ�	-----------------
	if(ptskMother != TSK_NO_MOTHER)
	{
		//--- ��̂̃^�X�N������ł����炱���ŏI�� ------------------
		if(ptskMother->uiAdrs == NULL){ return NULL;}
	}
	//--- ���Ԃ� -1	�Ȃ瑦�����s ------------------------------------
	if(iTime == -1)
	{
		*pfStart = fEnd;
		return NULL;
	}
	//--- ���łɏI���ʒu�ɂ����� ------------------------------------
	if(*pfStart == fEnd)
	{
		return NULL;
	}
	//--- �ȉ��A�ړ��^�X�N���N�����Đ��l��ݒ� ----------------------
	TSK_WORK	*t = tsk_start(TSK_MOVE, MoveCos, 0, 0);
	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	w->ptskMother = ptskMother;
	w->pfStart	  = pfStart;
	w->fStart	  = *pfStart;
	w->fEnd		  = fEnd;
	w->iTimeMax	  =
	w->iTime	  = iTime;
	w->fStartBak  = *pfStart;

	if(iDelay == -1){ tsk_quick_call();}
	else			{ t->sDelay = iDelay;}

	return t;
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( �]�� 0 �`90 �x�𗘗p ) >>>
|	����	*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
|			iDelay	   = �x������
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime, int iDelay)
{
	return	tsk_move_cos_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, iDelay);
}



/*------------------------------------------------------------------------------*
| <<< pfStar �� fEnd �� iTime �������|���ĕω�������( ������ 0 �`90 �x�𗘗p ) >>>
|	����	*pfStart   = �J�n�ʒu				���l�͕ێ�����Ȃ��̂Œ���
|			fEnd       = �I���ʒu
|			iTime	   = �ړ�����
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime)
{
	return	tsk_move_cos_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, -1);
}

