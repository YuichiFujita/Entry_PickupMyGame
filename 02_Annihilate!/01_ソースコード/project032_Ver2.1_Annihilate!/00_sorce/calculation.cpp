//==================================================================================
//
//	�ėp�֐����� [calculation.cpp]
//	Author�F���c�E�� & ��������
//
//==================================================================================
//**********************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************
#include "calculation.h"

//==================================================================================
//	�l���_�̈ʒu�̌v�Z����
//==================================================================================
//	�l�p�̒��_���W���v�Z����ۂɎg�p
//==================================================================================
void VecSizePos(D3DXVECTOR3 *sizePos, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight)
{
	// �ϐ���錾
	float fAngle, fLength;	// ���_���W�̌v�Z�p

	// �Ίp���̒����A�p�x�����߂�
	fAngle  = atan2f(fWidth, fHeight);
	fLength = sqrtf((fWidth * fWidth) + (fHeight * fHeight)) * 0.5f;
	
	// ���_���W�����߂�
	sizePos[0].x = pos.x + sinf(rot.y + (D3DX_PI + fAngle)) * fLength;
	sizePos[0].y = pos.y;
	sizePos[0].z = pos.z + cosf(rot.y + (D3DX_PI + fAngle)) * fLength;
	sizePos[1].x = pos.x + sinf(rot.y + (D3DX_PI - fAngle)) * fLength;
	sizePos[1].y = pos.y;
	sizePos[1].z = pos.z + cosf(rot.y + (D3DX_PI - fAngle)) * fLength;
	sizePos[2].x = pos.x + sinf(rot.y + fAngle) * fLength;
	sizePos[2].y = pos.y;
	sizePos[2].z = pos.z + cosf(rot.y + fAngle) * fLength;
	sizePos[3].x = pos.x + sinf(rot.y - fAngle) * fLength;
	sizePos[3].y = pos.y;
	sizePos[3].z = pos.z + cosf(rot.y - fAngle) * fLength;
}

//==================================================================================
//	�O�ς̍��E���f����
//==================================================================================
//	���E�����猩�āA���̋��E���̉E�ɂ��邩�A���ɂ��邩�̔���Ɏg�p
//==================================================================================
float LineOuterProduct(D3DXVECTOR3 posLeft, D3DXVECTOR3 posRight, D3DXVECTOR3 pos)
{
	// �ϐ���錾
	D3DXVECTOR3 vecLine;	// ���E���x�N�g��
	D3DXVECTOR3 vecToPos;	// ���[�ƈʒu�̃x�N�g��

	// ���E���x�N�g�������߂�
	vecLine = posRight - posLeft;

	// ���[�ƈʒu�̃x�N�g�������߂�
	vecToPos = pos - posLeft;

	// �O�ς̌v�Z���ʂ�Ԃ�
	return (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);
}

//==================================================================================
//	�O�ς̓����蔻��
//==================================================================================
//	���E���ɂԂ����Ă����ꍇ�A�Ԃ������ʒu�����߂�ۂɎg�p
//==================================================================================
void CollisionOuterProduct(D3DXVECTOR3 *Targetpos, D3DXVECTOR3 *TargetposOld, D3DXVECTOR3 WallRpos, D3DXVECTOR3 WallLpos, bool *bUse, int *nShadow)
{
	// �ϐ���錾
	D3DXVECTOR3 vecMove, vecLine, vecTopos, posCross;	// �O�ς̕ϐ�
	float       fRate;									// �����̕ϐ�

	// �e�̋O�Ղ𑪂�
	vecMove.x = (Targetpos->x - TargetposOld->x);
	vecMove.y = (Targetpos->y - TargetposOld->y);
	vecMove.z = (Targetpos->z - TargetposOld->z);

	// �ǂ̋��E���𑪂�
	vecLine.x = (WallRpos.x - WallLpos.x);
	vecLine.y = (WallRpos.y - WallLpos.y);
	vecLine.z = (WallRpos.z - WallLpos.z);

	// �ǂ̎n�_����e�̈ʒu�̋����𑪂�
	vecTopos.x = (Targetpos->x - WallLpos.x);
	vecTopos.y = (Targetpos->y - WallLpos.y);
	vecTopos.z = (Targetpos->z - WallLpos.z);

	// ���������߂�
	fRate = ((vecTopos.z * vecMove.x) - (vecTopos.x * vecMove.z)) / ((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

	// ��_�����߂�
	posCross.x = vecLine.x * fRate - WallLpos.x;
	posCross.y = vecLine.y * fRate - WallLpos.y;
	posCross.z = vecLine.z * fRate - WallLpos.z;

	if ((vecLine.z * vecTopos.x) - (vecLine.x * vecTopos.z) < 0.0f)
	{ // ���E���𒴂����ꍇ

		if (fRate >= 0.0f && fRate <= 1.0f)
		{ // ������0.0f�`1.0f�̊Ԃ�����(���E���𒴂���)�ꍇ

			// �ʒu��ݒ肷��
			Targetpos = &posCross;
		}
	}
}

//==================================================================================
//	��`�̓����蔻��
//==================================================================================
//	���E���̔���ȂǂɎg�p
//==================================================================================
bool CollisionSector(D3DXVECTOR3 centerPos, D3DXVECTOR3 targetPos, float fCenterRot, float fRadius, float fAngle)
{
	// �ϐ���錾
	float       fLength;		// ���S�ʒu�ƖڕW�ʒu�̋���
	float       fHalfAngle;		// �����̊p�x�̔����̒l�̑���p
	bool        bHit = false;	// �����蔻��̌���
	D3DXVECTOR3 vecToPos;		// ���[�ƈʒu�̃x�N�g��

	// �ϐ��z���錾
	float       fRotEdge[2];	// ��`�̉��̊p�x     [��] 0�F�� 1�F�E
	D3DXVECTOR3 posEdge[2];		// ��`�̉��̐�[�ʒu [��] 0�F�� 1�F�E
	D3DXVECTOR3 vecEdge[2];		// ��`�̉��x�N�g��   [��] 0�F�� 1�F�E

	// ���S�ʒu�ƖڕW�ʒu�̋������߂�
	fLength = (centerPos.x - targetPos.x) * (centerPos.x - targetPos.x)
			+ (centerPos.z - targetPos.z) * (centerPos.z - targetPos.z);

	if (fLength < fRadius * fRadius)
	{ // �~�͈͓̔��̏ꍇ

		// �����̊p�x�̔����̒l�����߂�
		fHalfAngle = fAngle * 0.5f;

		// ��`�̍����̊p�x�����߂�
		fRotEdge[0] = fCenterRot + fHalfAngle;	// �p�x�����ɌX����
		NormalizeRot(&fRotEdge[0]);				// �����𐳋K��

		// ��`�̉E���̊p�x�����߂�
		fRotEdge[1] = fCenterRot - fHalfAngle;	// �p�x���E�ɌX����
		NormalizeRot(&fRotEdge[1]);				// �����𐳋K��

		// ��`�̍����̐�[�ʒu�����߂�
		posEdge[0].x = centerPos.x + sinf(fRotEdge[0]) * 1.0f;
		posEdge[0].y = 0.0f;
		posEdge[0].z = centerPos.z + cosf(fRotEdge[0]) * 1.0f;

		// ��`�̉E���̐�[�ʒu�����߂�
		posEdge[1].x = centerPos.x + sinf(fRotEdge[1]) * 1.0f;
		posEdge[1].y = 0.0f;
		posEdge[1].z = centerPos.z + cosf(fRotEdge[1]) * 1.0f;

		// ��`�̍����̃x�N�g�������߂�
		vecEdge[0] = posEdge[0] - centerPos;
		vecEdge[0].y = 0.0f;

		// ��`�̉E���̃x�N�g�������߂�
		vecEdge[1] = posEdge[1] - centerPos;
		vecEdge[1].y = 0.0f;

		// ���[�ƈʒu�̃x�N�g�������߂�
		vecToPos = targetPos - centerPos;

		if ((vecEdge[0].z * vecToPos.x) - (vecEdge[0].x * vecToPos.z) < 0
		&&  (vecEdge[1].z * vecToPos.x) - (vecEdge[1].x * vecToPos.z) > 0)
		{ // ��`�̗����͈͓̔��̏ꍇ

			// �������Ă����Ԃɂ���
			bHit = true;
		}
	}

	// �����蔻��̌��ʂ�Ԃ�
	return bHit;
}

//==================================================================================
//	�����̐��K��
//==================================================================================
//	�����̒l�̕␳�Ɏg�p
//==================================================================================
void NormalizeRot(float *pRot)
{
	if (*pRot > D3DX_PI)
	{ // ������ 3.14 �𒴂����ꍇ

		// �����̐��K��
		*pRot -= D3DX_PI * 2;
	}
	else if (*pRot < -D3DX_PI)
	{ // ������ -3.14 �𒴂����ꍇ

		// �����̐��K��
		*pRot += D3DX_PI * 2;
	}
}