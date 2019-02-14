#pragma once
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Math/Quaternion.h"
#include <math.h>

Matrix4x4 Get_Cross_Matrix(Vector3 a);
Matrix4x4 Quaternion_To_Matrix(Quaternion q);
Quaternion Matrix_To_Quaternion(Matrix4x4& m);