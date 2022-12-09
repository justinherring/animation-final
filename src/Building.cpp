#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Building.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

using namespace std;
using namespace Eigen;

Building::Building(double x0, double y0, double width, double height, double depth,
	const shared_ptr<Shape> cube, const shared_ptr<Shape> sphere,
	bool isForeground) :
	x0(x0), y0(y0), width(width), height(height), depth(depth), cube(cube), _isForeground(isForeground)
{
	boundingBox = make_shared<Polygon>(x0, y0, width, height, sphere);
}

Building::~Building() {

}

void Building::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const
{
	MV->pushMatrix();
	if (!_isForeground) {
		MV->translate(boundingBox->x0, boundingBox->y0 + boundingBox->height / 2.0f, -0.5f - depth / 2.0f);
		MV->scale(boundingBox->width, boundingBox->height, depth);
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniform3fv(prog->getUniform("kdFront"), 1, _color.data());
		glUniform3fv(prog->getUniform("kdBack"), 1, _color.data());
	}
	else {
		MV->translate(boundingBox->x0, boundingBox->y0 + boundingBox->height / 2.0f, 0.0);
		MV->scale(boundingBox->width, boundingBox->height, depth);
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniform3fv(prog->getUniform("kdFront"), 1, _color.data());
		glUniform3fv(prog->getUniform("kdBack"), 1, _color.data());
	}
	cube->draw(prog);
	MV->popMatrix();
}

bool Building::collide(Ray ray) {
	return false;
}