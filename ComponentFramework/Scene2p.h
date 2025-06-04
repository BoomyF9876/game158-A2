#ifndef SCENE2P_H
#define SCENE2P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <Quaternion.h>
#include "Trackball.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene2p : public Scene {
private:
	Body*      planeBody;
	Mesh*      planeMesh;
	Body*      cueBall;
	Body*      targetBall;
	Mesh*      sphereMesh;
	Shader*    shader;
	Matrix4    projectionMatrix;
	Matrix4    viewMatrix;
	bool	   drawInWireMode;
	Vec3 	   planeNormal;
	Vec3	   cameraPos;
	Quaternion cameraOrientation;
	Trackball* trackball;
	// Umer says you need to divide to find the change in orientation quaternion...
	Quaternion initialTrackballOrientation;
	Quaternion finalTrackballOrientation;

public:
	explicit Scene2p();
	virtual ~Scene2p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};
#endif // SCENE2P_H