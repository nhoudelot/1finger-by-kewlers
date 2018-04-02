#ifndef CAMERA_H
#define CAMERA_H

#include "Rubber.h"
using glm::vec3;

namespace RubberSpace
{

class Camera
{
public:
	Camera();
	void getRocketValues(double row);
	void setMatrices(float aspectRatio = 0.0f);
	void setDefaults();
	
	vec3 getPos() const { return m_pos; }
	
	void setPos(vec3 pos) { m_pos = pos; }
	void setTgt(vec3 tgt) { m_tgt = tgt; }
	void setYvec(vec3 yvec) { m_yvec = yvec; }
	void setNearFar(float near, float far) { m_near = near; m_far = far; }
	void setFov(float fov) { m_fov = fov; }
	void setRoll(float roll) { m_roll = roll; }
		
private:
	vec3 m_pos;
	vec3 m_tgt;
	vec3 m_yvec;
	float m_near;
	float m_far;
	float m_fov;
	float m_roll;
};


}

#endif
