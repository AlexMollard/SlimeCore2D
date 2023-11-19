#pragma once
#include <engine/ConsoleLog.h>
#include "Item.h"
#include "engine/GameObject.h"

class Bullet : public GameObject
{
public:
	int damage;

	Bullet(int bulletDamage) : damage(bulletDamage) {}
};
