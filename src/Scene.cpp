#include <iostream>

#include <cstdlib>

#include "Scene.h"
#include "Particle.h"
#include "Shape.h"
#include "Program.h"
#include "Player.h"
#include "Building.h"

using namespace std;
using namespace Eigen;

Scene::Scene() :
	t(0.0),
	h(1e-2),
	grav(0.0, 0.0, 0.0),
	drawBoundingBoxes(true)
{
}

Scene::~Scene()
{
}

double randomDouble(double low, double high) {
	int randInt = rand() % (int) 1e3;
	return randInt / 1e3 * (high - low) + low;
}

void Scene::load(const string &RESOURCE_DIR)
{
	// Units: meters, kilograms, seconds
	h = 5e-3;
	
	grav << 0.0, -9.8, 0.0;

	sphere = make_shared<Shape>();
	cube = make_shared<Shape>();

	sphere->loadMesh(RESOURCE_DIR + "sphere2.obj");
	cube->loadMesh(RESOURCE_DIR + "cube.obj");

	player = make_shared<Player>(0, 3, 0.1, 0.2, cube, sphere);
	int maxBuildings = 100;
	srand(0);
	for (int i = 0; i < maxBuildings; i++) {
		double spacing = randomDouble(0.5, 2.5);
		double bWidth = randomDouble(0.75, 1.25);
		double bHeight = randomDouble(4.0, 8.0);
		double bDepth = randomDouble(1.5, 3.5);
		shared_ptr<Building> b = make_shared<Building>(spacing + i * 2 - 50, 0, bWidth, bHeight, bDepth, cube, sphere);
		double bR = randomDouble(0.2, 0.4);
		double bG = randomDouble(0.2, 0.4);
		double bB = randomDouble(0.2, 0.4);
		b->setColor(Vector3f(bR, bG, bB));
		backgroundBuildings.push_back(b);
	}
	for (int i = 0; i < maxBuildings; i++) {
		double spacing = randomDouble(0.5, 2.5);
		double bWidth = randomDouble(0.75, 1.25);
		double bHeight = randomDouble(0.5, 1.5);
		shared_ptr<Building> b = make_shared<Building>(spacing + i * 2 - 50, 0, bWidth, bHeight, 0.5, cube, sphere, true);
		double bR = randomDouble(0.5, 0.7);
		double bG = randomDouble(0.5, 0.7);
		double bB = randomDouble(0.5, 0.7);
		b->setColor(Vector3f(bR, bG, bB));
		foregroundBuildings.push_back(b);
	}
	
}

void Scene::init()
{
	sphere->init();
	cube->init();
}

void Scene::tare()
{
	for(int i = 0; i < (int)spheres.size(); ++i) {
		spheres[i]->tare();
	}
}

void Scene::reset()
{
	t = 0.0;
	player->reset();
}

void Scene::step(bool keys[256])
{
	t += h;
	player->step(h, grav, keys, foregroundBuildings);
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog) const
{
	// glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(1.0, 1.0, 1.0).data());
	
	player->draw(MV, prog);
	if (drawBoundingBoxes) {
		player->drawBoundingBox(MV, prog);
	}
	for (auto b : backgroundBuildings) {
		b->draw(MV, prog);
		if (drawBoundingBoxes)
			b->boundingBox->draw(MV, prog);
	}
	for (auto b : foregroundBuildings) {
		b->draw(MV, prog);
		if (drawBoundingBoxes)
			b->boundingBox->draw(MV, prog);
	}
}

void Scene::drawLines(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog) const {
	player->drawWebs();
}

std::shared_ptr<Player> Scene::getPlayer() { return player; }

void Scene::shootWeb(Vector2d d) {
	player->removeWeb();

	Vector3d currentPosition = player->position();
	currentPosition(0) += player->getWidth() * 0.5;
	currentPosition(1) += player->getHeight() * 0.5;
	Vector3d direction;
	direction << d(0), d(1), 0.0;
	direction.normalize();

	Ray potentialWeb(currentPosition, direction);
	double tmin, tmax;
	double actual_t = 1e18;
	bool hit = false;
	bool everHit = false;
	for (int i = 0; i < backgroundBuildings.size(); i++) {
		auto b = backgroundBuildings[i];
		hit = b->boundingBox->collide(potentialWeb, tmin, tmax);
		if (hit) {
			everHit = true;
			actual_t = min((tmin > 0) ? tmin : 1e18, actual_t);
		}
	}
	Vector3d target = currentPosition + actual_t * direction;
	if (0 < actual_t && actual_t < 30 && target(1) > 0 && everHit) {
		player->shootWeb(target);
	}
}

void Scene::removeWeb() {
	player->removeWeb();
}