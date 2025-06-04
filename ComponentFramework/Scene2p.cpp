#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene2p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include <QMath.h>
#include "Collision.h"

Scene2p::Scene2p() 
	: planeBody{nullptr}
	, shader{nullptr}
	, planeMesh{nullptr}
	, drawInWireMode{true}
	, cueBall{nullptr}
	, targetBall{nullptr}
	, sphereMesh{nullptr}
	, trackball{nullptr}
{
	Debug::Info("Created Scene2p: ", __FILE__, __LINE__);
}

Scene2p::~Scene2p() {
	Debug::Info("Deleted Scene2p: ", __FILE__, __LINE__);
}

bool Scene2p::OnCreate() {

	// How big is the trackball?
	std::cout << "A trackball is: " << sizeof(trackball) << " bytes big\n";
	std::cout << "A Vec3 is: " << sizeof(Vec3) << " bytes big\n";
	std::cout << "A Body is: " << sizeof(Body) << " bytes big\n";

	trackball = new Trackball();

	Debug::Info("Loading assets Scene2p: ", __FILE__, __LINE__);
	planeBody = new Body();
	planeBody->OnCreate();
	planeBody->orientation  = QMath::angleAxisRotation(90, Vec3(1, 0, 0));
	// Umer is making the plane bigger. It's weird I have a radius for a plane, I  know...
	planeBody->radius = 2.0f;
	planeNormal = Vec3(0, 0, -1);
	// Make sure you rotate the normal, whenever you rotate the plane
	// I like Christian's idea here to rotate by the planeBody->orientation
	// Just make sure the initial plane normal is along -ve z
	planeNormal = QMath::rotate(planeNormal, planeBody->orientation);
	planeNormal.print("plane normal vector");

	cueBall = new Body();
	cueBall->OnCreate();
	cueBall->pos = Vec3(0, 1.0, 0);
	cueBall->angularVel = Vec3(0, 0, 0);
	cueBall->radius = 1;
	// Umer testing angular acc
	// This should make the ball spin faster and faster!
	cueBall->angularAcc = Vec3(0, 0, 0);

	targetBall = new Body();
	targetBall->OnCreate();
	targetBall->pos = Vec3(-0.5, 1.0, -4);

	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();

	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/passColorFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	//viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	// With Scott's permission, let's build the view using the camera
	// pos and orientation
	cameraPos = cueBall->pos + Vec3(0, 0, 5);
	cameraOrientation = QMath::angleAxisRotation(0, Vec3(1, 0, 0));
	Matrix4 T = MMath::translate(cameraPos);
	Matrix4 R = MMath::toMatrix4(cameraOrientation);
	// Based on Umer's cat pictures
	// the view is inverse(R) * inverse(T)
	viewMatrix = MMath::inverse(R) * MMath::inverse(T);




	return true;
}

void Scene2p::OnDestroy() {
	Debug::Info("Deleting assets Scene2p: ", __FILE__, __LINE__);
	planeBody->OnDestroy();
	delete planeBody;
	planeMesh->OnDestroy();
	delete planeMesh;

	cueBall->OnDestroy();
	delete cueBall;

	targetBall->OnDestroy();
	delete targetBall;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;

	delete trackball;

}

void Scene2p::HandleEvents(const SDL_Event &sdlEvent) {
	// Use the trackball by hooking it with HandleEvents
	initialTrackballOrientation = trackball->getQuat();
	trackball->HandleEvents(sdlEvent);
	finalTrackballOrientation = trackball->getQuat();
	
	const float deltaAngleDegrees = 2.0f;
	switch( sdlEvent.type ) {
    case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_L: // Umer did L for lines
				drawInWireMode = !drawInWireMode;
				break;
			case SDL_SCANCODE_A:
				// Rotate the plane and it's normal vector
			{
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

			case SDL_SCANCODE_SPACE:
				// Whack the ball based on where the orbit camera is looking
				// Scott says this is always -z dammit
				//                          x, y,  z, w  (w = zero for things you don't want to translate)
			{
				Vec4 changeInVelCameraSpace(0, 0, -1, 0);
				Vec3 snookerTableNormal = Vec3(0, 1, 0);
				Matrix4 worldToCameraSpace = viewMatrix;
				Matrix4 cameraToWorldSpace = MMath::inverse(worldToCameraSpace);
				Vec4 changeInVelWorldSpace = cameraToWorldSpace * changeInVelCameraSpace;

				cueBall->vel += changeInVelWorldSpace;
				//cueBall->angularAcc += changeInVelWorldSpace;
				cueBall->angularVel += VMath::normalize(VMath::cross(snookerTableNormal, cueBall->vel)) * VMath::mag(cueBall->vel) / cueBall->radius;

				Vec3 velDir = VMath::normalize(VMath::cross(cueBall->angularVel, snookerTableNormal));
				cueBall->vel = VMath::mag(cueBall->vel) * velDir;
			}
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

void Scene2p::Update(const float deltaTime) {
	if (COLLISION::SphereSphereCollisionDetected(cueBall, targetBall)) {
		COLLISION::SphereSphereCollisionResponse(cueBall, targetBall);
	}

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
	float dist = cueBall->radius * sin(angle);

	float torqueMag = 0; // weight * dist

	// We have the magnitude of the torque
	// Now we need the axis of rotation
	// Following Umers scribbles
	// torqueDir = up CROSS normal
	Vec3 torqueDir;
	
	Vec3 torque = torqueMag * torqueDir;

	cueBall->ApplyTorque(torque);

	// See if your angular acceleration works
	cueBall->UpdateAngularVel(deltaTime);
	targetBall->UpdateAngularVel(deltaTime);
	
	// Change quaternion based on angular vel
	cueBall->UpdateOrientation(deltaTime);
	targetBall->UpdateOrientation(deltaTime);

	// Umer testing the space key to whack the ball
	cueBall->Update(deltaTime);
	targetBall->Update(deltaTime);

	// TODO for YOU:
	// Use Umer's scribbles
	// velocity mag = angular_vel mag * radius
	float velocityMag;

	// velocity direction = angular_vel dir CROSS planeNormal
	Vec3 velocityDir;

	// set the sphereBody's velocity to be velocityMag * velocityDir

	// Camera moves to orbit the cueBall. Anthony calls this a 3rd person camera
	// Based on Umer's scribbles on the board, we need to
	// 1 Translate the camera such that the ball would be at the origin
	cameraPos = cameraPos - cueBall->pos;

	// 2 Rotate using the track ball
	// We need to know how much has the trackball changed?
	Quaternion changeInTrackballOrientation = finalTrackballOrientation * QMath::inverse(initialTrackballOrientation);

	cameraOrientation *= changeInTrackballOrientation;
	cameraPos = QMath::rotate(cameraPos, changeInTrackballOrientation);

	// 3 Translate the camera back
	cameraPos = cameraPos + cueBall->pos;

	// Build the view matrix
	Matrix4 T = MMath::translate(cameraPos);
	Matrix4 R = MMath::toMatrix4(cameraOrientation);
	// Based on Umer's cat pictures
	// the view is inverse(R) * inverse(T)
	viewMatrix = MMath::inverse(R) * MMath::inverse(T);

}

void Scene2p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(drawInWireMode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	Vec4 green(0, 1, 0, 0);
	Vec4 white(1, 1, 1, 0);
	Vec4 red(1, 0, 0, 0);
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, planeBody->getModelMatrix());
	glUniform4fv(shader->GetUniformID("color"), 1, green);
	planeMesh->Render(GL_TRIANGLES);

	// Render the target ball
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, targetBall->getModelMatrix());
	glUniform4fv(shader->GetUniformID("color"), 1, red);
	sphereMesh->Render(GL_TRIANGLES);

	// Render the cue ball
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, cueBall->getModelMatrix());
	glUniform4fv(shader->GetUniformID("color"), 1, white);
	sphereMesh->Render(GL_TRIANGLES);



	glUseProgram(0);
}



	
