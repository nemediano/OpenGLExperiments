#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

namespace scene {
	class Material {
		private:
			//Ambient material color
			glm::vec3 m_Ka;
			//Diffuse material color
			glm::vec3 m_Kd;
			//Specular material color
			glm::vec3 m_Ks;
			//Shininess material coefficient
			float m_shininess;
		public:
			Material();
			Material(const glm::vec3& Ka, const glm::vec3& Kd, const glm::vec3& Ks, const float& shininess);
			Material(const glm::vec3& Ka, const glm::vec3& Kd, const glm::vec3& Ks);
			Material(const glm::vec3& Ka, const glm::vec3& Kd);
			Material(const glm::vec3& ambientAndDiffuse, const glm::vec3& Ks, const float& shininess);
			Material(const glm::vec3& ambientAndDiffuse, const float& shininess);
			Material(const glm::vec3& ambientAndDiffuse);
			//Getter and setters
			glm::vec3 getKa() const;
			glm::vec3 getKd() const;
			glm::vec3 getKs() const;
			float getShininnes() const;
			void setKa(const glm::vec3& Ka);
			void setKd(const glm::vec3& Kd);
			void setKs(const glm::vec3& Ks);
			void setShininess(const float& shininess);

	};
}
#endif