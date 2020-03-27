#include "Renderer2D.h"
#include <algorithm>

Renderer2D::Renderer2D()
{
    basicShader = new Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");
    cube = new Mesh("Cube");
    cube->CreateQuad();
}

Renderer2D::~Renderer2D()
{
    for (int i = 0; i < objectPool.size(); i++)
    {
        delete objectPool[i];
        objectPool[i] = nullptr;
    }

    delete basicShader;
    basicShader = nullptr;

    delete cube;
    cube = nullptr;
}

void Renderer2D::AddObject(GameObject* newObject)
{
    std::vector<GameObject*>::iterator it = find(objectPool.begin(), objectPool.end(), newObject);
    
    if (it != objectPool.end())
    {
        std::cout << "GameObject already in Renderer: " << *it << '\n';
        return;
    }

    GameObject* go = newObject;
    go->SetID(objectPool.size());
    objectPool.push_back(go);
}

GameObject* Renderer2D::CreateQuad(glm::vec3 pos, glm::vec3 color, glm::vec3 scale)
{
    GameObject* go = new GameObject(cube, basicShader);
    go->Create(pos, color, scale, objectPool.size());

    objectPool.push_back(go);
    return go;
}

void Renderer2D::Draw()
{
    for (int i = 0; i < objectPool.size(); i++)
    {
        if (objectPool[i]->GetShader() != currentShader)
        {
            currentShader = objectPool[i]->GetShader();
            currentShader->Use();
            currentShader->setMat4("OrthoMatrix", orthoMatrix);
        }

        // Temp
        objectPool[i]->Update(0.0f);
        
        objectPool[i]->Draw();
    }
}
