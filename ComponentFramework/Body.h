#ifndef BODY_H
#define BODY_H
#include <Vector.h> /// This is in GameDev
#include <Quaternion.h>
#include <Matrix.h>
using namespace MATH; 

/// Just forward declair these classes so I can define a pointer to them
/// Used later in the course
class Mesh;
class Texture;

class Body {
public:
    Body();
    ~Body();
public: // Not today Scott!!
public: /// Physics stuff
	Vec3 pos;
	Vec3 vel;
	Vec3 accel;
	float mass;
	// Minas is right! We need a quaternion to rotate things
	Quaternion orientation;
	Vec3 angularVel;
	Vec3 angularAcc;
	// Pretend all Body objects are spheres
	float radius;
	Matrix3 rotationalInertia;


private: /// Graphics stuff 
	Mesh *mesh;
	Texture *texture;
	
public:
	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;
	void ApplyForce(Vec3 force);
	void ApplyTorque(Vec3 torque);
	void setAccel(const Vec3 &accel_) { accel = accel_;}
	void UpdateOrientation(float deltaTime);
	void UpdateAngularVel(float deltaTime);

	// The following const is a promise
	// This method will not change the member variable
	Matrix4 getModelMatrix() const;
};

#endif
