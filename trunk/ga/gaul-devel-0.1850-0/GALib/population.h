#pragma once

class ievaluate;

class population
{
public:
	population(void);
	~population(void);

	int		_max_size;		/* Current maximum population size. */
	int _size;
	enity * _entityList[128];

	ievaluate * _evaluate;

	void Init(int size,ievaluate * evaluate)
	{
		
	}

	void AddEntity(enity * entity)
	{
		_entityList[_size] = entity;
		_size++;
	}

};
