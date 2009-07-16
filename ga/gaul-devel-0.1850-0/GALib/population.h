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
		_size = size;
		for(int i = 0; i < _size;i++)
		{
			_entityList[i] = world->createEntity();
		}



	}

	void AddEntity(entity * entity)
	{
		_entityList[_size] = entity;
		_size++;
	}

	void evaluate()
	{

		
		//적합성 업데이트
		for(int i = 0; i < _size;i++)
		{
			_world->struggle(_entityList,_entityList[i]);
		}
		
		//랭킹설정

		//미달제거

		//
		entity	*mother, *father;	/* Parent entities. */
		entity	*daughter, *son;

		for(int i = 0; i < _size;i++)
		{
			
			_world->select(&mother,&father);
			_world->crossover(&mother,&father,&daughter, &son);
		}


		


	}


};
