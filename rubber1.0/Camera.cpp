#include "Camera.h"
#include "BassRocket.h"

using namespace RubberSpace;
using glm::vec3;

Camera::Camera()
	: m_pos(vec3(0.0f, 0.0f, -1.0f))
	, m_tgt(vec3(0.0f, 0.0f, 0.0f))
	, m_yvec(0.0f, 1.0f, 0.0f)
	, m_near(0.1f)
	, m_far(1000.0f)
	, m_fov(60.0f)
	, m_roll(0.0f)
{
}

void Camera::getRocketValues(double row)
{
	float angle = SYNC("Camera:angle", row);
	float dist = SYNC("Camera:dist", row);
	
	m_pos = vec3(sinf(angle * PI) * dist, 0.0f, cosf(angle * PI) * dist);
	m_pos += vec3(SYNC("Camera:offset.x", row), SYNC("Camera:offset.y", row), SYNC("Camera:offset.z", row));
	m_tgt = vec3(SYNC("Camera:tgt.x", row), SYNC("Camera:tgt.y", row), SYNC("Camera:tgt.z", row));
	m_fov = SYNC("Camera:fov", row);
	m_roll = SYNC("Camera:roll", row);
}

void Camera::setMatrices(float aspectRatio)
{
	float aspect;
	
	if (aspectRatio == 0.0f)
	{
		//aspect = Rubber::getInstance()->getAspect();
		aspect = ASPECTRATIO;
	}
	else
	{
		aspect = aspectRatio;
	}
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_fov, aspect, m_near, m_far);
	glRotatef(m_roll, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_pos.x, m_pos.y, m_pos.z, m_tgt.x, m_tgt.y, m_tgt.z, m_yvec.x, m_yvec.y, m_yvec.z);
}

void Camera::setDefaults()
{
	m_pos = vec3(0.0f, 0.0f, -1.0f);
	m_tgt = vec3(0.0f, 0.0f, 0.0f);
	m_yvec = vec3(0.0f, 1.0f, 0.0f);
	m_fov = 60.0f;
	m_roll = 0.0f;
	m_near = 0.1f;
	m_far = 1000.0f;
}

