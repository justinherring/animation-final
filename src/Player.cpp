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

using namespace std;
using namespace Eigen;

Player::Player(double x0, double y0, double width, double height,
	const std::shared_ptr<Shape> cube, const std::shared_ptr<Shape> sphere) :
	x0(x0), y0(y0), width(width), height(height), cube(cube)
{
	boundingBox = make_shared<Polygon>(x0, y0, width, height, sphere, true);
}

Player::~Player() {

}

void Player::move(Vector2d direction) {

}

Ray Player::shootWeb(Vector2d position) {
	Vector3d direction;
	direction(0) = position(0);
	direction(1) = position(1);
	direction(2) = 0.0;
	this->web = Ray(Vector3d(), direction);
	return this->web;
}

void Player::removeWeb() {
	this->web = Ray();
}

void Player::step(double h, Vector3d grav, bool keys[256]) {
	Vector3d speed, dx;
	dx << 0.01, 0.0, 0.0;
	speed = (-keys['a'] + keys['d']) * dx;

	boundingBox->step(h, grav, speed);
}

void Player::drawWebs() const {
	if (web.d == Vector3d()) return;

	glColor3f(0.8f, 0.8f, 0.8f);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glVertex3f(boundingBox->x0, boundingBox->y0 + height / 2.0, 0.0f);
	glVertex3f(boundingBox->x0 + this->web.d(0), boundingBox->y0 + height / 2.0 + this->web.d(1), 0.0f);
	glEnd();
}

void Player::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const
{
	MV->pushMatrix();
	MV->translate(boundingBox->x0, boundingBox->y0 + boundingBox->height / 2.0f, 0.0f);
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
}