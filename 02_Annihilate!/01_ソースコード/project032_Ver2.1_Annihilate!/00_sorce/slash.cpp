//======================================================================================================================
//
//	�a������ [slash.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "slash.h"

#include "bullet.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "object.h"
#include "sound.h"
#include "weed.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_SLASH		(32)		// �g�p����|���S���� (�a���̍ő吔)

#define REV_SLASH_ROT	(0.09f)		// �a���̉�]�ʂ̕␳�W��
#define HIT_SLA_HEIGHT	(5.0f)		// �a���̏c�� / 2

#define SLA_DMG_PLAY	(25)		// �a���̍U���� (�v���C���[)
#define SLA_DMG_ENE		(15)		// �a���̍U���� (�G)

#define REF_BULL_PLUS	(0.1f)		// �e�̔��ˎ��̒ǉ��e�̕������Z��

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureSlash[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\slash000.png",	// �a�� (�v���C���[) �̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\slash001.png",	// �a�� (�G) �̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�\���̒�` (Slash)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// �ʒu
	D3DXVECTOR3 rot;				// ����
	D3DXMATRIX  mtxWorld;			// ���[���h�}�g���b�N�X
	SLASHTYPE   type;				// ���
	int         nLife;				// ����
	int         nMaxLife;			// ������
	float       fRadius;			// ���a
	float       fAlpha;				// ���l
	float       fChange;			// 1F���Ƃ̔����Ȃ��
	int        *pSlashIDParent;		// �a���̐e�̎a���C���f�b�N�X
	bool       *pUseParent;			// �a���̐e�̎g�p��
	bool        bUse;				// �g�p��
}Slash;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void CollisionPlayer(Slash *pSlash);	// �v���C���[�Ǝa���̓����蔻��
void CollisionEnemy(Slash *pSlash);		// �G�Ǝa���̓����蔻��
void CollisionWeed(Slash *pSlash);		// �G���Ǝa���̓����蔻��
void CollisionObject(Slash *pSlash);	// �I�u�W�F�N�g�Ǝa���̓����蔻��
void CollisionBullet(Slash *pSlash);	// �e�Ǝa���̓����蔻��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9		g_apTextureSlash[SLASHTYPE_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSlash = NULL;					// ���_�o�b�t�@�ւ̃|�C���^

Slash g_aSlash[MAX_SLASH];	// �a���̏��

//======================================================================================================================
//	�a���̏���������
//======================================================================================================================
void InitSlash(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntSlash = 0; nCntSlash < SLASHTYPE_MAX; nCntSlash++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureSlash[nCntSlash], &g_apTextureSlash[nCntSlash]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_3D) * 4 * MAX_SLASH,	// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,						// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffSlash,
		NULL
	);

	// �a���̏��̏�����
	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // �a���̍ő�\�������J��Ԃ�

		g_aSlash[nCntSlash].pos            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
		g_aSlash[nCntSlash].rot            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
		g_aSlash[nCntSlash].type           = SLASHTYPE_PLAYER;				// ���
		g_aSlash[nCntSlash].nLife          = 0;								// ����
		g_aSlash[nCntSlash].nMaxLife       = 0;								// ������
		g_aSlash[nCntSlash].fRadius        = 0.0f;							// ���a
		g_aSlash[nCntSlash].fAlpha         = 1.0f;							// ���l
		g_aSlash[nCntSlash].fChange        = 0.0f;							// 1F���Ƃ̔����Ȃ��
		g_aSlash[nCntSlash].pSlashIDParent = NULL;							// �a���̐e�̎a���C���f�b�N�X
		g_aSlash[nCntSlash].pUseParent     = NULL;							// �a���̐e�̎g�p��
		g_aSlash[nCntSlash].bUse           = false;							// �g�p��
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffSlash->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // �a���̍ő�\�������J��Ԃ�

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// �@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

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
	g_pVtxBuffSlash->Unlock();
}

//======================================================================================================================
//	�a���̏I������
//======================================================================================================================
void UninitSlash(void)
{
	// �e�N�X�`���̔j��
	for (int nCntSlash = 0; nCntSlash < SLASHTYPE_MAX; nCntSlash++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureSlash[nCntSlash] != NULL)
		{ // �ϐ� (g_apTextureSlash) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureSlash[nCntSlash]->Release();
			g_apTextureSlash[nCntSlash] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffSlash != NULL)
	{ // �ϐ� (g_pVtxBuffSlash) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffSlash->Release();
		g_pVtxBuffSlash = NULL;
	}
}

//======================================================================================================================
//	�a���̍X�V����
//======================================================================================================================
void UpdateSlash(void)
{
	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffSlash->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // �a���̍ő�\�������J��Ԃ�

		if (g_aSlash[nCntSlash].bUse == true)
		{ // �a�����g�p����Ă���ꍇ

			//----------------------------------------------------------------------------------------------------------
			//	�����蔻��
			//----------------------------------------------------------------------------------------------------------
			switch (g_aSlash[nCntSlash].type)
			{ // ��ނ��Ƃ̔���
			case SLASHTYPE_PLAYER:	// �v���C���[�̎a��

				// �G�Ǝa���̓����蔻��
				CollisionEnemy(&g_aSlash[nCntSlash]);

				// �G���Ǝa���̓����蔻��
				CollisionWeed(&g_aSlash[nCntSlash]);

				// �I�u�W�F�N�g�Ǝa���̓����蔻��
				CollisionObject(&g_aSlash[nCntSlash]);

				// �e�Ǝa���̓����蔻��
				CollisionBullet(&g_aSlash[nCntSlash]);

				// �����𔲂���
				break;

			case SLASHTYPE_ENEMY:	// �G�̎a��

				// �v���C���[�Ǝa���̓����蔻��
				CollisionPlayer(&g_aSlash[nCntSlash]);

				// �����𔲂���
				break;
			}

			//----------------------------------------------------------------------------------------------------------
			//	�a���̓�����
			//----------------------------------------------------------------------------------------------------------
			// ���l�����Z
			g_aSlash[nCntSlash].fAlpha -= g_aSlash[nCntSlash].fChange;

			// ���_�J���[�̐ݒ�
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);

			//----------------------------------------------------------------------------------------------------------
			//	�a���̍폜
			//----------------------------------------------------------------------------------------------------------
			// ���������炷
			g_aSlash[nCntSlash].nLife--;

			if (g_aSlash[nCntSlash].nLife <= 0)
			{ // �a���̎������s�����ꍇ

				// �g�p���Ă��Ȃ���Ԃɂ���
				g_aSlash[nCntSlash].bUse = false;

				if (g_aSlash[nCntSlash].pSlashIDParent != NULL)
				{ // �a���̐e�̎a���C���f�b�N�X�A�h���X���ݒ肳��Ă���ꍇ

					// �a���C���f�b�N�X�̏�����
					*g_aSlash[nCntSlash].pSlashIDParent = NONE_SLASH;
				}
			}

			if (g_aSlash[nCntSlash].pUseParent != NULL)
			{ // �a���̐e�̎g�p�󋵃A�h���X���ݒ肳��Ă���ꍇ

				if (*g_aSlash[nCntSlash].pUseParent == false)
				{ // �a���̐e���g�p����Ă��Ȃ��ꍇ

					// �a�����g�p���Ă��Ȃ���Ԃɂ���
					g_aSlash[nCntSlash].bUse = false;

					if (g_aSlash[nCntSlash].pSlashIDParent != NULL)
					{ // �a���̐e�̎a���C���f�b�N�X�A�h���X���ݒ肳��Ă���ꍇ

						// �a���C���f�b�N�X�̏�����
						*g_aSlash[nCntSlash].pSlashIDParent = NONE_SLASH;
					}
				}
			}
		}

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffSlash->Unlock();
}

//======================================================================================================================
//	�a���̕`�揈��
//======================================================================================================================
void DrawSlash(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxRot, mtxTrans;	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxView;				// �r���[�}�g���b�N�X�擾�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// �J�����O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // �a���̍ő�\�������J��Ԃ�

		if (g_aSlash[nCntSlash].bUse == true)
		{ // �a�����g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aSlash[nCntSlash].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aSlash[nCntSlash].rot.y, g_aSlash[nCntSlash].rot.x, g_aSlash[nCntSlash].rot.z);
			D3DXMatrixMultiply(&g_aSlash[nCntSlash].mtxWorld, &g_aSlash[nCntSlash].mtxWorld, &mtxRot);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aSlash[nCntSlash].pos.x, g_aSlash[nCntSlash].pos.y, g_aSlash[nCntSlash].pos.z);
			D3DXMatrixMultiply(&g_aSlash[nCntSlash].mtxWorld, &g_aSlash[nCntSlash].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aSlash[nCntSlash].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffSlash, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureSlash[g_aSlash[nCntSlash].type]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntSlash * 4, 2);
		}
	}

	// �J�����O��L���ɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//======================================================================================================================
//	�a���̐ݒ菈��
//======================================================================================================================
int SetSlash(SLASHTYPE type, int nLife, float fRadius, int *pSlashID, bool *pUse)
{
	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // �a���̍ő�\�������J��Ԃ�

		if (g_aSlash[nCntSlash].bUse == false)
		{ // �a�����g�p����Ă��Ȃ��ꍇ

			// ��������
			g_aSlash[nCntSlash].type     = type;	// ���
			g_aSlash[nCntSlash].nLife    = nLife;	// ����
			g_aSlash[nCntSlash].nMaxLife = nLife;	// ������
			g_aSlash[nCntSlash].fRadius  = fRadius;	// ���a

			// �����̃A�h���X����
			g_aSlash[nCntSlash].pSlashIDParent = pSlashID;	// �a���̐e�̎a���C���f�b�N�X
			g_aSlash[nCntSlash].pUseParent     = pUse;		// �a���̐e�̎g�p��

			// 1F���Ƃ̔����Ȃ�ʂ�ݒ�
			g_aSlash[nCntSlash].fChange = 0.8f / g_aSlash[nCntSlash].nMaxLife;

			// �a���̏���������
			g_aSlash[nCntSlash].pos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
			g_aSlash[nCntSlash].rot    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
			g_aSlash[nCntSlash].fAlpha = 1.0f;							// ���l

			// �g�p���Ă����Ԃɂ���
			g_aSlash[nCntSlash].bUse = true;

			// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
			g_pVtxBuffSlash->Lock(0, 0, (void**)&pVtx, 0);

			// ���_�f�[�^�̃|�C���^���g�p����a���̃|�C���^�܂Ői�߂�
			pVtx += nCntSlash * 4;

			// ���_���W�̐ݒ�
			pVtx[0].pos = D3DXVECTOR3(-fRadius, 0.0f,  fRadius);
			pVtx[1].pos = D3DXVECTOR3( fRadius, 0.0f,  fRadius);
			pVtx[2].pos = D3DXVECTOR3(-fRadius, 0.0f, -fRadius);
			pVtx[3].pos = D3DXVECTOR3( fRadius, 0.0f, -fRadius);

			// ���_�J���[�̐ݒ�
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// ���_�o�b�t�@���A�����b�N����
			g_pVtxBuffSlash->Unlock();

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_SLASH);	// SE (�a��)

			// �g�p�ł���a����ID��Ԃ�
			return nCntSlash;
		}
	}

	// �a���̑���������Ȃ��ꍇ -1��Ԃ�
	return -1;
}

//======================================================================================================================
//	�a���̈ʒu�E�����̐ݒ菈��
//======================================================================================================================
void SetPositionSlash(int nSlashID, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (nSlashID > NONE_SLASH)
	{ // �g�p�ł���ID�̏ꍇ

		// �����̎a����ݒ�
		g_aSlash[nSlashID].pos = pos;	// �ʒu
		g_aSlash[nSlashID].rot = rot;	// ����

		// �a������]
		g_aSlash[nSlashID].rot.y -= (g_aSlash[nSlashID].nMaxLife - g_aSlash[nSlashID].nLife) * REV_SLASH_ROT;

		// �����̐��K��
		if      (g_aSlash[nSlashID].rot.y >  D3DX_PI) { g_aSlash[nSlashID].rot.y -= D3DX_PI * 2; }
		else if (g_aSlash[nSlashID].rot.y < -D3DX_PI) { g_aSlash[nSlashID].rot.y += D3DX_PI * 2; }
	}
}

//======================================================================================================================
//	�v���C���[�Ǝa���̓����蔻��
//======================================================================================================================
void CollisionPlayer(Slash *pSlash)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �a���ƃv���C���[�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �a���ƃv���C���[�Ƃ̊Ԃ̋��������߂�
		fLength = (pSlash->pos.x - pPlayer->pos.x) * (pSlash->pos.x - pPlayer->pos.x)
				+ (pSlash->pos.z - pPlayer->pos.z) * (pSlash->pos.z - pPlayer->pos.z);

		if (fLength < ((pSlash->fRadius + PLAY_WIDTH) * (pSlash->fRadius + PLAY_WIDTH))	// �~�̓����蔻��
		&&  pSlash->pos.y + HIT_SLA_HEIGHT > pPlayer->pos.y								// �c�̔��� (��)
		&&  pSlash->pos.y - HIT_SLA_HEIGHT < pPlayer->pos.y + PLAY_HEIGHT)				// �c�̔��� (��)
		{ // �a�����v���C���[�ɓ������Ă���ꍇ

			// �v���C���[�̃_���[�W����
			HitPlayer(pPlayer, SLA_DMG_ENE);
		}
	}
}

//======================================================================================================================
//	�G�Ǝa���̓����蔻��
//======================================================================================================================
void CollisionEnemy(Slash *pSlash)
{
	// �ϐ���錾
	float fLength = 0.0f;			// �a���ƓG�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Enemy *pEnemy = GetEnemy();		// �G�̏��

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
	{ // �G�̍ő�\�������J��Ԃ�

		if (pEnemy->bUse == true)
		{ // �G���g�p����Ă���ꍇ

			// �a���ƓG�Ƃ̊Ԃ̋��������߂�
			fLength = (pSlash->pos.x - pEnemy->pos.x) * (pSlash->pos.x - pEnemy->pos.x)
					+ (pSlash->pos.z - pEnemy->pos.z) * (pSlash->pos.z - pEnemy->pos.z);

			if (fLength < ((pSlash->fRadius + pEnemy->pModelData->fRadius) * (pSlash->fRadius + pEnemy->pModelData->fRadius))	// �~�̓����蔻��
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pEnemy->pos.y																	// �c�̔��� (��)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pEnemy->pos.y + pEnemy->pModelData->fHeight)									// �c�̔��� (��)
			{ // �a�����G�ɓ������Ă���ꍇ

				// �G�̃_���[�W����
				HitEnemy(pEnemy, SLA_DMG_PLAY, DAMAGETYPE_SLASH);
			}
		}
	}
}

//======================================================================================================================
//	�G���Ǝa���̓����蔻��
//======================================================================================================================
void CollisionWeed(Slash *pSlash)
{
	// �ϐ���錾
	float fLength = 0.0f;		// �a���ƎG���Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Weed *pWeed = GetWeed();	// �G���̏��

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++, pWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		if (pWeed->bUse == true)
		{ // �G�����g�p����Ă���ꍇ

			// �a���ƎG���Ƃ̊Ԃ̋��������߂�
			fLength = (pSlash->pos.x - pWeed->pos.x) * (pSlash->pos.x - pWeed->pos.x)
					+ (pSlash->pos.z - pWeed->pos.z) * (pSlash->pos.z - pWeed->pos.z);

			if (fLength < ((pSlash->fRadius + WEED_WIDTH) * (pSlash->fRadius + WEED_WIDTH))	// �~�̓����蔻��
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pWeed->pos.y								// �c�̔��� (��)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pWeed->pos.y + WEED_HEIGHT)				// �c�̔��� (��)
			{ // �a�����G���ɓ������Ă���ꍇ

				// �G���̃_���[�W����
				HitWeed(pWeed, SLA_DMG_WEED);
			}
		}
	}
}

//======================================================================================================================
//	�I�u�W�F�N�g�Ǝa���̓����蔻��
//======================================================================================================================
void CollisionObject(Slash *pSlash)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �a���ƃI�u�W�F�N�g�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Object *pObject = GetObjectData();	// �I�u�W�F�N�g�̏��

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{ // �I�u�W�F�N�g�̍ő�\�������J��Ԃ�

		if (pObject->bUse == true && pObject->nBreakType != BREAKTYPE_NONE)
		{ // �I�u�W�F�N�g���g�p����Ă��銎�A�I�u�W�F�N�g�����Ȃ��ݒ�ł͂Ȃ��ꍇ

			// �a���ƃI�u�W�F�N�g�Ƃ̊Ԃ̋��������߂�
			fLength = (pSlash->pos.x - pObject->pos.x) * (pSlash->pos.x - pObject->pos.x)
					+ (pSlash->pos.z - pObject->pos.z) * (pSlash->pos.z - pObject->pos.z);

			if (fLength < ((pSlash->fRadius + pObject->pModelData->fRadius) * (pSlash->fRadius + pObject->pModelData->fRadius))	// �~�̓����蔻��
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pObject->pos.y																	// �c�̔��� (��)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pObject->pos.y + pObject->pModelData->fHeight)									// �c�̔��� (��)
			{ // �a�����I�u�W�F�N�g�ɓ������Ă���ꍇ

				// �I�u�W�F�N�g�̃_���[�W����
				HitObject(pObject, SLA_DMG_PLAY);
			}
		}
	}
}

//======================================================================================================================
//	�e�Ǝa���̓����蔻��
//======================================================================================================================
void CollisionBullet(Slash *pSlash)
{
	// �ϐ���錾
	float fLength = 0.0f;			// �a���ƒe�Ƃ̊Ԃ̋���
	float fRot    = 0.0f;			// �a�����猩���e�̕���

	// �|�C���^��錾
	Bullet *pBullet = GetBullet();	// �e�̏��

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++, pBullet++)
	{ // �e�̍ő�\�������J��Ԃ�

		if (pBullet->bUse == true && pBullet->type == BULLETTYPE_ENEMY)
		{ // �e���g�p����Ă��銎�A�e�̎�ނ��G�̒e�̏ꍇ

			// �a���ƒe�Ƃ̊Ԃ̋��������߂�
			fLength = (pSlash->pos.x - pBullet->pos.x) * (pSlash->pos.x - pBullet->pos.x)
					+ (pSlash->pos.z - pBullet->pos.z) * (pSlash->pos.z - pBullet->pos.z);

			if (fLength < ((pSlash->fRadius + BULL_WIDTH) * (pSlash->fRadius + BULL_WIDTH))	// �~�̓����蔻��
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pBullet->pos.y - BULL_HEIGHT				// �c�̔��� (��)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pBullet->pos.y + BULL_HEIGHT)				// �c�̔��� (��)
			{ // �a�����e�ɓ������Ă���ꍇ

				// �a�����猩���e�̕��������߂�
				fRot = atan2f(pBullet->pos.x - pSlash->pos.x, pBullet->pos.z - pSlash->pos.z);

				// �e�̏��̍Đݒ�
				pBullet->move = D3DXVECTOR3
				( // ����
					sinf(fRot) * PLAY_MOVE_BULLET,	// �ړ��� (x)
					0.0f,							// �ړ��� (y)
					cosf(fRot) * PLAY_MOVE_BULLET	// �ړ��� (z)
				);
				pBullet->type  = BULLETTYPE_PLAYER;	// ��ނ��v���C���[�̒e�ɕύX
				pBullet->nLife = LIFE_BULLET;		// �e�̎�����������

				// �e�̐ݒ�
				SetBullet
				( // ����
					pBullet->pos,		// �ʒu
					D3DXVECTOR3
					( // ����
						sinf(fRot + (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET,	// �ړ��� (x)
						0.0f,														// �ړ��� (y)
						cosf(fRot + (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET	// �ړ��� (z)
					),
					BULLETTYPE_PLAYER,	// ���
					false				// ���ʉ�
				);

				// �e�̐ݒ�
				SetBullet
				( // ����
					pBullet->pos,		// �ʒu
					D3DXVECTOR3
					( // ����
						sinf(fRot - (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET,	// �ړ��� (x)
						0.0f,														// �ړ��� (y)
						cosf(fRot - (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET	// �ړ��� (z)
					),
					BULLETTYPE_PLAYER,	// ���
					false				// ���ʉ�
				);

				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_REF);	// SE (����)
			}
		}
	}
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�a���̑����擾����
//**********************************************************************************************************************
int GetNumSlash(void)
{
	// �ϐ���錾
	int nNumSlash = 0;	// �a���̑����̊m�F�p

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // �a���̍ő�\�������J��Ԃ�

		if (g_aSlash[nCntSlash].bUse == true)
		{ // �a�����g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumSlash++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumSlash;	// �a���̑���
}
#endif