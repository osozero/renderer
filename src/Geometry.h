#pragma once

#include<cmath>
#include <ostream>

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

	template<class t>
		struct Vec3
	{
		union
		{
			struct
			{
				t x;
				t y;
				t z;
			};

			struct
			{
				t invert;
				t iuv;
				t inorm;
			};

			t raw[3];
		};

		Vec3()
			:x(0),y(0),z(0) {}

		Vec3(t _x, t _y, t _z)
			:x(_x),y(_y),z(_z) {}

		inline Vec3<t> cross(const Vec3<t> &v) const
		{
			return Vec3<t>(
					y*v.z - v.y*z,
					-(x*v.z - v.x*z),
					x*v.y - v.x*y);
		}

		inline t dot(const Vec3<t> &v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		inline Vec3<t> operator+(const Vec3<t> &v) const
		{
			return Vec3<t>(x + v.x, y + v.y, z + v.z);
		}

		inline Vec3<t> operator-(const Vec3<t> &v) const
		{
			return Vec3<t>(x - v.x, y - v.y, z - v.z);
		}

		inline Vec3<t> operator*(float f) const
		{
			return Vec3<t>(x*f, y *f, z *f);
		}

		float length() const
		{
			return sqrt(x.x + y.y + z.z);
		}

		Vec3<t>& normalize() const
		{
			*this = (*this) / length();
			return *this;
		}

		template<class > friend std::ostream& operator<<(std::ostream &os, Vec3<t> v);
	};

template <class>
std::ostream& operator<<(std::ostream& os, Vec2<float> vec)
{
	os << "(" << vec.x << ", " << vec.y <<")\n";
	return os;
}

template <class>
std::ostream& operator<<(std::ostream& os, Vec3<float> v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return os;
}

	typedef Vec2<float> Vec2f;
	typedef Vec2<int> Vec2i;
	typedef Vec3<float> Vec3f;
	typedef Vec3<int> Vec3i;