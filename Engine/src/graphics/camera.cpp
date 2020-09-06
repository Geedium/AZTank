#include "System.h"
#include "camera.h"

using namespace Augiwne::Graphics;

Camera::Camera(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	projection = Matrix4::Orthographic(left, right, bottom, top, near, far);
	view = Matrix4::Identity();
}

void Camera::Follow(Renderable2D* renderable)
{
	const float x = renderable->GetPosition().x + renderable->GetSize().x / 2.0f;
	const float y = renderable->GetPosition().y + renderable->GetSize().y / 2.0f;
	view = Matrix4::Translate(Vector2{ -x, -y });
}

void Camera::Apply(Shader& shader)
{
	shader.Enable();
	shader.SetUniformMatrix4("pr_matrix", projection);
	shader.SetUniformMatrix4("vw_matrix", view);
}