#pragma once

class Game2D;
class Renderer2D;

template<class T>
class StateMachine;

template<class T>
class State
{
public:
	virtual ~State() = default;
	virtual void Enter(StateMachine<T>* stateMachine, Game2D* game)  = 0;
	virtual void Update(StateMachine<T>* stateMachine, Game2D* game, float dt) = 0;
	virtual void Render(StateMachine<T>* stateMachine, Game2D* game, Renderer2D* renderer) = 0;
	virtual void Exit(StateMachine<T>* stateMachine, Game2D* game)   = 0;
};