#include "Button.h"
#include "Renderer2D.h"

Button::Button()
{
	mesh = Renderer2D::GetInstance()->GetQuadMesh();
	shader = Renderer2D::GetInstance()->GetBasicShader();
}

Button::~Button()
{
}

void Button::Update(float deltaTime)
{
	boundingBox.UpdateBoundingBox(position, scale);

	if (isHeld)
		SetPos(position.x - inputManager->GetDeltaMouse().x, position.y - inputManager->GetDeltaMouse().y);

	if (boundingBox.GetMouseColliding())
		OnHover();
	else
		SetColor(defaultColor);

}

void Button::Create(glm::vec3 pos, glm::vec3 color, std::string name, glm::vec3 scale, int id)
{
	SetPos(pos);
	defaultColor = color;
	SetText(name);
	SetColor(color);
	SetScale(scale);
	SetID(id);
}

std::string Button::GetText()
{
	return text;
}

void Button::SetText(std::string newText)
{
	text = newText;
}

void Button::OnHover()
{
	if (inputManager->GetMouseDown(0))
		OnPress();
	else
	{
		SetColor(defaultColor * 0.75f);
		isHeld = false;
	}

}

void Button::OnPress()
{
	SetColor(defaultColor * 0.5f);
	isHeld = true;
}
