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

	if (!fixed) {
		double stiffness = 3000.0;
		springs.push_back(createSpring(p00, p01, stiffness));
		springs.push_back(createSpring(p00, p10, stiffness));
		springs.push_back(createSpring(p00, p11, stiffness));
		springs.push_back(createSpring(p01, p10, stiffness));
		springs.push_back(createSpring(p01, p11, stiffness));
		springs.push_back(createSpring(p10, p11, stiffness));
	}
}

// let (x0, y0) be bottom left corner of polygon
Polygon::Polygon(double x0, double y0, double width, double height, bool fixed) :
	x0(x0), y0(y0), width(width), height(height), x_original(x0), y_original(y0)
{
	createRectangle(x0, y0, width, height, nullptr, fixed);
	M.resize(12, 12);
	K.resize(12, 12);
	v.resize(12);
	f.resize(12);

}

Polygon::Polygon(double x0, double y0, double width, double height, const shared_ptr<Shape> shape, bool fixed) : 
	x0(x0), y0(y0), width(width), height(height), sphere(shape)
{
	createRectangle(x0, y0, width, height, shape, fixed);
	M.resize(12, 12);
	K.resize(12, 12);
	v.resize(12);
	f.resize(12);
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

void Polygon::step(double h, Eigen::Vector3d grav, Eigen::Vector3d movement) {

	M.setZero();
	K.setZero();
	v.setZero();
	f.setZero();

	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->fixed) continue;
		auto p = vertices[i];

		Matrix3d Mi;
		Mi.setIdentity();
		Mi *= p->m;
		M.block<3, 3>(p->i, p->i) = Mi;
		v.segment<3>(p->i) = p->v + movement;
		f.segment<3>(p->i) = p->m * grav;
	}
	
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
			f.segment<3>(spring->p0->i) += fs;
			K.block<3, 3>(spring->p0->i, spring->p0->i) -= Ks;
		}
		if (!spring->p1->fixed) {
			f.segment<3>(spring->p1->i) -= fs;
			K.block<3, 3>(spring->p1->i, spring->p1->i) -= Ks;
		}
		// only add diagonal springs if both particles aren't fixed
		if (!spring->p0->fixed && !spring->p1->fixed) {
			K.block<3, 3>(spring->p0->i, spring->p1->i) += Ks;
			K.block<3, 3>(spring->p1->i, spring->p0->i) += Ks;
		}
	}
	
	// Collision detection and response
	double collisionConstant = 100.0;
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->fixed) continue;
		auto p = vertices[i];
		/*for (int j = 0; j < spheres.size(); j++) {
			auto s = spheres[j];

			Vector3d dx = s->x - p->x;
			double l = dx.norm();
			double d = s->r + p->r - l;

			if (d > 0) {
				Vector3d n = dx / l;
				f.segment<3>(p->i) -= collisionConstant * d * n;

				Matrix3d I3d;
				I3d.setIdentity();
				K.block<3, 3>(p->i, p->i) += collisionConstant * d * I3d;
			}
		}*/
		
	}
	
	MatrixXd A = M - h * h * K;
	MatrixXd b = M * v + h * f;
	//cout << "A:\n" << A << "\n\nb:\n" << b << endl;
	VectorXd solution = A.ldlt().solve(b); // solution contains v(k+1)
	for (int i = 0; i < vertices.size(); i++) {
		auto p = vertices[i];
		if (p->fixed) continue;
		p->v = solution.segment<3>(p->i);
		p->x = p->x + h * p->v;
	}
	for (auto p : vertices) {
		double physicsFloor = 0.0;
		if (p->x(1) <= physicsFloor) {
			p->x(1) = physicsFloor;
			p->v(1) = -p->v(1);
		}
	}
	x0 = vertices[0]->x(0);
	y0 = vertices[1]->x(1);
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
