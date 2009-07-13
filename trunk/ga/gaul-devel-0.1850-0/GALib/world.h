#pragma once

class population;

class entity;

class world
{
public:
	world(void);
	~world(void);
	virtual entity * createEntity(){return 0;};
	virtual bool crossover(entity ** mather,entity ** father){return true;};
	virtual bool mutation(entity **entity){return true;};
	virtual void struggle(population * pop , entity * entity){};
	virtual bool survival(entity ** m,entity ** f ){return true;};
};
