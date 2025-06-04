#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene3p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include <QMath.h>
#include "Collision.h"

Scene3p::Scene3p()
	: shader{ nullptr }
	, drawInWireMode{ true }
	, jellyfishHead{ nullptr }
	, sphereMesh{ nullptr }
	, trackball{ nullptr }
	, numSpheresPerAnchor{ 10 }
	, spacing{ 1.0f }
{
	Debug::Info("Created Scene3p: ", __FILE__, __LINE__);
}

Scene3p::~Scene3p() {
	Debug::Info("Deleted Scene3p: ", __FILE__, __LINE__);
}

bool Scene3p::OnCreate() {
	trackball = new Trackball();

	Debug::Info("Loading assets Scene3p: ", __FILE__, __LINE__);

	jellyfishHead = new Body();
	jellyfishHead->OnCreate();
	jellyfishHead->pos = Vec3(-1.5, 4, -25);
	jellyfishHead->radius = 6;

	// Make a line of 10 anchors
	const int numAnchors = 10;
	Vec3 anchorPos(-6.0f, 0.0f, -25);

	for (int j = 0; j < numAnchors; j++) {
		anchors.push_back(new Body());
		anchors[j]->pos = anchorPos;
		anchors[j]->radius = 0.5f;

		for (int i = 0; i < numSpheresPerAnchor; i++) {
			tentacleSpheres.push_back(new Body());
			anchorPos -= Vec3(0, spacing, 0);

			tentacleSpheres[j * numAnchors + i]->pos = anchorPos;
			tentacleSpheres[j * numAnchors + i]->radius = 0.2f;
		}

		anchorPos += Vec3(spacing, 0, 0);
		anchorPos.y = 0;
	}

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
	cameraPos = Vec3(0, 0, 5);
	cameraOrientation = QMath::angleAxisRotation(0, Vec3(0, 1, 0));
	Matrix4 T = MMath::translate(cameraPos);
	Matrix4 R = MMath::toMatrix4(cameraOrientation);
	// Based on Umer's cat pictures
	// the view is inverse(R) * inverse(T)
	viewMatrix = MMath::inverse(R) * MMath::inverse(T);
	return true;
}

void Scene3p::OnDestroy() {
	Debug::Info("Deleting assets Scene3p: ", __FILE__, __LINE__);

	jellyfishHead->OnDestroy();
	delete jellyfishHead;

	// One way to loop through a vector list of items
	for (int i = 0; i < anchors.size(); i++) {
		anchors[i]->OnDestroy();
		delete anchors[i];
	}

	// Another way to loop through a vector list
	for (Body* tentacleSphere : tentacleSpheres) {
		tentacleSphere->OnDestroy();
		delete tentacleSphere;
	}

	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;

	delete trackball;

}

void Scene3p::HandleEvents(const SDL_Event& sdlEvent) {
	// Use the trackball by hooking it with HandleEvents
	initialTrackballOrientation = trackball->getQuat();
	trackball->HandleEvents(sdlEvent);
	finalTrackballOrientation = trackball->getQuat();

	const float deltaAngleDegrees = 2.0f;
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_L: // Umer did L for lines
			drawInWireMode = !drawInWireMode;
			break;
			// TODO for YOU
			// Move Jellyfish head and anchors using WASD keys 
		case SDL_SCANCODE_A:
		{

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

void Scene3p::Update(const float deltaTime) {
	Vec3 gravityForce;
	float c = 2.5f;

	for (int i = 0; i < tentacleSpheres.size(); i++) {
		gravityForce.y = tentacleSpheres[i]->mass * -9.8f;

		// Start off with laminar flow, so drag force = -cv
		Vec3 dragForce = -c * tentacleSpheres[i]->vel;
		//if (VMath::mag(tentacleSpheres[sphereIndex]->vel) > 1.0f) {
		//	// Switch to turbulent flow if the spheres are moving fast
		//	// That means drag force = -cv^2
		//	dragForce = -c * tentacleSpheres[sphereIndex]->vel *
		//		VMath::mag(tentacleSpheres[sphereIndex]->vel);
		//}
		tentacleSpheres[i]->ApplyForce(gravityForce + dragForce);
		// calculate a first approximation of velocity based on acceleration
		tentacleSpheres[i]->UpdateVel(deltaTime);
		// TODO – Rod constraint physics!

		// update position using corrected velocities based on rod constraint
		tentacleSpheres[i]->UpdatePos(deltaTime);
	}

	// Camera moves to orbit the jellyfish. Anthony calls this a 3rd person camera
	// Based on Umer's scribbles on the board, we need to
	// 1 Translate the camera such that the ball would be at the origin
	cameraPos = cameraPos - jellyfishHead->pos;

	// 2 Rotate using the track ball
	// We need to know how much has the trackball changed?
	Quaternion changeInTrackballOrientation = finalTrackballOrientation * QMath::inverse(initialTrackballOrientation);

	cameraOrientation *= changeInTrackballOrientation;
	cameraPos = QMath::rotate(cameraPos, changeInTrackballOrientation);

	// 3 Translate the camera back
	cameraPos = cameraPos + jellyfishHead->pos;

	// Build the view matrix
	Matrix4 T = MMath::translate(cameraPos);
	Matrix4 R = MMath::toMatrix4(cameraOrientation);
	// Based on Umer's cat pictures
	// the view is inverse(R) * inverse(T)
	viewMatrix = MMath::inverse(R) * MMath::inverse(T);

}

void Scene3p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	Vec4 green(0, 1, 0, 0);
	Vec4 white(1, 1, 1, 0);
	Vec4 red(1, 0, 0, 0);
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);

	// Render the jellyfish head ball
	// Let's use Spongebobs jellyfish pink color
	// 240, 168, 227
	Vec4 pink(240.0f / 255.0f, 168 / 255.0f, 227 / 255.0f, 0);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, jellyfishHead->getModelMatrix());
	glUniform4fv(shader->GetUniformID("color"), 1, pink);
	sphereMesh->Render(GL_TRIANGLES);

	// Render the anchors
	for (Body* anchor : anchors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, anchor->getModelMatrix());
		sphereMesh->Render(GL_TRIANGLES);
	}

	// Render the tentacleSpheres
	for (Body* tentacleSphere : tentacleSpheres) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, tentacleSphere->getModelMatrix());
		sphereMesh->Render(GL_TRIANGLES);
	}

	glUseProgram(0);
}




