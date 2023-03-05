#pragma once

class GameObject;

enum type
{
	Water,
	Ground,
	TopLeft,
	TopCenter,
	TopRight,
	InnerTopLeft,
	InnerTopRight,
	MiddleLeft,
	MiddleRight,
	InnerBottomLeft,
	InnerBottomRight,
	BottomLeft,
	BottomCenter,
	BottomRight,
	Wall,
	Stone,
	Tree
};

struct Cell
{
	glm::vec2 position = glm::vec2(0);
	type cellType = type::Water;
	type preCellType = type::Water;
	GameObject* object = nullptr;
};