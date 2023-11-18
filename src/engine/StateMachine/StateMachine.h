#pragma once
#include "State.h"

template<class T>
class StateMachine
{
public:
	StateMachine() : currentState(nullptr) {}

	void ChangeState(State<T>* newState, Game2D* game)
	{
		if (currentState != nullptr)
		{
			currentState->Exit(this, game);
		}

		currentState = newState;

		if (currentState != nullptr)
		{
			currentState->Enter(this, game);
		}
	}

	void Enter(Game2D* game)
	{
		if (currentState != nullptr)
		{
			currentState->Enter(this, game);
		}
	}

	void Exit(Game2D* game)
	{
		if (currentState != nullptr)
		{
			currentState->Exit(this, game);
			delete currentState;
			currentState = nullptr;
		}
	}

	void Update(Game2D* game, float dt)
	{
		if (currentState != nullptr)
		{
			currentState->Update(this, game, dt);
		}
	}

	void Render(Game2D* game, Renderer2D* renderer)
	{
		if (currentState != nullptr)
		{
			currentState->Render(this, game, renderer);
		}
	}

private:
	State<T>* currentState;
};