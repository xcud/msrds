// galib_test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../galib/population.h"
#include "../galib/entity.h"
#include "../galib/world.h"


	static char *target_text="When we reflect on this struggle, we may console ourselves with the full belief, that the war of nature is not incessant, that no fear is felt, that death is generally prompt, and that the vigorous, the healthy, and the happy survive and multiply.";

class myWorld : world
{
public:



	//class myEntity : entity
	//{
	//public:
	//	int _type;//0 : 무조건 배신 ,1 믿고 보복 ,2 무조건 믿고

	//}
	
	int _chromosomeLen;

	class myWorld()
	{
		_chromosomeLen	= strlen(target_text);
	}

	entity * createEntity()
	{
		entity * pEntity = new entity;



		char * chromosome = new char [_chromosomeLen];

		for(int i =0 ; i < _chromosomeLen;i++)
		{
			chromosome[i] = (char)rand();
		}

		pEntity->_chromosome = chromosome;

		
		return pEntity;
	}

	int evaluate(population * pop)
	{

	}

	bool crossover(entity ** mather,entity ** father)
	{
		return true;
	}

	bool mutation(entity **entity)
	{
		

		return true;
	}

	void struggle(population * pop , entity * entity)
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
};



int _tmain(int argc, _TCHAR* argv[])
{

	population * pop = new population;

	pop->Init(1000,new myWorld);

	while(true)
	{
		pop->evaluate();
	}

	return 0;
}

