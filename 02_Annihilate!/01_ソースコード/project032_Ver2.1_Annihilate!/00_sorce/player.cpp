//======================================================================================================================
//
//	�v���C���[���� [player.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "input.h"
#include "tutorial.h"
#include "game.h"

#include "bullet.h"
#include "camera.h"
#include "enemy.h"
#include "explosion.h"
#include "life.h"
#include "object.h"
#include "particle.h"
#include "player.h"
#include "shadow.h"
#include "slash.h"
#include "sound.h"

#include "orbit.h"
#include "tornado.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define PLAYER_SETUP_TXT	"02_data\\00_TXT\\player.txt"	// �v���C���[�Z�b�g�A�b�v�p�̃e�L�X�g�t�@�C���̑��΃p�X

#define MOVE_PLAYER			(1.2f)		// �v���C���[�̈ړ���
#define DEL_MOVE_PLAYER		(0.01f)		// �v���C���[�̈ړ��ʂ��c��Œ�l
#define SUB_MOVE_PLAYER		(0.25f)		// �v���C���[�̈ړ��ʂ̌����W��
#define REV_ROT_PLAYER		(0.15f)		// �v���C���[�̉�]�ʂ̕␳�W��

#define PLAY_GRAVITY		(0.75f)		// �v���C���[�ɂ�����d��
#define PLAY_JUMP			(14.5f)		// �v���C���[�̃W�����v��

#define KICK_DMG_PLAY		(75)		// �L�b�N�̍U���� (�v���C���[)
#define PLAY_KICK_RADIUS	(28.0f)		// �v���C���[�̃L�b�N�̔��a
#define PLAY_KICK_HEIGHT	(4.0f)		// �v���C���[�̃L�b�N�̏c�� / 2
#define PLAY_KICK_JUMP		(8.0f)		// �v���C���[�̃L�b�N�������̒��˕Ԃ��

#define POS_BULL_PLAYER_X	(30.0f)		// �v���C���[�̒e���ˈʒu�̉��Z�� (x)
#define POS_BULL_PLAYER_Y	(30.0f)		// �v���C���[�̒e���ˈʒu�̉��Z�� (y)
#define POS_BULL_PLAYER_Z	(30.0f)		// �v���C���[�̒e���ˈʒu�̉��Z�� (z)

#define PLAY_SLASH_RADIUS	(85.0f)		// �v���C���[�̎a���̔��a
#define PLAY_SLASH_POS_Y	(40.0f)		// �v���C���[�̎a���ʒu�̉��Z�� (y)

#define SHADOW_PLAYER		(35.0f)		// �v���C���[�̉e�̔��a
#define SUB_MOTION_STOP		(2)			// ���[�v���Ȃ����[�V�����̒�~�J�E���g�̌��Z�p

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apModelPlayer[] =			// ���f���̑��΃p�X
{
	"02_data\\03_MODEL\\02_PLAYER\\body.x",		// �̂̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\head.x",		// ���̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\sleeve_l.x",	// ���r�̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\sleeve_r.x",	// �E�r�̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\wrist_l.x",	// �����̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\wrist_r.x",	// �E���̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\hand_l.x",	// ����̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\hand_r.x",	// �E��̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\pants_l.x",	// �����̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\pants_r.x",	// �E���̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\leg_l.x",	// ���ڂ̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\leg_r.x",	// �E�ڂ̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\shoes_l.x",	// ���C�̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\shoes_r.x",	// �E�C�̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\knife.x",	// ���� (�i�C�t) �̃��f�����΃p�X
	"02_data\\03_MODEL\\02_PLAYER\\gun.x",		// ���� (�e) �̃��f�����΃p�X
};

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void TutorialPlayer(void);						// �`���[�g���A���̃v���C���[�̍X�V����
void GamePlayer(void);							// �Q�[���̃v���C���[�̍X�V����

MOTIONTYPE MovePlayer(void);					// �v���C���[�̈ړ��ʂ̍X�V����
MOTIONTYPE JumpPlayer(void);					// �v���C���[�̃W�����v�̍X�V����
void PosPlayer(void);							// �v���C���[�̈ʒu�̍X�V����
void RotPlayer(void);							// �v���C���[�̌����̍X�V����
void RevPlayer(void);							// �v���C���[�̕␳�̍X�V����

void CollisionKickEnemy(Player *pPlayer);		// �v���C���[�̃L�b�N�̓����蔻��
void UpdateMotion(void);						// ���[�V�����̍X�V����
void SetMotion(MOTIONTYPE type);				// ���[�V�����̐ݒ菈��
void TxtSetPlayer(void);						// �v���C���[�̃Z�b�g�A�b�v����

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
Model  g_aModelPlayer[MODEL_PLAYER_MAX];		// �v���C���[�̃��f�����
Player g_player;								// �v���C���[�̖{�̏��

//======================================================================================================================
//	�v���C���[�̏���������
//======================================================================================================================
void InitPlayer(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	D3DXMATERIAL      *pMat;					// �}�e���A���ւ̃|�C���^

	// �v���C���[�̃��f�����̏�����
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // �v���C���[�Ɏg�p���郂�f���̍ő吔���J��Ԃ�

		g_aModelPlayer[nCntPlayer].apTexture[MAX_MODEL_TEXTURE] = {};						// �e�N�X�`���ւ̃|�C���^
		g_aModelPlayer[nCntPlayer].pMesh    = NULL;											// ���b�V�� (���_���) �ւ̃|�C���^
		g_aModelPlayer[nCntPlayer].pBuffMat = NULL;											// �}�e���A���ւ̃|�C���^
		g_aModelPlayer[nCntPlayer].dwNumMat = 0;											// �}�e���A���̐�
		g_aModelPlayer[nCntPlayer].vtxMin   = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// �ŏ��̒��_���W
		g_aModelPlayer[nCntPlayer].vtxMax   = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// �ő�̒��_���W
		g_aModelPlayer[nCntPlayer].fHeight  = 0.0f;											// ����
		g_aModelPlayer[nCntPlayer].fRadius  = 0.0f;											// ���a
	}

	// �v���C���[�̖{�̏��̏�����
	g_player.pos           = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���݂̈ʒu
	g_player.oldPos        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �O��̈ʒu
	g_player.move          = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ړ���
	g_player.rot           = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���݂̌���
	g_player.destRot       = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ڕW�̌���
	g_player.state         = ACTIONSTATE_NORMAL;			// �v���C���[�̏��
	g_player.nLife         = PLAY_LIFE;						// �̗�
	g_player.nCounterState = 0;								// ��ԊǗ��J�E���^�[
	g_player.nShadowID     = NONE_SHADOW;					// �e�̃C���f�b�N�X
	g_player.nSlashID      = NONE_SLASH;					// �a���̃C���f�b�N�X
	g_player.nTornadoID    = NONE_IDX;						// �����̃C���f�b�N�X
	g_player.bJump         = false;							// �W�����v��
	g_player.bKick         = false;							// �L�b�N��
	g_player.bUse          = false;							// �g�p��

	// ���[�V�������̏�����
	g_player.motion.type     = MOTIONTYPE_NONE;	// ���[�V�����̎��
	g_player.motion.nPose    = 0;				// ���[�V�����̃|�[�Y�ԍ�
	g_player.motion.nCounter = 0;				// ���[�V�����̃J�E���^�[

	// �p�[�c���̏�����
	for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
	{ // �v���C���[�̃p�[�c�̍ő吔���J��Ԃ�

		g_player.aParts[nCntParts].initPos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �����̈ʒu
		g_player.aParts[nCntParts].pos        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���݂̈ʒu
		g_player.aParts[nCntParts].initRot    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �����̌���
		g_player.aParts[nCntParts].rot        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���݂̌���
		g_player.aParts[nCntParts].pModelData = &g_aModelPlayer[nCntParts];		// ���f�����
		g_player.aParts[nCntParts].nParentID  = NONE_PLAYER_PARENT;				// �v���C���[�̐e�̃C���f�b�N�X
	}

	// �v���C���[�̃Z�b�g�A�b�v
	TxtSetPlayer();

	// ���[�V�����̐ݒ�
	SetMotion(MOTIONTYPE_NONE);

	// x�t�@�C���̓ǂݍ���
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // �v���C���[�Ɏg�p���郂�f���̍ő吔���J��Ԃ�

		D3DXLoadMeshFromX
		( // ����
			apModelPlayer[nCntPlayer],				// ���f�� (�p�[�c) �̑��΃p�X
			D3DXMESH_SYSTEMMEM,
			pDevice,								// �f�o�C�X�ւ̃|�C���^
			NULL,
			&g_aModelPlayer[nCntPlayer].pBuffMat,	// �}�e���A���ւ̃|�C���^
			NULL,
			&g_aModelPlayer[nCntPlayer].dwNumMat,	// �}�e���A���̐�
			&g_aModelPlayer[nCntPlayer].pMesh		// ���b�V�� (���_���) �ւ̃|�C���^
		);
	}

	// �e�N�X�`���̓ǂݍ���
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // �v���C���[�Ɏg�p���郂�f���̍ő吔���J��Ԃ�

		// �}�e���A�����ɑ΂���|�C���^���擾
		pMat = (D3DXMATERIAL*)g_aModelPlayer[nCntPlayer].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aModelPlayer[nCntPlayer].dwNumMat; nCntMat++)
		{ // �}�e���A���̐����J��Ԃ�

			if (pMat[nCntMat].pTextureFilename != NULL)
			{ // �e�N�X�`���t�@�C�������݂���ꍇ

				// �e�N�X�`���̓ǂݍ���
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aModelPlayer[nCntPlayer].apTexture[nCntMat]);
			}
		}
	}
}

//======================================================================================================================
//	�v���C���[�̏I������
//======================================================================================================================
void UninitPlayer(void)
{
	// �e�N�X�`���̔j��
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // �v���C���[�Ɏg�p���郂�f���̍ő吔���J��Ԃ�

		for (int nCntMat = 0; nCntMat < TEXTURE_PLAYER_MAX; nCntMat++)
		{ // �g�p����e�N�X�`�������J��Ԃ�

			if (g_aModelPlayer[nCntPlayer].apTexture[nCntMat] != NULL)
			{ // �ϐ� (g_aModelPlayer[nCntPlayer].apTexture) ��NULL�ł͂Ȃ��ꍇ

				g_aModelPlayer[nCntPlayer].apTexture[nCntMat]->Release();
				g_aModelPlayer[nCntPlayer].apTexture[nCntMat] = NULL;
			}
		}
	}

	// ���b�V���̔j��
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // �v���C���[�Ɏg�p���郂�f���̍ő吔���J��Ԃ�

		if (g_aModelPlayer[nCntPlayer].pMesh != NULL)
		{ // �ϐ� (g_aModelPlayer[nCntPlayer].pMesh) ��NULL�ł͂Ȃ��ꍇ

			g_aModelPlayer[nCntPlayer].pMesh->Release();
			g_aModelPlayer[nCntPlayer].pMesh = NULL;
		}
	}

	// �}�e���A���̔j��
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // �v���C���[�Ɏg�p���郂�f���̍ő吔���J��Ԃ�

		if (g_aModelPlayer[nCntPlayer].pBuffMat != NULL)
		{ // �ϐ� (g_aModelPlayer[nCntPlayer].pBuffMat) ��NULL�ł͂Ȃ��ꍇ

			g_aModelPlayer[nCntPlayer].pBuffMat->Release();
			g_aModelPlayer[nCntPlayer].pBuffMat = NULL;
		}
	}
}

//======================================================================================================================
//	�v���C���[�̍X�V����
//======================================================================================================================
void UpdatePlayer(void)
{
	switch (GetMode())
	{ // ���[�h���Ƃ̍X�V
	case MODE_TUTORIAL:	// �`���[�g���A�����

		// �`���[�g���A���̃v���C���[�̍X�V
		TutorialPlayer();

		// �����𔲂���
		break;

	case MODE_GAME:		// �Q�[�����

		// �Q�[���̃v���C���[�̍X�V
		GamePlayer();

		// �����𔲂���
		break;
	}
}

//======================================================================================================================
//	�v���C���[�̕`�揈��
//======================================================================================================================
void DrawPlayer(void)
{
	// �ϐ���錾
	D3DXMATRIX   mtxRot, mtxTrans;				// �v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;						// ���݂̃}�e���A���ۑ��p
	D3DXMATRIX   mtxRotParts, mtxTransParts;	// �K�w�\���̌v�Z�p�}�g���b�N�X
	D3DXMATRIX   mtxParent;						// �e�̃}�g���b�N�X

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	D3DXMATERIAL     *pMat;						// �}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATERIAL      redMat;					// �}�e���A���f�[�^ (��)

	if (g_player.bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_player.mtxWorld);

		// �����𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_player.rot.y, g_player.rot.x, g_player.rot.z);
		D3DXMatrixMultiply(&g_player.mtxWorld, &g_player.mtxWorld, &mtxRot);

		// �ʒu�𔽉f
		D3DXMatrixTranslation(&mtxTrans, g_player.pos.x, g_player.pos.y, g_player.pos.z);
		D3DXMatrixMultiply(&g_player.mtxWorld, &g_player.mtxWorld, &mtxTrans);

		// ���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &g_player.mtxWorld);

		// ���݂̃}�e���A�����擾
		pDevice->GetMaterial(&matDef);

		for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
		{ // �v���C���[�̃p�[�c�̍ő吔���J��Ԃ�

			// �p�[�c�̃��[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_player.aParts[nCntParts].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRotParts, g_player.aParts[nCntParts].rot.y, g_player.aParts[nCntParts].rot.x, g_player.aParts[nCntParts].rot.z);
			D3DXMatrixMultiply(&g_player.aParts[nCntParts].mtxWorld, &g_player.aParts[nCntParts].mtxWorld, &mtxRotParts);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTransParts, g_player.aParts[nCntParts].pos.x, g_player.aParts[nCntParts].pos.y, g_player.aParts[nCntParts].pos.z);
			D3DXMatrixMultiply(&g_player.aParts[nCntParts].mtxWorld, &g_player.aParts[nCntParts].mtxWorld, &mtxTransParts);

			// �p�[�c�̐e�}�g���b�N�X��ݒ�
			if (g_player.aParts[nCntParts].nParentID == NONE_PLAYER_PARENT)
			{ // ���݂̃p�[�c�ɐe�����݂��Ȃ��ꍇ

				// �v���C���[�̃}�g���b�N�X��ݒ�
				mtxParent = g_player.mtxWorld;
			}
			else
			{ // ���݂̃p�[�c�ɐe�����݂���ꍇ

				// �e�̃}�g���b�N�X��ݒ�
				mtxParent = g_player.aParts[g_player.aParts[nCntParts].nParentID].mtxWorld;
			}

			// �p�[�c�̃��[���h�}�g���b�N�X�Ɛe�}�g���b�N�X���|�����킹��
			D3DXMatrixMultiply(&g_player.aParts[nCntParts].mtxWorld, &g_player.aParts[nCntParts].mtxWorld, &mtxParent);

			// �p�[�c�̃��[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_player.aParts[nCntParts].mtxWorld);

			// �}�e���A���f�[�^�ւ̃|�C���^���擾
			pMat = (D3DXMATERIAL*)g_player.aParts[nCntParts].pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_player.aParts[nCntParts].pModelData->dwNumMat; nCntMat++)
			{ // �}�e���A���̐����J��Ԃ�

				switch (g_player.state)
				{ // ��Ԃ��Ƃ̏���
				case ACTIONSTATE_DAMAGE:	// �_���[�W���

					// �\���̗̂v�f���N���A
					ZeroMemory(&redMat, sizeof(D3DXMATERIAL));

					// �g�U���E�����E���Ȕ�����Ԃɂ���
					redMat.MatD3D.Diffuse  = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
					redMat.MatD3D.Ambient  = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
					redMat.MatD3D.Emissive = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

					// �}�e���A���̐ݒ�
					pDevice->SetMaterial(&redMat.MatD3D);			// ��

					// �����𔲂���
					break;

				default:					// ����ȊO�̏��

					// �}�e���A���̐ݒ�
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);	// �ʏ�

					// �����𔲂���
					break;
				}

				// �e�N�X�`���̐ݒ�
				pDevice->SetTexture(0, g_player.aParts[nCntParts].pModelData->apTexture[nCntMat]);

				// �p�[�c�̕`��
				g_player.aParts[nCntParts].pModelData->pMesh->DrawSubset(nCntMat);
			}
		}

		// �ۑ����Ă����}�e���A����߂�
		pDevice->SetMaterial(&matDef);
	}
}

//======================================================================================================================
//	�v���C���[�̈ʒu�E�����̐ݒ菈��
//======================================================================================================================
void SetPositionPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// ��������
	g_player.pos     = pos;		// ���݂̈ʒu
	g_player.oldPos  = pos;		// �O��̈ʒu
	g_player.rot     = rot;		// ���݂̌���
	g_player.destRot = rot;		// �ڕW�̌���

	// �e�̃C���f�b�N�X��ݒ�
	g_player.nShadowID = SetShadow(0.5f, SHADOW_PLAYER, &g_player.nShadowID, &g_player.bUse);

	// �e�̈ʒu�ݒ�
	SetPositionShadow(g_player.nShadowID, g_player.pos);

	// �̗͂�ݒ�
	switch (GetMode())
	{ // ���[�h���Ƃ̐ݒ�
	case MODE_TUTORIAL:	// �`���[�g���A�����

		// �ő�̗͂̔�������
		g_player.nLife = (int)(PLAY_LIFE * 0.5f);

		// �̗̓o�[�̐ݒ�
		SetLife(g_player.nLife, 0);

		// �����𔲂���
		break;

	default:			// ����ȊO�̉��

		// �ő�̗͂���
		g_player.nLife = PLAY_LIFE;

		// �̗̓o�[�̐ݒ�
		SetLife(g_player.nLife, 0);

		// �����𔲂���
		break;
	}

	// �g�p���Ă����Ԃɂ���
	g_player.bUse = true;
}

//======================================================================================================================
//	�v���C���[�̉񕜔���
//======================================================================================================================
void HealPlayer(Player *pPlayer, int nHeal)
{
	if (pPlayer->state == ACTIONSTATE_NORMAL)
	{ // �v���C���[���ʏ��Ԃ̏ꍇ

		// �̗̓o�[�̐ݒ�
		SetLife(pPlayer->nLife, nHeal);

		// �����̉񕜕���̗͂ɉ��Z
		pPlayer->nLife += nHeal;

		if (pPlayer->nLife > PLAY_LIFE)
		{ // �̗͂��ő�̗͂�葽���Ȃ����ꍇ

			// �̗͂�␳
			pPlayer->nLife = PLAY_LIFE;
		}

		// �T�E���h�̍Đ�
		PlaySound(SOUND_LABEL_SE_HEAL);		// SE (��)
	}
}

//======================================================================================================================
//	�v���C���[�̃_���[�W����
//======================================================================================================================
void HitPlayer(Player *pPlayer, int nDamage)
{
	if (pPlayer->state == ACTIONSTATE_NORMAL)
	{ // �v���C���[���ʏ��Ԃ̏ꍇ

		// �̗̓o�[�̐ݒ�
		SetLife(pPlayer->nLife, -nDamage);

		// �����̃_���[�W����̗͂��猸�Z
		pPlayer->nLife -= nDamage;

		if (pPlayer->nLife > 0)
		{ // �̗͂��c���Ă���ꍇ

			// �_���[�W��Ԃɂ���
			pPlayer->state = ACTIONSTATE_DAMAGE;

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pPlayer->pos,						// �ʒu
				D3DXCOLOR(0.8f, 0.0f, 0.3f, 1.0f),	// �F
				PARTICLETYPE_DAMAGE,				// ���
				SPAWN_PARTICLE_DAMAGE,				// �G�t�F�N�g��
				2									// ����
			);

			// �J�E���^�[��ݒ�
			pPlayer->nCounterState = DAMAGE_TIME_PLAY;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_DMG);			// SE (�_���[�W)
		}
		else
		{ // �̗͂��s�����ꍇ

			// �����̐ݒ�
			SetExplosion(pPlayer->pos, SOUNDTYPE_EXPLOSION);

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pPlayer->pos,						// �ʒu
				D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),	// �F
				PARTICLETYPE_EXPLOSION,				// ���
				SPAWN_PARTICLE_EXPLOSION,			// �G�t�F�N�g��
				2									// ����
			);

			// �g�p���Ă��Ȃ���Ԃɂ���
			pPlayer->bUse = false;
		}
	}
}

//======================================================================================================================
//	�`���[�g���A���̃v���C���[�̍X�V����
//======================================================================================================================
void TutorialPlayer(void)
{
	// �ϐ���錾
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// ���݂̃��[�V����
	MOTIONTYPE lessonMotion  = MOTIONTYPE_NONE;	// �`���[�g���A���̃��b�X�����Z�p

	if (g_player.bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �O��ʒu�̍X�V
		g_player.oldPos = g_player.pos;

		// ��Ԃ̍X�V
		if (g_player.nCounterState > 0)
		{ // �J�E���^�[�� 0���傫���ꍇ

			// �J�E���^�[�����Z
			g_player.nCounterState--;

			if (g_player.nCounterState == UNR_TIME_PLAY)
			{ // �J�E���^�[�����l�̏ꍇ

				// ���G��Ԃɂ���
				g_player.state = ACTIONSTATE_UNRIVALED;
			}
			else if (g_player.nCounterState <= 0)
			{ // �J�E���^�[�� 0�ȉ��̏ꍇ

				// �ʏ��Ԃɂ���
				g_player.state = ACTIONSTATE_NORMAL;
			}
		}

		// �v���C���[�̈ړ��ʂ̍X�V
		currentMotion = MovePlayer();

		if (GetLessonState() >= LESSON_01)
		{ // ���b�X��1�ɒ��풆�A�܂��̓N���A���Ă���ꍇ

			// �v���C���[�̃W�����v�̍X�V
			lessonMotion = JumpPlayer();
		}

		// �v���C���[�̈ʒu�̍X�V
		PosPlayer();

		// �v���C���[�̌����̍X�V
		RotPlayer();

		// �v���C���[�̕␳�̍X�V����
		RevPlayer();

		// �W�����v����
		if (g_player.pos.y < GetLimitStage().fField)
		{ // �n�ʂɓ������Ă���ꍇ

			// �W�����v���Ă��Ȃ���Ԃɂ���
			g_player.bJump = false;

			// �L�b�N���Ă��Ȃ���Ԃɂ���
			g_player.bKick = false;

			// �����̐����ݒ�
			SetGrowTornado
			( // ����
				g_player.nTornadoID,	// �ύX�C���f�b�N�X
				0.6f,	// �����̕ύX��
				0.012f,	// ������̐�����
				0.012f,	// �c����̐�����
				0.032f	// �����x�̐�����
			);

			// �n�ʂɈʒu��␳
			g_player.pos.y = GetLimitStage().fField;

			// �ړ��ʂ�������
			g_player.move.y = 0.0f;
		}
		else
		{ // �n�ʂɓ������Ă��Ȃ��ꍇ

			// �W�����v���Ă����Ԃɂ���
			g_player.bJump = true;
		}

		// �I�u�W�F�N�g�Ƃ̓����蔻��
		CollisionObject
		( // ����
			&g_player.pos,		// ���݂̈ʒu
			&g_player.oldPos,	// �O��̈ʒu
			PLAY_WIDTH,			// ����
			PLAY_DEPTH			// ���s
		);

		// �v���C���[�̃L�b�N�̓����蔻��
		CollisionKickEnemy(&g_player);

		if (GetLessonState() >= LESSON_02)
		{ // ���b�X��2�ɒ��풆�A�܂��̓N���A���Ă���ꍇ

			// �a���̍X�V
			if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true)
			{ // �U���̑��삪�����ꂽ�ꍇ

				if (g_player.nSlashID == NONE_SLASH && g_player.motion.type != MOTIONTYPE_ATTACK)
				{ // �a���̃C���f�b�N�X������������Ă��銎�A���݂̃��[�V�������U�����[�V�����ł͂Ȃ��ꍇ

					// ���݂̃��[�V�����ɍU����Ԃ���
					currentMotion = MOTIONTYPE_ATTACK;

					// �p�[�e�B�N���̐ݒ�
					SetParticle
					( // ����
						g_player.pos,						// �ʒu
						D3DXCOLOR(0.8f, 0.3f, 0.35f, 1.0f),	// �F
						PARTICLETYPE_SLASH,					// ���
						SPAWN_PARTICLE_SLASH,				// �G�t�F�N�g��
						4									// ����
					);

					// �a���̐ݒ�
					g_player.nSlashID = SetSlash
					( // ����
						SLASHTYPE_PLAYER,	// ���
						PLAY_SLASH_LIFE,	// ����
						PLAY_SLASH_RADIUS,	// ���a
						&g_player.nSlashID,	// �a���̐e�̎a���C���f�b�N�X
						&g_player.bUse		// �a���̐e�̎g�p��
					);
				}
			}

			// �a���̈ʒu�E�����̐ݒ�
			SetPositionSlash
			( // ����
				g_player.nSlashID,	// �a���̃C���f�b�N�X
				D3DXVECTOR3
				( // ����
					g_player.pos.x,						// �ʒu (x)
					g_player.pos.y + PLAY_SLASH_POS_Y,	// �ʒu (y)
					g_player.pos.z						// �ʒu (z)
				),
				g_player.rot		// ����
			);
		}

		//--------------------------------------------------------------------------------------------------------------
		//	�e�̍X�V
		//--------------------------------------------------------------------------------------------------------------
		// �e�̈ʒu�ݒ�
		SetPositionShadow
		( // ����
			g_player.nShadowID,	// �e�̃C���f�b�N�X
			g_player.pos		// �ʒu
		);

		//--------------------------------------------------------------------------------------------------------------
		//	���[�V�����̍X�V
		//--------------------------------------------------------------------------------------------------------------
		if (currentMotion != g_player.motion.type && g_player.motion.type != MOTIONTYPE_ATTACK)
		{ // ���݂̃��[�V�����ƑO��̃��[�V�������Ⴄ���A���݂̃��[�V�������U�����[�V�����ł͂Ȃ��ꍇ

			// ���[�V�����̐ݒ�
			SetMotion(currentMotion);
		}

		// ���[�V�����̍X�V
		UpdateMotion();

		//--------------------------------------------------------------------------------------------------------------
		//	���b�X���̍X�V
		//--------------------------------------------------------------------------------------------------------------
		switch (GetLessonState())
		{ // ���b�X�����Ƃ̏���
		case LESSON_00:	// ���b�X��0 (�ړ�)

			if (currentMotion == MOTIONTYPE_MOVE)
			{ // ���݂̃��[�V�������ړ���Ԃ̏ꍇ

				// ���b�X���̏�Ԃ̉��Z
				AddLessonState();
			}

			// �����𔲂���
			break;

		case LESSON_01:	// ���b�X��1 (�W�����v)

			if (lessonMotion == MOTIONTYPE_JUMP)
			{ // ���݂̃��[�V�������W�����v��Ԃ̏ꍇ

				// ���b�X���̏�Ԃ̉��Z
				AddLessonState();
			}

			// �����𔲂���
			break;

		case LESSON_02:	// ���b�X��2 (�U��)

			if (currentMotion == MOTIONTYPE_ATTACK)
			{ // ���݂̃��[�V�������U����Ԃ̏ꍇ

				// ���b�X���̏�Ԃ̉��Z
				AddLessonState();
			}

			// �����𔲂���
			break;
		}
	}
}

//======================================================================================================================
//	�Q�[���̃v���C���[�̍X�V����
//======================================================================================================================
void GamePlayer(void)
{
	// �ϐ���錾
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// ���݂̃��[�V����

	if (g_player.bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �O��ʒu�̍X�V
		g_player.oldPos = g_player.pos;
		
		// ��Ԃ̍X�V
		if (g_player.nCounterState > 0)
		{ // �J�E���^�[�� 0���傫���ꍇ

			// �J�E���^�[�����Z
			g_player.nCounterState--;

			if (g_player.nCounterState == UNR_TIME_PLAY)
			{ // �J�E���^�[�����l�̏ꍇ

				// ���G��Ԃɂ���
				g_player.state = ACTIONSTATE_UNRIVALED;
			}
			else if (g_player.nCounterState <= 0)
			{ // �J�E���^�[�� 0�ȉ��̏ꍇ

				// �ʏ��Ԃɂ���
				g_player.state = ACTIONSTATE_NORMAL;
			}
		}

		// �v���C���[�̈ړ��ʂ̍X�V
		currentMotion = MovePlayer();

		// �v���C���[�̃W�����v�̍X�V
		JumpPlayer();

		// �v���C���[�̈ʒu�̍X�V
		PosPlayer();

		// �v���C���[�̌����̍X�V
		RotPlayer();

		// �v���C���[�̕␳�̍X�V����
		RevPlayer();

		//--------------------------------------------------------------------------------------------------------------
		//	�W�����v����
		//--------------------------------------------------------------------------------------------------------------
		if (g_player.pos.y < GetLimitStage().fField)
		{ // �n�ʂɓ������Ă���ꍇ

			// �W�����v���Ă��Ȃ���Ԃɂ���
			g_player.bJump = false;

			// �L�b�N���Ă��Ȃ���Ԃɂ���
			g_player.bKick = false;

			// �����̐����ݒ�
			SetGrowTornado
			( // ����
				g_player.nTornadoID,	// �ύX�C���f�b�N�X
				0.6f,	// �����̕ύX��
				0.012f,	// ������̐�����
				0.012f,	// �c����̐�����
				0.032f	// �����x�̐�����
			);

			// �n�ʂɈʒu��␳
			g_player.pos.y  = GetLimitStage().fField;

			// �ړ��ʂ�������
			g_player.move.y = 0.0f;
		}
		else
		{ // �n�ʂɓ������Ă��Ȃ��ꍇ

			// �W�����v���Ă����Ԃɂ���
			g_player.bJump = true;
		}

		//--------------------------------------------------------------------------------------------------------------
		//	�����蔻��
		//--------------------------------------------------------------------------------------------------------------
		// �I�u�W�F�N�g�Ƃ̓����蔻��
		CollisionObject
		( // ����
			&g_player.pos,		// ���݂̈ʒu
			&g_player.oldPos,	// �O��̈ʒu
			PLAY_WIDTH,			// ����
			PLAY_DEPTH			// ���s
		);

		// �v���C���[�̃L�b�N�̓����蔻��
		CollisionKickEnemy(&g_player);

		//--------------------------------------------------------------------------------------------------------------
		//	�a���̍X�V
		//--------------------------------------------------------------------------------------------------------------
		if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true)
		{ // �U���̑��삪�����ꂽ�ꍇ

			if (g_player.nSlashID == NONE_SLASH && g_player.motion.type != MOTIONTYPE_ATTACK)
			{ // �a���̃C���f�b�N�X������������Ă��銎�A���݂̃��[�V�������U�����[�V�����ł͂Ȃ��ꍇ

				// ���݂̃��[�V�����ɍU����Ԃ���
				currentMotion = MOTIONTYPE_ATTACK;

				// �p�[�e�B�N���̐ݒ�
				SetParticle
				( // ����
					g_player.pos,						// �ʒu
					D3DXCOLOR(0.8f, 0.3f, 0.35f, 1.0f),	// �F
					PARTICLETYPE_SLASH,					// ���
					SPAWN_PARTICLE_SLASH,				// �G�t�F�N�g��
					4									// ����
				);

				// �a���̐ݒ�
				g_player.nSlashID = SetSlash
				( // ����
					SLASHTYPE_PLAYER,	// ���
					PLAY_SLASH_LIFE,	// ����
					PLAY_SLASH_RADIUS,	// ���a
					&g_player.nSlashID,	// �a���̐e�̎a���C���f�b�N�X
					&g_player.bUse		// �a���̐e�̎g�p��
				);
			}
		}

		// �a���̈ʒu�E�����̐ݒ�
		SetPositionSlash
		( // ����
			g_player.nSlashID,	// �a���̃C���f�b�N�X
			D3DXVECTOR3
			( // ����
				g_player.pos.x,						// �ʒu (x)
				g_player.pos.y + PLAY_SLASH_POS_Y,	// �ʒu (y)
				g_player.pos.z						// �ʒu (z)
			),
			g_player.rot		// ����
		);

		//--------------------------------------------------------------------------------------------------------------
		//	�e�̍X�V
		//--------------------------------------------------------------------------------------------------------------
		// �e�̈ʒu�ݒ�
		SetPositionShadow
		( // ����
			g_player.nShadowID,	// �e�̃C���f�b�N�X
			g_player.pos		// �ʒu
		);

		//--------------------------------------------------------------------------------------------------------------
		//	���[�V�����̍X�V
		//--------------------------------------------------------------------------------------------------------------
		if (currentMotion != g_player.motion.type && g_player.motion.type != MOTIONTYPE_ATTACK)
		{ // ���݂̃��[�V�����ƑO��̃��[�V�������Ⴄ���A���݂̃��[�V�������U�����[�V�����ł͂Ȃ��ꍇ

			// ���[�V�����̐ݒ�
			SetMotion(currentMotion);
		}

		// ���[�V�����̍X�V
		UpdateMotion();
	}
}

//======================================================================================================================
//	�v���C���[�̈ړ��ʂ̍X�V����
//======================================================================================================================
MOTIONTYPE MovePlayer(void)
{
	// �ϐ���錾
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// ���݂̃��[�V����

	// �|�C���^��錾
	Camera *pCamera = GetCamera();	// �J�����̏��

	if (GetKeyboardPress(DIK_W) == true || GetJoyKeyPress(JOYKEY_UP, 0) == true || GetJoyStickPressLY(0) > 0)
	{ // ���ړ��̑��삪�s��ꂽ�ꍇ

		// ���݂̃��[�V�����Ɉړ���Ԃ���
		currentMotion = MOTIONTYPE_MOVE;

		if (GetKeyboardPress(DIK_A) == true || GetJoyKeyPress(JOYKEY_LEFT, 0) == true || GetJoyStickPressLX(0) < 0)
		{ // ���ړ��̑�����s��ꂽ�ꍇ (�����ړ�)

			// �ړ��ʂ��X�V
			g_player.move.x += sinf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;
			g_player.move.z += cosf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;

			// �ڕW�������X�V
			g_player.destRot.y = D3DXToRadian(135) + pCamera->rot.y;
		}
		else if (GetKeyboardPress(DIK_D) == true || GetJoyKeyPress(JOYKEY_RIGHT, 0) == true || GetJoyStickPressLX(0) > 0)
		{ // �E�ړ��̑�����s��ꂽ�ꍇ (�E���ړ�)

			// �ړ��ʂ��X�V
			g_player.move.x -= sinf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;
			g_player.move.z -= cosf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;

			// �ڕW�������X�V
			g_player.destRot.y = D3DXToRadian(225) + pCamera->rot.y;
		}
		else
		{ // ���ړ��̑��삾�����s��ꂽ�ꍇ (���ړ�)

			// �ړ��ʂ��X�V
			g_player.move.x += sinf(pCamera->rot.y) * MOVE_PLAYER;
			g_player.move.z += cosf(pCamera->rot.y) * MOVE_PLAYER;

			// �ڕW�������X�V
			g_player.destRot.y = D3DXToRadian(180) + pCamera->rot.y;
		}
	}
	else if (GetKeyboardPress(DIK_S) == true || GetJoyKeyPress(JOYKEY_DOWN, 0) == true || GetJoyStickPressLY(0) < 0)
	{ // ��O�ړ��̑��삪�s��ꂽ�ꍇ

		// ���݂̃��[�V�����Ɉړ���Ԃ���
		currentMotion = MOTIONTYPE_MOVE;

		if (GetKeyboardPress(DIK_A) == true || GetJoyKeyPress(JOYKEY_LEFT, 0) == true || GetJoyStickPressLX(0) < 0)
		{ // ���ړ��̑�����s��ꂽ�ꍇ (����O�ړ�)

			// �ړ��ʂ��X�V
			g_player.move.x += sinf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;
			g_player.move.z += cosf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;

			// �ڕW�������X�V
			g_player.destRot.y = D3DXToRadian(45) + pCamera->rot.y;
		}
		else if (GetKeyboardPress(DIK_D) == true || GetJoyKeyPress(JOYKEY_RIGHT, 0) == true || GetJoyStickPressLX(0) > 0)
		{ // �E�ړ��̑�����s��ꂽ�ꍇ (�E��O�ړ�)

			// �ړ��ʂ��X�V
			g_player.move.x -= sinf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;
			g_player.move.z -= cosf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;

			// �ڕW�������X�V
			g_player.destRot.y = D3DXToRadian(315) + pCamera->rot.y;
		}
		else
		{ // ��O�ړ��̑��삾�����s��ꂽ�ꍇ (��O�ړ�)

			// �ړ��ʂ��X�V
			g_player.move.x -= sinf(pCamera->rot.y) * MOVE_PLAYER;
			g_player.move.z -= cosf(pCamera->rot.y) * MOVE_PLAYER;

			// �ڕW�������X�V
			g_player.destRot.y = D3DXToRadian(0) + pCamera->rot.y;
		}
	}
	else if (GetKeyboardPress(DIK_A) == true || GetJoyKeyPress(JOYKEY_LEFT, 0) == true || GetJoyStickPressLX(0) < 0)
	{ // ���ړ��̑��삪�s��ꂽ�ꍇ

		// ���݂̃��[�V�����Ɉړ���Ԃ���
		currentMotion = MOTIONTYPE_MOVE;

		// �ړ��ʂ��X�V
		g_player.move.x += sinf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;
		g_player.move.z += cosf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;

		// �ڕW�������X�V
		g_player.destRot.y = D3DXToRadian(90) + pCamera->rot.y;
	}
	else if (GetKeyboardPress(DIK_D) == true || GetJoyKeyPress(JOYKEY_RIGHT, 0) == true || GetJoyStickPressLX(0) > 0)
	{ // �E�ړ��̑��삪�s��ꂽ�ꍇ

		// ���݂̃��[�V�����Ɉړ���Ԃ���
		currentMotion = MOTIONTYPE_MOVE;

		// �ړ��ʂ��X�V
		g_player.move.x -= sinf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;
		g_player.move.z -= cosf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;

		// �ڕW�������X�V
		g_player.destRot.y = D3DXToRadian(270) + pCamera->rot.y;
	}

	// ���݂̃��[�V������Ԃ�
	return currentMotion;
}

//======================================================================================================================
//	�v���C���[�̃W�����v�̍X�V����
//======================================================================================================================
MOTIONTYPE JumpPlayer(void)
{
	// �ϐ���錾
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// ���݂̃��[�V����

	if (GetKeyboardTrigger(DIK_SPACE) == true || GetJoyKeyTrigger(JOYKEY_A, 0) == true)
	{ // �W�����v�̑��삪�s��ꂽ�ꍇ

		if (g_player.bJump == false)
		{ // �W�����v���ł͂Ȃ��ꍇ

			// ���݂̃��[�V�����ɃW�����v��Ԃ���
			currentMotion = MOTIONTYPE_JUMP;

			// �ړ��ʂ��X�V
			g_player.move.y += PLAY_JUMP;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_JUMP);	// SE (�W�����v)
		}
	}

	// ���݂̃��[�V������Ԃ�
	return currentMotion;
}

//======================================================================================================================
//	�v���C���[�̈ʒu�̍X�V����
//======================================================================================================================
void PosPlayer(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	�d�͂̍X�V
	//------------------------------------------------------------------------------------------------------------------
	g_player.move.y -= PLAY_GRAVITY;

	//------------------------------------------------------------------------------------------------------------------
	//	�ʒu�̍X�V
	//------------------------------------------------------------------------------------------------------------------
	// �ʒu�̍X�V
	g_player.pos += g_player.move;

	//------------------------------------------------------------------------------------------------------------------
	//	�ړ��ʂ̌����E�폜
	//------------------------------------------------------------------------------------------------------------------
	// �ړ��ʂ̌���
	g_player.move.x += (0.0f - g_player.move.x) * SUB_MOVE_PLAYER;
	g_player.move.z += (0.0f - g_player.move.z) * SUB_MOVE_PLAYER;

	// �ړ��ʂ̍폜
	if (g_player.move.x <  DEL_MOVE_PLAYER
	&&  g_player.move.x > -DEL_MOVE_PLAYER)
	{ // �v���C���[�̈ړ��� (x) ���폜����͈͓��̏ꍇ

		// �v���C���[�̈ړ��� (x) ���폜����
		g_player.move.x = 0.0f;
	}

	if (g_player.move.z <  DEL_MOVE_PLAYER
	&&  g_player.move.z > -DEL_MOVE_PLAYER)
	{ // �v���C���[�̈ړ��� (z) ���폜����͈͓��̏ꍇ

		// �v���C���[�̈ړ��� (z) ���폜����
		g_player.move.z = 0.0f;
	}
}

//======================================================================================================================
//	�v���C���[�̌����̍X�V����
//======================================================================================================================
void RotPlayer(void)
{
	// �ϐ���錾
	float fDiffRot = 0.0f;	// �v���C���[�̌����v�Z����p

	// �ڕW�����̐��K��
	if      (g_player.destRot.y >  D3DX_PI) { g_player.destRot.y -= D3DX_PI * 2; }
	else if (g_player.destRot.y < -D3DX_PI) { g_player.destRot.y += D3DX_PI * 2; }

	// �ڕW�����܂ł̍������v�Z
	fDiffRot = g_player.destRot.y - g_player.rot.y;

	// ���������̐��K��
	if      (fDiffRot >  D3DX_PI) { fDiffRot -= D3DX_PI * 2; }
	else if (fDiffRot < -D3DX_PI) { fDiffRot += D3DX_PI * 2; }

	// �����̍X�V
	g_player.rot.y += fDiffRot * REV_ROT_PLAYER;

	// �����̐��K��
	if      (g_player.rot.y >  D3DX_PI) { g_player.rot.y -= D3DX_PI * 2; }
	else if (g_player.rot.y < -D3DX_PI) { g_player.rot.y += D3DX_PI * 2; }
}

//======================================================================================================================
//	�v���C���[�̕␳�̍X�V����
//======================================================================================================================
void RevPlayer(void)
{
	if (g_player.pos.z > GetLimitStage().fNear - (PLAY_DEPTH * 2))
	{ // �͈͊O�̏ꍇ (��O)

		// ��O�Ɉʒu��␳
		g_player.pos.z = GetLimitStage().fNear - (PLAY_DEPTH * 2);
	}
	if (g_player.pos.z < GetLimitStage().fFar + (PLAY_DEPTH * 2))
	{ // �͈͊O�̏ꍇ (��)

		// ���Ɉʒu��␳
		g_player.pos.z = GetLimitStage().fFar + (PLAY_DEPTH * 2);
	}
	if (g_player.pos.x > GetLimitStage().fRight - (PLAY_WIDTH * 2))
	{ // �͈͊O�̏ꍇ (�E)

		// �E�Ɉʒu��␳
		g_player.pos.x = GetLimitStage().fRight - (PLAY_WIDTH * 2);
	}
	if (g_player.pos.x < GetLimitStage().fLeft + (PLAY_WIDTH * 2))
	{ // �͈͊O�̏ꍇ (��)

		// ���Ɉʒu��␳
		g_player.pos.x = GetLimitStage().fLeft + (PLAY_WIDTH * 2);
	}
}

//======================================================================================================================
//	�v���C���[�̃L�b�N�̓����蔻��
//======================================================================================================================
void CollisionKickEnemy(Player *pPlayer)
{
	// �ϐ���錾
	float fLength = 0.0f;			// �L�b�N�ƓG�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Enemy *pEnemy = GetEnemy();		// �G�̏��

	if (g_player.bKick == false)
	{ // �L�b�N���Ă��Ȃ���Ԃ̏ꍇ

		if (g_player.move.y < 0.0f)
		{ // �v���C���[�� y�ړ��ʂ��������̏ꍇ

			// �L�b�N���Ă����Ԃɂ���
			g_player.bKick = true;

			// �����̐ݒ�
			g_player.nTornadoID = SetTornado
			( // ����
				D3DXVECTOR3(0.0f, -10.0f, -8.0f),	// �ʒu
				D3DXVECTOR3(0.15f, 0.0f, 0.0f),		// ��������
				TORNADOTYPE_NORMAL,	// ���
				&pPlayer->mtxWorld,	// �e�̃}�g���b�N�X
				2,		// �Q�̎���
				64,		// �Q�̕�����
				0.4f,	// �����̕ύX��
				13.0f,	// �|���S���̑���
				8.0f,	// �|���S���O���� y���W���Z��
				5.0f,	// �������̉������
				0.6f,	// �������̓����x
				0.25f,	// ������̉��Z��
				0.55f,	// �c����̉��Z��
				0.004f,	// �����x�̌��Z��
				0.0f,	// ������̐�����
				0.0f,	// �c����̐�����
				0.0f	// �����x�̐�����
			);
		}
	}
	else
	{ // �L�b�N���Ă����Ԃ̏ꍇ

		for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
		{ // �G�̍ő�\�������J��Ԃ�

			if (pEnemy->bUse == true)
			{ // �G���g�p����Ă���ꍇ

				// �v���C���[�̑��ƓG�Ƃ̊Ԃ̋��������߂�
				fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
						+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

				if (fLength < ((PLAY_KICK_RADIUS + pEnemy->pModelData->fRadius) * (PLAY_KICK_RADIUS + pEnemy->pModelData->fRadius))	// �~�̓����蔻��
				&&  pPlayer->pos.y + PLAY_KICK_HEIGHT > pEnemy->pos.y																// �c�̔��� (��)
				&&  pPlayer->pos.y - PLAY_KICK_HEIGHT < pEnemy->pos.y + pEnemy->pModelData->fHeight)								// �c�̔��� (��)
				{ // �L�b�N���G�ɓ������Ă���ꍇ

					// �ړ��ʂ��X�V
					g_player.move.y = PLAY_KICK_JUMP;

					// �G�̃_���[�W����
					if (HitEnemy(pEnemy, KICK_DMG_PLAY, DAMAGETYPE_KICK) == true)
					{ // �|�����ꍇ

						// �����̐ݒ�
						SetTornado
						( // ����
							pEnemy->pos,		// �ʒu
							pEnemy->rot,		// ��������
							TORNADOTYPE_NORMAL,	// ���
							NULL,	// �e�̃}�g���b�N�X
							2,		// �Q�̎���
							64,		// �Q�̕�����
							0.2f,	// �����̕ύX��
							10.0f,	// �|���S���̑���
							6.0f,	// �|���S���O���� y���W���Z��
							5.0f,	// �������̉������
							0.8f,	// �������̓����x
							0.2f,	// ������̉��Z��
							0.4f,	// �c����̉��Z��
							0.004f,	// �����x�̌��Z��
							0.01f,	// ������̐�����
							0.01f,	// �c����̐�����
							0.04f	// �����x�̐�����
						);
					}
				}
			}
		}
	}
}

//======================================================================================================================
//	���[�V�����̍X�V����
//======================================================================================================================
void UpdateMotion(void)
{
	// �ϐ���錾
	D3DXVECTOR3 diffPos;		// ���̃|�[�Y�܂ł̍��� (�ʒu)
	D3DXVECTOR3 diffRot;		// ���̃|�[�Y�܂ł̍��� (����)
	D3DXVECTOR3 currentPos;		// ���݂̃t���[���̈ʒu
	D3DXVECTOR3 currentRot;		// ���݂̃t���[���̌���

	MOTIONTYPE  type  = g_player.motion.type;	// ���[�V�����̎��
	int         nPose = g_player.motion.nPose;	// ���[�V�����̃|�[�Y�ԍ�

	// �p�[�c�̈ʒu���X�V
	for (int nCntKey = 0; nCntKey < MAX_PARTS; nCntKey++)
	{ // �v���C���[�̃p�[�c�̍ő吔���J��Ԃ�

		// �ʒu�̍��������߂�
		diffPos = g_player.motion.aMotion[type].aKey[(nPose + 1) % g_player.motion.aMotion[type].nNumKey].aPos[nCntKey] - g_player.motion.aMotion[type].aKey[nPose].aPos[nCntKey];

		// �����̍��������߂�
		diffRot = g_player.motion.aMotion[type].aKey[(nPose + 1) % g_player.motion.aMotion[type].nNumKey].aRot[nCntKey] - g_player.motion.aMotion[type].aKey[nPose].aRot[nCntKey];

		// ���������̐��K��
		if      (diffRot.x >  D3DX_PI) { diffRot.x -= D3DX_PI * 2; }
		else if (diffRot.x < -D3DX_PI) { diffRot.x += D3DX_PI * 2; }
		if      (diffRot.y >  D3DX_PI) { diffRot.y -= D3DX_PI * 2; }
		else if (diffRot.y < -D3DX_PI) { diffRot.y += D3DX_PI * 2; }
		if      (diffRot.z >  D3DX_PI) { diffRot.z -= D3DX_PI * 2; }
		else if (diffRot.z < -D3DX_PI) { diffRot.z += D3DX_PI * 2; }

		// ���݂̃t���[���̈ʒu��ݒ�
		currentPos = g_player.motion.aMotion[type].aKey[nPose].aPos[nCntKey] + diffPos * ((float)g_player.motion.nCounter / (float)g_player.motion.aMotion[type].aKey[nPose].nFrame);

		// ���݂̃t���[���̌�����ݒ�
		currentRot = g_player.motion.aMotion[type].aKey[nPose].aRot[nCntKey] + diffRot * ((float)g_player.motion.nCounter / (float)g_player.motion.aMotion[type].aKey[nPose].nFrame);

		// ���������̐��K��
		if      (currentRot.x >  D3DX_PI) { currentRot.x -= D3DX_PI * 2; }
		else if (currentRot.x < -D3DX_PI) { currentRot.x += D3DX_PI * 2; }
		if      (currentRot.y >  D3DX_PI) { currentRot.y -= D3DX_PI * 2; }
		else if (currentRot.y < -D3DX_PI) { currentRot.y += D3DX_PI * 2; }
		if      (currentRot.z >  D3DX_PI) { currentRot.z -= D3DX_PI * 2; }
		else if (currentRot.z < -D3DX_PI) { currentRot.z += D3DX_PI * 2; }

		// ���݂̃p�[�c�̈ʒu���X�V
		g_player.aParts[nCntKey].pos = g_player.aParts[nCntKey].initPos + currentPos;

		// ���݂̃p�[�c�̌������X�V
		g_player.aParts[nCntKey].rot = g_player.aParts[nCntKey].initRot + currentRot;

		// �����̐��K��
		if      (g_player.aParts[nCntKey].rot.x >  D3DX_PI) { g_player.aParts[nCntKey].rot.x -= D3DX_PI * 2; }
		else if (g_player.aParts[nCntKey].rot.x < -D3DX_PI) { g_player.aParts[nCntKey].rot.x += D3DX_PI * 2; }
		if      (g_player.aParts[nCntKey].rot.y >  D3DX_PI) { g_player.aParts[nCntKey].rot.y -= D3DX_PI * 2; }
		else if (g_player.aParts[nCntKey].rot.y < -D3DX_PI) { g_player.aParts[nCntKey].rot.y += D3DX_PI * 2; }
		if      (g_player.aParts[nCntKey].rot.z >  D3DX_PI) { g_player.aParts[nCntKey].rot.z -= D3DX_PI * 2; }
		else if (g_player.aParts[nCntKey].rot.z < -D3DX_PI) { g_player.aParts[nCntKey].rot.z += D3DX_PI * 2; }
	}

	// ���[�V�����̑J�ڂ̍X�V
	if (g_player.motion.nCounter >= g_player.motion.aMotion[type].aKey[nPose].nFrame)
	{ // ���݂̃��[�V�����J�E���^�[�����݂̃|�[�Y�̍Đ��t���[�����𒴂��Ă���ꍇ

		// ���̃|�[�Y�Ɉڍs
		if (g_player.motion.aMotion[type].bLoop == true)
		{ // ���[�V���������[�v����ݒ�̏ꍇ

			// ���[�V�����J�E���^�[��������
			g_player.motion.nCounter = 0;

			// �|�[�Y�J�E���g�����Z (�����ɒB�����ꍇ 0�ɖ߂�)
			g_player.motion.nPose = (g_player.motion.nPose + 1) % g_player.motion.aMotion[type].nNumKey;
		}
		else
		{ // ���[�V���������[�v���Ȃ��ݒ�̏ꍇ

			if (g_player.motion.nPose < g_player.motion.aMotion[type].nNumKey - SUB_MOTION_STOP)
			{ // ���݂̃|�[�Y���ŏI�̃|�[�Y�ł͂Ȃ��ꍇ

				// ���[�V�����J�E���^�[��������
				g_player.motion.nCounter = 0;

				// �|�[�Y�J�E���g�����Z
				g_player.motion.nPose++;
			}
			else
			{ // ���݂̃|�[�Y���ŏI�̃|�[�Y�̏ꍇ

				switch (type)
				{ // ���[�V�����̎�ނ��Ƃ̏���
				case MOTIONTYPE_ATTACK:		// �U�����

					// �ҋ@���[�V�����Ɉڍs
					SetMotion(MOTIONTYPE_NONE);

					// �����𔲂���
					break;

				case MOTIONTYPE_JUMP:		// �W�����v���

					// ����

					// �����𔲂���
					break;

				case MOTIONTYPE_LANDING:	// ���n���

					// ����

					// �����𔲂���
					break;
				}
			}
		}
	}
	else
	{ // ���݂̃��[�V�����J�E���^�[�����݂̃|�[�Y�̍Đ��t���[�����𒴂��Ă��Ȃ��ꍇ

		// ���[�V�����J�E���^�[�����Z
		g_player.motion.nCounter++;
	}
}

//======================================================================================================================
//	���[�V�����̐ݒ菈��
//======================================================================================================================
void SetMotion(MOTIONTYPE type)
{
	// ��������
	g_player.motion.type = type;	// ���[�V�����̎��

	// ���[�V��������������
	g_player.motion.nPose    = 0;	// ���[�V�����̃|�[�Y�ԍ�
	g_player.motion.nCounter = 0;	// ���[�V�����̃J�E���^�[
}

//======================================================================================================================
//	�v���C���[�̃Z�b�g�A�b�v����
//======================================================================================================================
void TxtSetPlayer(void)
{
	// �ϐ���錾
	int nID        = 0;			// �C���f�b�N�X�̑���p
	int nNowMotion = 0;			// ���݂̃��[�V�����ԍ�
	int nNowPose   = 0;			// ���݂̃|�[�Y�ԍ�
	int nNowKey    = 0;			// ���݂̃L�[�ԍ�
	int nLoop      = 0;			// ���[�v�� ON / OFF �̕ϊ��p
	int nEnd       = 0;			// �e�L�X�g�ǂݍ��ݏI���̊m�F�p

	// �ϐ��z���錾
	char aString[MAX_STRING];	// �e�L�X�g�̕�����̑���p

	// �|�C���^��錾
	FILE *pFile;				// �t�@�C���|�C���^

	// �t�@�C����ǂݍ��݌`���ŊJ��
	pFile = fopen(PLAYER_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // �t�@�C�����J�����ꍇ

		do
		{ // �ǂݍ��񂾕����� EOF �ł͂Ȃ��ꍇ���[�v

			// �t�@�C�����當�����ǂݍ���
			nEnd = fscanf(pFile, "%s", &aString[0]);	// �e�L�X�g��ǂݍ��݂������� EOF ��Ԃ�

			//----------------------------------------------------------------------------------------------------------
			//	�L�����N�^�[�̐ݒ�
			//----------------------------------------------------------------------------------------------------------
			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{ // �ǂݍ��񂾕����� CHARACTERSET �̏ꍇ

				do
				{ // �ǂݍ��񂾕����� END_CHARACTERSET �ł͂Ȃ��ꍇ���[�v

					// �t�@�C�����當�����ǂݍ���
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "PARTSSET") == 0)
					{ // �ǂݍ��񂾕����� PARTSSET �̏ꍇ

						do
						{ // �ǂݍ��񂾕����� END_PARTSSET �ł͂Ȃ��ꍇ���[�v

							// �t�@�C�����當�����ǂݍ���
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "INDEX") == 0)
							{ // �ǂݍ��񂾕����� INDEX �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);	// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &nID);			// ���f�� (�p�[�c) �̃C���f�b�N�X��ǂݍ���
							}
							else if (strcmp(&aString[0], "PARENT") == 0)
							{ // �ǂݍ��񂾕����� PARENT �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);						// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_player.aParts[nID].nParentID);	// ���f�� (�p�[�c) �̐e�̃C���f�b�N�X��ǂݍ���
							}
							else if (strcmp(&aString[0], "POS") == 0)
							{ // �ǂݍ��񂾕����� POS �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);						// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_player.aParts[nID].initPos.x);	// X���W��ǂݍ���
								fscanf(pFile, "%f", &g_player.aParts[nID].initPos.y);	// Y���W��ǂݍ���
								fscanf(pFile, "%f", &g_player.aParts[nID].initPos.z);	// Z���W��ǂݍ���

								// ���݂̈ʒu�ɓǂݍ��񂾒l��ݒ�
								g_player.aParts[nID].pos = g_player.aParts[nID].initPos;
							}
							else if (strcmp(&aString[0], "ROT") == 0)
							{ // �ǂݍ��񂾕����� ROT �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);						// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_player.aParts[nID].initRot.x);	// X������ǂݍ���
								fscanf(pFile, "%f", &g_player.aParts[nID].initRot.y);	// Y������ǂݍ���
								fscanf(pFile, "%f", &g_player.aParts[nID].initRot.z);	// Z������ǂݍ���

								// ���݂̌����ɓǂݍ��񂾒l��ݒ�
								g_player.aParts[nID].rot = g_player.aParts[nID].initRot;
							}

						} while (strcmp(&aString[0], "END_PARTSSET") != 0);	// �ǂݍ��񂾕����� END_PARTSSET �ł͂Ȃ��ꍇ���[�v
					}
				} while (strcmp(&aString[0], "END_CHARACTERSET") != 0);		// �ǂݍ��񂾕����� END_CHARACTERSET �ł͂Ȃ��ꍇ���[�v
			}

			//----------------------------------------------------------------------------------------------------------
			//	���[�V�����̐ݒ�
			//----------------------------------------------------------------------------------------------------------
			else if (strcmp(&aString[0], "MOTIONSET") == 0)
			{ // �ǂݍ��񂾕����� MOTIONSET �̏ꍇ

				// ���݂̃|�[�Y�ԍ���������
				nNowPose = 0;

				do
				{ // �ǂݍ��񂾕����� END_MOTIONSET �ł͂Ȃ��ꍇ���[�v

					// �t�@�C�����當�����ǂݍ���
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "LOOP") == 0)
					{ // �ǂݍ��񂾕����� LOOP �̏ꍇ
						fscanf(pFile, "%s", &aString[0]);	// = ��ǂݍ��� (�s�v)
						fscanf(pFile, "%d", &nLoop);		// ���[�v�� ON / OFF ��ǂݍ���

						// �ǂݍ��񂾒l�� 0�Ȃ� true�A0�ȊO�Ȃ� false����
						g_player.motion.aMotion[nNowMotion].bLoop = (nLoop == 0) ? false : true;
					}
					else if (strcmp(&aString[0], "NUM_KEY") == 0)
					{ // �ǂݍ��񂾕����� NUM_KEY �̏ꍇ
						fscanf(pFile, "%s", &aString[0]);									// = ��ǂݍ��� (�s�v)
						fscanf(pFile, "%d", &g_player.motion.aMotion[nNowMotion].nNumKey);	// �L�[�̑�����ǂݍ���
					}
					else if (strcmp(&aString[0], "KEYSET") == 0)
					{ // �ǂݍ��񂾕����� KEYSET �̏ꍇ

						// ���݂̃L�[�ԍ���������
						nNowKey = 0;

						do
						{ // �ǂݍ��񂾕����� END_KEYSET �ł͂Ȃ��ꍇ���[�v

							// �t�@�C�����當�����ǂݍ���
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "FRAME") == 0)
							{ // �ǂݍ��񂾕����� FRAME �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);													// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].nFrame);	// �L�[���؂�ւ��܂ł̃t���[������ǂݍ���
							}
							else if (strcmp(&aString[0], "KEY") == 0)
							{ // �ǂݍ��񂾕����� KEY �̏ꍇ

								do
								{ // �ǂݍ��񂾕����� END_KEY �ł͂Ȃ��ꍇ���[�v

									// �t�@�C�����當�����ǂݍ���
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "POS") == 0)
									{ // �ǂݍ��񂾕����� POS �̏ꍇ
										fscanf(pFile, "%s", &aString[0]);															// = ��ǂݍ��� (�s�v)
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aPos[nNowKey].x);	// �ʒu (x) ��ǂݍ���
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aPos[nNowKey].y);	// �ʒu (y) ��ǂݍ���
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aPos[nNowKey].z);	// �ʒu (z) ��ǂݍ���
									}
									else if (strcmp(&aString[0], "ROT") == 0)
									{ // �ǂݍ��񂾕����� ROT �̏ꍇ
										fscanf(pFile, "%s", &aString[0]);															// = ��ǂݍ��� (�s�v)
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aRot[nNowKey].x);	// ���� (x) ��ǂݍ���
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aRot[nNowKey].y);	// ���� (y) ��ǂݍ���
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aRot[nNowKey].z);	// ���� (z) ��ǂݍ���
									}

								} while (strcmp(&aString[0], "END_KEY") != 0);	// �ǂݍ��񂾕����� END_KEY �ł͂Ȃ��ꍇ���[�v

								// ���݂̃L�[�ԍ������Z
								nNowKey++;
							}
						} while (strcmp(&aString[0], "END_KEYSET") != 0);		// �ǂݍ��񂾕����� END_KEYSET �ł͂Ȃ��ꍇ���[�v

						// ���݂̃|�[�Y�ԍ������Z
						nNowPose++;
					}
				} while (strcmp(&aString[0], "END_MOTIONSET") != 0);			// �ǂݍ��񂾕����� END_MOTIONSET �ł͂Ȃ��ꍇ���[�v

				// ���݂̃��[�V�����ԍ������Z
				nNowMotion++;
			}
		} while (nEnd != EOF);													// �ǂݍ��񂾕����� EOF �ł͂Ȃ��ꍇ���[�v
		
		// �t�@�C�������
		fclose(pFile);
	}
	else
	{ // �t�@�C�����J���Ȃ������ꍇ

		// �G���[���b�Z�[�W�{�b�N�X
		MessageBox(NULL, "�X�e�[�W�t�@�C���̓ǂݍ��݂Ɏ��s�I", "�x���I", MB_ICONWARNING);
	}
}

//======================================================================================================================
//	�v���C���[�̎擾����
//======================================================================================================================
Player *GetPlayer(void)
{
	// �v���C���[�{�̂̏��A�h���X��Ԃ�
	return &g_player;
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�v���C���[�̍��W�̎擾����
//**********************************************************************************************************************
D3DXVECTOR3 GetPlayerPos(void)
{
	// ���W��Ԃ�
	return g_player.move;
}

//**********************************************************************************************************************
//	�v���C���[�̌����̎擾����
//**********************************************************************************************************************
D3DXVECTOR3 GetPlayerRot(void)
{
	// ���W��Ԃ�
	return g_player.rot;
}

//**********************************************************************************************************************
//	�v���C���[�̍Đݒ菈��
//**********************************************************************************************************************
void ResetPlayer(void)
{
	// �O���[�o���ϐ����Đݒ�
	g_player.pos     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���݂̈ʒu
	g_player.oldPos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �O��̈ʒu
	g_player.rot     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���݂̌���
	g_player.destRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ڕW�̌���
}
#endif