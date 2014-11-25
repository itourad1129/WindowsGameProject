//--- 定数 --------------------------------------------------------------------
#define MAP_W		(__SCR_W / BLK_SIZE)	// 仮想マップの横ブロック数(640 / 32 = 20)
#define MAP_H		(__SCR_H / BLK_SIZE)	// 仮想マップの横ブロック数(480 / 32 = 15)
#define MAP_SIZE	(MAP_W * MAP_H)

//--- 仮想マップアクセス用のマクロ ---------------------------------------------
#define POS_ADR(x,y)	(int)((((y) / BLK_SIZE) * MAP_W) + ((x) / BLK_SIZE))
// 表示座標から仮想マップのアドレスを得る
#define ADR(x, y)	(int)((y) * MAP_W + x)

//---以下、外部参照宣言 --------------------------------------------------------
extern	char	chMap[];			// 仮想マップ
extern	char	chMapBak[];			// 仮想マップバックアップ用
extern	void	MapLoad(int iStage);
extern	void	MapDraw(void);
extern	void	SabMapDraw(void);
