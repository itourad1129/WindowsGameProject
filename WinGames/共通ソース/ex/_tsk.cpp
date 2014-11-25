/************************************************************************
|	[[[	�^�X�N�����v���O���� ]]]
************************************************************************/
//
//	tsk_check �� game_main ���Ăяo������
//


#ifdef	__TSK

//#include	"..\\main.h"
#include	<stdarg.h>



/************************************************************************
* <<< �萔 >>>
************************************************************************/
#define	TSK_DEBUG		OFF					// �^�X�N�p�f�o�b�O�X�C�b�`

#define	TSK_BUF_SIZE	((sizeof(TSK_WORK) * (TSK_MAX +	TSK_PRI_MAX	* 2)) +	(sizeof(int) * TSK_PRI_MAX * 3))


/************************************************************************
* <<< �ϐ� >>>
************************************************************************/

		TSK_WORK	TskDummy[1];			// �G���[�p�_�~�[�o�b�t�@
static	TSK_WORK	*TskList;				// �^�X�N���X�g�o�b�t�@
static	TSK_WORK	*TskBeginList;			// ���̃v���C�I���e�B�̏����O�ɌĂ΂��^�X�N
static	TSK_WORK	*TskEndList;			// ���̃v���C�I���e�B�̏�����ɌĂ΂��^�X�N
static	int			TskCountAll;			// ���ݓo�^����Ă���^�X�N�̐�
											// ���ݓo�^����Ă���^�X�N�̐�
static	int			*TskCount;				// �v���C�I���e�B��
static	int			*Tsk1st;				// �e�^�X�N�̏�����s�ԍ��������Ă���B���|�P�Ȃ�o�^�Ȃ�
static	int			*TskNext;				// �e�^�X�N�̍ŏI���s�ԍ��������Ă���B���|�P�Ȃ�o�^�Ȃ�
		uint		TskExePriolity;			// ���s���̃v���C�I���e�B
		int			TskStartNum	= -1;		// ���O�ɓo�^�����^�X�N�ԍ�



/************************************************************************
* <<< �v���O���� >>>
************************************************************************/


/*----------------------------------------------------------------------*
| <<< �^�X�N���������� >>>
|	�߂�l	FALSE = ���������s
*----------------------------------------------------------------------*/
BOOL	tsk_init(void)
{
	
	if(TskList != NULL){ return FALSE;}

	if(TSK_ALL >= 32){ return (BOOL)sys_error("TSK:�^�X�N�̐����������܂��BTSK_ALL : %d", TSK_ALL);}

	ushort		i;
	TSK_WORK	*t;
										// �^�X�N���[�N�m��
	if((TskList	= (TSK_WORK	*)malloc(TSK_BUF_SIZE))	==	NULL){ return (BOOL)sys_error("TSK:�������[���s�����Ă��܂��B");}

	TskBeginList = (TSK_WORK *)&TskList[	 TSK_MAX];
	TskEndList	 = (TSK_WORK *)&TskBeginList[TSK_PRI_MAX];
	TskCount	 = (int		 *)&TskEndList[	 TSK_PRI_MAX];
	Tsk1st		 = (int		 *)&TskCount[	 TSK_PRI_MAX];
	TskNext		 = (int		 *)&Tsk1st[		 TSK_PRI_MAX];

	TskCountAll	= 0;					// �o�^���N���A

	for(i =	0; i < TSK_PRI_MAX;	++i)	// �v���C�I���e�B���̏����N���A
	{
		TskCount[i]	= 0;
		Tsk1st[  i] =	
		TskNext[ i] = -1;
	}

	for(i =	0; i < TSK_PRI_MAX;	++i)	// �v���C�I���e�B���̏����N���A
	{
		TskBeginList[i].ucStat = TSK_OFF;
		TskEndList[	 i].ucStat = TSK_OFF;
		TskBeginList[i].uiAdrs = (uint)NULL;
		TskEndList[	 i].uiAdrs = (uint)NULL;
	}

	t =	TskList;						// ���X�g�擪
	for(i =	0; i < TSK_MAX;	++i, ++t)	// �^�X�N���X�g��������
	{
		t->ucStat =	TSK_OFF;			// ����X�e�[�^�X
		t->uiAdrs =	(uint)NULL;			// �o�^���[�`���̎��s�A�h���X
	}
	t =	TskList;						// ���X�g�擪

	return TRUE;
}



/*----------------------------------------------------------------------*
| <<< �^�X�N�I������ >>>
*----------------------------------------------------------------------*/
void	tsk_free(void)
{
	if(TskList != NULL){ free(TskList);}
}



/*----------------------------------------------------------------------*
| <<< �v���C�I���e�B���̃^�X�N�����������o�^ >>>
|	����	uiPri  = �o�^�v���C�I���e�B
|			*pAdr  = ���s�A�h���X			TSK_PRI0 �`	TSK_PRI7
|			uiID   = �l�󂯓n���Ɏg�p		�����[�U�[�����R�Ɏg���Ă悢
|			iDelay = ���s�܂ł̒x���^�C��	-1 �ő����s
*----------------------------------------------------------------------*/
TSK_WORK	*tsk_begin_start(uint uiPri, void *pAdr, uint uiID, int iDelay)
{

	TSK_WORK	*t = &TskBeginList[uiPri];

	t->ucStat =	TSK_USE	+ uiPri;		// ����J�n�ݒ�
	t->ucMode =	TSK_INIT;				// �������
	t->usID	  =	uiID;
	t->uiAdrs =	(uint)pAdr;				// �R�[���A�h���X�o�^
	t->sDelay =	iDelay;

	return t;
}



/*----------------------------------------------------------------------*
| <<< �v���C�I���e�B���̃^�X�N�����������J�� >>>
|	����	uiPri  = �o�^�v���C�I���e�B
*----------------------------------------------------------------------*/
void	tsk_begin_free(uint uiPri)
{

	TskBeginList[uiPri].ucStat = TSK_OFF;
	TskBeginList[uiPri].uiAdrs = NULL;
}



/*----------------------------------------------------------------------*
| <<< �v���C�I���e�B���̃^�X�N�I�������o�^ >>>
|	����	uiPri  = �o�^�v���C�I���e�B
|			*pAdr  = ���s�A�h���X			TSK_PRI0 �`	TSK_PRI7
|			uiID   = �l�󂯓n���Ɏg�p		�����[�U�[�����R�Ɏg���Ă悢
|			iDelay = ���s�܂ł̒x���^�C��	-1 �ő����s
*----------------------------------------------------------------------*/
TSK_WORK	*tsk_end_start(uint uiPri, void *pAdr, uint uiID, int iDelay)
{

	TSK_WORK	*t = &TskEndList[uiPri];

	t->ucStat =	TSK_USE	+ uiPri;			// ����J�n�ݒ�
	t->ucMode =	TSK_INIT;					// �������
	t->usID	  =	uiID;
	t->uiAdrs =	(uint)pAdr;					// �R�[���A�h���X�o�^
	t->sDelay =	iDelay;
	return t;
}



/*----------------------------------------------------------------------*
| <<< �v���C�I���e�B���̃^�X�N�I�������J�� >>>
|	����	uiPri  = �o�^�v���C�I���e�B
*----------------------------------------------------------------------*/
void	tsk_end_free(uint uiPri)
{

	TskEndList[uiPri].ucStat = TSK_OFF;
	TskEndList[uiPri].uiAdrs = NULL;
}



/*----------------------------------------------------------------------*
| <<< �^�X�N�o�^ >>>
|	����	uiPri  = �o�^�v���C�I���e�B
|			*pAdr  = ���s�A�h���X			tsk.h �ɋL�q
|			uiID   = �l�󂯓n���Ɏg�p		�����[�U�[�����R�Ɏg���Ă悢
|			iDelay = ���s�܂ł̒x���^�C��	-1 �ő����s
*----------------------------------------------------------------------*/
TSK_WORK	*tsk_start(uint uiPri, void *pAdr, uint uiID, int iDelay)
{

	ushort		i, j;
	TSK_WORK	*t;

	TskStartNum	= -1;						// �o�^�ԍ�������
	
	if(TskCountAll > TSK_MAX)				// �o�^���E�`�F�b�N
	{
		TskDummy[0].ucMode = TSK_INIT;
		TskDummy[0].uiAdrs = (uint)pAdr;
		return &TskDummy[0];
	}

	for(i =	0; i < TSK_MAX;	++i)
	{
		j =	i;
		t =	&TskList[j];

		if(t->ucStat & TSK_USE){ continue;}	// �󂫂�T��
		if(t->sDelay > 0      ){ continue;}	// �I���������H

		TskStartNum	= j;					// �ꉞ�A�o�^�ԍ��͂Ƃ��Ă���
		t->ucStat   = TSK_USE + uiPri;		// ����J�n�ݒ�
		t->ucMode	= TSK_INIT;				// �������
		t->usID		= uiID;
		t->uiAdrs	= (uint)pAdr;			// �R�[���A�h���X�o�^
		if(Tsk1st[uiPri] == -1)				// �o�^�v���C�I���e�B������
		{
			t->sBefore	   = -1;			// �Z�b�g����ĂȂ��Ƃ��̏���
			t->sAfter	   = -1;
			Tsk1st[ uiPri] = j;
			TskNext[uiPri] = j;
		}
		else								// �O��̃��[�N�ɂ��̃��[�N��
		{									// �ԍ����Z�b�g
			TskList[TskNext[uiPri]].sAfter = j;
			t->sBefore					   = TskNext[uiPri];
			t->sAfter					   = -1;
			TskNext[uiPri]				   = j;
		}
											// �^�X�N�̃��[�N�������ŃN���A�[
		memset(t->uiWork, 0, TSK_WSIZE * sizeof(uint));
											// ���Ă�����
											// �����s���̃^�X�N�v���C�I���e�B
											// ���Q�Ƃ��ĕK�����̎����Ɏ��s��
											// ��悤�Ƀf�B���C�l��ݒ�
		if(iDelay == -1)					// �����s�`�F�b�N
		{
			iDelay = 0;
			void(*func)(TSK_WORK*);
			func  =	(void(*)(TSK_WORK*))t->uiAdrs;
			(*func)(t);
		}
		if(uiPri < TskExePriolity){ t->sDelay = iDelay;}
		else				      { t->sDelay = iDelay + 1;}

		++TskCount[uiPri];					// �o�^���{�P
		++TskCountAll;
		return t;							// ���[�N�A�h���X��Ԃ�
	}


	TskDummy[0].ucMode = TSK_INIT;
	TskDummy[0].uiAdrs = (uint)pAdr;

#if	TSK_DEBUG
	sys_error("�^�X�N�I�[�o�[�ł��B");
#endif

	return &TskDummy[0];					// �o�^���s
}



/*----------------------------------------------------------------------*
| <<< �^�X�N�폜 >>>
|	����	t =	�^�X�N�̃��[�N�A�h���X
*----------------------------------------------------------------------*/
void	tsk_end(TSK_WORK *t)
{

	if(t ==	NULL){ return;}

	uint	uiPri = TSK_PRI_GET(t);

											// tsk_end ��������|����Ƃ��̏���
	if(t->ucStat ==	TSK_OFF	|| t->uiAdrs ==	(uint)NULL){ return;}
	
	--TskCount[uiPri];						// �o�^�� -	1
	--TskCountAll;							// ���o�^��	- 1

	t->ucStat =	TSK_OFF;					// ����X�e�[�^�X
	t->uiAdrs =	(uint)NULL;					// �o�^���[�`���̎��s�A�h���X
	t->sDelay =	4;							// �e�q�֌W�ێ��̂��߂S�����͂��̃^�X�N�G���A�̎g�p�֎~(4�͓K��)

	if(t->sAfter == -1)						// �ŏI�`�F�b�N
	{
		TskNext[uiPri] = t->sBefore;
	}
	else
	{
		TskList[t->sAfter].sBefore = t->sBefore;
		if(t->sBefore == -1){ Tsk1st[uiPri] = t->sAfter;}
	}

	if(t->sBefore == -1)					// �擪�`�F�b�N
	{
		Tsk1st[uiPri] = t->sAfter;
	}
	else
	{
		TskList[t->sBefore].sAfter = t->sAfter;
		if(t->sAfter ==	-1){ TskNext[uiPri] = t->sBefore;}
	}
}



/*----------------------------------------------------------------------*
| <<< �w��v���C�I���e�B�̃^�X�N���[�h��S�ĕύX���� >>>
|	����	uiPri  = �o�^�v���C�I���e�B
|			uiMode = �ύX���[�h
*----------------------------------------------------------------------*/
void	tsk_mode_change_priority(uint uiPri, uint uiMode)
{

	int			iNum, iStart, iEnd;
	TSK_WORK	*t;


	if(uiPri == TSK_ALL)					// �S�^�X�N�Ώ�
	{
		iStart = TSK_07;
		iEnd   = TSK_00;
	}
	else
	{
		iStart = iEnd =	uiPri;
	}

	for(int i =	iStart; i <= iEnd; i++)
	{
		if(TskCount[i] == 0){ continue;}	// �v���C�I���e�B�o�^���`�F�b�N
		iNum = Tsk1st[i];
		if(iNum == -1      ){ continue;}
		for(;;)
		{
			t =	&TskList[iNum];				// �v���C�I���e�B���[�N�Z�b�g
			if(!(t->ucStat & TSK_SAVE))		// SAVE	�^�X�N�͑ΏۊO�ƂȂ�
			{
				t->sDelay =	0;
				t->ucMode =	uiMode;
			}
											// �o�^�^�X�N�̍Ō�`�F�b�N
			if(t->sAfter ==	-1){ break;}
			iNum = t->sAfter;
		}
	}
}



/*----------------------------------------------------------------------*
| <<< ���O�ɓo�^���ꂽ�^�X�N�����s >>>
*----------------------------------------------------------------------*/
void	tsk_quick_call(void)
{

	if(TskStartNum	== -1){ return;}

	TSK_WORK	*t = &TskList[TskStartNum];

	void(*func)(TSK_WORK*);
	func = (void(*)(TSK_WORK*))t->uiAdrs;
	(*func)(t);
}



/*----------------------------------------------------------------------*
| <<< �v���C�I���e�B�ύX >>>
|	����	t     = �^�X�N�̃��[�N�A�h���X
|			uiPri = �o�^�v���C�I���e�B			
*----------------------------------------------------------------------*/
void	tsk_change_priority(TSK_WORK *t, uint uiPri)
{

	if(TSK_PRI_GET(t) == uiPri){ return;}	// �����v���C�I���e�B�̓��^�[��

	int	iDelay = 0;

	if((uint)TSK_PRI_GET(t) < uiPri){ iDelay++;}

	TSK_WORK	*tt	= tsk_start(uiPri, (void *)t->uiAdrs, t->usID, iDelay);
	tt->ucMode = t->ucMode;
	memcpy(tt->uiWork, t->uiWork, sizeof(t->uiWork));
	tsk_end(t);
}



/*----------------------------------------------------------------------*
| <<< �^�X�N�R�[���T�u >>>
|	����	t     = �^�X�N�̃��[�N�A�h���X
*----------------------------------------------------------------------*/
inline	void	tsk_call_sub(TSK_WORK *t)
{

	if(t->sDelay > 0)						// �f�B���C�`�F�b�N
	{
		t->sDelay--;
	}
	else									// �o�^���ꂽ�v���O���������s
	{
		void(*func)(TSK_WORK*);
		func = (void(*)(TSK_WORK*))t->uiAdrs;
		(*func)(t);
	}
}



/*----------------------------------------------------------------------*
| <<< �^�X�N�ɓo�^���ꂽ�v���O�����̎��s >>>	�� iPri �ɓo�^���ꂽ�^�X�N��S�Ď��s����
|	����	uiPri = �o�^�v���C�I���e�B
*----------------------------------------------------------------------*/
void	tsk_call(uint uiPri)
{

	int			no;
	TSK_WORK	*t;
	BOOL		bLoop = TRUE;

	TskExePriolity = uiPri;					// ���s�̃^�X�N�ԍ���ۑ�
											// �^�X�N�ŏ��̈��Ăяo��
	if(TskBeginList[TskExePriolity].uiAdrs != NULL)
	{
		tsk_call_sub(&TskBeginList[TskExePriolity]);
	}
	if(TskCount[TskExePriolity]	== 0)		// �v���C�I���e�B�o�^���`�F�b�N
	{
		bLoop = FALSE;
	}
	else
	{
		no = Tsk1st[TskExePriolity];
		if(no == -1){ bLoop = false;}
	}
	while(bLoop)
	{
		t =	&TskList[no];			// �v���C�I���e�B���[�N�Z�b�g
		tsk_call_sub(t);
									// �o�^�^�X�N�̍Ō�`�F�b�N
		if(t->sAfter ==	-1){ break;}
		no = t->sAfter;
	}
									// �^�X�N�Ō�̈��Ăяo��
	if(TskEndList[TskExePriolity].uiAdrs !=	NULL)
	{
		tsk_call_sub(&TskEndList[TskExePriolity]);
	}
}



/*----------------------------------------------------------------------*
| <<< �^�X�N�̏�Ԃ��`�F�b�N >>>
*----------------------------------------------------------------------*/
void	tsk_check(void)
{

	//--- �S�^�X�N������I�����Ă��� delay > 0 �̂��̂�T���� delay-- ����
	TSK_WORK	*t;
	int			i;

	for(i =	0; i < TSK_MAX;	++i)
	{
		t =	&TskList[i];
		if(t->ucStat ==	TSK_OFF	&& t->uiAdrs ==	(uint)NULL && t->sDelay	> 0)
		{
			t->sDelay--;
		}
	}

#if	TSK_DEBUG
	int		cnt	= 0;
	for(i =	0; i < TSK_PRI_MAX;	i++) cnt +=	TskCount[i];
	d3d_printf(0, 0, SCR_H - 64, 0xff0000ff, "���݂̃^�X�N�� : %d",	cnt);
#endif
}


#else

	BOOL	tsk_init(void){return TRUE;}
	void	tsk_free(void){}

#endif
