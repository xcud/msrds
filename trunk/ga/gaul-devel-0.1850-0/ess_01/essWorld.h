#pragma once




#include "../galib/population.h"
#include "../galib/entity.h"
#include "../galib/world.h"



class essWorld : public world
{
public:

	~essWorld(void);
	
	int _blackCount;
	int _blueCount;
	
	essWorld()
	{
		_chromosomeLen	= 1;
		_entitySize = 100;
	}

	entity * createEntity()
	{
		entity * pEntity = new entity;



		char * chromosome = new char [_chromosomeLen];

		for(int i =0 ; i < _chromosomeLen;i++)
		{
			int v = rand()%2;
			
			
			chromosome[i] = 0;
		}

		pEntity->_chromosome = chromosome;

		
		return pEntity;
	}

	int evaluate(population * pop)
	{

	}

	bool crossover(entity * mather,entity * father,entity **daughter, entity **son) 
	{
		

		for(int i =0;i < _chromosomeLen;i++)
		{
			int b = rand()%2;

			(*daughter)->_chromosome[i] = mather->_chromosome[i];
			(*son)->_chromosome[i] = father->_chromosome[i];

		}

		return true;
	}

	bool mutation(entity *entity)
	{
		

		if((rand()%100) < 10)
		{
			entity->_chromosome[0] = 100;
		}
		
		return true;
	}

	void rank(entity ** entityList , entity * entity)
	{

	}


	void struggle(entity ** entityList , entity * entity) 
	{
		entity->_fitness = 0.0;
		for(int i = 0 ; i< 10;i++)
		{
			int index = rand()%_entitySize;
			// 착한사람
			if( entity->_chromosome[0] == 0 
				&& entityList[index]->_chromosome[0] == 0)
			{
				entity->_fitness	-= 2;
			// 착한사람,속이는 사람
			}else if(entity->_chromosome[0] == 0 
				&& entityList[index]->_chromosome[0] == 1)
			{
				entity->_fitness	+= 1;
				// 
			}else if(entity->_chromosome[0] == 1 
				&& entityList[index]->_chromosome[0] == 0)
			{
				entity->_fitness	-= 1;
			}else
			{
				entity->_fitness	+= 2;
			}
		}

	}

	bool survival(entity ** m,entity ** f )
	{
		
		return true;
	}

	void select(entity ** entityList,entity ** mather,entity ** father)
	{
		
		*mather = entityList[rand()%_entitySize];
		*father = entityList[rand()%_entitySize];
	}

	void update(entity * entity)
	{
		entity->_chromosome[]
	}

	
};
