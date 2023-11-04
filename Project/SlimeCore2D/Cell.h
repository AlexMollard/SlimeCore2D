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
	glm::vec2 GetPosition() const { return m_position; }
	void SetPosition(glm::vec2 val) { m_position = val; }
	
	type GetCellType() const { return m_cellType; }
	void SetCellType(type val) { m_cellType = val; }
	
	type GetPreCellType() const { return m_preCellType; }
	void SetPreCellType(type val) { m_preCellType = val; }

	GameObject* GetGameObject() const { return m_object; }
	void SetGameObject(GameObject* val) { m_object = val; }
private:
	glm::vec2 m_position = glm::vec2(0);
	type m_cellType = type::Water;
	type m_preCellType = type::Water;
	GameObject* m_object = nullptr;
};