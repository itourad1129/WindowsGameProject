//--- �萔 --------------------------------------------------------------------
#define MAP_W		(__SCR_W / BLK_SIZE)	// ���z�}�b�v�̉��u���b�N��(640 / 32 = 20)
#define MAP_H		(__SCR_H / BLK_SIZE)	// ���z�}�b�v�̉��u���b�N��(480 / 32 = 15)
#define MAP_SIZE	(MAP_W * MAP_H)

//--- ���z�}�b�v�A�N�Z�X�p�̃}�N�� ---------------------------------------------
#define POS_ADR(x,y)	(int)((((y) / BLK_SIZE) * MAP_W) + ((x) / BLK_SIZE))
// �\�����W���牼�z�}�b�v�̃A�h���X�𓾂�
#define ADR(x, y)	(int)((y) * MAP_W + x)

//---�ȉ��A�O���Q�Ɛ錾 --------------------------------------------------------
extern	char	chMap[];			// ���z�}�b�v
extern	char	chMapBak[];			// ���z�}�b�v�o�b�N�A�b�v�p
extern	void	MapLoad(int iStage);
extern	void	MapDraw(void);
extern	void	SabMapDraw(void);
