#pragma once

#include "NxPhysics.h"
//#include "DebugRenderer.h"
#include "DrawObjects.h"
#include "Timing.h"
#include "Actors.h"
#include "Joints.h"
//#include "Utilities.h"
#include "Timing.h"
//#include "ErrorStream.h"
//#include "SamplePulleyJoint.h"
// Limit globals



extern NxScene*          gScene;

class Creature
{
public:
	Creature(void);
	~Creature(void);


  NxActor* _capsuleList[5] ;


	NxVec3 _StartPos;

  void Init2()
  {
	
		int z = 15;	
		NxD6Joint* d6Joint;

	  for(int i = 0;i< 5;i++)
	  {
			_capsuleList[ i]  = CreateBox(NxVec3(0,z,0),NxVec3(0.5,1,0.5),50);

			//_capsuleList[ i]  = CreateCapsule(NxVec3(0,z,0), 1, 0.5, 10);
			

			if( i != 0)
			{
				d6Joint = CreateD6Joint(_capsuleList[ i-1], _capsuleList[ i], NxVec3(0,z+2,0), NxVec3(0,-1,0));
			}
			z -= 2;
	  }
	}

    void Init()
	  {
		
			int z = 2;	
			NxD6Joint* d6Joint;

			_capsuleList[ 0]  = CreateBox(NxVec3(0,z,0),NxVec3(1,0.5,1),10);

			_capsuleList[ 1]  = CreateBox(NxVec3(1,z-0.5,1),NxVec3(0.25,0.5,0.25),50);
			CreateD6Joint(_capsuleList[ 0], _capsuleList[ 1], NxVec3(1,z,1), NxVec3(0,-1,0));

			_capsuleList[ 2]  = CreateBox(NxVec3(1,z-0.5,-1),NxVec3(0.25,0.5,0.25),50);
			CreateD6Joint(_capsuleList[ 0], _capsuleList[ 2], NxVec3(1,z,-1), NxVec3(0,-1,0));

			_capsuleList[ 3]  = CreateBox(NxVec3(-1,z-0.5,1),NxVec3(0.25,0.5,0.25),50);
			CreateD6Joint(_capsuleList[ 0], _capsuleList[ 3], NxVec3(-1,z,1), NxVec3(0,-1,0));

			_capsuleList[ 4]  = CreateBox(NxVec3(-1,z-0.5,-1),NxVec3(0.25,0.5,0.25),50);
			CreateD6Joint(_capsuleList[ 0], _capsuleList[ 4], NxVec3(-1,z,-1), NxVec3(0,-1,0));

		}

	void Update()
	{

		
		NxReal  movement = _capsuleList[ 0]->getGlobalPosition().distance(_StartPos);

		
		
			

		int f = 500;

		for(int i =0;i< 5;i++)
		{
			int x = rand() % f;
			int y = rand() % 5;
			int z = rand() % f;


			NxVec3 forceDir = NxVec3(x-f/2,0,z-f/2);
			//_capsuleList[ i]->addForce(forceDir);
			//_capsuleList[ i]->addLocalForce(forceDir);
			//_capsuleList[ i]->addLocalTorque(forceDir);
			_capsuleList[ i]->addTorque(forceDir);
		}


	}
  

	void GetPos(int index)
	{
		NxMat33 orient;
		NxVec3 xaxis, yaxis, zaxis;
		

		orient = _capsuleList[ index]->getGlobalOrientation();
		orient.getRow(0, xaxis);
		orient.getRow(1, yaxis);
		orient.getRow(2, zaxis);


		xaxis.normalize();
		yaxis.normalize();
		zaxis.normalize();


		NxReal xAngle = xaxis.dot(NxVec3(1,0,0));
		NxReal yAngle = yaxis.dot(NxVec3(0,1,0));
		NxReal zAngle = zaxis.dot(NxVec3(0,0,1));

	}

	NxD6Joint* CreateD6Joint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);

};
