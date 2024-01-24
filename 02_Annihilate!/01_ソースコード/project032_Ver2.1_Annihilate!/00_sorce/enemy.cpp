//======================================================================================================================
//
//	�G���� [enemy.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "input.h"

#include "bullet.h"
#include "enemy.h"
#include "explosion.h"
#include "game.h"
#include "object.h"
#include "particle.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"
#include "sound.h"
#include "timer.h"
#include "tutorial.h"
#include "weed.h"

#include "orbit.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define ENEMY_SETUP_TXT	"02_data\\00_TXT\\enemy.txt"	// �G�Z�b�g�A�b�v�p�̃e�L�X�g�t�@�C���̑��΃p�X

#define ENE_WIDTH		(15.0f)		// �G�̉��� / 2
#define ENE_DEPTH		(15.0f)		// �G�̉��s / 2

#define ENE_LIFE		(100)		// �G�̗̑�
#define ENE_BODY_DMG	(20)		// �G�{�̂̃_���[�W��

#define DAMAGE_TIME_ENE	(PLAY_SLASH_LIFE)			// �_���[�W��Ԃ�ۂ���
#define UNR_TIME_ENE	(PLAY_SLASH_LIFE - 10)		// ���G��ԂɕύX���鎞��

#define NUM_SPAWN_POS	(8)			// �o�����钆�S�ʒu�̑���
#define NUM_ENE_NORMAL	(2)			// �ʏ�G�̑���
#define SPAWN_MAT_ALPHA	(0.65f)		// �o�����̃}�e���A���̃��l

#define PLAY_WIDTH_MUL	(0.25f)		// �v���C���[�Ƃ̓����蔻�莞�̃v���C���[�T�C�Y�̏�Z�l

#define ENE_BULL_MOVE	(4.0f)		// �G�̒e�̈ړ���

#define ENE_SLA_LIFE	(18)		// �G�̎a���̎���
#define ENE_SLA_RADIUS	(60.0f)		// �G�̎a���̔��a

#define BULL_SCORE_PLUS	(400)		// �e�œ|�������̃v���X�X�R�A
#define KICK_SCORE_PLUS	(200)		// �L�b�N�œ|�������̃v���X�X�R�A

#define NOT_SPAWN_WEED	((int)((NUM_TIME_GAME / 60) * 0.1f))	// �G���������Ȃ��Ȃ鎞��

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apModelEnemy[] =	// ���f���̑��΃p�X
{
	"02_data\\03_MODEL\\03_ENEMY\\flower_face000.x",	// �� (�u���[) �̃��f�����΃p�X
	"02_data\\03_MODEL\\03_ENEMY\\flower_face001.x",	// �� (�s���N) �̃��f�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (ENEMYTYPE)
//**********************************************************************************************************************
typedef enum
{
	ENEMYTYPE_RANGE_SHORT = 0,		// �ߋ����^
	ENEMYTYPE_RANGE_LONG,			// �������^
	ENEMYTYPE_MAX,					// ���̗񋓌^�̑���
} ENEMYTYPE;

//**********************************************************************************************************************
//	�\���̒�` (WeedSpawn)
//**********************************************************************************************************************
typedef struct
{
	int nCounterSpawn;				// �o���p�x
	int nMinSpawn;					// �Œ�o����
	int nMaxSpawn;					// �ō��o����
	int nRandSpawnPos;				// �o���ʒu�̃u���̔��a
}WeedSpawn;

//**********************************************************************************************************************
//	�\���̒�` (EnemySpawn)
//**********************************************************************************************************************
typedef struct
{
	int         nMinSpawn;					// �Œ�o����
	int         nMaxSpawn;					// �ō��o����
	int         nRandSpawnPos;				// �o���ʒu�̃u���̔��a
	D3DXVECTOR3 aPosSpawn[NUM_SPAWN_POS];	// �o���̒��S�ʒu
}EnemySpawn;

//**********************************************************************************************************************
//	�\���̒�` (EnemyStatus)
//**********************************************************************************************************************
typedef struct
{
	int       nLife;						// �̗�
	int       nScore;						// �X�R�A���Z��
	int       nCounterAttack;				// �U���p�x
	float     fForwardMove;					// �O�i�̈ړ���
	float     fBackwardMove;				// ��ނ̈ړ���
	float     fLookRevision;				// �v���C���[�����������␳�W��
	float     fFindRadius;					// ���m�͈�
	float     fAttackRadius;				// �U���͈�
	float     fBackwardRadius;				// ��ޔ͈�
	bool      bBackward;					// ��ނ� ON / OFF
	WeedSpawn weed;							// �G���̃X�|�[�����
}EnemyStatus;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void TutorialEnemy(void);					// �`���[�g���A���̓G�̍X�V����
void GameEnemy(void);						// �Q�[���̓G�̍X�V����

void SpawnEnemy(void);												// �G�̏o������
void SpawnWeed(Enemy *pEnemy);										// �G���̏o������
void CollisionPlayer(Enemy *pEnemy);								// �G�ƃv���C���[�̓����蔻��
void CollisionEnemyFind(Enemy *pEnemy);								// �G�̌��m�͈͂̓����蔻��
bool CollisionEnemyAttack(Enemy *pEnemy, Player *pPlayer);		// �G�̍U���͈͂̓����蔻��
bool CollisionEnemyBackward(Enemy *pEnemy, Player *pPlayer);	// �G�̌�ޔ͈͂̓����蔻��
void LookPlayer(Enemy *pEnemy, Player *pPlayer);				// �v���C���[�̕����Ɍ�������
void RevPosEnemy(Enemy *pEnemy);									// �G�̈ʒu�̕␳����
void TxtSetEnemy(void);												// �G�̃Z�b�g�A�b�v����

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
Model       g_aModelEnemy[ENEMYTYPE_MAX];	// �G�̃��f�����
Enemy       g_aEnemy[MAX_ENEMY];			// �G�̏��
EnemySpawn  g_enemySpawn;					// �G�̃X�|�[�����
EnemyStatus g_aEnemyStatus[ENEMYTYPE_MAX];	// �G�̃X�e�[�^�X���
int         g_nNumEnemy;					// �G�̑���
int         g_nKillEnemy;					// �G�̓�����

//======================================================================================================================
//	�G�̏���������
//======================================================================================================================
void InitEnemy(void)
{
	// �ϐ���錾
	int         nNumVtx;		// ���f���̒��_��
	DWORD       dwSizeFVF;		// ���f���̒��_�t�H�[�}�b�g�̃T�C�Y
	BYTE        *pVtxBuff;		// ���f���̒��_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 vtx;			// ���f���̒��_���W
	D3DXVECTOR3 size;			// ���f���̑傫��

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	D3DXMATERIAL      *pMat;					// �}�e���A���ւ̃|�C���^

	// �O���[�o���ϐ���������
	g_nNumEnemy  = 0;			// �G�̑���
	g_nKillEnemy = 0;			// �G�̓�����

	// �G�̃��f�����̏�����
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		g_aModelEnemy[nCntEnemy].apTexture[MAX_MODEL_TEXTURE] = {};						// �e�N�X�`���ւ̃|�C���^
		g_aModelEnemy[nCntEnemy].pMesh    = NULL;										// ���b�V�� (���_���) �ւ̃|�C���^
		g_aModelEnemy[nCntEnemy].pBuffMat = NULL;										// �}�e���A���ւ̃|�C���^
		g_aModelEnemy[nCntEnemy].dwNumMat = 0;											// �}�e���A���̐�
		g_aModelEnemy[nCntEnemy].vtxMin   = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// �ŏ��̒��_���W
		g_aModelEnemy[nCntEnemy].vtxMax   = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// �ő�̒��_���W
		g_aModelEnemy[nCntEnemy].fHeight  = 0.0f;										// �c��
		g_aModelEnemy[nCntEnemy].fRadius  = 0.0f;										// ���a
	}

	// �G�̏��̏�����
	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		g_aEnemy[nCntEnemy].pos            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���݂̈ʒu
		g_aEnemy[nCntEnemy].oldPos         = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �O��̈ʒu
		g_aEnemy[nCntEnemy].rot            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
		g_aEnemy[nCntEnemy].pModelData     = NULL;							// ���f�����
		g_aEnemy[nCntEnemy].state          = ACTIONSTATE_NONE;				// ���
		g_aEnemy[nCntEnemy].nLife          = 0;								// �̗�
		g_aEnemy[nCntEnemy].nType          = 0;								// ���
		g_aEnemy[nCntEnemy].nCounterState  = 0;								// ��ԊǗ��J�E���^�[
		g_aEnemy[nCntEnemy].nCounterAttack = 0;								// �U���Ǘ��J�E���^�[
		g_aEnemy[nCntEnemy].nCounterWeed   = 0;								// �G���Ǘ��J�E���^�[
		g_aEnemy[nCntEnemy].nShadowID      = NONE_SHADOW;					// �e�̃C���f�b�N�X
		g_aEnemy[nCntEnemy].nSlashID       = NONE_SLASH;					// �a���̃C���f�b�N�X
		g_aEnemy[nCntEnemy].nOrbitID       = NONE_ORBIT;					// �O�Ղ̃C���f�b�N�X
		g_aEnemy[nCntEnemy].bUse           = false;							// �g�p��
	}

	// �G�̃Z�b�g�A�b�v����
	TxtSetEnemy();			// �X�e�[�^�X���E�X�|�[�����̏�����

	// x�t�@�C���̓ǂݍ���
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		D3DXLoadMeshFromX
		( // ����
			apModelEnemy[nCntEnemy],			// ���f���̑��΃p�X
			D3DXMESH_SYSTEMMEM,
			pDevice,							// �f�o�C�X�ւ̃|�C���^
			NULL,
			&g_aModelEnemy[nCntEnemy].pBuffMat,	// �}�e���A���ւ̃|�C���^
			NULL,
			&g_aModelEnemy[nCntEnemy].dwNumMat,	// �}�e���A���̐�
			&g_aModelEnemy[nCntEnemy].pMesh		// ���b�V�� (���_���) �ւ̃|�C���^
		);
	}

	// �����蔻��̍쐬
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		// ���f���̒��_�����擾
		nNumVtx = g_aModelEnemy[nCntEnemy].pMesh->GetNumVertices();

		// ���f���̒��_�t�H�[�}�b�g�̃T�C�Y���擾
		dwSizeFVF = D3DXGetFVFVertexSize(g_aModelEnemy[nCntEnemy].pMesh->GetFVF());

		// ���f���̒��_�o�b�t�@�����b�N
		g_aModelEnemy[nCntEnemy].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{ // ���f���̒��_�����J��Ԃ�

			// ���f���̒��_���W����
			vtx = *(D3DXVECTOR3*)pVtxBuff;

			// ���_���W (x) �̐ݒ�
			if (vtx.x < g_aModelEnemy[nCntEnemy].vtxMin.x)
			{ // ����̒��_���W (x) ���A����̒��_���W (x) �����������ꍇ

				// ����̒��_��� (x) ����
				g_aModelEnemy[nCntEnemy].vtxMin.x = vtx.x;
			}
			else if (vtx.x > g_aModelEnemy[nCntEnemy].vtxMax.x)
			{ // ����̒��_���W (x) ���A����̒��_���W (x) �����傫���ꍇ

				// ����̒��_��� (x) ����
				g_aModelEnemy[nCntEnemy].vtxMax.x = vtx.x;
			}

			// ���_���W (y) �̐ݒ�
			if (vtx.y < g_aModelEnemy[nCntEnemy].vtxMin.y)
			{ // ����̒��_���W (y) ���A����̒��_���W (y) �����������ꍇ

				// ����̒��_��� (y) ����
				g_aModelEnemy[nCntEnemy].vtxMin.y = vtx.y;
			}
			else if (vtx.y > g_aModelEnemy[nCntEnemy].vtxMax.y)
			{ // ����̒��_���W (y) ���A����̒��_���W (y) �����傫���ꍇ

				// ����̒��_��� (y) ����
				g_aModelEnemy[nCntEnemy].vtxMax.y = vtx.y;
			}

			// ���_���W (z) �̐ݒ�
			if (vtx.z < g_aModelEnemy[nCntEnemy].vtxMin.z)
			{ // ����̒��_���W (z) ���A����̒��_���W (z) �����������ꍇ

				// ����̒��_��� (z) ����
				g_aModelEnemy[nCntEnemy].vtxMin.z = vtx.z;
			}
			else if (vtx.z > g_aModelEnemy[nCntEnemy].vtxMax.z)
			{ // ����̒��_���W (z) ���A����̒��_���W (z) �����傫���ꍇ

				// ����̒��_��� (z) ����
				g_aModelEnemy[nCntEnemy].vtxMax.z = vtx.z;
			}

			// ���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
			pVtxBuff += dwSizeFVF;
		}

		// ���f���̒��_�o�b�t�@���A�����b�N
		g_aModelEnemy[nCntEnemy].pMesh->UnlockVertexBuffer();

		// ���f���T�C�Y�����߂�
		size = g_aModelEnemy[nCntEnemy].vtxMax - g_aModelEnemy[nCntEnemy].vtxMin;

		// ���f���̏c������
		g_aModelEnemy[nCntEnemy].fHeight = size.y;

		// ���f���̉~�̓����蔻����쐬
		g_aModelEnemy[nCntEnemy].fRadius = ((size.x * 0.5f) + (size.z * 0.5f)) * 0.5f;
	}

	// �e�N�X�`���̓ǂݍ���
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		// �}�e���A�����ɑ΂���|�C���^���擾
		pMat = (D3DXMATERIAL*)g_aModelEnemy[nCntEnemy].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aModelEnemy[nCntEnemy].dwNumMat; nCntMat++)
		{ // �}�e���A���̐����J��Ԃ�

			if (pMat[nCntMat].pTextureFilename != NULL)
			{ // �e�N�X�`���t�@�C�������݂���ꍇ

				// �e�N�X�`���̓ǂݍ���
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aModelEnemy[nCntEnemy].apTexture[nCntMat]);
			}
		}
	}
}

//======================================================================================================================
//	�G�̏I������
//======================================================================================================================
void UninitEnemy(void)
{
	// �e�N�X�`���̔j��
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		for (int nCntMat = 0; nCntMat < MAX_MODEL_TEXTURE; nCntMat++)
		{ // �g�p����e�N�X�`�������J��Ԃ�

			if (g_aModelEnemy[nCntEnemy].apTexture[nCntMat] != NULL)
			{ // �ϐ� (g_aModelEnemy[nCntEnemy].apTexture) ��NULL�ł͂Ȃ��ꍇ

				g_aModelEnemy[nCntEnemy].apTexture[nCntMat]->Release();
				g_aModelEnemy[nCntEnemy].apTexture[nCntMat] = NULL;
			}
		}
	}

	// ���b�V���̔j��
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		if (g_aModelEnemy[nCntEnemy].pMesh != NULL)
		{ // �ϐ� (g_aModelEnemy[nCntEnemy].pMesh) ��NULL�ł͂Ȃ��ꍇ

			g_aModelEnemy[nCntEnemy].pMesh->Release();
			g_aModelEnemy[nCntEnemy].pMesh = NULL;
		}
	}

	// �}�e���A���̔j��
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // �G�̎�ޕ��J��Ԃ�

		if (g_aModelEnemy[nCntEnemy].pBuffMat != NULL)
		{ // �ϐ� (g_aModelEnemy[nCntEnemy].pBuffMat) ��NULL�ł͂Ȃ��ꍇ

			g_aModelEnemy[nCntEnemy].pBuffMat->Release();
			g_aModelEnemy[nCntEnemy].pBuffMat = NULL;
		}
	}
}

//======================================================================================================================
//	�G�̍X�V����
//======================================================================================================================
void UpdateEnemy(void)
{
	switch (GetMode())
	{ // ���[�h���Ƃ̍X�V
	case MODE_TUTORIAL:	// �`���[�g���A�����

		// �`���[�g���A���̓G�̍X�V
		TutorialEnemy();

		// �����𔲂���
		break;

	case MODE_GAME:		// �Q�[�����

		// �Q�[���̓G�̍X�V
		GameEnemy();

		// �����𔲂���
		break;
	}
}

//======================================================================================================================
//	�G�̕`�揈��
//======================================================================================================================
void DrawEnemy(void)
{
	// �ϐ���錾
	D3DXMATRIX   mtxRot, mtxTrans;				// �v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;						// ���݂̃}�e���A���ۑ��p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	D3DXMATERIAL     *pMat;						// �}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATERIAL      redMat;					// �}�e���A���f�[�^ (��)
	D3DXMATERIAL      thinMat;					// �}�e���A���f�[�^ (������)

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // �G���g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aEnemy[nCntEnemy].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aEnemy[nCntEnemy].rot.y, g_aEnemy[nCntEnemy].rot.x, g_aEnemy[nCntEnemy].rot.z);
			D3DXMatrixMultiply(&g_aEnemy[nCntEnemy].mtxWorld, &g_aEnemy[nCntEnemy].mtxWorld, &mtxRot);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aEnemy[nCntEnemy].pos.x, g_aEnemy[nCntEnemy].pos.y, g_aEnemy[nCntEnemy].pos.z);
			D3DXMatrixMultiply(&g_aEnemy[nCntEnemy].mtxWorld, &g_aEnemy[nCntEnemy].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aEnemy[nCntEnemy].mtxWorld);

			// ���݂̃}�e���A�����擾
			pDevice->GetMaterial(&matDef);

			// �}�e���A���f�[�^�ւ̃|�C���^���擾
			pMat = (D3DXMATERIAL*)g_aEnemy[nCntEnemy].pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aEnemy[nCntEnemy].pModelData->dwNumMat; nCntMat++)
			{ // �}�e���A���̐����J��Ԃ�

				switch (g_aEnemy[nCntEnemy].state)
				{ // ��Ԃ��Ƃ̏���
				case ACTIONSTATE_SPAWN:		// �o�����

					// �ǂݍ��񂾃}�e���A������
					thinMat.MatD3D = pMat[nCntMat].MatD3D;

					// �}�e���A���̃��l��������
					thinMat.MatD3D.Diffuse.a = SPAWN_MAT_ALPHA;

					// �}�e���A���̐ݒ�
					pDevice->SetMaterial(&thinMat.MatD3D);			// ������

					// �����𔲂���
					break;

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
				pDevice->SetTexture(0, g_aEnemy[nCntEnemy].pModelData->apTexture[nCntMat]);

				// ���f���̕`��
				g_aEnemy[nCntEnemy].pModelData->pMesh->DrawSubset(nCntMat);
			}

			// �ۑ����Ă����}�e���A����߂�
			pDevice->SetMaterial(&matDef);
		}
	}
}

//======================================================================================================================
//	�G�̐ݒ菈��
//======================================================================================================================
void SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType)
{
	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (g_aEnemy[nCntEnemy].bUse == false)
		{ // �G���g�p����Ă��Ȃ��ꍇ

			// ��������
			g_aEnemy[nCntEnemy].pos    = pos;		// ���݂̈ʒu
			g_aEnemy[nCntEnemy].oldPos = pos;		// �O��̈ʒu
			g_aEnemy[nCntEnemy].rot    = rot;		// ����
			g_aEnemy[nCntEnemy].nType  = nType;		// ���

			// �G�̏���������
			g_aEnemy[nCntEnemy].state         = ACTIONSTATE_SPAWN;		// ���
			g_aEnemy[nCntEnemy].nCounterState = 0;						// ��ԊǗ��J�E���^�[
			g_aEnemy[nCntEnemy].nCounterWeed  = 0;						// �G���Ǘ��J�E���^�[
			g_aEnemy[nCntEnemy].nSlashID      = NONE_SLASH;				// �a���̃C���f�b�N�X

			// �̗͂�ݒ�
			g_aEnemy[nCntEnemy].nLife = g_aEnemyStatus[nType].nLife;	// �̗�

			// �U���Ǘ��J�E���^�[��ݒ�
			g_aEnemy[nCntEnemy].nCounterAttack = (int)(g_aEnemyStatus[nType].nCounterAttack * 0.5f);	// �����𑁂�����

			// �g�p���Ă����Ԃɂ���
			g_aEnemy[nCntEnemy].bUse = true;

			// ���f������ݒ�
			g_aEnemy[nCntEnemy].pModelData = &g_aModelEnemy[nType];		// ���f�����

			// �e�̃C���f�b�N�X��ݒ�
			g_aEnemy[nCntEnemy].nShadowID = SetShadow
			( // ����
				0.5f,																											// ���l
				fabsf(g_aModelEnemy[g_aEnemy[nCntEnemy].nType].vtxMax.x - g_aModelEnemy[g_aEnemy[nCntEnemy].nType].vtxMin.x),	// ���a
				&g_aEnemy[nCntEnemy].nShadowID,																					// �e�̐e�̉e�C���f�b�N�X
				&g_aEnemy[nCntEnemy].bUse																						// �e�̐e�̎g�p��
			);

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aEnemy[nCntEnemy].nShadowID, g_aEnemy[nCntEnemy].pos);

			// �O�Ղ̃C���f�b�N�X��ݒ�
			g_aEnemy[nCntEnemy].nOrbitID = SetOrbit
			( // ����
				&g_aEnemy[nCntEnemy].mtxWorld,		// ���[���h�}�g���b�N�X�̃A�h���X
				&g_aEnemy[nCntEnemy].bUse,			// �g�p�󋵂̃A�h���X
				&g_aEnemy[nCntEnemy].nOrbitID,		// �C���f�b�N�X�̃A�h���X
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// �F
				OFFSETTYPE_ENEMY_HEIGHT,			// �I�t�Z�b�g�̎��
				ORBITTYPE_NORMAL					// �e�N�X�`���̎��
			);

			// �G�̑��������Z
			g_nNumEnemy++;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_SPAWN);	// SE (�o��)

			// �����𔲂���
			break;
		}
	}
}

//======================================================================================================================
//	�G�̃_���[�W����
//======================================================================================================================
bool HitEnemy(Enemy *pEnemy, int nDamage, DAMAGETYPE type)
{
	if (pEnemy->state == ACTIONSTATE_NORMAL)
	{ // �G���ʏ��Ԃ̏ꍇ

		// �����̃_���[�W����̗͂��猸�Z
		pEnemy->nLife -= nDamage;

		if (pEnemy->nLife > 0)
		{ // �̗͂��c���Ă���ꍇ

			// �_���[�W��Ԃɂ���
			pEnemy->state = ACTIONSTATE_DAMAGE;

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pEnemy->pos,						// �ʒu
				D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f),	// �F
				PARTICLETYPE_DAMAGE,				// ���
				SPAWN_PARTICLE_DAMAGE,				// �G�t�F�N�g��
				2									// ����
			);

			// �J�E���^�[��ݒ�
			pEnemy->nCounterState = DAMAGE_TIME_ENE;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_DMG);			// SE (�_���[�W)
		}
		else
		{ // �̗͂��s�����ꍇ

			// �X�R�A�̉��Z
			AddScore(g_aEnemyStatus[pEnemy->nType].nScore);

			// �X�R�A�A�b�v
			switch (type)
			{ // �󂯂��U�����Ƃ̏���
			case DAMAGETYPE_SLASH:

				// ����

				// �����𔲂���
				break;

			case DAMAGETYPE_BULLET:

				// �X�R�A�̉��Z
				AddScore(BULL_SCORE_PLUS);

				// �����𔲂���
				break;

			case DAMAGETYPE_KICK:

				// �X�R�A�̉��Z
				AddScore(KICK_SCORE_PLUS);

				// �����𔲂���
				break;
			}

			// �����̐ݒ�
			SetExplosion(pEnemy->pos, SOUNDTYPE_EXPLOSION);

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pEnemy->pos,						// �ʒu
				D3DXCOLOR(0.3f, 0.6f, 1.0f, 1.0f),	// �F
				PARTICLETYPE_EXPLOSION,				// ���
				SPAWN_PARTICLE_EXPLOSION,			// �G�t�F�N�g��
				2									// ����
			);

			// �g�p���Ă��Ȃ���Ԃɂ���
			pEnemy->bUse = false;

			// �G�̑��������Z
			g_nNumEnemy--;

			// �G�̓����������Z
			g_nKillEnemy++;

			// ���S�̐^�U��Ԃ�
			return true;
		}
	}

	// ���S�̐^�U��Ԃ�
	return false;
}

//======================================================================================================================
//	�`���[�g���A���̓G�̍X�V����
//======================================================================================================================
void TutorialEnemy(void)
{
	if (GetLessonState() >= LESSON_06)
	{ // ���b�X��6�ɒ��풆�A�܂��̓N���A���Ă���ꍇ

		// �G�̏o������
		SpawnEnemy();
	}

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // �G���g�p����Ă���ꍇ

			switch (g_aEnemy[nCntEnemy].state)
			{ // ��Ԃ��Ƃ̏���
			case ACTIONSTATE_SPAWN:		// �o�����

				if (g_aEnemy[nCntEnemy].pos.y < 0.0f)
				{ // �o�����̏ꍇ

					// �n������X�|�[��������
					g_aEnemy[nCntEnemy].pos.y += (float)((rand() % (SPAWN_MAX_SPEED - SPAWN_MIN_SPEED + 1)) + SPAWN_MIN_SPEED);
				}
				else
				{ // �o�����������ꍇ

					// �G�̍��W (y) ��␳
					g_aEnemy[nCntEnemy].pos.y = 0.0f;

					// �ʏ��Ԃɂ���
					g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
				}

				// �����𔲂���
				break;

			default:					// ����ȊO�̏��

				// �v���C���[�̕�������������
				LookPlayer(&g_aEnemy[nCntEnemy], GetPlayer());

				// ��Ԃ̍X�V
				if (g_aEnemy[nCntEnemy].nCounterState > 0)
				{ // �J�E���^�[�� 0���傫���ꍇ

					// �J�E���^�[�����Z
					g_aEnemy[nCntEnemy].nCounterState--;

					if (g_aEnemy[nCntEnemy].nCounterState == UNR_TIME_ENE)
					{ // �J�E���^�[�����l�̏ꍇ

						// ���G��Ԃɂ���
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_UNRIVALED;
					}
					else if (g_aEnemy[nCntEnemy].nCounterState <= 0)
					{ // �J�E���^�[�� 0�ȉ��̏ꍇ

						// �ʏ��Ԃɂ���
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
					}
				}

				// �e�̈ʒu�ݒ�
				SetPositionShadow(g_aEnemy[nCntEnemy].nShadowID, g_aEnemy[nCntEnemy].pos);

				// �����𔲂���
				break;
			}
		}
	}
}

//======================================================================================================================
//	�Q�[���̓G�̍X�V����
//======================================================================================================================
void GameEnemy(void)
{
	// �G�̏o������
	SpawnEnemy();

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // �G���g�p����Ă���ꍇ

			switch (g_aEnemy[nCntEnemy].state)
			{ // ��Ԃ��Ƃ̏���
			case ACTIONSTATE_SPAWN:		// �o�����

				if (g_aEnemy[nCntEnemy].pos.y < 0.0f)
				{ // �o�����̏ꍇ

					// �n������X�|�[��������
					g_aEnemy[nCntEnemy].pos.y += (float)((rand() % (SPAWN_MAX_SPEED - SPAWN_MIN_SPEED + 1)) + SPAWN_MIN_SPEED);
				}
				else
				{ // �o�����������ꍇ

					// �G�̍��W (y) ��␳
					g_aEnemy[nCntEnemy].pos.y = 0.0f;

					// �ʏ��Ԃɂ���
					g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
				}

				// �����𔲂���
				break;

			default:					// ����ȊO�̏��

				//------------------------------------------------------------------------------------------------------
				//	�O��ʒu�̍X�V
				//------------------------------------------------------------------------------------------------------
				g_aEnemy[nCntEnemy].oldPos = g_aEnemy[nCntEnemy].pos;

				//------------------------------------------------------------------------------------------------------
				//	��Ԃ̍X�V
				//------------------------------------------------------------------------------------------------------
				if (g_aEnemy[nCntEnemy].nCounterState > 0)
				{ // �J�E���^�[�� 0���傫���ꍇ

					// �J�E���^�[�����Z
					g_aEnemy[nCntEnemy].nCounterState--;

					if (g_aEnemy[nCntEnemy].nCounterState == UNR_TIME_ENE)
					{ // �J�E���^�[�����l�̏ꍇ

						// ���G��Ԃɂ���
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_UNRIVALED;
					}
					else if (g_aEnemy[nCntEnemy].nCounterState <= 0)
					{ // �J�E���^�[�� 0�ȉ��̏ꍇ

						// �ʏ��Ԃɂ���
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
					}
				}

				//------------------------------------------------------------------------------------------------------
				//	�U���̍X�V
				//------------------------------------------------------------------------------------------------------
				// �G�̌��m�͈͂̓����蔻��
				CollisionEnemyFind(&g_aEnemy[nCntEnemy]);

				switch (g_aEnemy[nCntEnemy].nType)
				{ // ��ނ��Ƃ̏���
				case ENEMYTYPE_RANGE_SHORT:	// �ߋ����^�̓G

					// �a���̈ʒu�E�����̐ݒ�
					SetPositionSlash
					( // ����
						g_aEnemy[nCntEnemy].nSlashID,	// �a���̃C���f�b�N�X
						D3DXVECTOR3
						( // ����
							g_aEnemy[nCntEnemy].pos.x,														// �ʒu (x)
							g_aEnemy[nCntEnemy].pos.y + (g_aEnemy[nCntEnemy].pModelData->fHeight * 0.5f),	// �ʒu (y)
							g_aEnemy[nCntEnemy].pos.z														// �ʒu (z)
						),
						g_aEnemy[nCntEnemy].rot			// ����
					);

					// �����𔲂���
					break;

				case ENEMYTYPE_RANGE_LONG:	// �������^�̓G

					// ����

					// �����𔲂���
					break;
				}

				//------------------------------------------------------------------------------------------------------
				//	�G���̍X�V
				//------------------------------------------------------------------------------------------------------
				// �J�E���^�[�����Z
				g_aEnemy[nCntEnemy].nCounterWeed++;

				// �G���̏o������
				SpawnWeed(&g_aEnemy[nCntEnemy]);

				//------------------------------------------------------------------------------------------------------
				//	�����蔻��
				//------------------------------------------------------------------------------------------------------
				// �G�ƃv���C���[�̓����蔻��
				CollisionPlayer(&g_aEnemy[nCntEnemy]);

				// �I�u�W�F�N�g�Ƃ̓����蔻��
				CollisionObject
				( // ����
					&g_aEnemy[nCntEnemy].pos,		// ���݂̈ʒu
					&g_aEnemy[nCntEnemy].oldPos,	// �O��̈ʒu
					ENE_WIDTH,						// ����
					ENE_DEPTH						// ���s
				);

				//------------------------------------------------------------------------------------------------------
				//	�e�̍X�V
				//------------------------------------------------------------------------------------------------------
				// �e�̈ʒu�ݒ�
				SetPositionShadow(g_aEnemy[nCntEnemy].nShadowID, g_aEnemy[nCntEnemy].pos);

				//------------------------------------------------------------------------------------------------------
				//	�O�Ղ̍X�V
				//------------------------------------------------------------------------------------------------------
				//// �O�Ղ̈ʒu�ݒ�
				//SetPositionOrbit
				//( // ����
				//	g_aEnemy[nCntEnemy].nOrbitID,		// �O�Ղ̃C���f�b�N�X
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// �F
				//	OFFSETTYPE_NORMAL					// �I�t�Z�b�g�̎��
				//);

				// �����𔲂���
				break;
			}
		}
	}
}

//======================================================================================================================
//	�G�̏o������
//======================================================================================================================
void SpawnEnemy(void)
{
	// �ϐ���錾
	int         nSpawnEnemy;	// �o����
	int         nSpawnPoint;	// �o���̒��S�ʒu
	D3DXVECTOR3 pos;			// ���S����̃u���ʒu
	D3DXVECTOR3 rot;			// ����
	int         nType;			// ���

	if (g_nNumEnemy < g_enemySpawn.nMinSpawn)
	{ // �G�̌��݂̑�������萔��菭�Ȃ��Ȃ����ꍇ

		// �G�̏o������ݒ�
		nSpawnEnemy = (rand() % (g_enemySpawn.nMaxSpawn - g_enemySpawn.nMinSpawn + 1)) + g_enemySpawn.nMinSpawn;

		for (int nCntEnemy = 0; nCntEnemy < nSpawnEnemy; nCntEnemy++)
		{ // �G�̏o�������J��Ԃ�

			// �o���̒��S�ʒu�������_���ɐݒ�
			nSpawnPoint = rand() % NUM_SPAWN_POS;

			// ���S����̃u���������_���ɐݒ�
			pos = D3DXVECTOR3
			( // ����
				(float)((rand() % (g_enemySpawn.nRandSpawnPos + 1)) - (g_enemySpawn.nRandSpawnPos * 0.5f)),	// �ʒu (x)
				0.0f,																						// �ʒu (y)
				(float)((rand() % (g_enemySpawn.nRandSpawnPos + 1)) - (g_enemySpawn.nRandSpawnPos * 0.5f))	// �ʒu (z)
			);

			// �����������_���ɐݒ�
			rot = D3DXVECTOR3
			( // ����
				0.0f,							// ���� (x)
				((rand() % 628) - 314) * 0.01f,	// ���� (y)
				0.0f							// ���� (z)
			);

			// ��ނ������_���ɐݒ�
			nType = rand() % NUM_ENE_NORMAL;

			// �G�̐ݒ�
			SetEnemy
			( // ����
				g_enemySpawn.aPosSpawn[nSpawnPoint] + pos,	// �ʒu
				rot,										// ����
				nType										// ���
			);
		}
	}
}

//======================================================================================================================
//	�G���̏o������
//======================================================================================================================
void SpawnWeed(Enemy *pEnemy)
{
	// �ϐ���錾
	int         nSpawnWeed;		// �o����
	D3DXVECTOR3 pos;			// �G���W����̃u���ʒu
	D3DXVECTOR3 spawnPos;		// �o���ʒu

	if (pEnemy->nCounterWeed >= g_aEnemyStatus[pEnemy->nType].weed.nCounterSpawn
	&&  GetTime() >= NOT_SPAWN_WEED)
	{ // �J�E���^�[���o���p�x�̒l�ȏ㊎�A�^�C�}�[�����l�ȏ�̏ꍇ

		// �J�E���^�[��������
		pEnemy->nCounterWeed = 0;

		// �G�̏o������ݒ�
		nSpawnWeed = 
		(rand() % (g_aEnemyStatus[pEnemy->nType].weed.nMaxSpawn - g_aEnemyStatus[pEnemy->nType].weed.nMinSpawn + 1)) + g_aEnemyStatus[pEnemy->nType].weed.nMinSpawn;

		for (int nCntWeed = 0; nCntWeed < nSpawnWeed; nCntWeed++)
		{ // �G���̏o�������J��Ԃ�

			// �G���W����̃u���������_���ɐݒ�
			pos = D3DXVECTOR3
			( // ����
				(float)((rand() % (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos + 1)) - (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos * 0.5f)),	// �ʒu (x)
				0.0f,																																	// �ʒu (y)
				(float)((rand() % (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos + 1)) - (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos * 0.5f))	// �ʒu (z)
			);

			// �o���ʒu��ݒ�
			spawnPos = D3DXVECTOR3
			( // ����
				pEnemy->pos.x + pos.x,	// �ʒu (x)
				-WEED_HEIGHT,			// �ʒu (y)
				pEnemy->pos.z + pos.z	// �ʒu (z)
			);

			// �ʒu�̕␳
			if (spawnPos.z > GetLimitStage().fNear - WEED_SPAWN_REV)
			{ // �͈͊O�̏ꍇ (��O)

				// ��O�Ɉʒu��␳
				spawnPos.z = GetLimitStage().fNear - WEED_SPAWN_REV;
			}
			if (spawnPos.z < GetLimitStage().fFar + WEED_SPAWN_REV)
			{ // �͈͊O�̏ꍇ (��)

				// ���Ɉʒu��␳
				spawnPos.z = GetLimitStage().fFar + WEED_SPAWN_REV;
			}
			if (spawnPos.x > GetLimitStage().fRight - WEED_SPAWN_REV)
			{ // �͈͊O�̏ꍇ (�E)

				// �E�Ɉʒu��␳
				spawnPos.x = GetLimitStage().fRight - WEED_SPAWN_REV;
			}
			if (spawnPos.x < GetLimitStage().fLeft + WEED_SPAWN_REV)
			{ // �͈͊O�̏ꍇ (��)

				// ���Ɉʒu��␳
				spawnPos.x = GetLimitStage().fLeft + WEED_SPAWN_REV;
			}

			// �G���̐ݒ�
			SetWeed(spawnPos);
		}
	}
}

//======================================================================================================================
//	�G�ƃv���C���[�̓����蔻��
//======================================================================================================================
void CollisionPlayer(Enemy *pEnemy)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �G�ƃv���C���[�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �G�ƃv���C���[�Ƃ̊Ԃ̋��������߂�
		fLength = (pEnemy->pos.x - pPlayer->pos.x) * (pEnemy->pos.x - pPlayer->pos.x)
				+ (pEnemy->pos.z - pPlayer->pos.z) * (pEnemy->pos.z - pPlayer->pos.z);

		if (fLength < ((pEnemy->pModelData->fRadius + (PLAY_WIDTH * PLAY_WIDTH_MUL)) * (pEnemy->pModelData->fRadius + (PLAY_WIDTH * PLAY_WIDTH_MUL)))	// �~�̓����蔻��
		&&  pEnemy->pos.y + pEnemy->pModelData->fHeight > pPlayer->pos.y																				// �c�̔��� (��)
		&&  pEnemy->pos.y                               < pPlayer->pos.y + PLAY_HEIGHT)																	// �c�̔��� (��)
		{ // �G���v���C���[�ɓ������Ă���ꍇ

			// �v���C���[�̃_���[�W����
			HitPlayer(pPlayer, ENE_BODY_DMG);
		}
	}
}

//======================================================================================================================
//	�G�̌��m�͈͂̓����蔻��
//======================================================================================================================
void CollisionEnemyFind(Enemy *pEnemy)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �v���C���[�ƓG�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �v���C���[�ƓG�Ƃ̊Ԃ̋��������߂�
		fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
				+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

		if (fLength < ((PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fFindRadius) * (PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fFindRadius)))
		{ // �G�̌��m�͈͓��̏ꍇ

			// �v���C���[�̕�������������
			LookPlayer(pEnemy, pPlayer);

			if (CollisionEnemyAttack(pEnemy, pPlayer) == false)
			{ // �G�̍U���͈͊O�̏ꍇ

				// �v���C���[�̕����Ɉړ� (�O�i)
				pEnemy->pos.x -= sinf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fForwardMove;
				pEnemy->pos.z -= cosf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fForwardMove;
			}
			else
			{ // �G�̍U���͈͓��̏ꍇ

				if (CollisionEnemyBackward(pEnemy, pPlayer) == true
				&&  g_aEnemyStatus[pEnemy->nType].bBackward == true)
				{ // �G�̌�ޔ͈͓����A��ނ� ON �̏ꍇ

					// �v���C���[�̋t�����Ɉړ� (���)
					pEnemy->pos.x += sinf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fBackwardMove;
					pEnemy->pos.z += cosf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fBackwardMove;

					// �G�̈ʒu��␳
					RevPosEnemy(pEnemy);
				}

				// �J�E���^�[�����Z
				pEnemy->nCounterAttack++;

				if (pEnemy->nCounterAttack >= g_aEnemyStatus[pEnemy->nType].nCounterAttack)
				{ // �J�E���^�[�����l�ȏ�̏ꍇ

					// �J�E���^�[��������
					pEnemy->nCounterAttack = 0;

					switch (pEnemy->nType)
					{ // ��ނ��Ƃ̏���
					case ENEMYTYPE_RANGE_SHORT:	// �ߋ����^�̓G

						if (pEnemy->nSlashID == NONE_SLASH)
						{ // �a���̃C���f�b�N�X������������Ă���ꍇ

							// �a���̐ݒ�
							pEnemy->nSlashID = SetSlash
							( // ����
								SLASHTYPE_ENEMY,		// ���
								ENE_SLA_LIFE,			// ����
								ENE_SLA_RADIUS,			// ���a
								&pEnemy->nSlashID,		// �a���̐e�̎a���C���f�b�N�X
								&pEnemy->bUse			// �a���̐e�̎g�p��
							);
						}

						// �����𔲂���
						break;

					case ENEMYTYPE_RANGE_LONG:	// �������^�̓G

						// �e�̐ݒ�
						SetBullet
						( // ����
							D3DXVECTOR3
							( // ����
								pEnemy->pos.x + sinf(pEnemy->rot.y + D3DX_PI),			// �ʒu (x)
								pEnemy->pos.y + (pEnemy->pModelData->fHeight * 0.5f),	// �ʒu (y)
								pEnemy->pos.z + cosf(pEnemy->rot.y + D3DX_PI)			// �ʒu (z)
							),
							D3DXVECTOR3
							( // ����
								sinf(pEnemy->rot.y + D3DX_PI) * ENE_BULL_MOVE,			// �ړ��� (x)
								0.0f,													// �ړ��� (y)
								cosf(pEnemy->rot.y + D3DX_PI) * ENE_BULL_MOVE			// �ړ��� (z)
							),
							BULLETTYPE_ENEMY,	// ���
							true				// ���ʉ�
						);

						// �����𔲂���
						break;
					}
				}
			}
		}
	}
}

//======================================================================================================================
//	�G�̍U���͈͂̓����蔻��
//======================================================================================================================
bool CollisionEnemyAttack(Enemy *pEnemy, Player *pPlayer)
{
	// �ϐ���錾
	float fLength  = 0.0f;	// �v���C���[�ƓG�Ƃ̊Ԃ̋���

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �v���C���[�ƓG�Ƃ̊Ԃ̋��������߂�
		fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
				+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

		if (fLength < ((PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fAttackRadius) * (PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fAttackRadius)))
		{ // �G�̍U���͈͓��̏ꍇ

			// �U���͈͓��̏ꍇ true ��Ԃ�
			return true;
		}
	}

	// �U���͈͊O�̏ꍇ false ��Ԃ�
	return false;
}

//======================================================================================================================
//	�G�̌�ޔ͈͂̓����蔻��
//======================================================================================================================
bool CollisionEnemyBackward(Enemy *pEnemy, Player *pPlayer)
{
	// �ϐ���錾
	float fLength  = 0.0f;	// �v���C���[�ƓG�Ƃ̊Ԃ̋���

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �v���C���[�ƓG�Ƃ̊Ԃ̋��������߂�
		fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
				+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

		if (fLength < ((PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fBackwardRadius) * (PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fBackwardRadius)))
		{ // �G�̌�ޔ͈͓��̏ꍇ

			// ��ޔ͈͓��̏ꍇ true ��Ԃ�
			return true;
		}
	}

	// ��ޔ͈͊O�̏ꍇ false ��Ԃ�
	return false;
}

//======================================================================================================================
//	�v���C���[�̕����Ɍ�������
//======================================================================================================================
void LookPlayer(Enemy *pEnemy, Player *pPlayer)
{
	// �ϐ���錾
	float fDestRot = 0.0f;	// �ڕW�̌�������p
	float fDiffRot = 0.0f;	// �����̌v�Z����p

	// �v���C���[�̌�������
	fDestRot = atan2f(pEnemy->pos.x - pPlayer->pos.x, pEnemy->pos.z - pPlayer->pos.z);	// �ڕW�̌���

	// �ڕW�����܂ł̍������v�Z
	fDiffRot = fDestRot - pEnemy->rot.y;

	// ���������̐��K��
	if      (fDiffRot >  D3DX_PI) { fDiffRot -= D3DX_PI * 2; }
	else if (fDiffRot < -D3DX_PI) { fDiffRot += D3DX_PI * 2; }

	// �����̍X�V
	pEnemy->rot.y += fDiffRot * g_aEnemyStatus[pEnemy->nType].fLookRevision;

	// �����̐��K��
	if      (pEnemy->rot.y >  D3DX_PI) { pEnemy->rot.y -= D3DX_PI * 2; }
	else if (pEnemy->rot.y < -D3DX_PI) { pEnemy->rot.y += D3DX_PI * 2; }
}

//======================================================================================================================
//	�G�̈ʒu�̕␳����
//======================================================================================================================
void RevPosEnemy(Enemy *pEnemy)
{
	if (pEnemy->pos.z > GetLimitStage().fNear - pEnemy->pModelData->fRadius)
	{ // �͈͊O�̏ꍇ (��O)

		// ��O�Ɉʒu��␳
		pEnemy->pos.z = GetLimitStage().fNear - pEnemy->pModelData->fRadius;
	}
	if (pEnemy->pos.z < GetLimitStage().fFar + pEnemy->pModelData->fRadius)
	{ // �͈͊O�̏ꍇ (��)

		// ���Ɉʒu��␳
		pEnemy->pos.z = GetLimitStage().fFar + pEnemy->pModelData->fRadius;
	}
	if (pEnemy->pos.x > GetLimitStage().fRight - pEnemy->pModelData->fRadius)
	{ // �͈͊O�̏ꍇ (�E)

		// �E�Ɉʒu��␳
		pEnemy->pos.x = GetLimitStage().fRight - pEnemy->pModelData->fRadius;
	}
	if (pEnemy->pos.x < GetLimitStage().fLeft + pEnemy->pModelData->fRadius)
	{ // �͈͊O�̏ꍇ (��)

		// ���Ɉʒu��␳
		pEnemy->pos.x = GetLimitStage().fLeft + pEnemy->pModelData->fRadius;
	}
}

//======================================================================================================================
//	�G�̃Z�b�g�A�b�v����
//======================================================================================================================
void TxtSetEnemy(void)
{
	// �ϐ���錾
	int nType       = 0;		// ��ނ̑���p
	int nBackward   = 0;		// ��ނ� ON / OFF �̕ϊ��p
	int nSpawnPoint = 0;		// ���݂̃X�|�[�����S�ʒu�̔ԍ�
	int nEnd        = 0;		// �e�L�X�g�ǂݍ��ݏI���̊m�F�p

	// �ϐ��z���錾
	char aString[MAX_STRING];	// �e�L�X�g�̕�����̑���p

	// �|�C���^��錾
	FILE *pFile;				// �t�@�C���|�C���^

	// �t�@�C����ǂݍ��݌`���ŊJ��
	pFile = fopen(ENEMY_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // �t�@�C�����J�����ꍇ

		do
		{ // �ǂݍ��񂾕����� EOF �ł͂Ȃ��ꍇ���[�v

			// �t�@�C�����當�����ǂݍ���
			nEnd = fscanf(pFile, "%s", &aString[0]);	// �e�L�X�g��ǂݍ��݂������� EOF ��Ԃ�

			//----------------------------------------------------------------------------------------------------------
			//	�X�e�[�^�X�̐ݒ�
			//----------------------------------------------------------------------------------------------------------
			if (strcmp(&aString[0], "STATUS_ENEMYSET") == 0)
			{ // �ǂݍ��񂾕����� STATUS_ENEMYSET �̏ꍇ

				do
				{ // �ǂݍ��񂾕����� END_STATUS_ENEMYSET �ł͂Ȃ��ꍇ���[�v

					// �t�@�C�����當�����ǂݍ���
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "ENEMYSET") == 0)
					{ // �ǂݍ��񂾕����� ENEMYSET �̏ꍇ

						do
						{ // �ǂݍ��񂾕����� END_ENEMYSET �ł͂Ȃ��ꍇ���[�v

							// �t�@�C�����當�����ǂݍ���
							fscanf(pFile, "%s", &aString[0]);

							// �G�̃X�e�[�^�X���
							if (strcmp(&aString[0], "TYPE") == 0)
							{ // �ǂݍ��񂾕����� TYPE �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);	// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &nType);		// ��ނ�ǂݍ���
							}
							else if (strcmp(&aString[0], "LIFE") == 0)
							{ // �ǂݍ��񂾕����� LIFE �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].nLife);				// �̗͂�ǂݍ���
							}
							else if (strcmp(&aString[0], "SCORE") == 0)
							{ // �ǂݍ��񂾕����� SCORE �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].nScore);				// �X�R�A���Z�ʂ�ǂݍ���
							}
							else if (strcmp(&aString[0], "ATTACK_CNT") == 0)
							{ // �ǂݍ��񂾕����� ATTACK_CNT �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].nCounterAttack);		// �U���p�x��ǂݍ���
							}
							else if (strcmp(&aString[0], "FORWARD_MOVE") == 0)
							{ // �ǂݍ��񂾕����� FORWARD_MOVE �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fForwardMove);		// �O�i�̈ړ��ʂ�ǂݍ���
							}
							else if (strcmp(&aString[0], "BACKWARD_MOVE") == 0)
							{ // �ǂݍ��񂾕����� BACKWARD_MOVE �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fBackwardMove);		// ��ނ̈ړ��ʂ�ǂݍ���
							}
							else if (strcmp(&aString[0], "LOOK_REV") == 0)
							{ // �ǂݍ��񂾕����� LOOK_REV �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fLookRevision);		// �U������␳�W����ǂݍ���
							}
							else if (strcmp(&aString[0], "FIND_RADIUS") == 0)
							{ // �ǂݍ��񂾕����� FIND_RADIUS �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fFindRadius);		// ���m�͈͂�ǂݍ���
							}
							else if (strcmp(&aString[0], "ATTACK_RADIUS") == 0)
							{ // �ǂݍ��񂾕����� ATTACK_RADIUS �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fAttackRadius);		// �U���͈͂�ǂݍ���
							}
							else if (strcmp(&aString[0], "BACKWARD_RADIUS") == 0)
							{ // �ǂݍ��񂾕����� BACKWARD_RADIUS �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fBackwardRadius);	// ��ޔ͈͂�ǂݍ���
							}
							else if (strcmp(&aString[0], "BACWARD_ENABLE") == 0)
							{ // �ǂݍ��񂾕����� BACWARD_ENABLE �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &nBackward);								// ��ނ� ON / OFF ��ǂݍ���

								// �ǂݍ��񂾒l�� 0�Ȃ� true�A0�ȊO�Ȃ� false����
								g_aEnemyStatus[nType].bBackward = (nBackward == 0) ? true : false;
							}

							// �G���̃X�|�[�����
							else if (strcmp(&aString[0], "WEED_CNTSPAWN") == 0)
							{ // �ǂݍ��񂾕����� WEED_CNTSPAWN �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nCounterSpawn);	// �o���p�x��ǂݍ���
							}
							else if (strcmp(&aString[0], "WEED_MINSPAWN") == 0)
							{ // �ǂݍ��񂾕����� WEED_MINSPAWN �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nMinSpawn);		// �Œ�o������ǂݍ���
							}
							else if (strcmp(&aString[0], "WEED_MAXSPAWN") == 0)
							{ // �ǂݍ��񂾕����� WEED_MAXSPAWN �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nMaxSpawn);		// �ō��o������ǂݍ���
							}
							else if (strcmp(&aString[0], "WEED_RANDSPAWN") == 0)
							{ // �ǂݍ��񂾕����� WEED_RANDSPAWN �̏ꍇ
								fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nRandSpawnPos);	// �o���ʒu�̃u���̔��a��ǂݍ���
							}
						} while (strcmp(&aString[0], "END_ENEMYSET") != 0);	// �ǂݍ��񂾕����� END_ENEMYSET �ł͂Ȃ��ꍇ���[�v
					}
				} while (strcmp(&aString[0], "END_STATUS_ENEMYSET") != 0);	// �ǂݍ��񂾕����� END_STATUS_ENEMYSET �ł͂Ȃ��ꍇ���[�v
			}

			//----------------------------------------------------------------------------------------------------------
			//	�X�|�[���̐ݒ�
			//----------------------------------------------------------------------------------------------------------
			else if (strcmp(&aString[0], "SPAWN_ENEMYSET") == 0)
			{ // �ǂݍ��񂾕����� SPAWN_ENEMYSET �̏ꍇ

				do
				{ // �ǂݍ��񂾕����� END_SPAWN_ENEMYSET �ł͂Ȃ��ꍇ���[�v

					// �t�@�C�����當�����ǂݍ���
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "MINSPAWN") == 0)
					{ // �ǂݍ��񂾕����� MINSPAWN �̏ꍇ

						fscanf(pFile, "%s", &aString[0]);					// = ��ǂݍ��� (�s�v)
						fscanf(pFile, "%d", &g_enemySpawn.nMinSpawn);		// �Œ�o������ǂݍ���
					}
					else if (strcmp(&aString[0], "MAXSPAWN") == 0)
					{ // �ǂݍ��񂾕����� MAXSPAWN �̏ꍇ

						fscanf(pFile, "%s", &aString[0]);					// = ��ǂݍ��� (�s�v)
						fscanf(pFile, "%d", &g_enemySpawn.nMaxSpawn);		// �ō��o������ǂݍ���
					}
					else if (strcmp(&aString[0], "RANDSPAWN") == 0)
					{ // �ǂݍ��񂾕����� RANDSPAWN �̏ꍇ

						fscanf(pFile, "%s", &aString[0]);					// = ��ǂݍ��� (�s�v)
						fscanf(pFile, "%d", &g_enemySpawn.nRandSpawnPos);	// �o���ʒu�̃u���̔��a��ǂݍ���
					}
					else if (strcmp(&aString[0], "POS") == 0)
					{ // �ǂݍ��񂾕����� POS �̏ꍇ
						fscanf(pFile, "%s", &aString[0]);								// = ��ǂݍ��� (�s�v)
						fscanf(pFile, "%f", &g_enemySpawn.aPosSpawn[nSpawnPoint].x);	// �o���̒��S�ʒu (x) ��ǂݍ���
						fscanf(pFile, "%f", &g_enemySpawn.aPosSpawn[nSpawnPoint].y);	// �o���̒��S�ʒu (y) ��ǂݍ���
						fscanf(pFile, "%f", &g_enemySpawn.aPosSpawn[nSpawnPoint].z);	// �o���̒��S�ʒu (z) ��ǂݍ���

						// ���݂̃X�|�[�����S�ʒu�̔ԍ������Z
						nSpawnPoint++;
					}
				} while (strcmp(&aString[0], "END_SPAWN_ENEMYSET") != 0);	// �ǂݍ��񂾕����� END_SPAWN_ENEMYSET �ł͂Ȃ��ꍇ���[�v
			}
		} while (nEnd != EOF);												// �ǂݍ��񂾕����� EOF �ł͂Ȃ��ꍇ���[�v
		
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
//	�G�̓������̎擾����
//======================================================================================================================
int GetKillEnemy(void)
{
	// �G�̓�������Ԃ�
	return g_nKillEnemy;
}

//======================================================================================================================
//	�G�̎擾����
//======================================================================================================================
Enemy *GetEnemy(void)
{
	// �G�̏��̐擪�A�h���X��Ԃ�
	return &g_aEnemy[0];
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�G�̑����擾����
//**********************************************************************************************************************
int GetNumEnemy(void)
{
	// �ϐ���錾
	int nNumEnemy = 0;	// �G�̑����̊m�F�p

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // �G���g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumEnemy++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumEnemy;	// �G�̑���
}
#endif