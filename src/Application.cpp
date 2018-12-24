
#include <memory>
#include <iostream>
#include "TGAImage.h"
#include "Model.h"
#include <algorithm>

const TGAColor white(255, 255, 255, 255);
const TGAColor red(255, 0, 0, 255);
const TGAColor green(0, 255, 0, 255);

const int width = 800;
const int height = 800;


Vec3f world2screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

Vec3f barycentric2(Vec2i* pts, Vec2i P)
{
	Vec3f u = cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]),
	                Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

Vec3f barycentric3(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
	float u;
	float v;
	float w;

	//A.z = 0;
	//B.z = 0;
	//C.z = 0;
	//
	//P.z = 0;

	Vec3f AB(B - A);
	Vec3f AP(P - A);

	Vec3f BC(C - B);
	Vec3f BP(P - B);

	Vec3f CA(A - C);
	Vec3f CP(P - C);

	//for plane normal calculation
	Vec3f AC(C - A);

	Vec3f normal = cross(AB, AC);

	float denom = normal.dot(normal);

	Vec3f crossABAP = cross(AB, AP);

	if (normal.dot(crossABAP) < 0)
	{
		return Vec3f(-1, -1, -1);
	}

	Vec3f crossBCBP = cross(BC, BP);

	if ((u = normal.dot(crossBCBP)) < 0)
	{
		return Vec3f(-1, -1, -1);
	}

	Vec3f crossCACP = cross(CA, CP);

	if ((v = normal.dot(crossCACP) < 0))
	{
		return Vec3f(-1, -1, -1);
	}

	u /= denom;
	v /= denom;
	w = 1 - u - v;

	return Vec3f(w, u, v);
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];//x=> 1- > C.y-A.y , 0 -> C.x-A.x
		s[i][1] = B[i] - A[i];//y=> 1- > B.y-A.y , 0 -> B.x-A.x
		s[i][2] = A[i] - P[i];//z=> 1- > C.y-P.y , 0 -> C.x-P.x
	}
	Vec3f u = cross(s[0], s[1]);//CA.x,BA.x,CP.x  -  CA.y, BA.y, CP.y


	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void line(Vec2i v1, Vec2i v2, TGAImage& image, const TGAColor& color)
{
	bool steep = false;

	if (std::abs(v1.x - v2.x) < std::abs(v1.y - v2.y))
	{
		std::swap(v1.x, v1.y);
		std::swap(v2.x, v2.y);
		steep = true;
	}

	if (v1.x > v2.x)
	{
		std::swap(v1.x, v2.x);
		std::swap(v1.y, v2.y);
	}

	int dx = v2.x - v1.x;
	int dy = v2.y - v1.y;

	int derror = std::abs(dy) * 2;
	int error = 0;

	int y = v1.y;

	for (int x = v1.x; x <= v2.x; x++)
	{
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		error += derror;
		if (error > dx)
		{
			y += (v2.y > v1.y ? 1 : -1);
			error -= dx * 2;
		}
	}
}

//send to barycentric2
void triangle(Vec2i* pts, TGAImage& image, const TGAColor& color)
{
	Vec2i bboxmin(image.getWidth() - 1, image.getHeight() - 1);

	Vec2i bboxmax(0, 0);

	Vec2i clamp(image.getWidth() - 1, image.getHeight() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	Vec2i P;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric2(pts, P);

			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
			{
				continue;
			}

			image.set(P.x, P.y, color);
		}
	}
}

void triangleWithZBuffer(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(std::numeric_limits<float>::min(),std::numeric_limits<float>::min());
	Vec2f clamp(image.getWidth() - 1, image.getHeight() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y*width)] < P.z) {
				zbuffer[int(P.x + P.y*width)] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}
}

void loadModelAndDrawWireframe()
{
	std::string arg = "resource/model/head.obj";

	auto model = std::make_unique<Model>(Model(arg.c_str()));

	TGAImage image(width, height, TGAImage::RGB);

	const Vec3f lightDirection(0, 0, -1);

	Vec3f normal;
	float intensity = 0.0f;
	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		std::vector<int> face = model->face(i);

		Vec2i scrCoords[3];
		Vec3f worldCoords[3];
		for (int j = 0; j < 3; j++)
		{
			worldCoords[j] = model->vert(face[j]);
			scrCoords[j] = Vec2i((worldCoords[j].x + 1) * width / 2, (worldCoords[j].y + 1) * height / 2);
		}
		normal = cross(worldCoords[2] - worldCoords[0], worldCoords[1] - worldCoords[0]);

		normal.normalize();

		intensity = lightDirection.dot(normal);

		if (intensity > 0)
		{
			triangle(scrCoords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255));
		}
	}

	image.flipVertically();
	image.writeTGAFile("rasterized-lighted-face.tga");
}

void rasterizeOneTriangle()
{
	TGAImage frame(200, 200, TGAImage::RGB);
	Vec2i pts[3] = {Vec2i(10, 10), Vec2i(100, 30), Vec2i(190, 160)};


	triangle(pts, frame, TGAColor(255, 0, 0));

	frame.flipVertically();

	frame.writeTGAFile("rasterized2.tga");
}

int main(int argc, char** argv)
{
	float* zbuffer = new float[width * height];

	for (int i = 0; i < width * height; i++)
	{
		zbuffer[i] = std::numeric_limits<float>::min();
	}

	TGAImage image(width, height, TGAImage::RGB);

	std::string arg = "resource/model/head.obj";

	auto model = std::make_unique<Model>(Model(arg.c_str()));

	Vec3f lightDir(0.0, 0.0, -1.0);

	Vec3f normal;
	float intensity;

	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		std::vector<int> face = model->face(i);

		Vec3f pts[3];
		Vec3f worldCoords[3];
		for (int j = 0; j < 3; j++)
		{
			worldCoords[j] = model->vert(face[j]);
			pts[j] = world2screen(worldCoords[j]);
		}

		normal = cross(worldCoords[2] - worldCoords[0], worldCoords[1] - worldCoords[0]);

		normal.normalize();

		intensity = normal.dot(lightDir);

		if(intensity>0)
		{
			
		}

		triangleWithZBuffer(pts, zbuffer, image, TGAColor(intensity*255, intensity * 255, intensity * 255, 255));


	}

	image.flipVertically(); // i want to have the origin at the left bottom corner of the image
	image.writeTGAFile("zBuffered5.tga");

	return 0;
}
