//
//	�����\��
//

#ifndef	__PRINTF_
#define	__PRINTF_

//--- �萔 ----------------------------------------------------------------------------------------
#define	PRINTF_CHARA_MAX	2000					// �ő啶����


//--- �֐��錾 ------------------------------------------------------------------------------------
													// �����F��ݒ�
extern	void	printf_front_color_set(uchar a, uchar r, uchar g, uchar b);
													// �����̔w�i�F��ݒ�
extern	void	printf_back_color_set(uchar a, uchar r, uchar g, uchar b);
													// �������Z�b�g
extern	void	printf_reset(void);
													// ����������
extern	void	printf_init(void);
													// �����`��
extern	void	printf_flush(void);
													// �Q�c���W�ŕ����o�^
extern	void	printf_2d(int iX, int iY, int iSize, BOOL bBack, char *Format,...);
extern	void	printf_2d(int iX, int iY, int iSize,			 char *Format,...);
extern	void	printf_2d(int iX, int iY,						 char *Format,...);
extern	void	printf_2d(										 char *Format,...);

#ifdef	__CALC
														// �R�c���W�ŕ����o�^
	extern	void	printf_3d(VEC3 *pvecPos, int iSize, BOOL bBack, char *Format,...);
	extern	void	printf_3d(VEC3 *pvecPos, int iSize,             char *Format,...);
	extern	void	printf_3d(VEC3 *pvecPos,                        char *Format,...);
#endif


//--- ���őΉ��p
#define	Printf		printf_2d
#define	PrintfInit	printf_init
#define	PrintfFlush	printf_flush


#endif			//	__PRINTF_


