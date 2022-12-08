#pragma once
#ifndef Building_H
#define Building_H

#include "Player.h"

class Building {
public:
	Building(double x0, double y0, double width, double height, double depth,
		const std::shared_ptr<Shape> cube, const std::shared_ptr<Shape> sphere);
	~Building();

	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;

	bool collide(Ray ray);

	std::shared_ptr<Polygon> boundingBox;

private:
	std::shared_ptr<Shape> cube;
	std::shared_ptr<Texture> texture;

	double x0, y0, width, height, depth;
};

#endif