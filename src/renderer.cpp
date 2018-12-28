#include "renderer.h"
#include <algorithm>


renderer::renderer()
{
}

void renderer::setViewport(int x, int y, int w, int h)
{
	viewport = Matrix::identity(4);

	viewport[0][3] = x + w / 2.f;
	viewport[1][3] = y + h / 2.f;
	viewport[2][3] = 255.f / 2.f;
	viewport[0][0] = w / 2.f;
	viewport[1][1] = h / 2.f;
	viewport[2][2] = 255.f / 2.f;
}

void renderer::setProjection(float coeff)
{
	projection = Matrix::identity(4);

	projection[3][2] = coeff;
}

void renderer::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	auto z = (center - eye).normalize();
	auto x = cross(z, up).normalize();
	auto y = cross(z, x).normalize();

	modelview = Matrix::identity(4);

	for (int i=0;i<3;i++)
	{
		modelview[0][i] = x[i];
		modelview[1][i] = y[i];
		modelview[2][i] = z[i];
		modelview[i][3] = -center[i];
	}
}

void renderer::triangle(Vec4f* pts, IShader &shader,TGAImage& image, TGAImage& zbuffer)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

	for(int i =0;i<3;i++)
	{
		for(int j=0;j<2;j++)
		{
			bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
			bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
		}
	}

	Vec2i P;
	TGAColor color;

	for(P.x = bboxmin.x;P.x<bboxmax.x;P.x++)
	{
		for(P.y=bboxmin.y;P.y<bboxmax.y;P.y++)
		{
			auto v1= pts[0] / pts[0][3];
			auto v2 = pts[1] / pts[1][3];
			auto v3 = pts[2] / pts[2][3];

			Vec3f bary = barycentric(Vec2f(v1.x, v1.y), Vec2f(v2.x, v2.y), Vec2f(v3.x, v3.y), Vec2f(P.x, P.y));

			float z = pts[0][2] * bary.x + pts[1][2] * bary.y + pts[2][2] * bary.z;

			float w = pts[0][3] * bary.x + pts[1][3] * bary.y + pts[2][3] * bary.z;

			int fragDepth = std::max(0, std::min(255, int(z / w + 0.5f)));

			if(bary.x<0 || bary.y<0 || bary.z<0 || zbuffer.get(P.x,P.y)[0]>fragDepth)
			{
				continue;
			}

			bool discard = shader.fragment(bary, color);

			if(!discard)
			{
				zbuffer.set(P.x, P.y, TGAColor(fragDepth));
				image.set(P.x, P.y, color);
			}
		}
	}

}

renderer::~renderer()
{
}

Vec3f renderer::barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
{
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2)
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // discarded.
}
