#include "precompiled.h"

void MatrixToYawPitchRoll(D3DXMATRIX* m, D3DXVECTOR3* pVec)
{
	D3DXVECTOR3	Scale;
	D3DXQUATERNION	Rotation;
	D3DXVECTOR3	Translation; 

// *************************** DECOMPOSE MATRIX

	D3DXMatrixDecompose(&Scale,&Rotation,&Translation,m);

// *************************** CALCULATE COMPONENTS

	float tx  = 2.0f * Rotation.x;
	float ty  = 2.0f * Rotation.y;
	float tz  = 2.0f * Rotation.z;
	float txw =   tx * Rotation.w;
	float tyw =   ty * Rotation.w;
	float tzw =   tz * Rotation.w;
	float txx =   tx * Rotation.x;
	float tyx =   ty * Rotation.x;
	float tzx =   tz * Rotation.x;
	float tyy =   ty * Rotation.y;
	float tzy =   tz * Rotation.y;
	float tzz =   tz * Rotation.z;

	float	s = tzy - txw;   
	if(	s >  1.0f )  
		s =  1.0f;
        if(	s < -1.0f )  
		s = -1.0f;

// *************************** CALCULATE PITCH

	pVec->x = asin(-s );

// *************************** CALCULATE YAW/ROLL

	if ((Rotation.x > -D3DXToRadian(89)) || (Rotation.x < D3DXToRadian(89)))
	{
		pVec->y = atan2( tzx + tyw, 1.0f - ( txx + tyy ) );
		pVec->z = atan2( tyx + tzw, 1.0f - ( txx + tzz ) );
	}
	else
	{
		pVec->y = 0.0f;
		pVec->z = atan2( tzx - tyw, 1.0f -( tyy + tzz ) );
	}

// *************************** CHANGE TO DEGREE

	pVec->x = D3DXToDegree(pVec->x);
	pVec->y = D3DXToDegree(pVec->y);
	pVec->z = D3DXToDegree(pVec->z);
}
