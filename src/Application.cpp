
#include <memory>
#include "TGAImage.h"
#include "Model.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "Matrix.h"

const TGAColor white(255, 255, 255, 255);
const TGAColor red(255, 0, 0, 255);
const TGAColor green(0, 255, 0, 255);

Vec3f camera(0, 0,3);

float intensity;

const int width = 800;
const int height = 800;
const int depth = 255;

TGAColor getColorFromTextureMap(TGAImage& texture, Vec3f texCoords);

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec3f* textureCoords, TGAImage &image, TGAImage& texture,float intensity, int *zbuffer);

Vec3f world2screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
	Vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i]; //x=> 1- > C.y-A.y , 0 -> C.x-A.x
		s[i][1] = B[i] - A[i]; //y=> 1- > B.y-A.y , 0 -> B.x-A.x
		s[i][2] = A[i] - P[i]; //z=> 1- > C.y-P.y , 0 -> C.x-P.x
	}
	Vec3f u = cross(s[0], s[1]); //CA.x,BA.x,CP.x  -  CA.y, BA.y, CP.y


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

void render(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor* color, Vec3f* vertexNormals,
                         Vec3f* textureCoords, TGAImage& texture)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	Vec2f clamp(image.getWidth() - 1, image.getHeight() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	Vec3f P;
	Vec3f light(0.0, 0.0, -1.0);
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);

			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

			P.z = 0;

			for (int i = 0; i < 3; i++)
			{
				P.z += pts[i][2] * bc_screen[i];
			}


			if (zbuffer[int(P.x + P.y * width)] < P.z)
			{
				zbuffer[int(P.x + P.y * width)] = P.z;

				auto baryCentricTextCoords = textureCoords[0] * bc_screen[0] + textureCoords[1] * bc_screen[1] +
					textureCoords[2] * bc_screen[2];

				TGAColor finalColor;
				TGAColor texColor = getColorFromTextureMap(texture, baryCentricTextCoords);
				if(intensity>0)
				{
					image.set(P.x, P.y, texColor *intensity);
				}								
			}
		}
	}
}

TGAColor getColorFromTextureMap(TGAImage& texture, Vec3f texCoords)
{
	int worldX = (texCoords.x) * texture.getWidth();
	int worldY = (texCoords.y) * texture.getHeight();
	return texture.get(worldX, worldY);
}


//executes without barycentric coordinates
void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec3f* textureCoords, TGAImage& image, TGAImage& texture, float intensity,
	int* zbuffer)
{
	if (t0.y == t1.y && t0.y == t2.y) return;

	if (t0.y > t1.y) { std::swap(t0, t1); std::swap(textureCoords[0], textureCoords[1]); }
	if (t0.y > t2.y) { std::swap(t0, t2); std::swap(textureCoords[0], textureCoords[2]); }
	if (t1.y > t2.y) { std::swap(t1, t2); std::swap(textureCoords[1], textureCoords[2]); }

	auto totalHeight = t2.y - t0.y;

	for(int i=0;i<totalHeight;i++)
	{
		bool secondHalf = i > t1.y - t0.y || t1.y == t0.y;

		int segmentHeight = secondHalf ? t2.y - t1.y : t1.y - t0.y;

		float alpha = (float)i / totalHeight;

		float beta = (float)(i - (secondHalf ? t1.y - t0.y : 0)) / segmentHeight;

		auto A = t0 + Vec3f(t2 - t0)*alpha;
		auto B = secondHalf ? t1 + Vec3f(t2 - t1)*beta : t0 + Vec3f(t1 - t0)*beta;

		auto uvA = textureCoords[0] + (textureCoords[2] - textureCoords[0])*alpha;

		auto uvB = secondHalf ? textureCoords[1] + (textureCoords[2] - textureCoords[0])*beta : textureCoords[0] + (textureCoords[1] - textureCoords[0])*beta;

		if(A.x>B.x)
		{
			std::swap(A, B);
			std::swap(uvA, uvB);
		}

		for(int j=A.x;j<=B.x;j++)
		{
			float phi = B.x == A.x ? 1. : static_cast<float>(j - A.x) / static_cast<float>(B.x - A.x);

			Vec3i   P = Vec3f(A) + Vec3f(B - A)*phi;

			Vec3f uvP = uvA + (uvB - uvA)*phi;
		
			int idx = P.x + P.y*width;

			if(zbuffer[idx]<P.z)
			{
				zbuffer[idx] = P.z;
				auto color = getColorFromTextureMap(texture, uvP);
				image.set(P.x, P.y, color*intensity);
			}
		}


	}
}

Matrix makeViewPort(int x,int y, int w,int h)
{
	Matrix m = Matrix::identity(4);
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

Matrix vector2Matrix(Vec3f v)
{
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}


Matrix lookAt(Vec3f eye,Vec3f center, Vec3f up)
{
	Vec3f z = (eye - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z,x).normalize();

	Matrix result = Matrix::identity(4);

	for(int i=0;i<3;i++)
	{
		result[0][i] = x[i];
		result[1][i] = y[i];
		result[2][i] = z[i];

		result[i][3] = -center[i];
	}

	return result;
}

Vec3f matrix2Vector(Matrix m)
{
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

int main(int argc, char** argv)
{
	int* zbuffer = new int[width * height];

	for (int i = 0; i < width * height; i++)
	{
		zbuffer[i] = std::numeric_limits<int>::min();
	}

	Matrix projection = Matrix::identity(4);

	Matrix viewPort = makeViewPort(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

	projection[3][2] = -1.f / camera.z;

	std::string modelPath("resource/model/head.obj");
	std::string texturePath("resource/texture/head_diffuse.tga");

	auto model = std::make_unique<Model>(modelPath.c_str());

	TGAImage image(width, height, TGAImage::RGB);

	TGAImage texture;

	if (!texture.readTGAFile(texturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << texturePath << std::endl;

		return -1;
	}

	texture.flipVertically();

	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		std::vector<Vec3i> facesAndTextures = model->face(i);

		Vec3f worldCoords[3];
		Vec3f textureCoords[3];
		Vec3f screenCords[3];

		Vec3f vertexNormals[3];

		for (int j = 0; j < 3; j++)
		{
			worldCoords[j] = model->vert(facesAndTextures[j].x);

			textureCoords[j] = model->texture(facesAndTextures[j].y);

			vertexNormals[j] = model->normal(facesAndTextures[j].z);

			screenCords[j] = matrix2Vector((viewPort*projection)*vector2Matrix(worldCoords[j]));

		}

		Vec3f lDir(0.0, 0.0, -1.0);
		Vec3f normal = cross(worldCoords[2] - worldCoords[0], worldCoords[1] - worldCoords[0]);

		normal.normalize();

		intensity = normal.dot(lDir);

		if(intensity>0)
		{
			Vec3f t0 = screenCords[0];
			Vec3f t1 = screenCords[1];
			Vec3f t2 = screenCords[2];

			triangle(t0, t1, t2, textureCoords, image, texture, intensity, zbuffer);
		}

		
	}

	image.flipVertically();

	image.writeTGAFile("projection5.tga");

	delete[] zbuffer;
	return 0;
}
