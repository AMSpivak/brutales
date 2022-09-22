
void quat_cast(in matrix<float, 4, 4> m, out float4 Result)
{
	float fourXSquaredMinus1 = m._m00 - m._m11 - m._m22;
	float fourYSquaredMinus1 = m._m11 - m._m00 - m._m22;
	float fourZSquaredMinus1 = m._m22 - m._m00 - m._m11;
	float fourWSquaredMinus1 = m._m00 + m._m11 + m._m22;

	int biggestIndex = 0;
	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * (0.5f);
	float mult = (0.25f) / biggestVal;

	
	switch (biggestIndex)
	{
	case 0:
		Result.w = biggestVal;
		Result.x = (m._m12 - m._m21) * mult;
		Result.y = (m._m20 - m._m02) * mult;
		Result.z = (m._m01 - m._m10) * mult;
		break;
	case 1:
		Result.w = (m._m12 - m._m21) * mult;
		Result.x = biggestVal;
		Result.y = (m._m01 + m._m10) * mult;
		Result.z = (m._m20 + m._m02) * mult;
		break;
	case 2:
		Result.w = (m._m20 - m._m02) * mult;
		Result.x = (m._m01 + m._m10) * mult;
		Result.y = biggestVal;
		Result.z = (m._m12 + m._m21) * mult;
		break;
	case 3:
		Result.w = (m._m01 - m._m10) * mult;
		Result.x = (m._m20 + m._m02) * mult;
		Result.y = (m._m12 + m._m21) * mult;
		Result.z = biggestVal;
		break;
	}
}

void mat_cast(in float4 q, out matrix<float, 4, 4> Result)
{
	float qxx=(q.x * q.x);
	float qyy=(q.y * q.y);
	float qzz=(q.z * q.z);
	float qxz=(q.x * q.z);
	float qxy=(q.x * q.y);
	float qyz=(q.y * q.z);
	float qwx=(q.w * q.x);
	float qwy=(q.w * q.y);
	float qwz=(q.w * q.z);

	Result._m00 = 1.f - 2.f * (qyy + qzz);
	Result._m01 = 2.f * (qxy + qwz);
	Result._m02 = 2.f * (qxz - qwy);
	Result._m03 = 0.f;
				   
	Result._m10 = 2.f * (qxy - qwz);
	Result._m11 = 1.f - 2.f * (qxx + qzz);
	Result._m12 = 2.f * (qyz + qwx);
	Result._m13 = 0.f;

	Result._m20 = 2.f * (qxz + qwy);
	Result._m21 = 2.f * (qyz - qwx);
	Result._m22 = 1.f - 2.f * (qxx + qyy);
	Result._m23 = 0.f;

	Result._m30 = 0.f;
	Result._m31 = 0.f;
	Result._m32 = 0.f;
	Result._m33 = 1.f;
	

}


void quat_cast_xm(in matrix<float, 4, 4> m, out float4 q)
{
	float r22 = m[2][2];
	if (r22 <= 0.f)  // x^2 + y^2 >= z^2 + w^2
	{
		float dif10 = m[1][1] - m[0][0];
		float omr22 = 1.f - r22;
		if (dif10 <= 0.f)  // x^2 >= y^2
		{
			float fourXSqr = omr22 - dif10;
			float inv4x = 0.5f / sqrt(fourXSqr);
			q.x = fourXSqr * inv4x;
			q.y = (m[0][1] + m[1][0]) * inv4x;
			q.z = (m[0][2] + m[2][0]) * inv4x;
			q.w = (m[1][2] - m[2][1]) * inv4x;
		}
		else  // y^2 >= x^2
		{
			float fourYSqr = omr22 + dif10;
			float inv4y = 0.5f / sqrt(fourYSqr);
			q.x = (m[0][1] + m[1][0]) * inv4y;
			q.y = fourYSqr * inv4y;
			q.z = (m[1][2] + m[2][1]) * inv4y;
			q.w = (m[2][0] - m[0][2]) * inv4y;
		}
	}
	else  // z^2 + w^2 >= x^2 + y^2
	{
		float sum10 = m[1][1] + m[0][0];
		float opr22 = 1.f + r22;
		if (sum10 <= 0.f)  // z^2 >= w^2
		{
			float fourZSqr = opr22 - sum10;
			float inv4z = 0.5f / sqrt(fourZSqr);
			q.x = (m[0][2] + m[2][0]) * inv4z;
			q.y = (m[1][2] + m[2][1]) * inv4z;
			q.z = fourZSqr * inv4z;
			q.w = (m[0][1] - m[1][0]) * inv4z;
		}
		else  // w^2 >= z^2
		{
			float fourWSqr = opr22 + sum10;
			float inv4w = 0.5f / sqrt(fourWSqr);
			q.x = (m[1][2] - m[2][1]) * inv4w;
			q.y = (m[2][0] - m[0][2]) * inv4w;
			q.z = (m[0][1] - m[1][0]) * inv4w;
			q.w = fourWSqr * inv4w;
		}
	}
}

void mat_cast_xm(in float4 Quaternion, out matrix<float, 4, 4> M)
{
	const float4 Constant1110 = float4( 1.0f, 1.0f, 1.0f, 0.0f);

	float4 Q0 = Quaternion + Quaternion;
	float4 Q1 = Quaternion * Q0;

	float4 V0 = float4(Q1.yxx, Constant1110.w);
	float4 V1 = float4(Q1.zzy, Constant1110.w);
	float4 R0 = Constant1110 - V0;
	R0 = R0 - V1;

	V0 = (Quaternion.xxyw);
	V1 = (Q0.zyzw);
	V0 = V0 * V1;

	V1 = (Quaternion.wwww);
	float4 V2 = (Q0.yzxw);
	V1 = (V1 * V2);

	float4 R1 = (V0 +  V1);
	float4 R2 = (V0 - V1);

	V0 = float4(R1.y, R2.xy, R1.z);
	V1 = float4(R1.x, R2.z, R1.x, R2.z);// XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Z, XM_PERMUTE_0X, XM_PERMUTE_1Z>(R1, R2);

	M[0] = float4(R0.x, V0.x, V0.y, R0.w);//XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W>(R0, V0);
	M[1] = float4(V0.z, R0.y, V0.w, R0.w);//XMVectorPermute<XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W>(R0, V0);
	M[2] = float4(V1.x, V1.y, R0.z, R0.z);//XMVectorPermute<XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(R0, V1);
	M[3] = float4(0,0,0,1);

}