
#include <memory>
#include "TGAImage.h"
#include "Model.h"
#include <algorithm>
#include <iostream>
#include <string>

const TGAColor white(255, 255, 255, 255);
const TGAColor red(255, 0, 0, 255);
const TGAColor green(0, 255, 0, 255);

float intensity;

const int width = 800;
const int height = 800;

void triangleWithZBuffer(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor* color, Vec3f* vertexNormals,
                         Vec3f* textureCoords, TGAImage& texture);

Vec3f world2screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

TGAColor getColorFromTextureMap(TGAImage& texture, Vec3f texCoords);

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

void triangleWithZBuffer(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor* color, Vec3f* vertexNormals,
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
	Vec3f light(0, 0, -1);
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

				Vec3f normal = vertexNormals[0] * bc_screen[0] +
					vertexNormals[1] * bc_screen[1] +
					vertexNormals[2] * bc_screen[2];

				normal.normalize();
				light.normalize();
				float intensity = normal.dot(light);

				auto baryCentricTextCoords = textureCoords[0] * bc_screen[0] + textureCoords[1] * bc_screen[1] +
					textureCoords[2] * bc_screen[2];

				TGAColor finalColor;
				TGAColor texColor = getColorFromTextureMap(texture, baryCentricTextCoords);
				/*if (intensity >= 0)
				{
					image.set(P.x, P.y, texColor * intensity);
				}else*/
				{
					image.set(P.x, P.y, texColor/**intensity*/);
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


int main(int argc, char** argv)
{
	float* zbuffer = new float[width * height];

	for (int i = 0; i < width * height; i++)
	{
		zbuffer[i] = -std::numeric_limits<float>::max();
	}

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

		TGAColor colors[3];

		for (int j = 0; j < 3; j++)
		{
			worldCoords[j] = model->vert(facesAndTextures[j].x);

			textureCoords[j] = model->texture(facesAndTextures[j].y);

			vertexNormals[j] = model->normal(facesAndTextures[j].z);

			screenCords[j] = world2screen(worldCoords[j]);
		}

		triangleWithZBuffer(screenCords, zbuffer, image, colors, vertexNormals, textureCoords, texture);
	}

	image.flipVertically();

	image.writeTGAFile("texturedX2.tga");

	return 0;
}
