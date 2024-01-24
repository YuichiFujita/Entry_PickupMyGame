//======================================================================================================================
//
//	�A�C�e������ [item.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "input.h"

#include "item.h"
#include "particle.h"
#include "player.h"
#include "shadow.h"
#include "sound.h"
#include "tutorial.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define ITEM_HEAL			(100)	// �A�C�e���̉񕜗�
#define EFFECT_TIME_ITEM	(160)	// �p�[�e�B�N�����o���Ԋu
#define MOVE_ROT_ITEM		(0.03f)	// �A�C�e���̉�]��

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apModelItem[] =		// ���f���̑��΃p�X
{
	"02_data\\03_MODEL\\04_ITEM\\heal000.x",	// �񕜃A�C�e���̃��f�����΃p�X
};

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void TutorialItem(void);			// �`���[�g���A���̃A�C�e���̍X�V����
void GameItem(void);				// �Q�[���̃A�C�e���̍X�V����

void CollisionPlayer(Item *pItem);	// �A�C�e���ƃv���C���[�̓����蔻��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
Model g_aModelItem[ITEMTYPE_MAX];	// �A�C�e���̃��f�����
Item  g_aItem[MAX_ITEM];			// �A�C�e���̏��

//======================================================================================================================
//	�A�C�e���̏���������
//======================================================================================================================
void InitItem(void)
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

	// �A�C�e���̃��f�����̏�����
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		g_aModelItem[nCntItem].apTexture[MAX_MODEL_TEXTURE] = {};						// �e�N�X�`���ւ̃|�C���^
		g_aModelItem[nCntItem].pMesh    = NULL;											// ���b�V�� (���_���) �ւ̃|�C���^
		g_aModelItem[nCntItem].pBuffMat = NULL;											// �}�e���A���ւ̃|�C���^
		g_aModelItem[nCntItem].dwNumMat = 0;											// �}�e���A���̐�
		g_aModelItem[nCntItem].vtxMin   = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// �ŏ��̒��_���W
		g_aModelItem[nCntItem].vtxMax   = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// �ő�̒��_���W
		g_aModelItem[nCntItem].fHeight  = 0.0f;											// �c��
		g_aModelItem[nCntItem].fRadius  = 0.0f;											// ���a
	}

	// �A�C�e���̏��̏�����
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		g_aItem[nCntItem].pos            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
		g_aItem[nCntItem].rot            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
		g_aItem[nCntItem].pModelData     = NULL;							// ���f�����
		g_aItem[nCntItem].nType          = 0;								// ���
		g_aItem[nCntItem].nCounterEffect = 0;								// �G�t�F�N�g�Ǘ��J�E���^�[
		g_aItem[nCntItem].nShadowID      = NONE_SHADOW;						// �e�̃C���f�b�N�X
		g_aItem[nCntItem].bUse           = false;							// �g�p��
	}

	// x�t�@�C���̓ǂݍ���
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		D3DXLoadMeshFromX
		( // ����
			apModelItem[nCntItem],				// ���f���̑��΃p�X
			D3DXMESH_SYSTEMMEM,
			pDevice,							// �f�o�C�X�ւ̃|�C���^
			NULL,
			&g_aModelItem[nCntItem].pBuffMat,	// �}�e���A���ւ̃|�C���^
			NULL,
			&g_aModelItem[nCntItem].dwNumMat,	// �}�e���A���̐�
			&g_aModelItem[nCntItem].pMesh		// ���b�V�� (���_���) �ւ̃|�C���^
		);
	}

	// �����蔻��̍쐬
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		// ���f���̒��_�����擾
		nNumVtx = g_aModelItem[nCntItem].pMesh->GetNumVertices();

		// ���f���̒��_�t�H�[�}�b�g�̃T�C�Y���擾
		dwSizeFVF = D3DXGetFVFVertexSize(g_aModelItem[nCntItem].pMesh->GetFVF());

		// ���f���̒��_�o�b�t�@�����b�N
		g_aModelItem[nCntItem].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{ // ���f���̒��_�����J��Ԃ�

			// ���f���̒��_���W����
			vtx = *(D3DXVECTOR3*)pVtxBuff;

			// ���_���W (x) �̐ݒ�
			if (vtx.x < g_aModelItem[nCntItem].vtxMin.x)
			{ // ����̒��_���W (x) ���A����̒��_���W (x) �����������ꍇ

				// ����̒��_��� (x) ����
				g_aModelItem[nCntItem].vtxMin.x = vtx.x;
			}
			else if (vtx.x > g_aModelItem[nCntItem].vtxMax.x)
			{ // ����̒��_���W (x) ���A����̒��_���W (x) �����傫���ꍇ

				// ����̒��_��� (x) ����
				g_aModelItem[nCntItem].vtxMax.x = vtx.x;
			}

			// ���_���W (y) �̐ݒ�
			if (vtx.y < g_aModelItem[nCntItem].vtxMin.y)
			{ // ����̒��_���W (y) ���A����̒��_���W (y) �����������ꍇ

				// ����̒��_��� (y) ����
				g_aModelItem[nCntItem].vtxMin.y = vtx.y;
			}
			else if (vtx.y > g_aModelItem[nCntItem].vtxMax.y)
			{ // ����̒��_���W (y) ���A����̒��_���W (y) �����傫���ꍇ

				// ����̒��_��� (y) ����
				g_aModelItem[nCntItem].vtxMax.y = vtx.y;
			}

			// ���_���W (z) �̐ݒ�
			if (vtx.z < g_aModelItem[nCntItem].vtxMin.z)
			{ // ����̒��_���W (z) ���A����̒��_���W (z) �����������ꍇ

				// ����̒��_��� (z) ����
				g_aModelItem[nCntItem].vtxMin.z = vtx.z;
			}
			else if (vtx.z > g_aModelItem[nCntItem].vtxMax.z)
			{ // ����̒��_���W (z) ���A����̒��_���W (z) �����傫���ꍇ

				// ����̒��_��� (z) ����
				g_aModelItem[nCntItem].vtxMax.z = vtx.z;
			}

			// ���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
			pVtxBuff += dwSizeFVF;
		}

		// ���f���̒��_�o�b�t�@���A�����b�N
		g_aModelItem[nCntItem].pMesh->UnlockVertexBuffer();

		// ���f���T�C�Y�����߂�
		size = g_aModelItem[nCntItem].vtxMax - g_aModelItem[nCntItem].vtxMin;

		// ���f���̏c������
		g_aModelItem[nCntItem].fHeight = size.y;

		// ���f���̉~�̓����蔻����쐬
		g_aModelItem[nCntItem].fRadius = ((size.x * 0.5f) + (size.z * 0.5f)) * 0.5f;
	}

	// �e�N�X�`���̓ǂݍ���
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		// �}�e���A�����ɑ΂���|�C���^���擾
		pMat = (D3DXMATERIAL*)g_aModelItem[nCntItem].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aModelItem[nCntItem].dwNumMat; nCntMat++)
		{ // �}�e���A���̐����J��Ԃ�

			if (pMat[nCntMat].pTextureFilename != NULL)
			{ // �e�N�X�`���t�@�C�������݂���ꍇ

				// �e�N�X�`���̓ǂݍ���
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aModelItem[nCntItem].apTexture[nCntMat]);
			}
		}
	}
}

//======================================================================================================================
//	�A�C�e���̏I������
//======================================================================================================================
void UninitItem(void)
{
	// �e�N�X�`���̔j��
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		for (int nCntMat = 0; nCntMat < MAX_MODEL_TEXTURE; nCntMat++)
		{ // �g�p����e�N�X�`�������J��Ԃ�

			if (g_aModelItem[nCntItem].apTexture[nCntMat] != NULL)
			{ // �ϐ� (g_aModelItem[nCntItem].apTexture) ��NULL�ł͂Ȃ��ꍇ

				g_aModelItem[nCntItem].apTexture[nCntMat]->Release();
				g_aModelItem[nCntItem].apTexture[nCntMat] = NULL;
			}
		}
	}

	// ���b�V���̔j��
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		if (g_aModelItem[nCntItem].pMesh != NULL)
		{ // �ϐ� (g_aModelItem[nCntItem].pMesh) ��NULL�ł͂Ȃ��ꍇ

			g_aModelItem[nCntItem].pMesh->Release();
			g_aModelItem[nCntItem].pMesh = NULL;
		}
	}

	// �}�e���A���̔j��
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // �A�C�e���̎�ޕ��J��Ԃ�

		if (g_aModelItem[nCntItem].pBuffMat != NULL)
		{ // �ϐ� (g_aModelItem[nCntItem].pBuffMat) ��NULL�ł͂Ȃ��ꍇ

			g_aModelItem[nCntItem].pBuffMat->Release();
			g_aModelItem[nCntItem].pBuffMat = NULL;
		}
	}
}

//======================================================================================================================
//	�A�C�e���̍X�V����
//======================================================================================================================
void UpdateItem(void)
{
	switch (GetMode())
	{ // ���[�h���Ƃ̍X�V
	case MODE_TUTORIAL:	// �`���[�g���A�����

		// �`���[�g���A���̃A�C�e���̍X�V
		TutorialItem();

		// �����𔲂���
		break;

	case MODE_GAME:		// �Q�[�����

		// �Q�[���̃A�C�e���̍X�V
		GameItem();

		// �����𔲂���
		break;
	}
}

//======================================================================================================================
//	�A�C�e���̕`�揈��
//======================================================================================================================
void DrawItem(void)
{
	// �ϐ���錾
	D3DXMATRIX   mtxRot, mtxTrans;				// �v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;						// ���݂̃}�e���A���ۑ��p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	D3DXMATERIAL     *pMat;						// �}�e���A���f�[�^�ւ̃|�C���^

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		if (g_aItem[nCntItem].bUse == true)
		{ // �A�C�e�����g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aItem[nCntItem].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aItem[nCntItem].rot.y, g_aItem[nCntItem].rot.x, g_aItem[nCntItem].rot.z);
			D3DXMatrixMultiply(&g_aItem[nCntItem].mtxWorld, &g_aItem[nCntItem].mtxWorld, &mtxRot);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aItem[nCntItem].pos.x, g_aItem[nCntItem].pos.y, g_aItem[nCntItem].pos.z);
			D3DXMatrixMultiply(&g_aItem[nCntItem].mtxWorld, &g_aItem[nCntItem].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aItem[nCntItem].mtxWorld);

			// ���݂̃}�e���A�����擾
			pDevice->GetMaterial(&matDef);

			// �}�e���A���f�[�^�ւ̃|�C���^���擾
			pMat = (D3DXMATERIAL*)g_aItem[nCntItem].pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aItem[nCntItem].pModelData->dwNumMat; nCntMat++)
			{ // �}�e���A���̐����J��Ԃ�

				// �}�e���A���̐ݒ�
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// �e�N�X�`���̐ݒ�
				pDevice->SetTexture(0, g_aItem[nCntItem].pModelData->apTexture[nCntMat]);

				// ���f���̕`��
				g_aItem[nCntItem].pModelData->pMesh->DrawSubset(nCntMat);
			}

			// �ۑ����Ă����}�e���A����߂�
			pDevice->SetMaterial(&matDef);
		}
	}
}

//======================================================================================================================
//	�A�C�e���̐ݒ菈��
//======================================================================================================================
void SetItem(D3DXVECTOR3 pos, int nType)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		if (g_aItem[nCntItem].bUse == false)
		{ // �A�C�e�����g�p����Ă��Ȃ��ꍇ

			// ��������
			g_aItem[nCntItem].pos    = pos;		// �ʒu
			g_aItem[nCntItem].nType  = nType;	// ���

			// �A�C�e���̏���������
			g_aItem[nCntItem].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
			g_aItem[nCntItem].nCounterEffect = 0;					// �G�t�F�N�g�Ǘ��J�E���^�[

			// �g�p���Ă����Ԃɂ���
			g_aItem[nCntItem].bUse = true;

			// ���f������ݒ�
			g_aItem[nCntItem].pModelData = &g_aModelItem[nType];	// ���f�����

			// �e�̃C���f�b�N�X��ݒ�
			g_aItem[nCntItem].nShadowID = SetShadow
			( // ����
				0.5f,																									// ���l
				fabsf(g_aModelItem[g_aItem[nCntItem].nType].vtxMax.x - g_aModelItem[g_aItem[nCntItem].nType].vtxMin.x),	// ���a
				&g_aItem[nCntItem].nShadowID,																			// �e�̐e�̉e�C���f�b�N�X
				&g_aItem[nCntItem].bUse																					// �e�̐e�̎g�p��
			);

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aItem[nCntItem].nShadowID, g_aItem[nCntItem].pos);

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_SPAWN);	// SE (�o��)

			// �����𔲂���
			break;
		}
	}
}

//======================================================================================================================
//	�A�C�e���ƃv���C���[�̓����蔻��
//======================================================================================================================
void CollisionPlayer(Item *pItem)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �A�C�e���ƃv���C���[�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �A�C�e���ƃv���C���[�Ƃ̊Ԃ̋��������߂�
		fLength = (pItem->pos.x - pPlayer->pos.x) * (pItem->pos.x - pPlayer->pos.x)
				+ (pItem->pos.z - pPlayer->pos.z) * (pItem->pos.z - pPlayer->pos.z);

		if (fLength < ((pItem->pModelData->fRadius + PLAY_WIDTH) * (pItem->pModelData->fRadius + PLAY_WIDTH)))
		{ // �A�C�e�����v���C���[�ɓ������Ă���ꍇ

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pItem->pos,							// �ʒu
				D3DXCOLOR(0.6f, 0.2f, 0.55f, 1.0f),	// �F
				PARTICLETYPE_GETHEAL,				// ���
				SPAWN_PARTICLE_GETHEAL,				// �G�t�F�N�g��
				1									// ����
			);

			// �v���C���[�̉񕜔���
			HealPlayer(pPlayer, ITEM_HEAL);

			// �g�p���Ă��Ȃ���Ԃɂ���
			pItem->bUse = false;
		}
	}
}

//======================================================================================================================
//	�`���[�g���A���̃A�C�e���̍X�V����
//======================================================================================================================
void TutorialItem(void)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		if (g_aItem[nCntItem].bUse == true)
		{ // �A�C�e�����g�p����Ă���ꍇ

			//--------------------------------------------------------------------------------------------------------------
			//	�����̍X�V
			//--------------------------------------------------------------------------------------------------------------
			// �����̍X�V
			g_aItem[nCntItem].rot.y += MOVE_ROT_ITEM;

			// �����̐��K��
			if      (g_aItem[nCntItem].rot.y >  D3DX_PI) { g_aItem[nCntItem].rot.y -= D3DX_PI * 2; }
			else if (g_aItem[nCntItem].rot.y < -D3DX_PI) { g_aItem[nCntItem].rot.y += D3DX_PI * 2; }

			//--------------------------------------------------------------------------------------------------------------
			//	�p�[�e�B�N���̍X�V
			//--------------------------------------------------------------------------------------------------------------
			if (g_aItem[nCntItem].nCounterEffect < EFFECT_TIME_ITEM)
			{ // �J�E���^�[�����l��菬�����ꍇ

				// �J�E���^�[�����Z
				g_aItem[nCntItem].nCounterEffect++;
			}
			else
			{ // �J�E���^�[�����l�ȏ�̏ꍇ

				// �J�E���^�[��������
				g_aItem[nCntItem].nCounterEffect = 0;

				// �p�[�e�B�N���̐ݒ�
				SetParticle
				( // ����
					g_aItem[nCntItem].pos,				// �ʒu
					D3DXCOLOR(1.0f, 0.2f, 0.6f, 1.0f),	// �F
					PARTICLETYPE_HEAL,					// ���
					SPAWN_PARTICLE_HEAL,				// �G�t�F�N�g��
					1									// ����
				);
			}

			//------------------------------------------------------------------------------------------------------
			//	�����蔻��
			//------------------------------------------------------------------------------------------------------
			if (GetLessonState() >= LESSON_05)
			{ // ���b�X��5�ɒ��풆�A�܂��̓N���A���Ă���ꍇ

				// �A�C�e���ƃv���C���[�̓����蔻��
				CollisionPlayer(&g_aItem[nCntItem]);
			}

			//------------------------------------------------------------------------------------------------------
			//	�e�̍X�V
			//------------------------------------------------------------------------------------------------------
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aItem[nCntItem].nShadowID, g_aItem[nCntItem].pos);
		}
	}
}

//======================================================================================================================
//	�Q�[���̃A�C�e���̍X�V����
//======================================================================================================================
void GameItem(void)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		if (g_aItem[nCntItem].bUse == true)
		{ // �A�C�e�����g�p����Ă���ꍇ

			//--------------------------------------------------------------------------------------------------------------
			//	�����̍X�V
			//--------------------------------------------------------------------------------------------------------------
			// �����̍X�V
			g_aItem[nCntItem].rot.y += MOVE_ROT_ITEM;

			// �����̐��K��
			if      (g_aItem[nCntItem].rot.y >  D3DX_PI) { g_aItem[nCntItem].rot.y -= D3DX_PI * 2; }
			else if (g_aItem[nCntItem].rot.y < -D3DX_PI) { g_aItem[nCntItem].rot.y += D3DX_PI * 2; }

			//--------------------------------------------------------------------------------------------------------------
			//	�p�[�e�B�N���̍X�V
			//--------------------------------------------------------------------------------------------------------------
			if (g_aItem[nCntItem].nCounterEffect < EFFECT_TIME_ITEM)
			{ // �J�E���^�[�����l��菬�����ꍇ

				// �J�E���^�[�����Z
				g_aItem[nCntItem].nCounterEffect++;
			}
			else
			{ // �J�E���^�[�����l�ȏ�̏ꍇ

				// �J�E���^�[��������
				g_aItem[nCntItem].nCounterEffect = 0;

				// �p�[�e�B�N���̐ݒ�
				SetParticle
				( // ����
					g_aItem[nCntItem].pos,				// �ʒu
					D3DXCOLOR(1.0f, 0.2f, 0.6f, 1.0f),	// �F
					PARTICLETYPE_HEAL,					// ���
					SPAWN_PARTICLE_HEAL,				// �G�t�F�N�g��
					1									// ����
				);
			}

			//------------------------------------------------------------------------------------------------------
			//	�����蔻��
			//------------------------------------------------------------------------------------------------------
			// �A�C�e���ƃv���C���[�̓����蔻��
			CollisionPlayer(&g_aItem[nCntItem]);

			//------------------------------------------------------------------------------------------------------
			//	�e�̍X�V
			//------------------------------------------------------------------------------------------------------
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aItem[nCntItem].nShadowID, g_aItem[nCntItem].pos);
		}
	}
}

//======================================================================================================================
//	�A�C�e���̎擾����
//======================================================================================================================
Item *GetItem(void)
{
	// �A�C�e���̏��̐擪�A�h���X��Ԃ�
	return &g_aItem[0];
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�A�C�e���̑����擾����
//**********************************************************************************************************************
int GetNumItem(void)
{
	// �ϐ���錾
	int nNumItem = 0;	// �A�C�e���̑����̊m�F�p

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // �A�C�e���̍ő�\�������J��Ԃ�

		if (g_aItem[nCntItem].bUse == true)
		{ // �A�C�e�����g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumItem++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumItem;	// �A�C�e���̑���
}
#endif