#pragma once


#include <stdio.h>
#include "../../../../galib/world.h"

class MyWorld :public world
{
public:
	MyWorld(void);
	~MyWorld(void);



	virtual entity * createEntity() 
	{
		return NULL;
	}
	virtual bool crossover(entity * mather,entity * father,entity **daughter, entity **son) 
	{
		return true;
	}

	virtual  bool mutation(entity *entity) 
	{
		return true;
	}
	virtual void struggle(entity ** entityList , entity * entity) 
	{

	}
	virtual bool survival(entity ** m,entity ** f ) 
	{
		return true;
	}
	virtual void select(entity ** entityList,entity ** mather,entity ** father) 
	{

	}
	virtual void update(entity * entityList) 
	{

	}



};
