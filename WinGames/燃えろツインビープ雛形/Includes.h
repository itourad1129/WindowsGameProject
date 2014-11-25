
//--- �A�v���� ------------------------------------------------------------------------------------
#define	APP_NAME	"�R����c�C���r�[�v"			// �A�v����
#define	VERSION		"ver1.00"						// �o�[�W�������

//--- �Q�[�����[�h --------------------------------------------------------------------------------
enum	MODE_ID
{
	MODE_TITLE,										// �^�C�g��
	MODE_INFO,										// ����
	MODE_GAME,										// �Q�[����
	MODE_OVER,										// �Q�[���I�[�o�[
};
//--- X�t�@�C�� ------------------------------------------------------------------------------------
enum	MESH_ID
{
	X_PLAYER,										// �v���C���[
	X_METEO,										// 覐�
	X_BELL,											// �x��
	X_STAR,											// ��
};
//--- �T�E���h�ԍ� --------------------------------------------------------------------------------
enum	SND_ID
{
	SND_START,										// �X�^�[�g��
	SND_DAMAGE,										// �_���[�W��
	SND_BGM,										// BGM
	SND_MOVE,										// �ړ�
	SND_BELL,										// �x��
};
//--- �e�N�X�`���[ --------------------------------------------------------------------------------
enum	TEX_ID
{
	TEX_MOJI,										// ����
	TEX_TITLE,										// �^�C�g��
	TEX_INFO,										// ����
	TEX_OVER,										// �Q�[���I�[�o�[
	TEX_PARTICLE,									// �p�[�e�B�N��
};

//--- �C���N���[�h�t�@�C�� ------------------------------------------------------------------------
#include	"Game.h"
#include	"Printf.h"
#include	"Camera.h"
#include	"Title.h"
#include	"Particle.h"
#include	"Player.h"
#include	"Star.h"
#include	"Meteo.h"
#include	"Afterimage.h"
#include	"Bell.h"
#include	"CameraMove.h"
