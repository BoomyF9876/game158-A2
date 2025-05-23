#include "Body.h"  // Use "" when its a local file in this project
#include <MMath.h> // Use <> for system libraries or we have pointed to them
#include <QMath.h>
Body::Body()
	: 
	pos{}
	, vel{}
	, accel{}
	, mass{1.0f}
	, mesh{nullptr}
	, texture{nullptr}
	, radius{ 1.0f }

{
	// TODO for YOU
	// Set up the rotational inertia matrix 
	// using Umer's assignment doc scribbles
}

Body::~Body() {}

void Body::Update(float deltaTime) {
	/// From 1st semester physics class
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
	vel += accel * deltaTime;
}

void Body::ApplyForce(Vec3 force) {
	accel = force / mass;
}

void Body::ApplyTorque(Vec3 torque)
{
	// Watch out, Umer is lying a little here...
	// angularAcc = torque / rotationalInertia;
	// You can't divide by a matrix, 
	// but you can multiply with its inverse
	// TODO for YOU
	// angularAcc = ....
}

void Body::UpdateOrientation(float deltaTime)
{
	// Update the orientation using angular velocity
	// Need angle and axis
	float angularSpeed = VMath::mag(angularVel);
	// If we are not spinning, get outta here
	if (angularSpeed < VERY_SMALL) {
		return;
	}

	float angleRadians = angularSpeed * deltaTime;
	float angleDegrees = angleRadians * RADIANS_TO_DEGREES;
	Vec3 axisOfRotation = VMath::normalize(angularVel);
	Quaternion rotation = QMath::angleAxisRotation(angleDegrees, axisOfRotation);

	// Combine quaternions by multiplying
	orientation *= rotation;
}

void Body::UpdateAngularVel(float deltaTime)
{
	// TODO for YOU
	// Code up Umer's scribbles (also in circularMotion slides)
	// final angular_vel = initial angular_vel + angular_acc * deltaTime
}

// Note to self
// Our model matrix just does rotations at this point...
// and translations
// and scale!
Matrix4 Body::getModelMatrix() const
{
	Matrix4 rotation    = MMath::toMatrix4(orientation);
	Matrix4 translation = MMath::translate(pos);
	Matrix4 scaling = MMath::scale(radius, radius, radius);
	// Scale first, rotate second, then translate. Order matters!
	// Read from right to left
	return  translation * rotation * scaling;
}

bool Body::OnCreate() {
	return true;
}

void Body::OnDestroy() {
}

void Body::Render() const {
}

