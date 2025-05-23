#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene1p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include <QMath.h>

Scene1p::Scene1p() 
	: planeBody{nullptr}
	, shader{nullptr}
	, planeMesh{nullptr}
	, drawInWireMode{true}
	, sphereBody{nullptr}
	, sphereMesh{nullptr}
{
	Debug::Info("Created Scene1p: ", __FILE__, __LINE__);
}

Scene1p::~Scene1p() {
	Debug::Info("Deleted Scene1p: ", __FILE__, __LINE__);
}

bool Scene1p::OnCreate() {

	Debug::Info("Loading assets Scene1p: ", __FILE__, __LINE__);
	planeBody = new Body();
	planeBody->OnCreate();
	planeBody->orientation  = QMath::angleAxisRotation(90, Vec3(1, 0, 0));
	planeNormal = Vec3(0, 0, -1);
	// Make sure you rotate the normal, whenever you rotate the plane
	// I like Christian's idea here to rotate by the planeBody->orientation
	// Just make sure the initial plane normal is along -ve z
	planeNormal = QMath::rotate(planeNormal, planeBody->orientation);
	planeNormal.print("plane normal vector");

	sphereBody = new Body();
	sphereBody->OnCreate();
	sphereBody->pos = Vec3(0, 1.0, 0);
	sphereBody->angularVel = Vec3(0, 0, 0);
	sphereBody->radius = 1;
	// Umer testing angular acc
	// This should make the ball spin faster and faster!
	sphereBody->angularAcc = Vec3(0, 0, 1);


	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();

	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

	return true;
}

void Scene1p::OnDestroy() {
	Debug::Info("Deleting assets Scene1p: ", __FILE__, __LINE__);
	planeBody->OnDestroy();
	delete planeBody;
	planeMesh->OnDestroy();
	delete planeMesh;

	sphereBody->OnDestroy();
	delete sphereBody;
	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;
}

void Scene1p::HandleEvents(const SDL_Event &sdlEvent) {
	const float deltaAngleDegrees = 2.0f;
	switch( sdlEvent.type ) {
    case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_L: // Umer did L for lines
				drawInWireMode = !drawInWireMode;
				break;
			case SDL_SCANCODE_A:
			{
				// Rotate the plane and it's normal vector
				Vec3 axis = Vec3(0, 0, 1);
				Quaternion rotation = QMath::angleAxisRotation(deltaAngleDegrees, axis);
				// Combine rotations by multiplying
				planeBody->orientation = rotation * planeBody->orientation;
				// Don't forget to rotate the normal too
				planeNormal = QMath::rotate(planeNormal, rotation);
				planeNormal.print("plane normal vector");
			}
				break;

			case SDL_SCANCODE_D:
				// TODO for YOU to rotate using all WASD keys
				break;


		}
		break;

	case SDL_MOUSEMOTION:          
		break;

	case SDL_MOUSEBUTTONDOWN:              
		break; 

	case SDL_MOUSEBUTTONUP:            
	break;

	default:
		break;
    }
}

void Scene1p::Update(const float deltaTime) {
	
	// TODO for YOU
	// Calculate torqueMag using forceMag * distance to pivot
	// The force is the weight of the sphere
	// The distance to the pivot relies on the angle 
	// between the weight and the normal

	// Kemel says the z rule can be used here
	// This is the SAME angle between the up vector and the normal

	// Sam says, lets use the dot product! 

	const Vec3 up(0, 1, 0);
	// plane normal DOT up = cos(angle)
	float angle = 0; // acos(plane normal DOT up)

	// Using scribble on the board
	// we know the perpendicular distance between 
	// pivot and force
	float dist = sphereBody->radius * sin(angle);

	float torqueMag = 0; // weight * dist

	// We have the magnitude of the torque
	// Now we need the axis of rotation
	// Following Umers scribbles
	// torqueDir = up CROSS normal
	Vec3 torqueDir;
	
	Vec3 torque = torqueMag * torqueDir;

	sphereBody->ApplyTorque(torque);

	// See if your angular acceleration works
	sphereBody->UpdateAngularVel(deltaTime);
	
	// CHange quaternion based on angular vel
	sphereBody->UpdateOrientation(deltaTime);
	// TODO for YOU:
	// Use Umer's scribbles
	// velocity mag = angular_vel mag * radius
	float velocityMag;

	// velocity direction = angular_vel dir CROSS planeNormal
	Vec3  velocityDir;

	// set the sphereBody's velocity to be velocityMag * velocityDir


}

void Scene1p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(drawInWireMode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, planeBody->getModelMatrix());
	planeMesh->Render(GL_TRIANGLES);

	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereBody->getModelMatrix());
	sphereMesh->Render(GL_TRIANGLES);

	glUseProgram(0);
}



	
