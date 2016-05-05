#include "Light.h"
#include "../glut/Globals.h"

#include <glm/gtx/norm.hpp>

namespace scene {

	Light::Light() {
		setDirection(glm::vec3(0.0f, -1.0, 0.0f));
		setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		setAperture(TAU);
		setType(DIRECTIONAL);
		setDefaultColor();
	}

	Light::Light(const glm::vec3& direction, const glm::vec3& position, const float& aperture, const LIGHT_TYPE& type) {
		setDirection(direction);
		setPosition(position);
		setAperture(aperture);
		setType(type);
		setDefaultColor();
	}

	Light::Light(const glm::vec3& direction, const glm::vec3& position, const LIGHT_TYPE& type) {
		setDirection(direction);
		setPosition(position);
		setType(type);
		setAperture(PI);
		setDefaultColor();
	}

	Light::Light(const glm::vec3& vector, const LIGHT_TYPE& type) {
		switch (type) {
		case PUNTUAL:
			m_position = vector;
			m_direction = glm::vec3(0.0, -1.0f, 0.0f);
			m_aperture = PI;
			break;
		case DIRECTIONAL:
			m_position = glm::vec3(0.0f, 0.0f, 0.0f);
			setDirection(vector);
			setAperture(TAU);
			break;
		case SPOT:
			m_position = vector;
			m_direction = glm::vec3(0.0f, 0.0f, 0.0f);
			setAperture(PI);
			break;
		default:
			break;
		}
		setType(type);
		setDefaultColor();
	}

	Light::Light(const glm::vec3& position) {
		m_position = position;
		setDirection(glm::vec3(0.0f, -1.0, 0.0f));
		setAperture(TAU);
		setType(DIRECTIONAL);
		setDefaultColor();
	}

	
	glm::vec3 Light::getPosition() const {
		return m_position;
	}

	glm::vec3 Light::getDirection() const {
		return m_direction;
	}

	float Light::getAperture() const {
		return m_aperture;
	}

	LIGHT_TYPE Light::getType() const {
		return m_type;
	}

	void Light::setPosition(const glm::vec3& position) {
		m_position = position;
	}

	void Light::setDirection(const glm::vec3& direction) {	
		if (glm::length2(direction) == 0.0f) {
			m_direction = glm::vec3(0.0f);
		} else {
			m_direction = glm::normalize(direction);
		}
	}

	void Light::setAperture(const float& aperture) {
		if (aperture > TAU) {
			m_aperture = TAU;
		} else if (aperture < 0.0f) {
			m_aperture = 0.0f;
		} else {
			m_aperture = aperture;
		}
	}

	void Light::setType(const LIGHT_TYPE& type) {
		m_type = type;
	}

	void Light::setLa(const glm::vec3& La) {
		if (glm::all(glm::greaterThanEqual(La, glm::vec3(0.0f)))) {
			m_La = La;
		}
	}

	void Light::setLd(const glm::vec3& Ld) {
		if (glm::all(glm::greaterThanEqual(Ld, glm::vec3(0.0f)))) {
			m_Ld = Ld;
		}
	}

	void Light::setLs(const glm::vec3& Ls) {
		if (glm::all(glm::greaterThanEqual(Ls, glm::vec3(0.0f)))) {
			m_Ls = Ls;
		}
	}

	glm::vec3 Light::getLa() const {
		return m_La;
	}

	glm::vec3 Light::getLs() const {
		return m_Ls;
	}

	glm::vec3 Light::getLd() const {
		return m_Ld;
	}

	void Light::setDefaultColor() {
		setLa(glm::vec3(0.3f));
		setLd(glm::vec3(0.5f));
		setLs(glm::vec3(0.9f));
	}

}
