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
		_blackCount = 0;
			_blueCount = 0;
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
			(*daughter)->_fitness = mather->_fitness;
			(*son)->_chromosome[i] = father->_chromosome[i];
			(*son)->_fitness = father->_fitness;

		}

		return true;
	}

	bool mutation(entity *entity)
	{
		

		if((rand()%100) < 1)
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
		for(int i = 0 ; i< 100;i++)
		{
			int index = rand()%_entitySize;
			
			if( entity->_chromosome[0] == 0 //비둘기
				&& entityList[index]->_chromosome[0] == 0/*비둘기*/)
			{
				entity->_fitness	-= 40;
				entityList[index]->_fitness += 10;
			// 착한사람,속이는 사람
			}else if(entity->_chromosome[0] == 0 //비둘기
				&& entityList[index]->_chromosome[0] == 100/*매*/)
			{
				entity->_fitness	+= 0;
				entityList[index]->_fitness -= 50;
				// 
			}else if(entity->_chromosome[0] == 100 
				&& entityList[index]->_chromosome[0] == 0)
			{
				entity->_fitness	-= 50;
				entityList[index]->_fitness += 0;

			}else
			{
				entity->_fitness	+= 80;
				entityList[index]->_fitness -= 50;
			}
		}

	}

	bool survival(entity ** m,entity ** f )
	{
		
		return true;
	}

	void select(entity ** entityList,entity ** mather,entity ** father)
	{
		
		*mather = entityList[rand()%25];
		*father = entityList[rand()%25];
	}

	void update(entity * entity)
	{
		if( entity->_chromosome[0] == 0)
		{
			_blueCount++;	
		}else
		{
			_blackCount++;
		}
	}

	
};
