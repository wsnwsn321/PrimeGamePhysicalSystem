#include "Util.h"

Matrix4x4 Get_Cross_Matrix(Vector3 a)
{
	//Get the cross product matrix of vector a
	Matrix4x4 A = Matrix4x4();

	A.m[0][ 0] = 0;
	A.m[0][ 1] = -a.getZ();
	A.m[0][ 2] = a.getY();
	A.m[1][ 0] = a.getZ();
	A.m[1][ 1] = 0;
	A.m[1][ 2] = -a.getX();
	A.m[2][ 0] = -a.getY();
	A.m[2][ 1] = a.getX();
	A.m[2][ 2] = 0;
	A.m[3][ 3] = 1;
	return A;
}

Matrix4x4 Quaternion_To_Matrix(Quaternion q)
{
	// Get the rotation matrix result from quaternion q
	Matrix4x4 result = Matrix4x4();
	result.m[0][ 0] = q.getW() * q.getW() + q.getX() * q.getX() - q.getY() * q.getY() - q.getZ() * q.getZ();
	result.m[0][ 1] = 2 * (q.getX() * q.getY() - q.getW() * q.getZ());
	result.m[0][ 2] = 2 * (q.getX() * q.getZ() + q.getW() * q.getY());
	result.m[1][ 0] = 2 * (q.getX() * q.getY() + q.getW() * q.getZ());
	result.m[1][ 1] = q.getW() * q.getW() - q.getX() * q.getX() + q.getY() * q.getY() - q.getZ() * q.getZ();
	result.m[1][ 2] = 2 * (q.getY() * q.getZ() - q.getW() * q.getX());
	result.m[2][ 0] = 2 * (q.getX() * q.getZ() - q.getW() * q.getY());
	result.m[2][ 1] = 2 * (q.getY() * q.getZ() + q.getW() * q.getX());
	result.m[2][ 2] = q.getW() * q.getW() - q.getX() * q.getX() - q.getY() * q.getY() + q.getZ() * q.getZ();
	result.m[3][ 3] = 1;
	return result;
}

Quaternion Matrix_To_Quaternion(Matrix4x4& m)
{
	float w = sqrtf(m.m[0][0] + m.m[1][1] +m.m[2][2]+1)/2.0f;
	float y = (m.m[2][0] - m.m[0][2])/(4.0f*w);
	float z = (m.m[0][1] - m.m[1][0]) / (4.0f*w);
	float x = (m.m[1][2] - m.m[2][1]) / (4.0f*w);
	Quaternion result = Quaternion(w,x,y,z);
	return result;
}

/*
Matrix4x4 Float_Matrix_Mul(float num, Matrix4x4 m)
{
	Matrix4x4 result = Matrix4x4();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m.m[i][j] * num;
		}
	}
	return result;
}

Matrix4x4 Matrix_Matrix_Min(Matrix4x4 m, Matrix4x4 n)
{
	Matrix4x4 result = Matrix4x4();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m.m[i][j] - n.m[i][j];
		}
	}
	return result;
}
*/