//======================================================================================================================
//
//	�G������ [weed.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "weed.h"
#include "enemy.h"
#include "particle.h"
#include "player.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define TIP_MOVE		(0.6f)		// �G���̐�[�̈ړ���
#define TIP_POS_DIS		(20.0f)		// �G�������܂ꂽ���̐�[�ʒu�̋���

#define WEED_GROW_CNT	(500)		// �G���̐����܂ł̎���
#define WEED_SIZE_LEVEL	(0.4f)		// �����i�K�̃T�C�Y���Z�ʂ̔{��
#define SHADOW_WEED		(35.0f)		// �G���̉e�̔��a
#define SCORE_WEED		(10)		// �G���̃X�R�A���Z��

#define WEED_LIFE		((int)(SLA_DMG_WEED * 0.5f))	// �G���̗̑�
#define WEED_TIP_LIMIT	(WEED_WIDTH * 1.5f)				// ��[�ʒu�̈ړ��ʂ̔��]�ʒu
#define ADD_WEED_WIDTH	(0.2f)							// 1F���Ƃ̃T�C�Y�ύX�� (����)
#define ADD_WEED_HEIGHT	(ADD_WEED_WIDTH * 2)			// 1F���Ƃ̃T�C�Y�ύX�� (�c��)

#define DAMAGE_TIME_WEED	(PLAY_SLASH_LIFE)			// �_���[�W��Ԃ�ۂ���
#define UNR_TIME_WEED		(PLAY_SLASH_LIFE - 10)		// ���G��ԂɕύX���鎞��

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureWeed[] =	// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\weed000.png",	// �G�� (�ʏ�) �̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_WEED)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_WEED_NORMAL = 0,		// �G�� (�ʏ�)
	TEXTURE_WEED_MAX,				// ���̗񋓌^�̑���
} TEXTURE_WEED;

//**********************************************************************************************************************
//	�񋓌^��` (GROWSTATE)
//**********************************************************************************************************************
typedef enum
{
	GROWSTATE_SMALL = 0,			// ����� (��)
	GROWSTATE_MEDIUM,				// ����� (��)
	GROWSTATE_LARGE,				// ��O��� (��)
	GROWSTATE_MAX,					// ���̗񋓌^�̑���
} GROWSTATE;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
bool CollisionPlayer(Weed *pWeed);				// �G���ƃv���C���[�̓����蔻��
void TreadWeed(Weed *pWeed, int nCntWeed);		// �G�������܂�Ă���ۂ̏���

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureWeed[TEXTURE_WEED_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWeed = NULL;					// ���_�o�b�t�@�ւ̃|�C���^

Weed  g_aWeed[MAX_WEED];	// �G���̏��
float g_fMoveTip;			// ��[�̈ړ���
float g_fPosTip;			// ��[�̈ʒu
int   g_nNumWeed;			// �G���̑���
int   g_nKillWeed;			// �G���̊����萔

//======================================================================================================================
//	�G���̏���������
//======================================================================================================================
void InitWeed(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntWeed = 0; nCntWeed < TEXTURE_WEED_MAX; nCntWeed++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureWeed[nCntWeed], &g_apTextureWeed[nCntWeed]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_3D) * 4 * MAX_WEED,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffWeed,
		NULL
	);

	// �O���[�o���ϐ��̏�����
	g_fMoveTip  = TIP_MOVE;		// ��[�̈ړ���
	g_fPosTip   = 0.0f;			// ��[�̈ʒu
	g_nNumWeed  = 0;			// �G���̑���
	g_nKillWeed = 0;			// �G���̊����萔

	// �G���̏��̏�����
	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		g_aWeed[nCntWeed].pos           = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
		g_aWeed[nCntWeed].state         = ACTIONSTATE_SPAWN;				// ���
		g_aWeed[nCntWeed].nGrowState    = GROWSTATE_SMALL;					// �������
		g_aWeed[nCntWeed].nLife         = WEED_LIFE;						// �̗�
		g_aWeed[nCntWeed].nCounterState = 0;								// ��ԊǗ��J�E���^�[
		g_aWeed[nCntWeed].nCounterGrow  = 0;								// �����Ǘ��J�E���^�[
		g_aWeed[nCntWeed].fWidth        = WEED_WIDTH;						// ����
		g_aWeed[nCntWeed].fHeight       = WEED_HEIGHT;						// �c��
		g_aWeed[nCntWeed].nShadowID     = NONE_SHADOW;						// �e�̃C���f�b�N�X
		g_aWeed[nCntWeed].bUse          = false;							// �g�p��
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-WEED_WIDTH, WEED_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( WEED_WIDTH, WEED_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-WEED_WIDTH, 0.0f,        0.0f);
		pVtx[3].pos = D3DXVECTOR3( WEED_WIDTH, 0.0f,        0.0f);

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
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	�G���̏I������
//======================================================================================================================
void UninitWeed(void)
{
	// �e�N�X�`���̔j��
	for (int nCntWeed = 0; nCntWeed < TEXTURE_WEED_MAX; nCntWeed++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureWeed[nCntWeed] != NULL)
		{ // �ϐ� (g_apTextureWeed) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureWeed[nCntWeed]->Release();
			g_apTextureWeed[nCntWeed] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffWeed != NULL)
	{ // �ϐ� (g_pVtxBuffWeed) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffWeed->Release();
		g_pVtxBuffWeed = NULL;
	}
}

//======================================================================================================================
//	�G���̍X�V����
//======================================================================================================================
void UpdateWeed(void)
{
	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	// �G���̐�[�ʒu�Ɉړ��ʂ����Z
	g_fPosTip += g_fMoveTip;

	if (g_fPosTip - WEED_WIDTH < -WEED_TIP_LIMIT
	||  g_fPosTip + WEED_WIDTH >  WEED_TIP_LIMIT)
	{ // ��[�ʒu���E�A�܂��͍��ɍs���߂����ꍇ

		// �ړ��ʂ𔽓]
		g_fMoveTip *= -1;
	}

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		if (g_aWeed[nCntWeed].bUse == true)
		{ // �G�����g�p����Ă���ꍇ

			switch (g_aWeed[nCntWeed].state)
			{ // ��Ԃ��Ƃ̏���
			case ACTIONSTATE_SPAWN:		// �o�����

				if (g_aWeed[nCntWeed].pos.y < 0.0f)
				{ // �o�����̏ꍇ

					// �n������X�|�[��������
					g_aWeed[nCntWeed].pos.y += (float)((rand() % (SPAWN_MAX_SPEED - SPAWN_MIN_SPEED + 1)) + SPAWN_MIN_SPEED);
				}
				else
				{ // �o�����������ꍇ

					// �G�̍��W (y) ��␳
					g_aWeed[nCntWeed].pos.y = 0.0f;

					// �ʏ��Ԃɂ���
					g_aWeed[nCntWeed].state = ACTIONSTATE_NORMAL;
				}

				// �����𔲂���
				break;

			case ACTIONSTATE_GROW:		// �������

				// �G���̃T�C�Y�����Z
				g_aWeed[nCntWeed].fWidth  += ADD_WEED_WIDTH;	// ����
				g_aWeed[nCntWeed].fHeight += ADD_WEED_HEIGHT;	// �c��

				if (g_aWeed[nCntWeed].fWidth >= WEED_WIDTH * (1.0f + (WEED_SIZE_LEVEL * (g_aWeed[nCntWeed].nGrowState + 1))))
				{ // ���������T�C�Y�ȏ�ɂȂ����ꍇ

					// �G���̃T�C�Y��␳
					g_aWeed[nCntWeed].fWidth  = WEED_WIDTH  * (1.0f + (WEED_SIZE_LEVEL * (g_aWeed[nCntWeed].nGrowState + 1)));
					g_aWeed[nCntWeed].fHeight = WEED_HEIGHT * (1.0f + (WEED_SIZE_LEVEL * (g_aWeed[nCntWeed].nGrowState + 1)));

					// �����i�K�����Z
					g_aWeed[nCntWeed].nGrowState++;

					// �̗͂����Z
					g_aWeed[nCntWeed].nLife += WEED_LIFE;

					// �ʏ��Ԃɂ���
					g_aWeed[nCntWeed].state = ACTIONSTATE_NORMAL;
				}

				// �����𔲂���
				break;

			default:					// ����ȊO�̏��

				//--------------------------------------------------------------------------------------------------
				//	��Ԃ̍X�V
				//--------------------------------------------------------------------------------------------------
				if (g_aWeed[nCntWeed].nCounterState > 0)
				{ // �J�E���^�[�� 0���傫���ꍇ

					// �J�E���^�[�����Z
					g_aWeed[nCntWeed].nCounterState--;

					if (g_aWeed[nCntWeed].nCounterState == UNR_TIME_WEED)
					{ // �J�E���^�[�����l�̏ꍇ

						// ���G��Ԃɂ���
						g_aWeed[nCntWeed].state = ACTIONSTATE_UNRIVALED;
					}
					else if (g_aWeed[nCntWeed].nCounterState <= 0)
					{ // �J�E���^�[�� 0�ȉ��̏ꍇ

						// �ʏ��Ԃɂ���
						g_aWeed[nCntWeed].state = ACTIONSTATE_NORMAL;
					}
				}

				//--------------------------------------------------------------------------------------------------
				//	�����̍X�V
				//--------------------------------------------------------------------------------------------------
				if (g_aWeed[nCntWeed].nGrowState < GROWSTATE_LARGE)
				{ // �����i�K���ő听����菬�����ꍇ

					// �J�E���^�[�����Z
					g_aWeed[nCntWeed].nCounterGrow++;

					if (g_aWeed[nCntWeed].nCounterGrow >= WEED_GROW_CNT)
					{ // �J�E���^�[�����l�ȏ�̏ꍇ

						// �J�E���^�[��������
						g_aWeed[nCntWeed].nCounterGrow = 0;

						// ������Ԃɂ���
						g_aWeed[nCntWeed].state = ACTIONSTATE_GROW;
					}
				}

				// �����𔲂���
				break;
			}

			//------------------------------------------------------------------------------------------------------
			//	�G���̓��܂�E�h��߂�����
			//------------------------------------------------------------------------------------------------------
			if (CollisionPlayer(&g_aWeed[nCntWeed]) == true)
			{ // �v���C���[�ɓ��܂�Ă���ꍇ

				// �G���̓��܂ꏈ��
				TreadWeed(&g_aWeed[nCntWeed], nCntWeed);
			}
			else
			{ // �v���C���[�ɓ��܂�Ă��Ȃ��ꍇ

				// �G���̐�[�ʒu�𓮂��� (�h��߂�)
				pVtx[0].pos = D3DXVECTOR3(g_fPosTip - g_aWeed[nCntWeed].fWidth, g_aWeed[nCntWeed].fHeight, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(g_fPosTip + g_aWeed[nCntWeed].fWidth, g_aWeed[nCntWeed].fHeight, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-g_aWeed[nCntWeed].fWidth,            0.0f,                      0.0f);
				pVtx[3].pos = D3DXVECTOR3( g_aWeed[nCntWeed].fWidth,            0.0f,                      0.0f);
			}

			//--------------------------------------------------------------------------------------------------------------
			//	�e�̍X�V
			//--------------------------------------------------------------------------------------------------------------
			// �e�̈ʒu�ݒ�
			SetPositionShadow
			( // ����
				g_aWeed[nCntWeed].nShadowID,	// �e�̃C���f�b�N�X
				g_aWeed[nCntWeed].pos			// �ʒu
			);
		}

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	�G���̕`�揈��
//======================================================================================================================
void DrawWeed(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxView;		// �r���[�}�g���b�N�X�擾�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// �J�����O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// ���e�X�g�̎Q�ƒl�ݒ�

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		if (g_aWeed[nCntWeed].bUse == true)
		{ // �G�����g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aWeed[nCntWeed].mtxWorld);

			// �r���[�}�g���b�N�X���擾
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// �|���S�����J�����ɑ΂��Đ��ʂɌ�����
			D3DXMatrixInverse(&g_aWeed[nCntWeed].mtxWorld, NULL, &mtxView);	// �t�s������߂�
			g_aWeed[nCntWeed].mtxWorld._41 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._42 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._43 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._21 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._23 = 0.3f;
			g_aWeed[nCntWeed].mtxWorld._24 = 0.0f;

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aWeed[nCntWeed].pos.x, g_aWeed[nCntWeed].pos.y, g_aWeed[nCntWeed].pos.z);
			D3DXMatrixMultiply(&g_aWeed[nCntWeed].mtxWorld, &g_aWeed[nCntWeed].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aWeed[nCntWeed].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffWeed, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureWeed[TEXTURE_WEED_NORMAL]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntWeed * 4, 2);
		}
	}

	// �J�����O��L���ɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ���e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// ���e�X�g�̎Q�ƒl�ݒ�
}

//======================================================================================================================
//	�G���̐ݒ菈��
//======================================================================================================================
void SetWeed(D3DXVECTOR3 pos)
{
	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		if (g_aWeed[nCntWeed].bUse == false)
		{ // �G�����g�p����Ă��Ȃ��ꍇ

			// ��������
			g_aWeed[nCntWeed].pos   = pos;						// �ʒu
			g_aWeed[nCntWeed].state = ACTIONSTATE_SPAWN;		// ���

			// �\���̗̂v�f��������
			g_aWeed[nCntWeed].nGrowState    = GROWSTATE_SMALL;	// �������
			g_aWeed[nCntWeed].nLife         = WEED_LIFE;		// �̗�
			g_aWeed[nCntWeed].nCounterState = 0;				// ��ԊǗ��J�E���^�[
			g_aWeed[nCntWeed].nCounterGrow  = 0;				// �����Ǘ��J�E���^�[
			g_aWeed[nCntWeed].fWidth        = WEED_WIDTH;		// ����
			g_aWeed[nCntWeed].fHeight       = WEED_HEIGHT;		// �c��

			// �g�p���Ă����Ԃɂ���
			g_aWeed[nCntWeed].bUse = true;

			// �e�̃C���f�b�N�X��ݒ�
			g_aWeed[nCntWeed].nShadowID = SetShadow(0.2f, SHADOW_WEED, &g_aWeed[nCntWeed].nShadowID, &g_aWeed[nCntWeed].bUse);

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aWeed[nCntWeed].nShadowID, g_aWeed[nCntWeed].pos);

			// �G���̑��������Z
			g_nNumWeed++;

			// ���_���W�̐ݒ�
			pVtx[0].pos = D3DXVECTOR3(-WEED_WIDTH, WEED_HEIGHT, 0.0f);
			pVtx[1].pos = D3DXVECTOR3( WEED_WIDTH, WEED_HEIGHT, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-WEED_WIDTH, 0.0f,        0.0f);
			pVtx[3].pos = D3DXVECTOR3( WEED_WIDTH, 0.0f,        0.0f);

			// �����𔲂���
			break;
		}

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	�G���̃_���[�W����
//======================================================================================================================
void HitWeed(Weed *pWeed, int nDamage)
{
	if (pWeed->state == ACTIONSTATE_NORMAL || pWeed->state == ACTIONSTATE_GROW)
	{ // �G�����ʏ��ԁA�܂��͐�����Ԃ̏ꍇ

		// �����̃_���[�W����̗͂��猸�Z
		pWeed->nLife -= nDamage;

		if (pWeed->nLife > 0)
		{ // �̗͂��c���Ă���ꍇ

			// �_���[�W��Ԃɂ���
			pWeed->state = ACTIONSTATE_DAMAGE;

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pWeed->pos,							// �ʒu
				D3DXCOLOR(0.0f, 0.9f, 0.4f, 1.0f),	// �F
				PARTICLETYPE_WEED,					// ���
				SPAWN_PARTICLE_WEED,				// �G�t�F�N�g��
				1									// ����
			);

			// �J�E���^�[��ݒ�
			pWeed->nCounterState = DAMAGE_TIME_WEED;
		}
		else
		{ // �̗͂��s�����ꍇ

			// �X�R�A�̉��Z
			AddScore(SCORE_WEED);

			// �p�[�e�B�N���̐ݒ�
			SetParticle
			( // ����
				pWeed->pos,							// �ʒu
				D3DXCOLOR(0.6f, 1.0f, 0.4f, 1.0f),	// �F
				PARTICLETYPE_SLASHWEED,				// ���
				SPAWN_PARTICLE_SLASHWEED,			// �G�t�F�N�g��
				1									// ����
			);

			// �g�p���Ă��Ȃ���Ԃɂ���
			pWeed->bUse = false;

			// �G���̑��������Z
			g_nNumWeed--;

			// �G���̊����萔�����Z
			g_nKillWeed++;
		}
	}
}

//======================================================================================================================
//	�G���ƃv���C���[�̓����蔻��
//======================================================================================================================
bool CollisionPlayer(Weed *pWeed)
{
	// �ϐ���錾
	float fLength = 0.0f;				// �G���ƃv���C���[�Ƃ̊Ԃ̋���

	// �|�C���^��錾
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	if (pPlayer->bUse == true)
	{ // �v���C���[���g�p����Ă���ꍇ

		// �G���ƃv���C���[�Ƃ̊Ԃ̋��������߂�
		fLength = (pWeed->pos.x - pPlayer->pos.x) * (pWeed->pos.x - pPlayer->pos.x)
				+ (pWeed->pos.z - pPlayer->pos.z) * (pWeed->pos.z - pPlayer->pos.z);

		if (fLength < ((pWeed->fWidth + PLAY_WIDTH) * (pWeed->fWidth + PLAY_WIDTH))	// �~�̓����蔻��
		&&  pWeed->pos.y + pWeed->fHeight > pPlayer->pos.y							// �c�̔��� (��)
		&&  pWeed->pos.y                  < pPlayer->pos.y + PLAY_HEIGHT)			// �c�̔��� (��)
		{ // �G�����v���C���[�ɓ������Ă���ꍇ

			// ���܂�Ă���ꍇ true ��Ԃ�
			return true;
		}
	}

	// ���܂�Ă��Ȃ��ꍇ false ��Ԃ�
	return false;
}

//======================================================================================================================
//	�G�������܂�Ă���ۂ̏���
//======================================================================================================================
void TreadWeed(Weed *pWeed, int nCntWeed)
{
	// �ϐ���錾
	D3DXVECTOR3 tipPos;			// ��[�ʒu�̑���p
	float       fRot = 0.0f;	// �v���C���[���猩���G���̕���

	// �|�C���^��錾
	VERTEX_3D  *pVtx;					// ���_���ւ̃|�C���^
	Player *pPlayer = GetPlayer();	// �v���C���[�̏��

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	// �|�C���^�̃A�h���X���ړ�
	pVtx += nCntWeed * 4;

	// �v���C���[���猩���G���̕��������߂�
	fRot = atan2f(pWeed->pos.x - pPlayer->pos.x, pWeed->pos.z - pPlayer->pos.z);

	// ��[�̐�Έʒu��ݒ�
	tipPos.x = sinf(fRot) * TIP_POS_DIS;
	tipPos.z = cosf(fRot) * TIP_POS_DIS;

	// �G���̐�[�ʒu�𓮂���
	pVtx[0].pos = D3DXVECTOR3(tipPos.x - pWeed->fWidth, pWeed->fHeight, tipPos.z - pWeed->fWidth);
	pVtx[1].pos = D3DXVECTOR3(tipPos.x + pWeed->fWidth, pWeed->fHeight, tipPos.z + pWeed->fWidth);
	pVtx[2].pos = D3DXVECTOR3(-pWeed->fWidth,           0.0f,           0.0f);
	pVtx[3].pos = D3DXVECTOR3( pWeed->fWidth,           0.0f,           0.0f);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	�G���̗]�萔�̎擾����
//======================================================================================================================
int GetLeaveWeed(void)
{
	// �G���̑�����Ԃ�
	return g_nNumWeed;
}

//======================================================================================================================
//	�G���̎擾����
//======================================================================================================================
Weed *GetWeed(void)
{
	// �G���̏��̐擪�A�h���X��Ԃ�
	return &g_aWeed[0];
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�G���̑����擾����
//**********************************************************************************************************************
int GetNumWeed(void)
{
	// �ϐ���錾
	int nNumWeed = 0;	// �G���̑����̊m�F�p

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // �G���̍ő�\�������J��Ԃ�

		if (g_aWeed[nCntWeed].bUse == true)
		{ // �G�����g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumWeed++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumWeed;	// �G���̑���
}
#endif