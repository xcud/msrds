#pragma once

#include "world.h"
#include "entity.h"
class ievaluate;
//class entity;

class population
{
public:
	population(void);
	~population(void);

	int		_max_size;		/* Current maximum population size. */
	int _size;
	entity * _entityList[512];

	ievaluate * _evaluate;
	world * _world;

	void Init(world * world)
	{
		_world = world;
		_size = world->_entitySize;
		for(int i = 0; i < 512;i++)
		{
			_entityList[i] = world->createEntity();
		}



	}

	void AddEntity(entity * entity)
	{
		_entityList[_size] = entity;
		_size++;
	}
	
	entity * pTemp;

	void rank(int max)
	{
		for(int i = 0; i < max;i++)
		{
			for(int r = i+1; r < max;r++)
			{
				if( _entityList[r]->_fitness < _entityList[i]->_fitness)
				{
					pTemp = _entityList[i];
					_entityList[i] = _entityList[r];
					_entityList[r] = pTemp;
				}
			}
		}

	}

	void evaluate()
	{

		
		//적합성 업데이트
		for(int i = 0; i < _size;i++)
		{
			_world->struggle(_entityList,_entityList[i]);
		}
		
		//랭킹설정
		
		

		//_world->rank();

		//미달제거

		//
		entity	*mother, *father;	/* Parent entities. */
		//entity	*daughter, *son;

		int size = _size;
		int i = 0;

		for(int i = 0; i< _size;i++)
		{
		
			//선택시 골고루 잡아줘야한다.
			_world->select(_entityList,&mother,&father);
			//_world->crossover(&(_entityList[i++]),&(_entityList[i++]),&(_entityList[size]),&(_entityList[size+1]));
			_world->crossover(mother,father,&(_entityList[size]),&(_entityList[size+1]));
			_world->struggle(_entityList,_entityList[size]);
			_world->struggle(_entityList,_entityList[size+1]);

			size +=2;
		}

		rank(size);

		
		printf("%f", _entityList[0]->_fitness);

		

	}


};
