#include "Enemy.h"
#include "j1EntityController.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Render.h"


Enemy::Enemy(Entity::entityType type) : Entity(type)
{
}

Enemy::Enemy(Entity::entityType type, iPoint pos) : Entity(type)
{
	position.x = pos.x;
	position.y = pos.y;
	
	speed = { 0,0 };
	maxSpeed = { 300,300 };
	gravity = 0;
	direction_x = 1;
	colOffset = { 0,0 };
	Collider.h = 50;
	Collider.w = 50;
	Collider.x = pos.x;
	Collider.y = pos.y;
	sightOffset = { 400,300 };
	SightCollider.x = pos.x - sightOffset.x;
	SightCollider.y = pos.y - sightOffset.y;
	SightCollider.w = 1000;
	SightCollider.h = 650;
}




Enemy::~Enemy()
{
}
