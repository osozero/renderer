#pragma once

#include<cmath>

template<class t>
	struct Vec2
{
	union
	{
		struct
		{
			t u;
			t v;
		};

		struct
		{
			t x;
			t y;
		};
	};

	Vec2()
	: u(0),v(0)	{}

	Vec2(t _u,t _v)
	: u(_u),v(_v) {}

	inline Vec2<t> operator+(const Vec2<t> &vec) const
	{
		return Vec2<t>(u + vec.u, v + vec.v);
	}

	inline Vec2<t> operator-(const Vec2<t> &vec) const
	{
		return Vec2<t>(u - vec.u.v - vec.v);
	}

	inline Vec2<t> operator*(float f)	const
	{
		return Vec2<t>(u*f, v*f);
	}

	template <class > 
		friend std::ostream& operator<<(std::ostream &os, Vec2<t> vec);
};