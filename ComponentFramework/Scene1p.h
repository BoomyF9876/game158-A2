#ifndef SCENE1P_H
#define SCENE1P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene1p : public Scene {
private:
	Body*   planeBody;
	Mesh*   planeMesh;
	Body*   sphereBody;
	Mesh*   sphereMesh;
	Shader* shader;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;
	Vec3 planeNormal;

public:
	explicit Scene1p();
	virtual ~Scene1p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};
#endif // SCENE1P_H