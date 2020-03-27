#include "Game2D.h"

Game2D::Game2D()
{
	testShader = new Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");
	cube = new Mesh("Cube");
	cube->create(0.5f, 0.5f);
	testShader->Use();
	go = new GameObject(cube, testShader);
}

Game2D::~Game2D()
{
	delete testShader;
	delete cube;
	delete go;
}

void Game2D::Update(float deltaTime)
{
	timer += deltaTime;
	go->SetPos(glm::cos(timer), glm::sin(timer));
	go->SetColor(glm::sin(timer), glm::sin(timer + 1), glm::sin(timer + 2));
}

void Game2D::Draw()
{
	testShader->setMat4("OrthoMatrix", orthoMatrix);
	go->Draw();
}
