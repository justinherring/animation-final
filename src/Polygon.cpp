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
#include "Ray.h"

using namespace std;
using namespace Eigen;

void Polygon::createRectangle(double x0, double y0, double width, double height, const std::shared_ptr<Shape> shape, bool fixed)
{
	shared_ptr<Particle> p00, p01, p10, p11;
	if (shape) {
		p00 = make_shared<Particle>(shape);
		p01 = make_shared<Particle>(shape);
		p10 = make_shared<Particle>(shape);
		p11 = make_shared<Particle>(shape);
	}
	else {
		p00 = make_shared<Particle>();
		p01 = make_shared<Particle>();
		p10 = make_shared<Particle>();
		p11 = make_shared<Particle>();
	}
	Eigen::Vector3d x, v;
	x.setZero();
	v.setZero();
	double mass = 1.0;

	x(0) = x0 - 0.5 * width;
	x(1) = y0;
	p00->i = 0;
	p00->m = 0.25 * mass;
	p00->x = x;
	p00->v = v;
	p00->r = 0.02;
	p00->fixed = fixed;
	vertices.push_back(p00);

	x(0) = x0 + 0.5 * width;
	p01->i = 3;
	p01->m = 0.25 * mass;
	p01->x = x;
	p01->v = v;
	p01->r = 0.02;
	p01->fixed = fixed;
	vertices.push_back(p01);

	x(0) = x0 - 0.5 * width;
	x(1) = y0 + height;
	p10->i = 6;
	p10->m = 0.25 * mass;
	p10->x = x;
	p10->v = v;
	p10->r = 0.02;
	p10->fixed = fixed;
	vertices.push_back(p10);

	x(0) = x0 + 0.5 * width;
	p11->i = 9;
	p11->m = 0.25 * mass;
	p11->x = x;
	p11->v = v;
	p11->r = 0.02;
	p11->fixed = fixed;
	vertices.push_back(p11);

	tare();
}

// let (x0, y0) be bottom left corner of polygon
Polygon::Polygon(double x0, double y0, double width, double height, bool fixed) :
	x0(x0), y0(y0), width(width), height(height), x_original(x0), y_original(y0)
{
	createRectangle(x0, y0, width, height, nullptr, fixed);

}

Polygon::Polygon(double x0, double y0, double width, double height, const shared_ptr<Shape> shape, bool fixed) : 
	x0(x0), y0(y0), width(width), height(height), sphere(shape), x_original(x0), y_original(y0)
{
	createRectangle(x0, y0, width, height, shape, fixed);
}

Polygon::~Polygon() {}

void Polygon::tare() {
	for (auto v : vertices) {
		v->tare();
	}
	x_original = x0;
	y_original = y0;
}

void Polygon::reset() {
	for (auto v : vertices) {
		v->reset();
	}
	x0 = x_original;
	y0 = y_original;
}

void Polygon::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog) const
{
	if (sphere) {
		for (auto v : vertices) {
			MV->pushMatrix();
			MV->translate(v->x(0), v->x(1), v->x(2));
			MV->scale(v->r);
			glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(0.8, 0.8, 0.8).data());
			glUniform3fv(prog->getUniform("kdBack"), 1, Vector3f(0.8, 0.8, 0.8).data());
			glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
			sphere->draw(prog);
			MV->popMatrix();
		}
	}
}

void Polygon::step(Eigen::Vector3d position) {

	auto p00 = vertices[0];
	p00->x = position;
	p00->x(0) -= width / 2.0;
	p00->x(1) -= height / 2.0;

	auto p01 = vertices[1];
	p01->x = position;
	p01->x(0) += width / 2.0;
	p01->x(1) -= height / 2.0;

	auto p10 = vertices[2];
	p10->x = position;
	p10->x(0) -= width / 2.0;
	p10->x(1) += height / 2.0;

	auto p11 = vertices[3];
	p11->x = position;
	p11->x(0) += width / 2.0;
	p11->x(1) += height / 2.0;

	x0 = p00->x(0);
	y0 = p00->x(1);
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

// code for ray-AABB intersection adapted from https://tavianator.com/2011/ray_box.html
bool Polygon::collide(Ray r, double& tmin, double& tmax) {
	tmin = 0, tmax = INFINITY;

	auto p00 = vertices[0];
	auto p11 = vertices[3];

	if (r.d(0) != 0.0) {
		double tx1 = (p00->x(0) - r.x(0)) / r.d(0);
		double tx2 = (p11->x(0) - r.x(0)) / r.d(0);

		tmin = max(tmin, min(tx1, tx2));
		tmax = min(tmax, max(tx1, tx2));
	}

	if (r.d(1) != 0.0) {
		double ty1 = (p00->x(1) - r.x(1)) / r.d(1);
		double ty2 = (p11->x(1) - r.x(1)) / r.d(1);

		tmin = max(tmin, min(ty1, ty2));
		tmax = min(tmax, max(ty1, ty2));
	}

	return tmax >= tmin;
}