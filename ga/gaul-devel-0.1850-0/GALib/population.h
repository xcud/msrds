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
	entity * _entityList2[512];
	ievaluate * _evaluate;
	world * _world;

	void Init(world * world)
	{
		_world = world;
		_size = world->_entitySize;
		for(int i = 0; i < 512;i++)
		{
			_entityList[i] = world->createEntity();
			_entityList2[i] = world->createEntity();
		}



	}

	void AddEntity(entity * entity)
	{
		_entityList[_size] = entity;
		_size++;
	}
	
	entity * pTemp;

	void rank(entity ** entityList,int max)
	{
		for(int i = 0; i < max;i++)
		{
			for(int r = i+1; r < max;r++)
			{
				if( entityList[r]->_fitness < entityList[i]->_fitness)
				{
					pTemp = entityList[i];
					entityList[i] = entityList[r];
					entityList[r] = pTemp;
				}
			}
		}

	}


	void swap(int count)
	{
		int j = 0;
		for(int i = _size-count;i< _size;i++)
		{
			entity * temp = _entityList[i];
			_entityList[i] = _entityList2[j];
			_entityList2[j] = temp;
			j++;
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
		rank(_entityList,_size);

		//미달제거

		//
		entity	*mother, *father;	/* Parent entities. */
		//entity	*daughter, *son;

		int size = 0;
		int i = 0;

		for(int i = 0; i< _size;i++)
		{
		
			//선택시 골고루 잡아줘야한다.
			//변이 추가
			_world->select(_entityList,&mother,&father);
			//_world->crossover(&(_entityList[i++]),&(_entityList[i++]),&(_entityList[size]),&(_entityList[size+1]));
			_world->crossover(mother,father,&(_entityList2[size]),&(_entityList2[size+1]));
			_world->struggle(_entityList2,_entityList2[size]);
			_world->struggle(_entityList2,_entityList2[size+1]);

			size +=2;
		}

		rank(_entityList2,size);
	
		swap(10);

		printf("%f", _entityList[0]->_fitness);

		

	}


};
