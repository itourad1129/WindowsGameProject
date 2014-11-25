
//--- �A�v���� ------------------------------------------------------------------------------------
#define	APP_NAME	"���ɔ�"				// �A�v����
#define	VERSION		"1.00"						// �o�[�W�������
#define ROUND_MAX	5						// �X�e�[�W��

//--- �萔���` ----------------------------------------------------------------------------------
#define BLK_SIZE	32						// �u���b�N�T�C�Y

//--- ���[�h --------------------------------------------------------------------------------------
enum
{
	MODE_TITLE,				// �^�C�g��
	MODE_INFO,				// ����
	MODE_GAME,				// �Q�[��
	MODE_CLEAR,				// �X�e�[�W�N���A�[
	MODE_ALLCLEAR,			// �S�X�e�[�W�N���A�[
	MODE_OVER,				// �Q�[���I�[�o�[
};
// --- ���� ---------------------------------------------------------------------------------------
enum
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};
//--- �e�N�X�`���[ --------------------------------------------------------------------------------
enum
{
	TEX_MOJI,				// ����
	TEX_MAP,				// �}�b�v
	TEX_TITLE,				// �^�C�g��
	TEX_CLEAR,				// �N���A
	TEX_OVER,				// �I�[�o�[
	TEX_ALLCLEAR,			// �I�[���N���A
	TEX_INFO,				// �C���t�H
	TEX_PLAYER,				// �v���C���[
};
//--- �T�E���h�ԍ� --------------------------------------------------------------------------------
enum
{
	SND_WALK,				// ���s
	SND_OUCH,				// �ǂɂԂ���
	SND_START,				// �X�^�[�g
	SND_ATTAINMENT,			// �ړI�n���B
	SND_CLEAR,				// �����̃N���A�[
	SND_UNDO,				// �A���h�D
	SND_TIMEUP,				// �^�C���A�b�v
	SND_GIVEUP,				// �M�u�A�b�v
	SND_ALLCLEAR,			// �I�[���N���A
	SND_NONE,				// ����
};
//--- X�t�@�C�� -----------------------------------------------------------------------------------
enum
{
	X_WALL,					// ��
	X_MOKUTEKICHI,			// �ړI�n
	X_NIMOTU,				// �ו�
	X_PLAYER,				// �v���C���[
};
//--- �C���N���[�h�t�@�C�� ------------------------------------------------------------------------

#include	"Game.h"
#include	"Map.h"
#include	"Stage.h"
#include	"Title.h"
#include	"Printf.h"
#include	"Player.h"
#include	"Timer.h"
#include	"Undo.h"
#include	"Camera.h"
#include	"Menu.h"
