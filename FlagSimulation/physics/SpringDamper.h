#ifndef SPRINGDAMPER_H_
#define SPRINGDAMPER_H_
#include "Particle.h"
#include <utility>
namespace physics {
	class SpringDamper {
	public:
		SpringDamper();
		SpringDamper(Particle* a, Particle* b);
		SpringDamper(Particle* a, Particle* b, float k_s, float k_d);
		void add_force();
		void set_spring_constant(float k_s);
		void set_damper_constant(float k_d);
		float get_spring_constant();
		float get_damper_constant();
		float get_initial_lenght();
		void make_connection(Particle* a, Particle* b);
		void set_initial_lenght(float L);
		std::pair<glm::vec3, glm::vec3> get_ends_positions();
		~SpringDamper();
	private:
		float m_ks;
		float m_kd;
		float m_L;
		Particle* m_a;
		Particle* m_b;
	};

	
}

#endif