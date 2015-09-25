#ifndef PARTICLE_H_
#define PARTICLE_H_
#include <glm/glm.hpp>
namespace physics {
	class Particle 	{
	public:
		Particle();
		Particle(glm::vec3& p, glm::vec3& v, glm::vec3& f, float& m);
		Particle(glm::vec3& p, glm::vec3& v, float& m);
		Particle(glm::vec3& p, float& m);
		Particle(float& m);
		Particle(glm::vec3& p, glm::vec3& v, glm::vec3& f);
		Particle(glm::vec3& p, glm::vec3& v);
		Particle(glm::vec3& p);
		~Particle ();

		void add_force(glm::vec3 f);
		void add_velocity(glm::vec3 v);
		void add_position(glm::vec3 p);
		void set_position(glm::vec3 p);
		void set_velocity(glm::vec3 v);
		void set_force(glm::vec3 f);
		void set_mass(float& m);
		float get_mass();
		glm::vec3 get_position();
		glm::vec3 get_velocity();
		glm::vec3 get_force();
		void fix();
		void release();
		bool fixed();
	private:
		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_force;
		float m_mass;
		bool m_fixed;
	};
}

#endif