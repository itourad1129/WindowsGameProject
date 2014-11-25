//
//	‹‘äƒwƒbƒ_
//

#ifndef	__FRUSTUM_H

#define	__FRUSTUM_H


extern	void	frustum_init(void);
extern	int		frustum_hit_check(D3_HIT *pHit, MTX *mWorld);
extern	int		frustum_hit_check(VEC3 *apvecPos, MTX *mtxWorld);


#endif					// end _frustum.h


