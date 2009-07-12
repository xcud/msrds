// galib_test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../galib/population.h"
#include "../gaalib/entity.h"




class myWorld : world
{
	
	entity * createEntity()
	{

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

		for(int i = 0 ; i < pop->_size;i++)
		{
			if( pop->_entityList[i] != entity)
			{
				pop->_entityList[i]->
				entity->_fitness 
			}
		}


	}

	bool survival(entity ** m,entity ** f )
	{
		
		return true;
	}
}



int _tmain(int argc, _TCHAR* argv[])
{

	population * pop = new population;

	pop->init(1000,new myWorld);

	while(true)
	{
		pop->evaluate();
	}

	return 0;
}

