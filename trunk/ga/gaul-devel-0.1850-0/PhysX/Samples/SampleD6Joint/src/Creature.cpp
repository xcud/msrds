#include "Creature.h"


NxReal gLinearLimit = 1.0f;
NxReal gSwing1Limit = NxPiF32 / 180.0f * 30.0f;
NxReal gSwing2Limit = NxPiF32 / 180.0f * 30.0f;
NxReal gTwistLowLimit = NxPiF32 / 180.0f * -90.0f;
NxReal gTwistHighLimit = NxPiF32 / 180.0f * 45.0f;



Creature::Creature(void)
{
}

Creature::~Creature(void)
{
}

NxD6Joint* Creature::CreateD6Joint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;
	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);

	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LIMITED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LIMITED;

	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.linearLimit.value = gLinearLimit;
	d6Desc.swing1Limit.value = gSwing1Limit;
	d6Desc.swing2Limit.value = gSwing2Limit;
	d6Desc.twistLimit.low.value =  gTwistLowLimit;
	d6Desc.twistLimit.high.value = gTwistHighLimit;

//	d6Desc.projectionMode = NX_JPM_NONE;
	d6Desc.projectionMode = NX_JPM_POINT_MINDIST;

//	d6Desc.jointFlags |= NX_JF_COLLISION_ENABLED;

	NxJoint* d6Joint = gScene->createJoint(d6Desc);

	return (NxD6Joint*)d6Joint->is(NX_JOINT_D6);
}