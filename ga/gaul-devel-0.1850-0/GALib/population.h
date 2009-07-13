#pragma once

#include "world.h"

class ievaluate;
class entity;

class population
{
public:
	population(void);
	~population(void);

	int		_max_size;		/* Current maximum population size. */
	int _size;
	entity * _entityList[128];

	ievaluate * _evaluate;
	world * _world;

	void Init(int size,world * world)
	{
		_world = world;
	}

	void AddEntity(entity * entity)
	{
		_entityList[_size] = entity;
		_size++;
	}

	void evaluate()
	{
		_world->
	}


};
