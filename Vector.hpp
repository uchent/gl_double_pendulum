#ifndef _VEC_H_
#define _VEC_H_

struct Vector3f
{
public:
	float x, y, z;

	Vector3f()
	{
		x = y = z = 0;
	}

	Vector3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

#endif
