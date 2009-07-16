#pragma once

class population;

class entity;

class world
{
public:
	world(void);
	~world(void);
	virtual entity * createEntity() = 0;
	virtual bool crossover(entity ** mather,entity ** father,entity **daughter, entity **son) = 0;
	virtual  bool mutation(entity **entity) = 0;
	virtual void struggle(entity ** entityList , entity * entity) = 0;
	virtual bool survival(entity ** m,entity ** f ) = 0;
	virtual void select(entity ** mather,entity ** father) = 0;
};
