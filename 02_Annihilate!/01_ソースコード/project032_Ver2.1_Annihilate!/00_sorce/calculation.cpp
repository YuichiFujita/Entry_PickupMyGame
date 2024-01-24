//==================================================================================
//
//	汎用関数処理 [calculation.cpp]
//	Author：藤田勇一 & 小原立暉
//
//==================================================================================
//**********************************************************************************
//	インクルードファイル
//**********************************************************************************
#include "calculation.h"

//==================================================================================
//	四頂点の位置の計算処理
//==================================================================================
//	四つ角の頂点座標を計算する際に使用
//==================================================================================
void VecSizePos(D3DXVECTOR3 *sizePos, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight)
{
	// 変数を宣言
	float fAngle, fLength;	// 頂点座標の計算用

	// 対角線の長さ、角度を求める
	fAngle  = atan2f(fWidth, fHeight);
	fLength = sqrtf((fWidth * fWidth) + (fHeight * fHeight)) * 0.5f;
	
	// 頂点座標を求める
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
//	外積の左右判断処理
//==================================================================================
//	境界線から見て、その境界線の右にいるか、左にいるかの判定に使用
//==================================================================================
float LineOuterProduct(D3DXVECTOR3 posLeft, D3DXVECTOR3 posRight, D3DXVECTOR3 pos)
{
	// 変数を宣言
	D3DXVECTOR3 vecLine;	// 境界線ベクトル
	D3DXVECTOR3 vecToPos;	// 左端と位置のベクトル

	// 境界線ベクトルを求める
	vecLine = posRight - posLeft;

	// 左端と位置のベクトルを求める
	vecToPos = pos - posLeft;

	// 外積の計算結果を返す
	return (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);
}

//==================================================================================
//	外積の当たり判定
//==================================================================================
//	境界線にぶつかっていた場合、ぶつかった位置を求める際に使用
//==================================================================================
void CollisionOuterProduct(D3DXVECTOR3 *Targetpos, D3DXVECTOR3 *TargetposOld, D3DXVECTOR3 WallRpos, D3DXVECTOR3 WallLpos, bool *bUse, int *nShadow)
{
	// 変数を宣言
	D3DXVECTOR3 vecMove, vecLine, vecTopos, posCross;	// 外積の変数
	float       fRate;									// 割合の変数

	// 弾の軌跡を測る
	vecMove.x = (Targetpos->x - TargetposOld->x);
	vecMove.y = (Targetpos->y - TargetposOld->y);
	vecMove.z = (Targetpos->z - TargetposOld->z);

	// 壁の境界線を測る
	vecLine.x = (WallRpos.x - WallLpos.x);
	vecLine.y = (WallRpos.y - WallLpos.y);
	vecLine.z = (WallRpos.z - WallLpos.z);

	// 壁の始点から弾の位置の距離を測る
	vecTopos.x = (Targetpos->x - WallLpos.x);
	vecTopos.y = (Targetpos->y - WallLpos.y);
	vecTopos.z = (Targetpos->z - WallLpos.z);

	// 割合を求める
	fRate = ((vecTopos.z * vecMove.x) - (vecTopos.x * vecMove.z)) / ((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

	// 交点を求める
	posCross.x = vecLine.x * fRate - WallLpos.x;
	posCross.y = vecLine.y * fRate - WallLpos.y;
	posCross.z = vecLine.z * fRate - WallLpos.z;

	if ((vecLine.z * vecTopos.x) - (vecLine.x * vecTopos.z) < 0.0f)
	{ // 境界線を超えた場合

		if (fRate >= 0.0f && fRate <= 1.0f)
		{ // 割合が0.0f～1.0fの間だった(境界線を超えた)場合

			// 位置を設定する
			Targetpos = &posCross;
		}
	}
}

//==================================================================================
//	扇形の当たり判定
//==================================================================================
//	視界内の判定などに使用
//==================================================================================
bool CollisionSector(D3DXVECTOR3 centerPos, D3DXVECTOR3 targetPos, float fCenterRot, float fRadius, float fAngle)
{
	// 変数を宣言
	float       fLength;		// 中心位置と目標位置の距離
	float       fHalfAngle;		// 引数の角度の半分の値の代入用
	bool        bHit = false;	// 当たり判定の結果
	D3DXVECTOR3 vecToPos;		// 左端と位置のベクトル

	// 変数配列を宣言
	float       fRotEdge[2];	// 扇形の縁の角度     [※] 0：左 1：右
	D3DXVECTOR3 posEdge[2];		// 扇形の縁の先端位置 [※] 0：左 1：右
	D3DXVECTOR3 vecEdge[2];		// 扇形の縁ベクトル   [※] 0：左 1：右

	// 中心位置と目標位置の距離求める
	fLength = (centerPos.x - targetPos.x) * (centerPos.x - targetPos.x)
			+ (centerPos.z - targetPos.z) * (centerPos.z - targetPos.z);

	if (fLength < fRadius * fRadius)
	{ // 円の範囲内の場合

		// 引数の角度の半分の値を求める
		fHalfAngle = fAngle * 0.5f;

		// 扇形の左縁の角度を求める
		fRotEdge[0] = fCenterRot + fHalfAngle;	// 角度を左に傾ける
		NormalizeRot(&fRotEdge[0]);				// 向きを正規化

		// 扇形の右縁の角度を求める
		fRotEdge[1] = fCenterRot - fHalfAngle;	// 角度を右に傾ける
		NormalizeRot(&fRotEdge[1]);				// 向きを正規化

		// 扇形の左縁の先端位置を求める
		posEdge[0].x = centerPos.x + sinf(fRotEdge[0]) * 1.0f;
		posEdge[0].y = 0.0f;
		posEdge[0].z = centerPos.z + cosf(fRotEdge[0]) * 1.0f;

		// 扇形の右縁の先端位置を求める
		posEdge[1].x = centerPos.x + sinf(fRotEdge[1]) * 1.0f;
		posEdge[1].y = 0.0f;
		posEdge[1].z = centerPos.z + cosf(fRotEdge[1]) * 1.0f;

		// 扇形の左縁のベクトルを求める
		vecEdge[0] = posEdge[0] - centerPos;
		vecEdge[0].y = 0.0f;

		// 扇形の右縁のベクトルを求める
		vecEdge[1] = posEdge[1] - centerPos;
		vecEdge[1].y = 0.0f;

		// 左端と位置のベクトルを求める
		vecToPos = targetPos - centerPos;

		if ((vecEdge[0].z * vecToPos.x) - (vecEdge[0].x * vecToPos.z) < 0
		&&  (vecEdge[1].z * vecToPos.x) - (vecEdge[1].x * vecToPos.z) > 0)
		{ // 扇形の両縁の範囲内の場合

			// 当たっている状態にする
			bHit = true;
		}
	}

	// 当たり判定の結果を返す
	return bHit;
}

//==================================================================================
//	向きの正規化
//==================================================================================
//	向きの値の補正に使用
//==================================================================================
void NormalizeRot(float *pRot)
{
	if (*pRot > D3DX_PI)
	{ // 向きが 3.14 を超えた場合

		// 向きの正規化
		*pRot -= D3DX_PI * 2;
	}
	else if (*pRot < -D3DX_PI)
	{ // 向きが -3.14 を超えた場合

		// 向きの正規化
		*pRot += D3DX_PI * 2;
	}
}