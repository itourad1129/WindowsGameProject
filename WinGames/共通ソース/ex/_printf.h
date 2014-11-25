//
//	•¶š•\¦
//

#ifndef	__PRINTF_
#define	__PRINTF_

//--- ’è” ----------------------------------------------------------------------------------------
#define	PRINTF_CHARA_MAX	2000					// Å‘å•¶š”


//--- ŠÖ”éŒ¾ ------------------------------------------------------------------------------------
													// •¶šF‚ğİ’è
extern	void	printf_front_color_set(uchar a, uchar r, uchar g, uchar b);
													// •¶š‚Ì”wŒiF‚ğİ’è
extern	void	printf_back_color_set(uchar a, uchar r, uchar g, uchar b);
													// •¶šƒŠƒZƒbƒg
extern	void	printf_reset(void);
													// •¶š‰Šú‰»
extern	void	printf_init(void);
													// •¶š•`‰æ
extern	void	printf_flush(void);
													// ‚Q‚cÀ•W‚Å•¶š“o˜^
extern	void	printf_2d(int iX, int iY, int iSize, BOOL bBack, char *Format,...);
extern	void	printf_2d(int iX, int iY, int iSize,			 char *Format,...);
extern	void	printf_2d(int iX, int iY,						 char *Format,...);
extern	void	printf_2d(										 char *Format,...);

#ifdef	__CALC
														// ‚R‚cÀ•W‚Å•¶š“o˜^
	extern	void	printf_3d(VEC3 *pvecPos, int iSize, BOOL bBack, char *Format,...);
	extern	void	printf_3d(VEC3 *pvecPos, int iSize,             char *Format,...);
	extern	void	printf_3d(VEC3 *pvecPos,                        char *Format,...);
#endif


//--- ‹Œ”Å‘Î‰—p
#define	Printf		printf_2d
#define	PrintfInit	printf_init
#define	PrintfFlush	printf_flush


#endif			//	__PRINTF_


