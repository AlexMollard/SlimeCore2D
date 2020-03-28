#include "Renderer2D.h"
#include <algorithm>

Renderer2D::Renderer2D()
{
    basicShader = new Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");
    quad = new Mesh("Cube");
    quad->CreateQuad();

    LoadTexture("..\\Textures\\White.png");
    LoadTexture("..\\Textures\\Test.png");
    LoadTexture("..\\Textures\\CreeperHead.png");
    LoadTexture("..\\Textures\\CreeperSkin.png");
}

Renderer2D::~Renderer2D()
{
    for (int i = 0; i < objectPool.size(); i++)
    {
        delete objectPool[i];
        objectPool[i] = nullptr;
    }

    for (int i = 0; i < buttonPool.size(); i++)
    {
        delete buttonPool[i];
        buttonPool[i] = nullptr;
    }

    for (int i = 0; i < texturePool.size(); i++)
    {
        delete texturePool[i];
        texturePool[i] = nullptr;
    }

    delete basicShader;
    basicShader = nullptr;

    delete quad;
    quad = nullptr;
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

GameObject* Renderer2D::CreateQuad(glm::vec3 pos, glm::vec3 color, glm::vec3 scale, int TexIndex)
{
    GameObject* go = new GameObject(quad, basicShader);
    go->Create(pos, color, scale, objectPool.size());
    
    go->SetTexture(texturePool[TexIndex]);

    objectPool.push_back(go);
    return go;
}

Button* Renderer2D::CreateButton(glm::vec3 pos, glm::vec3 color, std::string text, glm::vec3 scale, int TexIndex)
{
    Button* go = new Button();
    go->Create(pos, color, text, scale, buttonPool.size());


    go->SetTexture(texturePool[TexIndex]);

    buttonPool.push_back(go);
    return go;
}

Texture* Renderer2D::LoadTexture(std::string dir)
{
    Texture* tempTex = new Texture(dir);
    
    texturePool.push_back(tempTex);
    return tempTex;
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

        if (objectPool[i]->GetTexture() == nullptr)
        {
            currentTexture = nullptr;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if (objectPool[i]->GetTexture() != currentTexture)
        {
            currentTexture = objectPool[i]->GetTexture();

            glActiveTexture(GL_TEXTURE0);
            currentTexture->Bind();
        }

        // Temp
        objectPool[i]->Update(0.0f);
        
        objectPool[i]->Draw();
    }

    for (int i = 0; i < buttonPool.size(); i++)
    {
        if (buttonPool[i]->GetShader() != currentShader)
        {
            currentShader = buttonPool[i]->GetShader();
            currentShader->Use();
            currentShader->setMat4("OrthoMatrix", orthoMatrix);
        }

        if (buttonPool[i]->GetTexture() == nullptr)
        {
            currentTexture = nullptr;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if (buttonPool[i]->GetTexture() != currentTexture)
        {
            currentTexture = buttonPool[i]->GetTexture();
            glActiveTexture(GL_TEXTURE0);
            currentTexture->Bind();
        }

        // Temp
        buttonPool[i]->Update(0.0f);

        buttonPool[i]->Draw();
    }
}

Mesh* Renderer2D::GetQuadMesh()
{
    return quad;
}

Shader* Renderer2D::GetBasicShader()
{
    return basicShader;
}
