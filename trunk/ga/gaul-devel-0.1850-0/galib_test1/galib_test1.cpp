// galib_test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../galib/population.h"
#include "../galib/entity.h"
#include "../galib/world.h"


	static char *target_text="When we reflect on this struggle, we may console ourselves with the full belief, that the war of nature is not incessant, that no fear is felt, that death is generally prompt, and that the vigorous, the healthy, and the happy survive and multiply.";

class myWorld : public world
{
public:



	//class myEntity : entity
	//{
	//public:
	//	int _type;//0 : 무조건 배신 ,1 믿고 보복 ,2 무조건 믿고

	//}
	
	myWorld()
	{
		_chromosomeLen	= strlen(target_text);
		_entitySize = 100;
	}

	entity * createEntity()
	{
		entity * pEntity = new entity;



		char * chromosome = new char [_chromosomeLen];

		for(int i =0 ; i < _chromosomeLen;i++)
		{
			int v = rand();
			v = v % ('~'-' ') +' ';
			
			chromosome[i] = (char)v;
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

			if( b == 0)
			{
				(*daughter)->_chromosome[i] = mather->_chromosome[i];
				(*son)->_chromosome[i] = father->_chromosome[i];
			}else
			{
				(*daughter)->_chromosome[i] = father->_chromosome[i];
				(*son)->_chromosome[i] = mather->_chromosome[i];

			}

		}

		return true;
	}

	bool mutation(entity *entity)
	{
		int index = rand()%_chromosomeLen;

		if((rand()%2)==0)
		{
			entity->_chromosome[index]++;
		}else
		{
			entity->_chromosome[index]--;
		}

		

		return true;
	}

	void rank(entity ** entityList , entity * entity)
	{

	}

	void struggle(entity ** entityList , entity * entity) 
	{
		entity->_fitness = 0.0;
		
		char* chromosome = (char*)entity->_chromosome;

		for(int i = 0 ; i < _chromosomeLen;i++)
		{
			entity->_fitness  += abs(target_text[i] - chromosome[i]);
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
};



int _tmain(int argc, _TCHAR* argv[])
{

	population * pop = new population;
	world * pWorld = new myWorld();
	pop->Init(pWorld);

	for(int i = 0;i< 5000;i++)
	{
		pop->evaluate();
	}

	printf("%s",target_text);

	
	char * str = new char[pWorld->_chromosomeLen+1];
	memcpy(str,pop->_entityList[0]->_chromosome,pWorld->_chromosomeLen);

	str[pWorld->_chromosomeLen] = 0;

	printf("%s",str);

	return 0;
}

