#include <iostream>

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

void Scene::load(const string &RESOURCE_DIR)
{
	// Units: meters, kilograms, seconds
	h = 5e-3;
	
	grav << 0.0, -9.8, 0.0;

	sphere = make_shared<Shape>();
	cube = make_shared<Shape>();

	sphere->loadMesh(RESOURCE_DIR + "sphere2.obj");
	cube->loadMesh(RESOURCE_DIR + "cube.obj");

	player = make_shared<Player>(0, 1, 0.1, 0.2, cube, sphere);
	int maxBuildings = 10;
	int spacing = 1;
	for (int i = 0; i < maxBuildings; i++) {
		shared_ptr<Building> b = make_shared<Building>((1 + spacing) * i - maxBuildings / (1 + spacing), 0, 1, 5, 3, cube, sphere);
		backgroundBuildings.push_back(b);
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
	for(int i = 0; i < (int)spheres.size(); ++i) {
		spheres[i]->reset();
	}
}

void Scene::step(bool keys[256])
{
	t += h;
	player->step(h, grav, keys);
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
}

void Scene::drawLines(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog) const {
	player->drawWebs();
}

std::shared_ptr<Player> Scene::getPlayer() { return player; }
