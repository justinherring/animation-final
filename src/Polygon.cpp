#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Particle.h"
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Polygon.h"

using namespace std;

// let (x0, y0) be bottom left corner of polygon
Polygon::Polygon(double x0, double y0, double width, double height) :
	x0(x0), y0(y0), width(width), height(height) {}

Polygon::Polygon(double x0, double y0, double width, double height, const shared_ptr<Shape> shape) : 
	x0(x0), y0(y0), width(width), height(height), sphere(shape) {}

Polygon::~Polygon() {}

void Polygon::tare() {
	for (auto v : vertices) {
		v->tare();
	}
}

void Polygon::reset() {
	for (auto v : vertices) {
		v->reset();
	}
}

void Polygon::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog) const
{
	if (sphere) {
		for (auto v : vertices) {
			MV->pushMatrix();
			MV->translate(v->x(0), v->x(1), v->x(2));
			MV->scale(v->r);
			glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
			sphere->draw(prog);
			MV->popMatrix();
		}
	}
}

void Polygon::step(double h, Eigen::Vector3d grav, Eigen::Vector3d movement) {
	x0 += movement(0);
	y0 += movement(1);
	for (auto p : vertices) {
		p->v = movement;
		p->x += p->v;
	}
}

bool Polygon::collide(shared_ptr<Particle> v) {
	return (x0 + width) >= v->x(0)
		&& v->x(0) >= x0
		&& (y0 + height) >= v->x(1)
		&& v->x(1) >= y0;
}

bool Polygon::collide(shared_ptr<Polygon> p) {
	return x0 <= (p->x0 + p->width)
		&& p->x0 <= (x0 + width)
		&& y0 <= (p->y0 + p->height)
		&& p->y0 <= (y0 + height);
}
