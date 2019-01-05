#pragma once
#include "IShader.h"
#include <glm/detail/type_mat3x3.hpp>
#include "Texture.h"

class DepthShader :
	public IShader
{
public:
	glm::mat3x3 varying_tri;

	Model model;

	glm::mat4 viewport;
	glm::mat4 proj;
	glm::mat4 modelview;

	DepthShader(Model &model_, glm::mat4 &viewport_, glm::mat4 &proj_, glm::mat4 &modelview_)
	:model(model_),
	viewport(viewport_),
	proj(proj_),
	modelview(modelview_)
	{};
	~DepthShader(){};
	Vec4f vertex(int iface, int nthVert) override;
	bool fragment(Vec3f bar, TGAColor& color) override;
};

