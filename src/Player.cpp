#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Player.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Building.h"

using namespace std;
using namespace Eigen;

Player::Player(double x0, double y0, double width, double height,
	const std::shared_ptr<Shape> cube, const std::shared_ptr<Shape> sphere) :
	x0(x0), y0(y0), width(width), height(height), cube(cube), x_original(x0), y_original(y0)
{
	boundingBox = make_shared<Polygon>(x0, y0, width, height, sphere, false);
	M.resize(3, 3);
	K.resize(3, 3);
	v.resize(3);
	f.resize(3);

	center = make_shared<Particle>(sphere);
	center->x(0) = x0 + width * 0.5;
	center->x(1) = y0 + height * 0.5;
	center->x(2) = 0.0;
	center->m = 1;
	center->r = width * 0.5;
	center->fixed = false;
	center->tare();
}

Player::~Player() {}

void Player::createWeb(shared_ptr<Particle> target) {
	double stiffness = 1000.0;
	springs.push_back(boundingBox->createSpring(center, target, stiffness));
}

void Player::removeBBWeb(shared_ptr<Particle> oldTarget) {
	springs.clear();
	webTarget = nullptr;
}

void Player::shootWeb(Vector3d d) {
	if (webTarget) {
		removeBBWeb(webTarget);
	}
	webTarget = make_shared<Particle>();
	webTarget->x = d;
	createWeb(webTarget);
}

void Player::removeWeb() {
	this->web = Ray();
	removeBBWeb(webTarget);
}

void Player::reset() {
	if (webTarget) {
		removeBBWeb(webTarget);
		webTarget = nullptr;
	}
	center->reset();
	boundingBox->reset();
}

void Player::step(double h, Vector3d grav, bool keys[256], vector<shared_ptr<Building> > fgBuildings) {
	Vector3d speed, dx;
	dx << 0.02, 0.0, 0.0;
	speed = (-keys['a'] + keys['d']) * dx;

	M.setZero();
	K.setZero();
	v.setZero();
	f.setZero();

	M.setIdentity();
	M *= center->m;
	
	v = center->v + speed;
	f = center->m * grav;

	for (int i = 0; i < springs.size(); i++) {
		auto spring = springs[i];

		Vector3d dx = spring->p1->x - spring->p0->x;
		double l = dx.norm();
		double lfrac = (l - spring->L) / l;
		Vector3d fs = spring->E * lfrac * dx;
		Matrix3d I3d;
		I3d.setIdentity();
		Matrix3d Ks = ((1 - lfrac) * dx * dx.transpose() + lfrac * dx.dot(dx) * I3d);
		Ks *= spring->E / (l * l);
		if (!spring->p0->fixed) {
			f += fs;
			K -= Ks;
		}
		/*if (!spring->p1->fixed) {
			f.segment<3>(spring->p1->i) -= fs;
			K.block<3, 3>(spring->p1->i, spring->p1->i) -= Ks;
		}
		// only add diagonal springs if both particles aren't fixed
		if (!spring->p0->fixed && !spring->p1->fixed) {
			K.block<3, 3>(spring->p0->i, spring->p1->i) += Ks;
			K.block<3, 3>(spring->p1->i, spring->p0->i) += Ks;
		}*/
	}

	MatrixXd A = M - h * h * K;
	MatrixXd b = M * v + h * f;
	//cout << "A:\n" << A << "\n\nb:\n" << b << endl;
	VectorXd solution = A.ldlt().solve(b); // solution contains v(k+1)
	center->v = solution;
	center->x = center->x + h * center->v;

	// Collision detection and response
	double collisionConstant = 2;
	for (int j = 0; j < fgBuildings.size(); j++) {
		auto b = fgBuildings[j];
		if (b->boundingBox->collide(boundingBox)) {
			auto p00 = boundingBox->vertices[0];
			auto p11 = boundingBox->vertices[3];
			auto bp00 = b->boundingBox->vertices[0];
			auto bp11 = b->boundingBox->vertices[3];
			// colliding from top
			if (p00->x(1) <= bp11->x(1) && p11->x(1) >= bp11->x(1)) {
				double dy = bp11->x(1) - p00->x(1);
				center->x(1) += collisionConstant * dy;
				center->v(1) = 0;
				continue;
			}
			// colliding from left
			if (p11->x(0) >= bp00->x(0) && p00->x(0) <= bp00->x(0)) {
				double dx = p11->x(0) - bp00->x(0);
				center->x(0) -= collisionConstant * dx;
				center->v(0) = 0;
			}
			// colliding from right
			if (p00->x(0) <= bp11->x(0) && p11->x(0) >= bp11->x(0)) {
				double dx = bp11->x(0) - p00->x(0);
				center->x(0) += collisionConstant * dx;
				center->v(0) = 0;
			}
			
		}
	}

	double physicsFloor = height / 2.0;
	double damping = 0.5;
	if (center->x(1) <= physicsFloor) {
		center->x(1) = physicsFloor;
		center->v(1) = -center->v(1) * damping;
	}

	boundingBox->step(center->x);

}

void Player::drawWebs() const {
	if (!webTarget) return;

	glColor3f(0.8f, 0.8f, 0.8f);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glVertex3f(center->x(0), center->x(1), 0.0f);
	glVertex3f(this->webTarget->x(0), this->webTarget->x(1), -0.5f);
	glEnd();
}

void Player::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const
{
	MV->pushMatrix();
	MV->translate(boundingBox->x0 + width / 2.0, boundingBox->y0 + boundingBox->height / 2.0f, 0.0f);
	MV->scale(boundingBox->width, boundingBox->height, 0.1f);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(1.0, 0.0, 0.0).data());
	glUniform3fv(prog->getUniform("kdBack"), 1, Vector3f(1.0, 0.0, 0.0).data());
	cube->draw(prog);
	MV->popMatrix();
}

Vector3d Player::position() const {
	Vector3d pos;
	pos << boundingBox->x0, boundingBox->y0, 0;
	return pos;
}

void Player::drawBoundingBox(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog) const {
	boundingBox->draw(MV, prog);
	center->draw(MV, prog);
}