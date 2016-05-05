#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

namespace scene {

	enum LIGHT_TYPE {PUNTUAL, DIRECTIONAL, SPOT};

	class Light {
	private:
		glm::vec3 m_position;
		glm::vec3 m_direction;
		float m_aperture;
		LIGHT_TYPE m_type;
		glm::vec3 m_La;
		glm::vec3 m_Ld;
		glm::vec3 m_Ls;
		void setDefaultColor();
	public:
		Light();
		Light(const glm::vec3& direction, const glm::vec3& position, const float& aperture, const LIGHT_TYPE& type);
		Light(const glm::vec3& direction, const glm::vec3& position, const LIGHT_TYPE& type);
		Light(const glm::vec3& vector, const LIGHT_TYPE& type);
		Light(const glm::vec3& position);
		//Setter and getters
		glm::vec3 getPosition() const;
		glm::vec3 getDirection() const;
		float getAperture() const;
		LIGHT_TYPE getType() const;
		glm::vec3 getLa() const;
		glm::vec3 getLs() const;
		glm::vec3 getLd() const;
		void setPosition(const glm::vec3& position);
		void setDirection(const glm::vec3& direction);
		void setAperture(const float& aperture);
		void setType(const LIGHT_TYPE& type);
		void setLa(const glm::vec3& La);
		void setLd(const glm::vec3& Ld);
		void setLs(const glm::vec3& Ls);
	};
}

#endif