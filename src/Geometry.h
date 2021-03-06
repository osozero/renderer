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

		t raw[2];
	};

	Vec2<t>()
	: u(0),v(0)	{}

	Vec2<t>(t _u,t _v)
	: u(_u),v(_v) {}


	inline Vec2<t> operator+(const Vec2<t> &vec) const
	{
		return Vec2<t>(u + vec.u, v + vec.v);
	}

	inline Vec2<t> operator-(const Vec2<t> &vec) const
	{
		return Vec2<t>(u - vec.u,v - vec.v);
	}

	inline Vec2<t> operator*(float f)	const
	{
		return Vec2<t>(u*f, v*f);
	}

	inline t& operator[](int i)
	{
		return raw[i];
	}

	inline t operator[](int i) const
	{
		return raw[i];
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

		Vec3<t>()
			:x(0),y(0),z(0) {}

		Vec3<t>(t _x, t _y, t _z)
			:x(_x),y(_y),z(_z) {}

		Vec3<t>(Vec2<t> v2, t f)
			:x(v2.x), y(v2.y), z(f){}
	
		template <class u> Vec3<t>(const Vec3<u>& v);

			inline Vec3<t>(const Vec3<t> &v) : x(t()), y(t()), z(t()) { *this = v; }


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
			return Vec3<t>(this->x - v.x, this->y - v.y, this->z - v.z);
		}

		inline Vec3<t> operator*(float f) const
		{
			return Vec3<t>(x*f, y *f, z *f);
		}

		inline Vec3<t> operator*=(Vec3<t> &v)
		{
			Vec3<t> r;
			r.x = this->x * v.x;
			r.y = this->y * v.y;
			r.z = this->z * v.z;

			return r;
		}

		inline Vec3<t> operator/(float f)
		{
			this->x/= f;
			this->y/= f;
			this->z /= f;

			return *this;
		}

		

		inline t& operator[](int i)
		{
			return raw[i];
		}

		inline t operator[](int i) const
		{
			return raw[i];
		}

		float length() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		Vec3<t>& normalize()
		{
			*this = (*this) * (1/this->length());
			return *this;
		}

		template<class > friend std::ostream& operator<<(std::ostream &os, Vec3<t> v);
			
		};

template<class t>
	struct Vec4
{
		union
		{
			struct
			{
				t x;
				t y;
				t z;
				t w;
			};
			t raw[4];
		};

		Vec4<t>()
			: x(0), y(0), z(0),w(0) {}

		Vec4<t>(t _x, t _y, t _z,t _w)
			: x(_x), y(_y), z(_z),w(_w) {}

		Vec4<t>(Vec3<t> v,t _w)
			:x(v.x),y(v.y),z(v.z),w(_w){}


		inline t& operator[](int i)
		{
			return raw[i];
		}

		inline t operator[](int i) const
		{
			return raw[i];
		}

		inline Vec4<t> operator+(const Vec4<t> &v) const
		{
			return Vec4<t>(x + v.x, y + v.y, z + v.z,w);
		}

		inline Vec4<t> operator-(const Vec4<t> &v) const
		{
			return Vec4<t>(this->x - v.x, this->y - v.y, this->z - v.z,w);
		}

		inline Vec4<t> operator*(float f) const
		{
			return Vec4<t>(x*f, y *f, z *f,w);
		}

		inline Vec4<t> operator/(float f)
		{
			Vec4<t> v;
			v.x = this->x / f;
			v.y = this->y / f;
			v.z = this->z / f;

			return v;
		}

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

template<class t>
 Vec3<t> cross(const Vec3<t> &v1, const Vec3<t> &v2)
{
	return Vec3<t>(v1.y * v2.z - v1.z*v2.y, -(v1.x*v2.z - v2.x*v1.z), v1.x*v2.y - v2.x*v1.y);
}

  template <> template <>
  inline Vec3<int>::Vec3(const Vec3<float> &v)
	 : x(int(v.x + .5)), y(int(v.y + .5)), z(int(v.z + .5)) {}

  template <> template <>
  inline Vec3<float>::Vec3(const Vec3<int> &v)
	 : x(v.x), y(v.y), z(v.z) {}


	typedef Vec2<float> Vec2f;
	typedef Vec2<int> Vec2i;
	typedef Vec3<float> Vec3f;
	typedef Vec3<int> Vec3i;

	typedef Vec4<float> Vec4f;
	typedef Vec4<int> Vec4i;