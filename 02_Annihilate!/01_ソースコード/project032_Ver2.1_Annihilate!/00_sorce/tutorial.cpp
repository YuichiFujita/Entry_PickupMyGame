//======================================================================================================================
//
//	�`���[�g���A����ʏ��� [tutorial.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "tutorial.h"

#include "billboard.h"
#include "bullet.h"
#include "camera.h"
#include "effect.h"
#include "enemy.h"
#include "explosion.h"
#include "field.h"
#include "item.h"
#include "life.h"
#include "light.h"
#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshwall.h"
#include "object.h"
#include "particle.h"
#include "player.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"
#include "timer.h"
#include "tornado.h"
#include "wall.h"
#include "weed.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_TUTORIAL		(1)			// �g�p����|���S����
#define END_TUTORIAL_TIME	(30)		// �`���[�g���A���I���܂ł̗]�C�t���[��

#define TUTORIAL_POS_X		(640.0f)	// �`���[�g���A���̐�΍��W (x)
#define TUTORIAL_POS_Y		(570.0f)	// �`���[�g���A���̐�΍��W (y)
#define TUTORIAL_WIDTH		(500.0f)	// �`���[�g���A���̕� / 2 (��)
#define TUTORIAL_HEIGHT		(110.0f)	// �`���[�g���A���̕� / 2 (����)

#define LESSON_00_CNT		(120)		// ���b�X��00�̉��Z�^�C�~���O
#define LESSON_01_CNT		(3)			// ���b�X��01�̉��Z�^�C�~���O
#define LESSON_02_CNT		(3)			// ���b�X��02�̉��Z�^�C�~���O
#define LESSON_03_CNT		(1)			// ���b�X��03�̉��Z�^�C�~���O
#define LESSON_04_CNT		(1)			// ���b�X��04�̉��Z�^�C�~���O
#define LESSON_05_CNT		(1)			// ���b�X��05�̉��Z�^�C�~���O
#define LESSON_06_CNT		(0)			// ���b�X��06�̉��Z�^�C�~���O

#define SET_ENEMY_X_00		(180.0f)	// �G�̈ʒu (x)
#define SET_ENEMY_Y_00		(-80.0f)	// �G�̈ʒu (y)
#define SET_ENEMY_Z_00		(-90.0f)	// �G�̈ʒu (z)
#define SET_ENEMY_ROT_00	(0)			// �G�̌���
#define SET_ENEMY_TYPE_00	(1)			// �G�̎��

#define SET_ENEMY_X_01		(0.0f)		// �G�̈ʒu (x)
#define SET_ENEMY_Y_01		(-80.0f)	// �G�̈ʒu (y)
#define SET_ENEMY_Z_01		(90.0f)		// �G�̈ʒu (z)
#define SET_ENEMY_ROT_01	(0)			// �G�̌���
#define SET_ENEMY_TYPE_01	(0)			// �G�̎��

#define SET_ENEMY_X_02		(-180.0f)	// �G�̈ʒu (x)
#define SET_ENEMY_Y_02		(-80.0f)	// �G�̈ʒu (y)
#define SET_ENEMY_Z_02		(-90.0f)	// �G�̈ʒu (z)
#define SET_ENEMY_ROT_02	(0)			// �G�̌���
#define SET_ENEMY_TYPE_02	(1)			// �G�̎��

#define SET_OBJECT_X_00		(180.0f)	// �I�u�W�F�N�g�̈ʒu (x)
#define SET_OBJECT_Y_00		(0.0f)		// �I�u�W�F�N�g�̈ʒu (y)
#define SET_OBJECT_Z_00		(90.0f)		// �I�u�W�F�N�g�̈ʒu (z)
#define SET_OBJECT_ROT_00	(315)		// �I�u�W�F�N�g�̌���
#define SET_OBJECT_BREAK_00	(1)			// �I�u�W�F�N�g�̉����̎��
#define SET_OBJECT_TYPE_00	(7)			// �I�u�W�F�N�g�̎��

#define SET_OBJECT_X_01		(0.0f)		// �I�u�W�F�N�g�̈ʒu (x)
#define SET_OBJECT_Y_01		(0.0f)		// �I�u�W�F�N�g�̈ʒu (y)
#define SET_OBJECT_Z_01		(180.0f)	// �I�u�W�F�N�g�̈ʒu (z)
#define SET_OBJECT_ROT_01	(0)			// �I�u�W�F�N�g�̌���
#define SET_OBJECT_BREAK_01	(1)			// �I�u�W�F�N�g�̉����̎��
#define SET_OBJECT_TYPE_01	(6)			// �I�u�W�F�N�g�̎��

#define SET_OBJECT_X_02		(-180.0f)	// �I�u�W�F�N�g�̈ʒu (x)
#define SET_OBJECT_Y_02		(0.0f)		// �I�u�W�F�N�g�̈ʒu (y)
#define SET_OBJECT_Z_02		(90.0f)		// �I�u�W�F�N�g�̈ʒu (z)
#define SET_OBJECT_ROT_02	(45)		// �I�u�W�F�N�g�̌���
#define SET_OBJECT_BREAK_02	(1)			// �I�u�W�F�N�g�̉����̎��
#define SET_OBJECT_TYPE_02	(7)			// �I�u�W�F�N�g�̎��

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureTutorial[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\tutorial000.png",	// ���b�X��0 (�ړ�) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\tutorial001.png",	// ���b�X��1 (�W�����v) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\tutorial002.png",	// ���b�X��2 (�U��) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\tutorial003.png",	// ���b�X��3 (����) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\tutorial004.png",	// ���b�X��4 (�j��) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\tutorial005.png",	// ���b�X��5 (��) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\tutorial006.png",	// ���b�X��6 (����) �̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
int CheckNumEnemy(void);				// ���݂̓G�̑����̊m�F����
int CheckNumBreakObject(void);			// ���݂̉���I�u�W�F�N�g�̑����̊m�F����
int CheckNumItem(void);					// ���݂̃A�C�e���̑����̊m�F����

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureTutorial[LESSON_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorial = NULL;				// ���_�o�b�t�@�ւ̃|�C���^

TUTORIALSTATE g_tutorialState;			// �`���[�g���A���̏��
int           g_nLessonState;			// ���b�X���̏��
int           g_nCounterTutorialState;	// �`���[�g���A���̏�ԊǗ��J�E���^�[
int           g_nCounterLessonState;	// ���b�X���̏�ԊǗ��J�E���^�[
bool          g_bTutorialEnd;			// ���[�h�̑J�ڏ�

//======================================================================================================================
//	�`���[�g���A����ʂ̏���������
//======================================================================================================================
void InitTutorial(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�`���[�g���A���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_2D        *pVtx;						// ���_���ւ̃|�C���^

	for (int nCntTutorial = 0; nCntTutorial < LESSON_MAX; nCntTutorial++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		// �e�N�X�`���̓ǂݍ���
		D3DXCreateTextureFromFile(pDevice, apTextureTutorial[nCntTutorial], &g_apTextureTutorial[nCntTutorial]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_2D) * 4 * MAX_TUTORIAL,	// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorial,
		NULL
	);

	// �O���[�o���ϐ���������
	g_tutorialState         = TUTORIALSTATE_NORMAL;		// �`���[�g���A���̏��
	g_nLessonState          = LESSON_00;				// ���b�X���̏��
	g_nCounterTutorialState = 0;						// �`���[�g���A���̏�ԊǗ��J�E���^�[
	g_nCounterLessonState   = 0;						// ���b�X���̏�ԊǗ��J�E���^�[
	g_bTutorialEnd          = false;					// ���[�h�̑J�ڏ�

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffTutorial->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(TUTORIAL_POS_X - TUTORIAL_WIDTH, TUTORIAL_POS_Y - TUTORIAL_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TUTORIAL_POS_X + TUTORIAL_WIDTH, TUTORIAL_POS_Y - TUTORIAL_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TUTORIAL_POS_X - TUTORIAL_WIDTH, TUTORIAL_POS_Y + TUTORIAL_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TUTORIAL_POS_X + TUTORIAL_WIDTH, TUTORIAL_POS_Y + TUTORIAL_HEIGHT, 0.0f);

	// rhw �̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffTutorial->Unlock();

	//------------------------------------------------------------------------------------------------------------------
	//	�g�p����\�[�X�t�@�C���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// �J�����̏�����
	InitCamera();

	// ���C�g�̏�����
	InitLight();

	// ���b�V���h�[���̏�����
	InitMeshDome();

	// ���b�V���V�����_�[�̏�����
	InitMeshCylinder();

	//// ���̏�����
	//InitField();

	//// �ǂ̏�����
	//InitWall();

	// ���b�V���t�B�[���h�̏�����
	InitMeshField();

	// ���b�V���E�H�[���̏�����
	InitMeshWall();

	// �e�̏�����
	InitShadow();

	// �G���̏�����
	InitWeed();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�̏�����
	InitEnemy();

	// �I�u�W�F�N�g�̏�����
	InitObject();

	// �A�C�e���̏�����
	InitItem();

	//// �r���{�[�h�̏�����
	//InitBillboard();

	// �e�̏�����
	InitBullet();

	// �����̏�����
	InitExplosion();

	// �a���̏�����
	InitSlash();

	// �����̏�����
	InitTornado();

	// �G�t�F�N�g�̏�����
	InitEffect();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// �̗̓Q�[�W�̏�����
	InitLife();

	// �^�C�}�[�̏�����
	InitTimer();

	// �X�R�A�̏�����
	InitScore();

	// �X�e�[�W�̃Z�b�g�A�b�v
	TxtSetStage();

	// �T�E���h�̍Đ�
	PlaySound(SOUND_LABEL_BGM_001);	// BGM (�`���[�g���A�����)
}

//======================================================================================================================
//	�`���[�g���A����ʂ̏I������
//======================================================================================================================
void UninitTutorial(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�`���[�g���A���̏I��
	//------------------------------------------------------------------------------------------------------------------
	// �e�N�X�`���̔j��
	for (int nCntTutorial = 0; nCntTutorial < LESSON_MAX; nCntTutorial++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureTutorial[nCntTutorial] != NULL)
		{ // �ϐ� (g_apTextureTutorial) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureTutorial[nCntTutorial]->Release();
			g_apTextureTutorial[nCntTutorial] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffTutorial != NULL)
	{ // �ϐ� (g_pVtxBuffTutorial) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffTutorial->Release();
		g_pVtxBuffTutorial = NULL;
	}

	//------------------------------------------------------------------------------------------------------------------
	//	�g�p�����\�[�X�t�@�C���̏I��
	//------------------------------------------------------------------------------------------------------------------
	// �J�����̏I��
	UninitCamera();

	// ���C�g�̏I��
	UninitLight();

	// ���b�V���h�[���̏I��
	UninitMeshDome();

	// ���b�V���V�����_�[�̏I��
	UninitMeshCylinder();

	//// ���̏I��
	//UninitField();

	//// �ǂ̏I��
	//UninitWall();

	// ���b�V���t�B�[���h�̏I��
	UninitMeshField();

	// ���b�V���E�H�[���̏I��
	UninitMeshWall();

	// �e�̏I��
	UninitShadow();

	// �v���C���[�̏I��
	UninitPlayer();

	// �G�̏I��
	UninitEnemy();

	// �G���̏I��
	UninitWeed();

	// �I�u�W�F�N�g�̏I��
	UninitObject();

	// �A�C�e���̏I��
	UninitItem();

	//// �r���{�[�h�̏I��
	//UninitBillboard();

	// �e�̏I��
	UninitBullet();

	// �����̏I��
	UninitExplosion();

	// �a���̏I��
	UninitSlash();

	// �����̏I��
	UninitTornado();

	// �G�t�F�N�g�̏I��
	UninitEffect();

	// �p�[�e�B�N���̏I��
	UninitParticle();

	// �̗̓Q�[�W�̏I��
	UninitLife();

	// �^�C�}�[�̏I��
	UninitTimer();

	// �X�R�A�̏I��
	UninitScore();

	// �T�E���h�̒�~
	StopSound();
}

//======================================================================================================================
//	�`���[�g���A����ʂ̍X�V����
//======================================================================================================================
void UpdateTutorial(void)
{
	if (g_bTutorialEnd == false)
	{ // �J�ڐݒ肪����Ă��Ȃ��ꍇ

		if (GetTime() == 0)
		{ // �^�C�}�[�� 0�ɂȂ����ꍇ

			// �J�ڐݒ肪���ꂽ��Ԃɂ���
			g_bTutorialEnd = true;

			// �Q�[����ʂ̏�Ԑݒ�
			SetTutorialState(TUTORIALSTATE_END, END_TUTORIAL_TIME);	// �I�����
		}
	}

	switch (g_nLessonState)
	{ // ���b�X�����Ƃ̏���
	case LESSON_00:	// ���b�X��0 (�ړ�)

		// ����

		// �����𔲂���
		break;

	case LESSON_01:	// ���b�X��1 (�W�����v)

		// ����

		// �����𔲂���
		break;

	case LESSON_02:	// ���b�X��2 (�U��)

		// ����

		// �����𔲂���
		break;

	case LESSON_03:	// ���b�X��3 (����)

		if (CheckNumEnemy() == 0)
		{ // �G�̑����� 0�̏ꍇ

			// ���b�X���̏�Ԃ̉��Z
			AddLessonState();
		}

		// �����𔲂���
		break;

	case LESSON_04:	// ���b�X��4 (�j��)

		if (CheckNumBreakObject() == 0)
		{ // ����I�u�W�F�N�g�̑����� 0�̏ꍇ

			// ���b�X���̏�Ԃ̉��Z
			AddLessonState();
		}

		// �����𔲂���
		break;

	case LESSON_05:	// ���b�X��5 (��)

		if (CheckNumItem() == 0)
		{ // �A�C�e���̑����� 0�̏ꍇ

			// ���b�X���̏�Ԃ̉��Z
			AddLessonState();
		}

		// �����𔲂���
		break;

	case LESSON_06:	// ���b�X��6 (����)

		// ����

		// �����𔲂���
		break;
	}

	switch (g_tutorialState)
	{ // ��Ԃ��Ƃ̏���
	case TUTORIALSTATE_NORMAL:		// �ʏ���

		// ����

		// �����𔲂���
		break;

	case TUTORIALSTATE_END:			// �I�����

		if (g_nCounterTutorialState > 0)
		{ // �J�E���^�[�� 0���傫���ꍇ

			// �J�E���^�[�����Z
			g_nCounterTutorialState--;
		}
		else
		{ // �J�E���^�[�� 0�ȉ��̏ꍇ

			// ���[�h�I�� (�^�C�g����ʂɈڍs)
			SetFade(MODE_TITLE);
		}

		// �����𔲂���
		break;
	}

	// ���C�g�̍X�V
	UpdateLight();

	// �J�����̍X�V
	UpdateCamera();

	// ���b�V���h�[���̍X�V
	UpdateMeshDome();

	// ���b�V���V�����_�[�̍X�V
	UpdateMeshCylinder();

	//// ���̍X�V
	//UpdateField();
	
	//// �ǂ̍X�V
	//UpdateWall();

	// ���b�V���t�B�[���h�̍X�V
	UpdateMeshField();

	// ���b�V���E�H�[���̍X�V
	UpdateMeshWall();

	// �v���C���[�̍X�V
	UpdatePlayer();

	// �G�̍X�V
	UpdateEnemy();

	// �G���̍X�V
	UpdateWeed();

	// �I�u�W�F�N�g�̍X�V
	UpdateObject();

	// �A�C�e���̍X�V
	UpdateItem();

	//// �r���{�[�h�̍X�V
	//UpdateBillboard();

	// �e�̍X�V
	UpdateBullet();

	// �����̍X�V
	UpdateExplosion();

	// �����̍X�V
	UpdateTornado();

	// �a���̍X�V
	UpdateSlash();

	// �G�t�F�N�g�̍X�V
	UpdateEffect();

	// �p�[�e�B�N���̍X�V
	UpdateParticle();

	// �̗̓Q�[�W�̍X�V
	UpdateLife();

	if (GetLessonState() >= LESSON_06)
	{ // ���b�X��6�ɒ��풆�A�܂��̓N���A���Ă���ꍇ

		// �^�C�}�[�̍X�V
		UpdateTimer();
	}

	// �X�R�A�̍X�V
	UpdateScore();

	// �e�̍X�V
	UpdateShadow();
}

//======================================================================================================================
//	�`���[�g���A����ʂ̕`�揈��
//======================================================================================================================
void DrawTutorial(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�g�p����\�[�X�t�@�C���̕`��
	//------------------------------------------------------------------------------------------------------------------
	// �J�����̐ݒ�
	SetCamera();

	// ���b�V���h�[���̕`��
	DrawMeshDome();

	// ���b�V���V�����_�[�̕`��
	DrawMeshCylinder();

	//// ���̕`��
	//DrawField();

	//// �ǂ̕`��
	//DrawWall();

	// ���b�V���t�B�[���h�̕`��
	DrawMeshField();

	// ���b�V���E�H�[���̕`��
	DrawMeshWall();

	// �e�̕`��
	DrawShadow();

	// �v���C���[�̕`��
	DrawPlayer();

	// �G�̕`��
	DrawEnemy();

	// �G���̕`��
	DrawWeed();

	// �I�u�W�F�N�g�̕`��
	DrawObject();

	// �A�C�e���̕`��
	DrawItem();

	//// �r���{�[�h�̕`��
	//DrawBillboard();

	// �G�t�F�N�g�̕`��
	DrawEffect();

	// �p�[�e�B�N���̕`��
	DrawParticle();

	// �e�̕`��
	DrawBullet();

	// �����̕`��
	DrawExplosion();

	// �����̕`��
	DrawTornado();

	// �a���̕`��
	DrawSlash();

	// �̗̓Q�[�W�̕`��
	DrawLife();

	if (GetLessonState() >= LESSON_06)
	{ // ���b�X��6�ɒ��풆�A�܂��̓N���A���Ă���ꍇ

		// �^�C�}�[�̕`��
		DrawTimer();
	}

	// �X�R�A�̕`��
	DrawScore();

	//------------------------------------------------------------------------------------------------------------------
	//	�`���[�g���A���̕`��
	//------------------------------------------------------------------------------------------------------------------
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffTutorial, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_apTextureTutorial[g_nLessonState]);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//======================================================================================================================
//	���b�X���̏�Ԃ̉��Z����
//======================================================================================================================
void AddLessonState(void)
{
	// �ϐ��z���錾
	int aNextLesson[LESSON_MAX] = 
	{ // �����l
		LESSON_00_CNT,	// ���b�X��00�̉��Z�^�C�~���O
		LESSON_01_CNT,	// ���b�X��01�̉��Z�^�C�~���O
		LESSON_02_CNT,	// ���b�X��02�̉��Z�^�C�~���O
		LESSON_03_CNT,	// ���b�X��03�̉��Z�^�C�~���O
		LESSON_04_CNT,	// ���b�X��04�̉��Z�^�C�~���O
		LESSON_05_CNT,	// ���b�X��05�̉��Z�^�C�~���O
		LESSON_06_CNT	// ���b�X��06�̉��Z�^�C�~���O
	};

	if (g_nLessonState < LESSON_MAX - 1)
	{ // ���݂̃��b�X�������b�X���̍ő吔 -1��菬�����ꍇ

		// ���b�X���J�E���^�[�����Z
		g_nCounterLessonState++;

		if (g_nCounterLessonState >= aNextLesson[g_nLessonState])
		{ // ���b�X���J�E���^�[�����̃��b�X���Ɉڍs����l�ȏ�ɂȂ����ꍇ

			// �J�E���^�[��������
			g_nCounterLessonState = 0;

			// ���b�X�������Z (���̃��b�X���ɐi�߂�)
			g_nLessonState++;

			switch (g_nLessonState)
			{ // ���b�X�����Ƃ̏���
			case LESSON_00:	// ���b�X��0 (�ړ�)

				// ����

				// �����𔲂���
				break;

			case LESSON_01:	// ���b�X��1 (�W�����v)

				// ����

				// �����𔲂���
				break;

			case LESSON_02:	// ���b�X��2 (�U��)

				// ����

				// �����𔲂���
				break;

			case LESSON_03:	// ���b�X��3 (����)

				// �G�̐ݒ�
				SetEnemy
				( // ����
					D3DXVECTOR3(SET_ENEMY_X_00, SET_ENEMY_Y_00, SET_ENEMY_Z_00),	// �ʒu
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_ENEMY_ROT_00), 0.0f),		// ����
					SET_ENEMY_TYPE_00												// ���
				);

				// �G�̐ݒ�
				SetEnemy
				( // ����
					D3DXVECTOR3(SET_ENEMY_X_01, SET_ENEMY_Y_01, SET_ENEMY_Z_01),	// �ʒu
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_ENEMY_ROT_01), 0.0f),		// ����
					SET_ENEMY_TYPE_01												// ���
				);

				// �G�̐ݒ�
				SetEnemy
				( // ����
					D3DXVECTOR3(SET_ENEMY_X_02, SET_ENEMY_Y_02, SET_ENEMY_Z_02),	// �ʒu
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_ENEMY_ROT_02), 0.0f),		// ����
					SET_ENEMY_TYPE_02												// ���
				);

				// �����𔲂���
				break;

			case LESSON_04:	// ���b�X��4 (�j��)

				// �I�u�W�F�N�g�̐ݒ�
				SetObject
				( // ����
					D3DXVECTOR3(SET_OBJECT_X_00, SET_OBJECT_Y_00, SET_OBJECT_Z_00),	// �ʒu
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_OBJECT_ROT_00), 0.0f),		// ����
					SET_OBJECT_BREAK_00,											// �����̎��
					SET_OBJECT_TYPE_00												// �I�u�W�F�N�g�̎��
				);

				// �I�u�W�F�N�g�̐ݒ�
				SetObject
				( // ����
					D3DXVECTOR3(SET_OBJECT_X_01, SET_OBJECT_Y_01, SET_OBJECT_Z_01),	// �ʒu
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_OBJECT_ROT_01), 0.0f),		// ����
					SET_OBJECT_BREAK_01,											// �����̎��
					SET_OBJECT_TYPE_01												// �I�u�W�F�N�g�̎��
				);

				// �I�u�W�F�N�g�̐ݒ�
				SetObject
				( // ����
					D3DXVECTOR3(SET_OBJECT_X_02, SET_OBJECT_Y_02, SET_OBJECT_Z_02),	// �ʒu
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_OBJECT_ROT_02), 0.0f),		// ����
					SET_OBJECT_BREAK_02,											// �����̎��
					SET_OBJECT_TYPE_02												// �I�u�W�F�N�g�̎��
				);

				// �����𔲂���
				break;

			case LESSON_05:	// ���b�X��5 (��)

				// ����

				// �����𔲂���
				break;

			case LESSON_06:	// ���b�X��6 (����)

				// ����

				// �����𔲂���
				break;
			}

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_DEC_00);	// SE (����00)
		}
	}
}

//======================================================================================================================
//	�`���[�g���A����ʂ̏�Ԑݒ菈��
//======================================================================================================================
void SetTutorialState(TUTORIALSTATE state, int nCounter)
{
	// �����̏�Ԃ���
	g_tutorialState = state;

	// �����̗]�C����
	g_nCounterTutorialState = nCounter;
}

//======================================================================================================================
//	���b�X���̏�Ԃ̎擾����
//======================================================================================================================
TUTORIALSTATE GetLessonState(void)
{
	// ���b�X���̏�Ԃ�Ԃ�
	return (TUTORIALSTATE)g_nLessonState;

}

//======================================================================================================================
//	���݂̓G�̑����̊m�F����
//======================================================================================================================
int CheckNumEnemy(void)
{
	// �ϐ���錾
	int nNumEnemy = 0;	// �G�̑����̊m�F�p

	// �|�C���^��錾
	Enemy *pEnemy = GetEnemy();		// �G�̏��

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (pEnemy->bUse == true)
		{ // �G���g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumEnemy++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumEnemy;	// �G�̑���
}

//======================================================================================================================
//	���݂̉���I�u�W�F�N�g�̑����̊m�F����
//======================================================================================================================
int CheckNumBreakObject(void)
{
	// �ϐ���錾
	int nNumObject = 0;	// ����I�u�W�F�N�g�̑����̊m�F�p

	// �|�C���^��錾
	Object *pObject = GetObjectData();	// �I�u�W�F�N�g�̏��

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{ // �I�u�W�F�N�g�̍ő�\�������J��Ԃ�

		if (pObject->bUse == true && pObject->nBreakType != BREAKTYPE_NONE)
		{ // �I�u�W�F�N�g���g�p����Ă��銎�A�I�u�W�F�N�g�����Ȃ��ݒ�ł͂Ȃ��ꍇ

			// �J�E���^�[�����Z
			nNumObject++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumObject;	// ����I�u�W�F�N�g�̑���
}

//======================================================================================================================
//	���݂̃A�C�e���̑����̊m�F����
//======================================================================================================================
int CheckNumItem(void)
{
	// �ϐ���錾
	int nNumItem = 0;	// �A�C�e���̑����̊m�F�p

	// �|�C���^��錾
	Item *pItem = GetItem();	// �A�C�e���̏��

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		if (pItem->bUse == true)
		{ // �A�C�e�����g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumItem++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumItem;	// �A�C�e���̑���
}