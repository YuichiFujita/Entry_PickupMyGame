//======================================================================================================================
//
//	�e���� [bullet.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"

#include "game.h"
#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "explosion.h"
#include "meshwall.h"
#include "object.h"
#include "particle.h"
#include "shadow.h"
#include "sound.h"
#include "weed.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define BULL_DMG_PLAY	(100)		// �e�̍U���� (�v���C���[)
#define BULL_DMG_ENE	(10)		// �e�̍U���� (�G)

#define SHADOW_BULL		(30.0f)		// �e�̉e�̔��a

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureBullet[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\bullet000.png",	// �e (�ʏ�) �̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_BULLET)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_BULLET_NORMAL,			// �e (�ʏ�)
	TEXTURE_BULLET_MAX,				// ���̗񋓌^�̑���
} TEXTURE_BULLET;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void CollisionPlayer(Bullet *pBullet);			// �v���C���[�ƒe�̓����蔻��
void CollisionEnemy(Bullet *pBullet);			// �G�ƒe�̓����蔻��
void CollisionWeed(Bullet *pBullet);			// �G���ƒe�̓����蔻��
void CollisionObject(Bullet *pBullet);			// �I�u�W�F�N�g�ƒe�̓����蔻��
void CollisionMeshWall(Bullet *pBullet);		// ���b�V���E�H�[���ƒe�̓����蔻��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureBullet[TEXTURE_BULLET_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBullet = NULL;					// ���_�o�b�t�@�ւ̃|�C���^

Bullet g_aBullet[MAX_BULLET];		// �e�̏��

//======================================================================================================================
//	�e�̏���������
//======================================================================================================================
void InitBullet(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntBullet = 0; nCntBullet < TEXTURE_BULLET_MAX; nCntBullet++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureBullet[nCntBullet], &g_apTextureBullet[nCntBullet]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_3D) * 4 * MAX_BULLET,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffBullet,
		NULL
	);

	// �e�̏��̏�����
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		g_aBullet[nCntBullet].pos       = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���݂̈ʒu
		g_aBullet[nCntBullet].oldPos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �O��̈ʒu
		g_aBullet[nCntBullet].move      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ړ���
		g_aBullet[nCntBullet].type      = BULLETTYPE_PLAYER;				// ���
		g_aBullet[nCntBullet].nLife     = 0;								// ����
		g_aBullet[nCntBullet].nShadowID = NONE_SHADOW;						// �e�̃C���f�b�N�X
		g_aBullet[nCntBullet].bUse      = false;							// �g�p��
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffBullet->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-BULL_WIDTH,  BULL_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( BULL_WIDTH,  BULL_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-BULL_WIDTH, -BULL_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3( BULL_WIDTH, -BULL_HEIGHT, 0.0f);

		// �@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

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
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffBullet->Unlock();
}

//======================================================================================================================
//	�e�̏I������
//======================================================================================================================
void UninitBullet(void)
{
	// �e�N�X�`���̔j��
	for (int nCntBullet = 0; nCntBullet < TEXTURE_BULLET_MAX; nCntBullet++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureBullet[nCntBullet] != NULL)
		{ // �ϐ� (g_apTextureBullet) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureBullet[nCntBullet]->Release();
			g_apTextureBullet[nCntBullet] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffBullet != NULL)
	{ // �ϐ� (g_pVtxBuffBullet) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffBullet->Release();
		g_pVtxBuffBullet = NULL;
	}
}

//======================================================================================================================
//	�e�̍X�V����
//======================================================================================================================
void UpdateBullet(void)
{
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		if (g_aBullet[nCntBullet].bUse == true)
		{ // �e���g�p����Ă���ꍇ

			//----------------------------------------------------------------------------------------------------------
			//	�O��ʒu�̍X�V
			//----------------------------------------------------------------------------------------------------------
			g_aBullet[nCntBullet].oldPos = g_aBullet[nCntBullet].pos;

			//----------------------------------------------------------------------------------------------------------
			//	�ʒu�̍X�V
			//----------------------------------------------------------------------------------------------------------
			g_aBullet[nCntBullet].pos.x += g_aBullet[nCntBullet].move.x;
			g_aBullet[nCntBullet].pos.z += g_aBullet[nCntBullet].move.z;

			//----------------------------------------------------------------------------------------------------------
			//	�����蔻��
			//----------------------------------------------------------------------------------------------------------
			switch (g_aBullet[nCntBullet].type)
			{ // ��ނ��Ƃ̏���
			case BULLETTYPE_PLAYER:		// �u���C���[�̒e

				// �G�ƒe�̓����蔻��
				CollisionEnemy(&g_aBullet[nCntBullet]);

				// �G���ƒe�̓����蔻��
				CollisionWeed(&g_aBullet[nCntBullet]);

				// �����𔲂���
				break;

			case BULLETTYPE_ENEMY:		// �G�̒e

				// �v���C���[�ƒe�̓����蔻��
				CollisionPlayer(&g_aBullet[nCntBullet]);

				// �����𔲂���
				break;
			}

			// �I�u�W�F�N�g�ƒe�̓����蔻��
			CollisionObject(&g_aBullet[nCntBullet]);

			// ���b�V���E�H�[���ƒe�̓����蔻��
			CollisionMeshWall(&g_aBullet[nCntBullet]);

			//----------------------------------------------------------------------------------------------------------
			//	�G�t�F�N�g�̍X�V
			//----------------------------------------------------------------------------------------------------------
			switch (g_aBullet[nCntBullet].type)
			{ // ��ނ��Ƃ̏���
			case BULLETTYPE_PLAYER:		// �u���C���[�̒e

				// �G�t�F�N�g�̐ݒ�
				SetEffect
				( // ����
					g_aBullet[nCntBullet].pos,			// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// �ړ���
					D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),	// �F
					EFFECTTYPE_NORMAL,					// ���
					14,									// ����
					20.0f,								// ���a
					1.0f								// ���Z�� (���a)
				);

				// �G�t�F�N�g�̐ݒ�
				SetEffect
				( // ����
					g_aBullet[nCntBullet].pos,			// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// �ړ���
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f),	// �F
					EFFECTTYPE_NORMAL,					// ���
					14,									// ����
					20.0f,								// ���a
					1.5f								// ���Z�� (���a)
				);

				// �����𔲂���
				break;

			case BULLETTYPE_ENEMY:		// �G�̒e

				// �G�t�F�N�g�̐ݒ�
				SetEffect
				( // ����
					g_aBullet[nCntBullet].pos,			// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// �ړ���
					D3DXCOLOR(0.3f, 0.6f, 1.0f, 1.0f),	// �F
					EFFECTTYPE_NORMAL,					// ���
					14,									// ����
					20.0f,								// ���a
					1.0f								// ���Z�� (���a)
				);

				// �G�t�F�N�g�̐ݒ�
				SetEffect
				( // ����
					g_aBullet[nCntBullet].pos,			// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// �ړ���
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f),	// �F
					EFFECTTYPE_NORMAL,					// ���
					14,									// ����
					20.0f,								// ���a
					1.5f								// ���Z�� (���a)
				);

				// �����𔲂���
				break;
			}

			//----------------------------------------------------------------------------------------------------------
			//	�e�̍X�V
			//----------------------------------------------------------------------------------------------------------
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aBullet[nCntBullet].nShadowID, g_aBullet[nCntBullet].pos);

			//----------------------------------------------------------------------------------------------------------
			//	�e�̍폜
			//----------------------------------------------------------------------------------------------------------
			// ���������炷
			g_aBullet[nCntBullet].nLife--;

			if (g_aBullet[nCntBullet].pos.z > GetLimitStage().fNear
			||  g_aBullet[nCntBullet].pos.z < GetLimitStage().fFar
			||  g_aBullet[nCntBullet].pos.x > GetLimitStage().fRight
			||  g_aBullet[nCntBullet].pos.x < GetLimitStage().fLeft
			||  g_aBullet[nCntBullet].nLife <= 0)
			{ // �e���͈͊O�A�܂��͎������s�����ꍇ

				// �����̐ݒ�
				SetExplosion(g_aBullet[nCntBullet].pos, SOUNDTYPE_EXPLOSION);

				// �p�[�e�B�N���̐ݒ�
				SetParticle
				( // ����
					g_aBullet[nCntBullet].pos,			// �ʒu
					D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),	// �F
					PARTICLETYPE_EXPLOSION,				// ���
					SPAWN_PARTICLE_EXPLOSION,			// �G�t�F�N�g��
					2									// ����
				);

				// �g�p���Ă��Ȃ���Ԃɂ���
				g_aBullet[nCntBullet].bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	�e�̕`�揈��
//======================================================================================================================
void DrawBullet(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxView;		// �r���[�}�g���b�N�X�擾�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// ���e�X�g�̎Q�ƒl�ݒ�

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		if (g_aBullet[nCntBullet].bUse == true)
		{ // �e���g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aBullet[nCntBullet].mtxWorld);

			// �r���[�}�g���b�N�X���擾
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// �|���S�����J�����ɑ΂��Đ��ʂɌ�����
			D3DXMatrixInverse(&g_aBullet[nCntBullet].mtxWorld, NULL, &mtxView);	// �t�s������߂�
			g_aBullet[nCntBullet].mtxWorld._41 = 0.0f;
			g_aBullet[nCntBullet].mtxWorld._42 = 0.0f;
			g_aBullet[nCntBullet].mtxWorld._43 = 0.0f;

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aBullet[nCntBullet].pos.x, g_aBullet[nCntBullet].pos.y, g_aBullet[nCntBullet].pos.z);
			D3DXMatrixMultiply(&g_aBullet[nCntBullet].mtxWorld, &g_aBullet[nCntBullet].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aBullet[nCntBullet].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffBullet, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureBullet[TEXTURE_BULLET_NORMAL]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntBullet * 4, 2);
		}
	}

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ���e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// ���e�X�g�̎Q�ƒl�ݒ�
}

//======================================================================================================================
//	�e�̐ݒ菈��
//======================================================================================================================
void SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 move, BULLETTYPE type, bool bSound)
{
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		if (g_aBullet[nCntBullet].bUse == false)
		{ // �e���g�p����Ă��Ȃ��ꍇ

			// ��������
			g_aBullet[nCntBullet].pos    = pos;		// ���݂̈ʒu
			g_aBullet[nCntBullet].oldPos = pos;		// �O��̈ʒu
			g_aBullet[nCntBullet].move   = move;	// �ړ���
			g_aBullet[nCntBullet].type   = type;	// ���

			// ������ݒ�
			g_aBullet[nCntBullet].nLife = LIFE_BULLET;

			// �g�p���Ă����Ԃɂ���
			g_aBullet[nCntBullet].bUse = true;

			// �e�̃C���f�b�N�X��ݒ�
			g_aBullet[nCntBullet].nShadowID = SetShadow(0.5f, SHADOW_BULL, &g_aBullet[nCntBullet].nShadowID, &g_aBullet[nCntBullet].bUse);

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aBullet[nCntBullet].nShadowID, g_aBullet[nCntBullet].pos);

			if (bSound == true)
			{ // ���ʉ��̍Đ��� ON �̏ꍇ

				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_SHOT);		// SE (����)
			}

			// �����𔲂���
			break;
		}
	}
}

//======================================================================================================================
//	�v���C���[�ƒe�̓����蔻��
//======================================================================================================================
void CollisionPlayer(Bullet *pBullet)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �e�ƃv���C���[�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �e�ƃv���C���[�Ƃ̊Ԃ̋��������߂�
		fLength = (pBullet->pos.x - pPlayer->pos.x) * (pBullet->pos.x - pPlayer->pos.x)
				+ (pBullet->pos.z - pPlayer->pos.z) * (pBullet->pos.z - pPlayer->pos.z);

		if (fLength < ((BULL_WIDTH + PLAY_WIDTH) * (BULL_WIDTH + PLAY_WIDTH))	// �~�̓����蔻��
		&&  pBullet->pos.y + BULL_HEIGHT > pPlayer->pos.y						// �c�̔��� (��)
		&&  pBullet->pos.y - BULL_HEIGHT < pPlayer->pos.y + PLAY_HEIGHT)		// �c�̔��� (��)
		{ // �e���v���C���[�ɓ������Ă���ꍇ

			// �v���C���[�̃_���[�W����
			HitPlayer(pPlayer, BULL_DMG_ENE);

			// �g�p���Ă��Ȃ���Ԃɂ���
			pBullet->bUse = false;
		}
	}
}

//======================================================================================================================
//	�G�ƒe�̓����蔻��
//======================================================================================================================
void CollisionEnemy(Bullet *pBullet)
{
	// �ϐ���錾
	float fLength = 0.0f;			// �e�ƓG�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Enemy *pEnemy = GetEnemy();		// �G�̏��

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (pEnemy->bUse == true)
		{ // �G���g�p����Ă���ꍇ

			// �e�ƓG�Ƃ̊Ԃ̋��������߂�
			fLength = (pBullet->pos.x - pEnemy->pos.x) * (pBullet->pos.x - pEnemy->pos.x)
					+ (pBullet->pos.z - pEnemy->pos.z) * (pBullet->pos.z - pEnemy->pos.z);

			if (fLength < ((BULL_WIDTH + pEnemy->pModelData->fRadius) * (BULL_WIDTH + pEnemy->pModelData->fRadius))	// �~�̓����蔻��
			&&  pBullet->pos.y + BULL_HEIGHT > pEnemy->pos.y														// �c�̔��� (��)
			&&  pBullet->pos.y - BULL_HEIGHT < pEnemy->pos.y + pEnemy->pModelData->fHeight)							// �c�̔��� (��)
			{ // �e���G�ɓ������Ă���ꍇ

				// �G�̃_���[�W����
				HitEnemy(pEnemy, BULL_DMG_PLAY, DAMAGETYPE_BULLET);

				// �g�p���Ă��Ȃ���Ԃɂ���
				pBullet->bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	�G���ƒe�̓����蔻��
//======================================================================================================================
void CollisionWeed(Bullet *pBullet)
{
	// �ϐ���錾
	float fLength = 0.0f;		// �e�ƎG���Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Weed *pWeed = GetWeed();	// �G���̏��

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++, pWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		if (pWeed->bUse == true)
		{ // �G�����g�p����Ă���ꍇ

			// �e�ƎG���Ƃ̊Ԃ̋��������߂�
			fLength = (pBullet->pos.x - pWeed->pos.x) * (pBullet->pos.x - pWeed->pos.x)
					+ (pBullet->pos.z - pWeed->pos.z) * (pBullet->pos.z - pWeed->pos.z);

			if (fLength < ((BULL_WIDTH + WEED_WIDTH) * (BULL_WIDTH + WEED_WIDTH)))
			{ // �e���G���ɓ������Ă���ꍇ

				// �G���̃_���[�W����
				HitWeed(pWeed, BULL_DMG_PLAY);
			}
		}
	}
}

//======================================================================================================================
//	�I�u�W�F�N�g�ƒe�̓����蔻��
//======================================================================================================================
void CollisionObject(Bullet *pBullet)
{
	// �|�C���^��錾
	Object *pObject = GetObjectData();	// �I�u�W�F�N�g�̏��

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{ // �I�u�W�F�N�g�̍ő�\�������J��Ԃ�

		if (pObject->bUse == true)
		{ // �I�u�W�F�N�g���g�p����Ă���ꍇ

			if (pBullet->pos.x + BULL_WIDTH  > pObject->pos.x + pObject->pModelData->vtxMin.x
			&&  pBullet->pos.y + BULL_HEIGHT > pObject->pos.y + pObject->pModelData->vtxMin.y
			&&  pBullet->pos.z + BULL_DEPTH  > pObject->pos.z + pObject->pModelData->vtxMin.z
			&&  pBullet->pos.x - BULL_WIDTH  < pObject->pos.x + pObject->pModelData->vtxMax.x
			&&  pBullet->pos.y - BULL_HEIGHT < pObject->pos.y + pObject->pModelData->vtxMax.y
			&&  pBullet->pos.z - BULL_DEPTH  < pObject->pos.z + pObject->pModelData->vtxMax.z)
			{ // �e���I�u�W�F�N�g�ɓ������Ă���ꍇ

				if (pBullet->type == BULLETTYPE_PLAYER && pObject->nBreakType != BREAKTYPE_NONE)
				{ // �e�̎�ނ��v���C���[���A�I�u�W�F�N�g�̉����̎�ނ����Ȃ���ނł͂Ȃ��ꍇ

					// �I�u�W�F�N�g�̃_���[�W����
					HitObject(pObject, BULL_DMG_PLAY);
				}

				// �����̐ݒ�
				SetExplosion(pBullet->pos, SOUNDTYPE_EXPLOSION);

				// �p�[�e�B�N���̐ݒ�
				SetParticle
				( // ����
					pBullet->pos,						// �ʒu
					D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),	// �F
					PARTICLETYPE_EXPLOSION,				// ���
					SPAWN_PARTICLE_EXPLOSION,			// �G�t�F�N�g��
					2									// ����
				);

				// �g�p���Ă��Ȃ���Ԃɂ���
				pBullet->bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	���b�V���E�H�[���ƒe�̓����蔻��
//======================================================================================================================
void CollisionMeshWall(Bullet *pBullet)
{
	// �ϐ���錾
	D3DXVECTOR3 posLeft;		// ���b�V���E�H�[���̍��[�̍��W
	D3DXVECTOR3 posRight;		// ���b�V���E�H�[���̉E�[�̍��W
	D3DXVECTOR3 vecMove;		// �ړ��x�N�g��
	D3DXVECTOR3 vecLine;		// ���E���x�N�g��
	D3DXVECTOR3 vecToPos;		// ���b�V���E�H�[���̍��[�ƒe�̌��݈ʒu�̃x�N�g��
	D3DXVECTOR3 vecToOldPos;	// ���b�V���E�H�[���̍��[�ƒe�̉ߋ��ʒu�̃x�N�g��
	D3DXVECTOR3 posCloss;		// �����̈ʒu
	float       fSmallArea;		// ���݂̖ʐ�
	float       fBigArea;		// �ő�̖ʐ�
	float       fRate;			// �ʐς̊���

	// �|�C���^��錾
	MeshWall *pMeshWall = GetMeshWall();	// ���b�V���E�H�[���̏��

	for (int nCntObject = 0; nCntObject < MAX_MESHWALL; nCntObject++, pMeshWall++)
	{ // ���b�V���E�H�[���̍ő�\�������J��Ԃ�

		if (pMeshWall->bUse == true)
		{ // ���b�V���E�H�[�����g�p����Ă���ꍇ
	
			// ���b�V���E�H�[���̍��[�̍��W�����߂�
			posLeft = D3DXVECTOR3
			( // ����
				pMeshWall->pos.x - sinf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f),	// x
				0.0f,																						// y
				pMeshWall->pos.z - cosf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f)	// z
			);

			// ���b�V���E�H�[���̉E�[�̍��W�����߂�
			posRight = D3DXVECTOR3
			( // ����
				pMeshWall->pos.x + sinf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f),	// x
				0.0f,																						// y
				pMeshWall->pos.z + cosf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f)	// z
			);

			// �ړ��x�N�g�������߂�
			vecMove = pBullet->pos - pBullet->oldPos;

			// ���E���x�N�g�������߂�
			vecLine = posRight - posLeft;

			// ���b�V���E�H�[���̍��[�ƒe�̌��݈ʒu�̃x�N�g�������߂�
			vecToPos = pBullet->pos - posLeft;

			// ���b�V���E�H�[���̍��[�ƒe�̌��݈ʒu�̃x�N�g�������߂�
			vecToOldPos = pBullet->oldPos - posLeft;

			// vecToPos �� vecMove �łł������s�l�ӌ`�̖ʐς����߂�
			fSmallArea = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

			// vecLine �� vecMove �łł������s�l�ӌ`�̖ʐς����߂�
			fBigArea = (vecLine.z  * vecMove.x) - (vecLine.x  * vecMove.z);

			// �ʐς̊��������߂�
			fRate = fSmallArea / fBigArea;

			if ((vecLine.z * vecToOldPos.x) - (vecLine.x * vecToOldPos.z) > 0			// �e�̑O��̈ʒu�����E���x�N�g�����猩�ĉE�ɂ���ꍇ
			&&  (vecLine.z * vecToPos.x   ) - (vecLine.x * vecToPos.z   ) < 0			// �e�̌��݂̈ʒu�����E���x�N�g�����猩�č��ɂ���ꍇ
			&&  pBullet->pos.y - BULL_HEIGHT < pMeshWall->pos.y + pMeshWall->fHeight	// �e�̌��݂̈ʒu (y) ���ǂ̈ʒu (y) ��艺�ɂ���ꍇ
			&&  pBullet->pos.y + BULL_HEIGHT > pMeshWall->pos.y)						// �e�̌��݂̈ʒu (y) ���ǂ̈ʒu (y) ����ɂ���ꍇ
			{ // �e�����E���ɓ������Ă���ꍇ

				if (fRate >= 0.0f && fRate <= 1.0f)
				{ // �e�����b�V���E�H�[���ɓ������Ă���ꍇ (������ 0.0f �` 1.0f �͈͓̔�)

					// �����̈ʒu��ݒ�
					posCloss = D3DXVECTOR3
					( // ����
						posLeft.x + (vecLine.x * fRate),	// �ړ��x�N�g���Ƌ��E���x�N�g���̌������W (x)
						pBullet->pos.y,						// �e�̍��W (y)
						posLeft.z + (vecLine.z * fRate)		// �ړ��x�N�g���Ƌ��E���x�N�g���̌������W (z)
					);

					// �����̐ݒ�
					SetExplosion(posCloss, SOUNDTYPE_EXPLOSION);

					// �p�[�e�B�N���̐ݒ�
					SetParticle
					( // ����
						posCloss,							// �ʒu
						D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),	// �F
						PARTICLETYPE_EXPLOSION,				// ���
						SPAWN_PARTICLE_EXPLOSION,			// �G�t�F�N�g��
						2									// ����
					);

					// �g�p���Ă��Ȃ���Ԃɂ���
					pBullet->bUse = false;
				}
			}
		}
	}
}

//======================================================================================================================
//	�e�̎擾����
//======================================================================================================================
Bullet *GetBullet(void)
{
	// �e�̏��̐擪�A�h���X��Ԃ�
	return &g_aBullet[0];
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�e�̑����擾����
//**********************************************************************************************************************
int GetNumBullet(void)
{
	// �ϐ���錾
	int nNumBullet = 0;	// �e�̑����̊m�F�p

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		if (g_aBullet[nCntBullet].bUse == true)
		{ // �e���g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumBullet++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumBullet;	// �e�̑���
}
#endif