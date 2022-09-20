
float4 quat_cast(matrix m)
{
	float4 Result;
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
	return Result;
}

