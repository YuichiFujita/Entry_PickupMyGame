//======================================================================================================================
//
//	�Q�[����ʏ��� [game.cpp]
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

#include "game.h"

#include "billboard.h"
#include "bullet.h"
#include "camera.h"
#include "continue.h"
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
#include "pause.h"
#include "player.h"
#include "ranking.h"
#include "result.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"
#include "timer.h"
#include "wall.h"
#include "weed.h"

#include "orbit.h"
#include "tornado.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_GAME		(1)		// �g�p����|���S����
#define RESULT_TIME		(40)	// ���U���g�܂ł̗]�C�t���[��

#define ADD_ALPHA_GAME	(0.04f)	// ���l�̉��Z��
#define MUL_FADE_ALPHA	(0.5f)	// �t�F�[�h�w�i�̓����x�̌��Z�{��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffGame = NULL;	// ���_�o�b�t�@�ւ̃|�C���^

GAMESTATE  g_gameState;			// �Q�[���̏��
Alpha      g_alphaGame;			// �����x�̏��
int        g_nCounterGameState;	// ��ԊǗ��J�E���^�[
bool       g_bPause;			// �|�[�Y��Ԃ� ON / OFF
bool       g_bGameEnd;			// ���[�h�̑J�ڏ�

//======================================================================================================================
//	�Q�[����ʂ̏���������
//======================================================================================================================
void InitGame(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�Q�[���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_2D        *pVtx;						// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_2D) * 4 * MAX_GAME,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffGame,
		NULL
	);

	// �O���[�o���ϐ���������
	g_gameState         = GAMESTATE_NORMAL;		// �Q�[���̏��
	g_nCounterGameState = RESULT_TIME;			// ��ԊǗ��J�E���^�[
	g_bPause            = false;				// �|�[�Y��Ԃ� ON / OFF
	g_bGameEnd          = false;				// ���[�h�̑J�ڏ�

	// �����x�̏�Ԃ�������
	g_alphaGame.state  = ALPHASTATE_WAIT;		// �����x�̕ύX���
	g_alphaGame.fAlpha = 0.0f;					// �w�i�̓����x

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffGame->Lock(0, 0, (void**)&pVtx, 0);

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
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffGame->Unlock();

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

	// �v���C���[�̏�����
	InitPlayer();

	// �G�̏�����
	InitEnemy();

	// �G���̏�����
	InitWeed();

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

	// �O�Ղ̏�����
	InitOrbit();

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

	// �|�[�Y�̏�����
	InitPause();

	// ���U���g�̏�����
	InitResult();

	// �����L���O�̏�����
	InitRanking();

	// �R���e�j���[�̏�����
	InitContinue();

	// �X�e�[�W�̃Z�b�g�A�b�v
	TxtSetStage();

	// �T�E���h�̍Đ�
	PlaySound(SOUND_LABEL_BGM_002);	// BGM (�Q�[�����)
}

//======================================================================================================================
//	�Q�[����ʂ̏I������
//======================================================================================================================
void UninitGame(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�Q�[���̏I��
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffGame != NULL)
	{ // �ϐ� (g_pVtxBuffGame) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffGame->Release();
		g_pVtxBuffGame = NULL;
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

	// �O�Ղ̏I��
	UninitOrbit();

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

	// �|�[�Y�̏I��
	UninitPause();

	// ���U���g�̏I��
	UninitResult();

	// �����L���O�̏I��
	UninitRanking();

	// �R���e�j���[�̏I��
	UninitContinue();

	// �T�E���h�̒�~
	StopSound();
}

//======================================================================================================================
//	�Q�[����ʂ̍X�V����
//======================================================================================================================
void UpdateGame(void)
{
	// �|�C���^��錾
	VERTEX_2D  *pVtx;						// ���_���ւ̃|�C���^
	Player *pPlayer = GetPlayer();		// �v���C���[�̏��

	if (g_bGameEnd == false)
	{ // �J�ڐݒ肪����Ă��Ȃ��ꍇ

		if (GetTime() == 0 || pPlayer->bUse == false)
		{ // �^�C�}�[�� 0�ɂȂ����A�������̓v���C���[�����S�����ꍇ

			// �J�ڐݒ肪���ꂽ��Ԃɂ���
			g_bGameEnd = true;

			// �g�[�^���X�R�A�̐ݒ�
			SetTotalScore();

			// �����L���O�̐ݒ�
			SetRanking();

			// �Q�[����ʂ̏�Ԑݒ�
			SetGameState(GAMESTATE_END, RESULT_TIME);	// �I�����

			if (GetTime() == 0)
			{ // �^�C�}�[�� 0�̏ꍇ

				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_RES_00);		// SE (���U���g�ڍs00)
			}
			else if (pPlayer->bUse == false)
			{ // �v���C���[�����S���Ă���ꍇ

				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_RES_01);		// SE (���U���g�ڍs01)
			}
		}
	}

	switch (g_gameState)
	{ // ��Ԃ��Ƃ̏���
	case GAMESTATE_NORMAL:		// �ʏ���

		if (GetFade() == FADE_NONE)
		{ // �t�F�[�h���Ă��Ȃ��ꍇ

			if (GetKeyboardTrigger(DIK_P) == true || GetJoyKeyTrigger(JOYKEY_START, 0) == true)
			{ // �|�[�Y�̑��삪�s��ꂽ�ꍇ

				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_DEC_01);				// SE (����01)

				// �|�[�Y�̑I�����ڂ̍Đݒ�
				ResetPauseSelect();

				// �ϐ� (g_bPause) �^�U�������� (�O�����Z�q)	// ���������^�Ȃ� true �A�U�Ȃ� false ����
				g_bPause = (g_bPause == false) ? true : false;	// �� �������F() �̒�
			}
		}

		// �����𔲂���
		break;

	case GAMESTATE_END:			// �I�����

		switch (g_alphaGame.state)
		{ // �����x�̕ύX��Ԃ��Ƃ̏���
		case ALPHASTATE_WAIT:	// �����x�̕ύX�ҋ@���

			if (g_nCounterGameState > 0)
			{ // �J�E���^�[�� 0���傫���ꍇ

				// �J�E���^�[�����Z
				g_nCounterGameState--;
			}
			else
			{ // �J�E���^�[�� 0�ȉ��̏ꍇ

				// �����x�̕ύX��Ԃɂ���
				g_alphaGame.state = ALPHASTATE_CHANGE;
			}

			// �����𔲂���
			break;

		case ALPHASTATE_CHANGE:	// �����x�̕ύX���

			if (g_alphaGame.fAlpha < 1.0f)
			{ // �s�����x�� 1.0f ��菬�����ꍇ

				// �s�����x���グ��
				g_alphaGame.fAlpha += ADD_ALPHA_GAME;
			}
			else
			{ // �s�����x�� 1.0f �ȏ�̏ꍇ

				// �s�����x��␳
				g_alphaGame.fAlpha = 1.0f;

				// �������Ȃ���Ԃɂ���
				g_alphaGame.state = ALPHASTATE_NONE;
			}

			// �����𔲂���
			break;

		case ALPHASTATE_NONE:	// �������Ă��Ȃ����

			// ����

			// �����𔲂���
			break;
		}

		// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		g_pVtxBuffGame->Lock(0, 0, (void**)&pVtx, 0);

		// ���_�J���[�̍X�V
		pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);
		pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);
		pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);
		pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);

		// ���_�o�b�t�@���A�����b�N����
		g_pVtxBuffGame->Unlock();

		// ���U���g�̍X�V
		UpdateResult(g_alphaGame.fAlpha);

		// �����L���O�̍X�V
		UpdateRanking(g_alphaGame.fAlpha);

		// �R���e�j���[�̍X�V
		UpdateContinue(g_alphaGame.fAlpha, g_alphaGame.state);

		// �����𔲂���
		break;
	}

	if (g_bPause == false)
	{ // �|�[�Y��Ԃł͂Ȃ��ꍇ

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

		if (g_gameState == GAMESTATE_NORMAL)
		{ // �Q�[����ʂ��ʏ��Ԃ̏ꍇ

			// �v���C���[�̍X�V
			UpdatePlayer();

			// �G�̍X�V
			UpdateEnemy();
		}

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

		// �a���̍X�V
		UpdateSlash();

		// �����̍X�V
		UpdateTornado();

		// �G�t�F�N�g�̍X�V
		UpdateEffect();

		// �p�[�e�B�N���̍X�V
		UpdateParticle();

		// �̗̓Q�[�W�̍X�V
		UpdateLife();

		// �^�C�}�[�̍X�V
		UpdateTimer();

		// �X�R�A�̍X�V
		UpdateScore();

		// �e�̍X�V
		UpdateShadow();

		// �O�Ղ̍X�V
		UpdateOrbit();
	}
	else
	{ // �|�[�Y��Ԃ̏ꍇ

		// �|�[�Y�̍X�V
		UpdatePause();
	}
}

//======================================================================================================================
//	�Q�[����ʂ̕`�揈��
//======================================================================================================================
void DrawGame(void)
{
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

	// �O�Ղ̕`��
	DrawOrbit();

	// �����̕`��
	DrawTornado();

	// �a���̕`��
	DrawSlash();

	// �̗̓Q�[�W�̕`��
	DrawLife();

	// �^�C�}�[�̕`��
	DrawTimer();

	// �X�R�A�̕`��
	DrawScore();

	if (g_bPause == true)
	{ // �|�[�Y��Ԃ̏ꍇ

		// �|�[�Y�̕`��
		DrawPause();
	}

	switch (g_gameState)
	{ // ��Ԃ��Ƃ̏���
	case GAMESTATE_NORMAL:

		// ����

		// �����𔲂���
		break;

	case GAMESTATE_END:

		//--------------------------------------------------------------------------------------------------------------
		//	�w�i�̕`��
		//--------------------------------------------------------------------------------------------------------------
		// �|�C���^��錾
		LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

		// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
		pDevice->SetStreamSource(0, g_pVtxBuffGame, 0, sizeof(VERTEX_2D));

		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_2D);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);

		// �|���S���̕`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//--------------------------------------------------------------------------------------------------------------
		//	���U���g�E�����L���O�E�R���e�j���[�̕`��
		//--------------------------------------------------------------------------------------------------------------
		// ���U���g�̕`��
		DrawResult(g_alphaGame.fAlpha);

		// �����L���O�̕`��
		DrawRanking(g_alphaGame.fAlpha);

		// �R���e�j���[�̕`��
		DrawContinue(g_alphaGame.fAlpha);

		// �����𔲂���
		break;
	}
}

//======================================================================================================================
//	�Q�[����ʂ̏�Ԑݒ菈��
//======================================================================================================================
void SetGameState(GAMESTATE state, int nCounter)
{
	// �����̏�Ԃ���
	g_gameState = state;

	// �����̗]�C����
	g_nCounterGameState = nCounter;
}

//======================================================================================================================
//	�|�[�Y��Ԃ̐ݒ菈��
//======================================================================================================================
void SetEnablePause(bool bPause)
{
	// ���݂̃|�[�Y��ԂɈ����̃|�[�Y��Ԃ���
	g_bPause = bPause;
}

//======================================================================================================================
//	�Q�[����ʂ̏�Ԏ擾����
//======================================================================================================================
GAMESTATE GetGameState(void)
{
	// �Q�[���̏�Ԃ�Ԃ�
	return g_gameState;
}

//======================================================================================================================
//	�|�[�Y��Ԃ̎擾����
//======================================================================================================================
bool GetPause(void)
{
	// �|�[�Y��Ԃ̐^�U��Ԃ�
	return g_bPause;
}