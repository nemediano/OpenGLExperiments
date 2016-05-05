#include "Material.h"

namespace scene {
	Material::Material() {
		setKa(glm::vec3(0.25f, 0.25f, 0.25f));
		setKd(glm::vec3(0.5f, 0.5f, 0.5f));
		setKs(glm::vec3(1.0f, 1.0f, 1.0f));
		setShininess(8.0f);
	}

	Material::Material(const glm::vec3& Ka, const glm::vec3& Kd, const glm::vec3& Ks, const float& shininess) {
		setKa(Ka);
		setKd(Kd);
		setKs(Ks);
		setShininess(shininess);
	}

	Material::Material(const glm::vec3& Ka, const glm::vec3& Kd, const glm::vec3& Ks) {
		setKa(Ka);
		setKd(Kd);
		setKs(Ks);
		setShininess(1.0f);
	}

	Material::Material(const glm::vec3& Ka, const glm::vec3& Kd) {
		setKa(Ka);
		setKd(Kd);
		setKs(glm::vec3(1.0f, 1.0f, 1.0f));
		setShininess(1.0f);
	}

	Material::Material(const glm::vec3& ambientAndDiffuse, const glm::vec3& Ks, const float& shininess) {
		setKa(ambientAndDiffuse);
		setKd(ambientAndDiffuse);
		setKs(Ks);
		setShininess(shininess);
	}

	Material::Material(const glm::vec3& ambientAndDiffuse, const float& shininess) {
		setKa(ambientAndDiffuse);
		setKd(ambientAndDiffuse);
		setKs(glm::vec3(1.0f, 1.0f, 1.0f));
		setShininess(shininess);
	}

	Material::Material(const glm::vec3& ambientAndDiffuse) {
		setKa(ambientAndDiffuse);
		setKd(ambientAndDiffuse);
		setKs(glm::vec3(1.0f, 1.0f, 1.0f));
		setShininess(1.0f);
	}
	//Getter and setters
	glm::vec3 Material::getKa() const {
		return m_Ka;
	}

	glm::vec3 Material::getKd() const {
		return m_Kd;
	}

	glm::vec3 Material::getKs() const {
		return m_Ks;
	}

	float Material::getShininnes() const {
		return m_shininess;
	}

	void Material::setKa(const glm::vec3& Ka) {
		if (glm::all(glm::greaterThanEqual(Ka, glm::vec3(0.0f)))) {
			m_Ka = Ka;
		} else {
			m_Ka = glm::vec3(0.0f);
		}
	}

	void Material::setKd(const glm::vec3& Kd) {
		if (glm::all(glm::greaterThanEqual(Kd, glm::vec3(0.0f)))) {
			m_Kd = Kd;
		} else {
			m_Kd = glm::vec3(0.0f);
		}
	}

	void Material::setKs(const glm::vec3& Ks) {
		if (glm::all(glm::greaterThanEqual(Ks, glm::vec3(0.0f)))) {
			m_Ks = Ks;
		}
		else {
			m_Ks = glm::vec3(0.0f);
		}
	}

	void Material::setShininess(const float& shininess) {
		if (shininess >= 0.0 && shininess <= 256.0) {
			m_shininess = shininess;
		} else {
			m_shininess = 1.0f;
		}
	}
}
