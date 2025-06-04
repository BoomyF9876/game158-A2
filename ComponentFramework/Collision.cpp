#include "Collision.h"
#include <cmath>
using namespace std;

bool COLLISION::SphereSphereCollisionDetected(const Body* body1, const Body* body2)
{
	Vec3 bodyVector = body2->pos - body1->pos;
	float distance = VMath::mag(bodyVector);
	Vec3 bodyVectorNormalized = bodyVector / distance;

	float radius = body1->radius + body2->radius;
	float dotProduct = VMath::dot(body1->vel - body2->vel, bodyVectorNormalized);
	
	if (distance <= radius && dotProduct > 0) {
		return true;
	}
	return false;
}

void COLLISION::SphereSphereCollisionResponse(Body* body1, Body* body2)
{
	Vec3 snookerTableNormal = Vec3(0, 1, 0);
	Vec3 bodyVector = body2->pos - body1->pos;
	Vec3 bodyVectorNormalized = bodyVector / VMath::mag(bodyVector);

	float absVel = VMath::mag(body1->vel);
	
	Vec3 velAlongNormal = (body1->vel.x * bodyVectorNormalized.x + body1->vel.y * bodyVectorNormalized.y + body1->vel.z * bodyVectorNormalized.z) * bodyVectorNormalized;

	Vec3 newVelAlongNormal1 = velAlongNormal * (body1->mass - body2->mass) / (body1->mass + body2->mass);
	Vec3 newVelAlongNormal2 = velAlongNormal * body1->mass * 2 / (body1->mass + body2->mass);

	Vec3 newVelPerpenNormal = body1->vel - velAlongNormal;

	body1->vel = newVelPerpenNormal + newVelAlongNormal1;
	body2->vel = newVelAlongNormal2;

	body1->angularVel = VMath::normalize(VMath::cross(snookerTableNormal, body1->vel)) * VMath::mag(body1->vel) / body1->radius;
	body2->angularVel = VMath::normalize(VMath::cross(snookerTableNormal, body2->vel)) * VMath::mag(body2->vel) / body2->radius;

	Vec3 velDir1 = VMath::normalize(VMath::cross(body1->angularVel, snookerTableNormal));
	Vec3 velDir2 = VMath::normalize(VMath::cross(body2->angularVel, snookerTableNormal));
	
	body1->vel = VMath::mag(body1->vel) * velDir1;
	body2->vel = VMath::mag(body2->vel) * velDir2;
}