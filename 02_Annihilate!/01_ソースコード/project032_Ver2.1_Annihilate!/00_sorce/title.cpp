//======================================================================================================================
//
//	�^�C�g����ʏ��� [title.cpp]
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

#include "title.h"

#include "camera.h"
#include "light.h"
#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshwall.h"
#include "object.h"
#include "shadow.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_TITLE			(5)			// �g�p����|���S����
#define TITLE_MOVE			(1.2f)		// �^�C�g���̈ړ���
#define SUB_ALPHA_TITLE		(0.006f)	// ���l�̌��Z��
#define FIRST_ALPHA_TITLE	(0.85f)		// �^�C�g���̃t�F�[�h�̏������l

#define TITLE_POS_X			(640.0f)	// �^�C�g���̒�~���̐�΍��W (x)
#define TITLE_POS_Y			(220.0f)	// �^�C�g���̒�~���̐�΍��W (y)
#define TITLE_WIDTH			(600.0f)	// �^�C�g���̉��� / 2
#define TITLE_HEIGHT		(120.0f)	// �^�C�g���̏c�� / 2

#define TIT_SELECT_POS_X	(350.0f)	// �^�C�g���̑I�����̐�΍��W (x)
#define TIT_SELECT_POS_Y	(560.0f)	// �^�C�g���̑I�����̐�΍��W (y)
#define TIT_SELECT_SPACE	(580.0f)	// �^�C�g���̑I�����Ԃ̕� (x)
#define TIT_SELECT_WIDTH	(210.0f)	// �^�C�g���̑I�����̉��� / 2
#define TIT_SELECT_HEIGHT	(70.0f)		// �^�C�g���̑I�����̏c�� / 2

#define TIT_SEL_BG_HEIGHT	(60.0f)		// �^�C�g���̑I�����̔w�i�̏c�� / 2

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureTitle[] =	// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\title000.png",	// �^�C�g���̃e�N�X�`�����΃p�X
	NULL,									// NULL
	"02_data\\02_TEXTURE\\title001.png",	// �Q�[���J�n�̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\title002.png",	// �`���[�g���A���J�n�̃e�N�X�`�����΃p�X
	NULL,									// NULL
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_TITLE)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_TITLE_TITLE = 0,			// �^�C�g��
	TEXTURE_TITLE_NULL_00,				// NULL
	TEXTURE_TITLE_START_GAME,			// �Q�[���J�n
	TEXTURE_TITLE_START_TUTORIAL,		// �`���[�g���A���J�n
	TEXTURE_TITLE_NULL_01,				// NULL
	TEXTURE_TITLE_MAX,					// ���̗񋓌^�̑���
} TEXTURE_TITLE;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void SelectTitle(void);			// �^�C�g���̑��쏈��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureTitle[TEXTURE_TITLE_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle = NULL;						// ���_�o�b�t�@�ւ̃|�C���^

D3DXVECTOR3 g_posTitle;			// �^�C�g���̐�΍��W
TITLESTATE  g_stateTitle;		// �^�C�g���̏��
Alpha       g_alphaTitle;		// �����x�̏��
int         g_nSelectTitle;		// ���݂̑I�𒆃��j���[
int         g_nOldSelectTitle;	// �O��̑I�𒆃��j���[

//======================================================================================================================
//	�^�C�g����ʂ̏���������
//======================================================================================================================
void InitTitle(void)
{
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

	// ���b�V���t�B�[���h�̏�����
	InitMeshField();

	// ���b�V���E�H�[���̏�����
	InitMeshWall();

	// �e�̏�����
	InitShadow();

	// �I�u�W�F�N�g�̏�����
	InitObject();

	// �X�e�[�W�̃Z�b�g�A�b�v
	TxtSetStage();

	//------------------------------------------------------------------------------------------------------------------
	//	�^�C�g���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_2D         *pVtx;					// ���_���ւ̃|�C���^

	for (int nCntTitle = 0; nCntTitle < TEXTURE_TITLE_MAX; nCntTitle++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		// �e�N�X�`���̓ǂݍ���
		D3DXCreateTextureFromFile(pDevice, apTextureTitle[nCntTitle], &g_apTextureTitle[nCntTitle]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_2D) * 4 * MAX_TITLE,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL
	);

	// �O���[�o���ϐ���������
	g_posTitle        = D3DXVECTOR3(TITLE_POS_X, 0.0f, 0.0f);	// �^�C�g���̐�΍��W
	g_stateTitle      = TITLESTATE_MOVE;						// �^�C�g���̏��
	g_nSelectTitle    = TITLESELECT_GAME;						// ���݂̑I�𒆃��j���[
	g_nOldSelectTitle = TITLESELECT_GAME;						// �O��̑I�𒆃��j���[

	// �����x�̏�Ԃ�������
	g_alphaTitle.state  = ALPHASTATE_CHANGE;	// �����x�̕ύX���
	g_alphaTitle.fAlpha = FIRST_ALPHA_TITLE;	// �w�i�̓����x

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	//------------------------------------------------------------------------------------------------------------------
	//	�^�C�g���A�C�R���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);

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

	// ���_�f�[�^�̃|�C���^�� 4���i�߂�
	pVtx += 4;

	//------------------------------------------------------------------------------------------------------------------
	//	�Z���N�g�w�i�̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(0.0f,         TIT_SELECT_POS_Y - TIT_SEL_BG_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, TIT_SELECT_POS_Y - TIT_SEL_BG_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f,         TIT_SELECT_POS_Y + TIT_SEL_BG_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, TIT_SELECT_POS_Y + TIT_SEL_BG_HEIGHT, 0.0f);

	// rhw �̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�f�[�^�̃|�C���^�� 4���i�߂�
	pVtx += 4;

	//------------------------------------------------------------------------------------------------------------------
	//	�Z���N�g���j���[�̏�����
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntTitle = 0; nCntTitle < TITLESELECT_MAX; nCntTitle++)
	{ // �I�����̍��ڐ����J��Ԃ�

		// ���_���W��ݒ�
		pVtx[0].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) - TIT_SELECT_WIDTH, TIT_SELECT_POS_Y - TIT_SELECT_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) + TIT_SELECT_WIDTH, TIT_SELECT_POS_Y - TIT_SELECT_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) - TIT_SELECT_WIDTH, TIT_SELECT_POS_Y + TIT_SELECT_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) + TIT_SELECT_WIDTH, TIT_SELECT_POS_Y + TIT_SELECT_HEIGHT, 0.0f);

		// rhw �̐ݒ�
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[1].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[2].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[3].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	//------------------------------------------------------------------------------------------------------------------
	//	�t�F�[�h�̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(0.0f,         0.0f,          0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f,          0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f,         SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// rhw �̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffTitle->Unlock();
}

//======================================================================================================================
//	�^�C�g����ʂ̏I������
//======================================================================================================================
void UninitTitle(void)
{
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

	// ���b�V���t�B�[���h�̏I��
	UninitMeshField();

	// ���b�V���E�H�[���̏I��
	UninitMeshWall();

	// �e�̏I��
	UninitShadow();

	// �I�u�W�F�N�g�̏I��
	UninitObject();

	//------------------------------------------------------------------------------------------------------------------
	//	�^�C�g���̏I��
	//------------------------------------------------------------------------------------------------------------------
	// �e�N�X�`���̔j��
	for (int nCntTitle = 0; nCntTitle < TEXTURE_TITLE_MAX; nCntTitle++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureTitle[nCntTitle] != NULL)
		{ // �ϐ� (g_apTextureTitle) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureTitle[nCntTitle]->Release();
			g_apTextureTitle[nCntTitle] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffTitle != NULL)
	{ // �ϐ� (g_pVtxBuffTitle) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}

	//------------------------------------------------------------------------------------------------------------------
	//	�T�E���h�̏I��
	//------------------------------------------------------------------------------------------------------------------
	// �T�E���h�̒�~
	StopSound();
}

//======================================================================================================================
//	�^�C�g����ʂ̍X�V����
//======================================================================================================================
void UpdateTitle(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�g�p����\�[�X�t�@�C���̍X�V
	//------------------------------------------------------------------------------------------------------------------
	// ���C�g�̍X�V
	UpdateLight();

	// �J�����̍X�V
	UpdateCamera();

	// ���b�V���h�[���̍X�V
	UpdateMeshDome();

	// ���b�V���V�����_�[�̍X�V
	UpdateMeshCylinder();

	// ���b�V���t�B�[���h�̍X�V
	UpdateMeshField();

	// ���b�V���E�H�[���̍X�V
	UpdateMeshWall();

	// �e�̍X�V
	UpdateShadow();

	// �I�u�W�F�N�g�̍X�V
	UpdateObject();

	//------------------------------------------------------------------------------------------------------------------
	//	�^�C�g���̍X�V
	//------------------------------------------------------------------------------------------------------------------
	// �|�C���^��錾
	VERTEX_2D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	// �t�F�[�h�̓����x
	switch (g_alphaTitle.state)
	{ // �����x�̕ύX��Ԃ��Ƃ̏���
	case ALPHASTATE_WAIT:	// �����x�̕ύX�ҋ@���

		// ����

		// �����𔲂���
		break;

	case ALPHASTATE_CHANGE:	// �����x�̕ύX���

		if (g_alphaTitle.fAlpha > 0.0f)
		{ // �����x�� 0.0f ���傫���ꍇ

			// �����x���グ��
			g_alphaTitle.fAlpha -= SUB_ALPHA_TITLE;
		}
		else
		{ // �����x�� 0.0f �ȉ��̏ꍇ

			// �����x��␳
			g_alphaTitle.fAlpha = 0.0f;

			// �������Ȃ���Ԃɂ���
			g_alphaTitle.state = ALPHASTATE_NONE;
		}

		// �����𔲂���
		break;

	case ALPHASTATE_NONE:	// �������Ă��Ȃ����

		// ����

		// �����𔲂���
		break;
	}

	// �^�C�g���̓���
	switch (g_stateTitle)
	{ // ��Ԃ̊m�F
	case TITLESTATE_MOVE:	// �ړ���Ԃ̏ꍇ

		// �^�C�g���̐�΍��W���ړ� (��)
		g_posTitle.y += TITLE_MOVE;

		if (g_posTitle.y >= TITLE_POS_Y)
		{ // �^�C�g����������؂����ꍇ

			// y���W��␳
			g_posTitle.y = TITLE_POS_Y;

			// ��Ԃ��������Ȃ��ɐݒ�
			g_stateTitle = TITLESTATE_NONE;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_BGM_000);	// BGM (�^�C�g�����)
		}

		// �����𔲂���
		break;

	case TITLESTATE_NONE:	// �������Ă��Ȃ���Ԃ̏ꍇ

		// �^�C�g���̑��쏈��
		SelectTitle();

		// ���_�f�[�^�̃|�C���^��O�t���[���ɃZ���N�g���Ă����v�f�̒��_�Ɉړ�
		pVtx += (g_nOldSelectTitle * 4);

		// ���_�J���[�̍X�V (�Z���N�g���j���[)
		pVtx[8].col  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[9].col  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[10].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[11].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// ���_�f�[�^�̃|�C���^��擪�A�h���X�ɖ߂�
		pVtx -= (g_nOldSelectTitle * 4);

		// ���_�f�[�^�̃|�C���^�����t���[���ŃZ���N�g���Ă���v�f�̒��_�Ɉړ�
		pVtx += (g_nSelectTitle * 4);

		// ���_�J���[�̍X�V (�Z���N�g���j���[)
		pVtx[8].col  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[9].col  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[10].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[11].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ���_�f�[�^�̃|�C���^��擪�A�h���X�ɖ߂�
		pVtx -= (g_nSelectTitle * 4);

		// ���݃Z���N�g���̗v�f����
		g_nOldSelectTitle = g_nSelectTitle;

		// �����𔲂���
		break;
	}

	// ���o�̃X�L�b�v
	if (g_stateTitle != TITLESTATE_NONE && GetFade() == FADE_NONE)
	{ // �^�C�g�����������Ȃ���Ԃł͂Ȃ����A�t�F�[�h���Ă��Ȃ��ꍇ

		if (GetKeyboardTrigger(DIK_RETURN) == true || GetKeyboardTrigger(DIK_SPACE) == true
		||  GetJoyKeyTrigger(JOYKEY_A, 0)  == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true
		||  GetJoyKeyTrigger(JOYKEY_X, 0)  == true || GetJoyKeyTrigger(JOYKEY_Y, 0) == true)
		{ // �X�L�b�v���삪�s��ꂽ�ꍇ

			// �����x��␳
			g_alphaTitle.fAlpha = 0.0f;

			// �����x�ύX���������Ȃ���Ԃɂ���
			g_alphaTitle.state = ALPHASTATE_NONE;

			// y���W��␳
			g_posTitle.y = TITLE_POS_Y;

			// ��Ԃ��������Ȃ��ɐݒ�
			g_stateTitle = TITLESTATE_NONE;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_BGM_000);	// BGM (�^�C�g�����)
		}
	}

	// ���_���W���X�V (�^�C�g���A�C�R��)
	pVtx[0].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);

	// ���_�J���[�̍X�V (�t�F�[�h)
	pVtx[16].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);
	pVtx[17].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);
	pVtx[18].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);
	pVtx[19].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffTitle->Unlock();
}

//======================================================================================================================
//	�^�C�g����ʂ̕`�揈��
//======================================================================================================================
void DrawTitle(void)
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

	// ���b�V���t�B�[���h�̕`��
	DrawMeshField();

	// ���b�V���E�H�[���̕`��
	DrawMeshWall();

	// �e�̕`��
	DrawShadow();

	// �I�u�W�F�N�g�̕`��
	DrawObject();

	//------------------------------------------------------------------------------------------------------------------
	//	�^�C�g���̕`��
	//------------------------------------------------------------------------------------------------------------------
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{ // �g�p����|���S�������J��Ԃ�

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, g_apTextureTitle[nCntTitle]);

		// �|���S���̕`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTitle * 4, 2);
	}
}

//======================================================================================================================
//	�^�C�g����ʂ̏�Ԃ̎擾����
//======================================================================================================================
TITLESTATE GetTitleState(void)
{
	// �^�C�g���̏�Ԃ�Ԃ�
	return g_stateTitle;
}

//======================================================================================================================
//	�^�C�g���̑��쏈��
//======================================================================================================================
void SelectTitle(void)
{
	if (GetFade() == FADE_NONE)
	{ // �t�F�[�h���Ă��Ȃ��ꍇ

		if (GetKeyboardTrigger(DIK_A) == true || GetKeyboardTrigger(DIK_LEFT) == true || GetJoyKeyTrigger(JOYKEY_LEFT, 0) == true)
		{ // ���ړ��̑��삪�s��ꂽ�ꍇ

			// �ϐ� (g_nSelectTitle) �� 1�������Ă��� ( 0�̏ꍇ 1�ɖ߂�)
			g_nSelectTitle = (g_nSelectTitle + (TITLESELECT_MAX - 1)) % TITLESELECT_MAX;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (�J�[�\���ړ�)
		}
		else if (GetKeyboardTrigger(DIK_D) == true || GetKeyboardTrigger(DIK_RIGHT) == true || GetJoyKeyTrigger(JOYKEY_RIGHT, 0) == true)
		{ // �E�ړ��̑��삪�s��ꂽ�ꍇ

			// �ϐ� (g_nSelectTitle) �� 1�������Ă��� ( 1�̏ꍇ 0�ɖ߂�)
			g_nSelectTitle = (g_nSelectTitle + 1) % TITLESELECT_MAX;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (�J�[�\���ړ�)
		}

		if (GetKeyboardTrigger(DIK_RETURN) == true || GetKeyboardTrigger(DIK_SPACE) == true
		||  GetJoyKeyTrigger(JOYKEY_A, 0)  == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true
		||  GetJoyKeyTrigger(JOYKEY_X, 0)  == true || GetJoyKeyTrigger(JOYKEY_Y, 0) == true)
		{ // ����̑��삪�s��ꂽ�ꍇ

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_DEC_01);	// SE (����01)

			switch (g_nSelectTitle)
			{ // ��ʂ̈ڍs��̊m�F
			case TITLESELECT_GAME:		// ���g���C�̏ꍇ

				// ���[�h�I�� (�Q�[����ʂɈڍs)
				SetFade(MODE_GAME);

				// �����𔲂���
				break;

			case TITLESELECT_TUTORIAL:		// �I���̏ꍇ

				// ���[�h�I�� (�`���[�g���A����ʂɈڍs)
				SetFade(MODE_TUTORIAL);

				// �����𔲂���
				break;
			}
		}
	}
}