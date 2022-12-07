#pragma  once
#ifndef Camera_H
#define Camera_H

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <Eigen/Dense>

class MatrixStack;

class Camera
{
public:
	enum {
		ROTATE = 0,
		TRANSLATE,
		SCALE
	};
	
	Camera();
	virtual ~Camera();
	void setInitDistance(float z) { translations.z = -std::abs(z); }
	void setAspect(float a) { aspect = a; };
	void setFovy(float f) { fovy = f; };
	void setZnear(float z) { znear = z; };
	void setZfar(float z) { zfar = z; };
	void setRotationFactor(float f) { rfactor = f; };
	void setTranslationFactor(float f) { tfactor = f; };
	void setScaleFactor(float f) { sfactor = f; };
	void mouseClicked(float x, float y, bool shift, bool ctrl, bool alt);
	void mouseMoved(float x, float y);
	void applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const;
	void applyViewMatrix(std::shared_ptr<MatrixStack> MV) const;

	void setTranslation(glm::vec3 t) { translations = t; }
	void setTranslation(Eigen::Vector3d t) {
		translations.x = t(0);
		translations.y = t(1);
		translations.z = t(2);
	}
	void setTranslation(float x, float y, float z) {
		translations.x = x;
		translations.y = y;
		translations.z = z;
	}

	float x() { return translations.x; }
	float y() { return translations.y; }
	float z() { return translations.z; }
	glm::vec3 offset() { 
		glm::vec3 t;
		t.x = 0.0f;
		t.y = -0.2f;
		t.z = -2.0f;
		return t;
	}

	void followPlayerTranslation(Eigen::Vector3d playerPosition);

private:
	float aspect;
	float fovy;
	float znear;
	float zfar;
	glm::vec2 rotations;
	glm::vec3 translations;
	glm::vec2 mousePrev;
	int state;
	float rfactor;
	float tfactor;
	float sfactor;
};

#endif
