#include "Renderer.h"
#include <algorithm>


Renderer::Renderer()
	:vPort(1.0f),
	proj(1.0f),
	mView(1.0f)
{
}

void Renderer::setViewport(int x, int y, int w, int h)
{
	viewport = Matrix::identity(4);

	viewport[0][3] = x + w / 2.f;
	viewport[1][3] = y + h / 2.f;
	viewport[2][3] = 255.f / 2.f;
	viewport[0][0] = w / 2.f;
	viewport[1][1] = h / 2.f;
	viewport[2][2] = 255.f / 2.f;

	
	vPort[0][3] = viewport[0][3];
	vPort[1][3] = viewport[1][3];
	vPort[2][3] = viewport[2][3];

	vPort[0][0] = viewport[0][0];
	vPort[1][1] = viewport[1][1];
	vPort[2][2] = viewport[2][2];
}

void Renderer::setProjection(float coeff)
{
	projection = Matrix::identity(4);

	projection[3][2] = coeff;

	proj[3][2] = coeff;
	
}

void Renderer::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f z = (eye - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();
	modelview = Matrix::identity(4);
	for (int i = 0; i < 3; i++) {
		modelview[0][i] = x[i];
		modelview[1][i] = y[i];
		modelview[2][i] = z[i];
		modelview[i][3] = -center[i];


		mView[0][i] = x[i];
		mView[1][i] = y[i];
		mView[2][i] = z[i];
		mView[i][3] = -center[i];
	}
}


void Renderer::triangle(Vec4f* pts, IShader &shader,TGAImage& image, TGAImage& zbuffer)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
			bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
		}
	}
	Vec2i P;
	TGAColor color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			
			auto v1 = pts[0] / pts[0][3];
			auto v2 = pts[1] / pts[1][3];
			auto v3 = pts[2] / pts[2][3];

		Vec3f c = barycentric(Vec2f(v1.x, v1.y), Vec2f(v2.x, v2.y), Vec2f(v3.x, v3.y), Vec2f(P.x, P.y));


			float z = pts[0][2] * c.x + pts[1][2] * c.y + pts[2][2] * c.z;
			float w = pts[0][3] * c.x + pts[1][3] * c.y + pts[2][3] * c.z;
			int frag_depth = std::max(0, std::min(255, int(z / w + .5)));
			if (c.x < 0 || c.y < 0 || c.z<0 || zbuffer.get(P.x, P.y)[0]>frag_depth) continue;
			bool discard = shader.fragment(c, color);
			if (!discard) {
				zbuffer.set(P.x, P.y, TGAColor(frag_depth));
				image.set(P.x, P.y, color);
			}
		}
	}

}


void Renderer::triangle(glm::mat4x3 &clipc, IShader &shader, TGAImage &image, float *zbuffer) {
	glm::mat3x3 m3x3vPort(vPort);
	glm::mat<3, 4, float> pts = glm::transpose(m3x3vPort*clipc);
	glm::mat<3, 2, float> pts2;
	for (int i = 0; i < 3; i++) {
		pts2[i] = (pts[i] / pts[i][3]);
	}
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.getWidth() - 1, image.getHeight() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
		}
	}
	Vec2i P;
	TGAColor color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(Vec2f(pts2[0][0],pts2[0][1]), Vec2f(pts2[1][0],pts2[1][1]), Vec2f(pts2[2][0],pts2[2][1]), Vec2f(P.x,P.y));
			Vec3f bc_clip = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
			float frag_depth = glm::dot(clipc[2], glm::vec3(bc_clip.x,bc_clip.y,bc_clip.z));
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z<0 || zbuffer[P.x + P.y*image.getWidth()]>frag_depth) continue;
			bool discard = shader.fragment(bc_clip, color);
			if (!discard) {
				zbuffer[P.x + P.y*image.getWidth()] = frag_depth;
				image.set(P.x, P.y, color);
			}
		}
	}
}

Renderer::~Renderer()
{
}

Vec3f Renderer::barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
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
