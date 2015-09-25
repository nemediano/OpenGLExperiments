#include "SpringDamper.h"
#include <iostream>
namespace physics {
	SpringDamper::SpringDamper() : m_kd(0.0f), m_ks(0.0f), m_L(0.0f), m_a(nullptr), m_b(nullptr) {
		
	}

	SpringDamper::~SpringDamper() {
	}

	SpringDamper::SpringDamper(Particle* a, Particle* b) : m_kd(0.0f), m_ks(0.0f) {
		assert(a != nullptr);
		assert(b != nullptr);
		m_a = a;
		m_b = b;
		m_L = glm::distance(a->get_position(), b->get_position());
	}
		
	SpringDamper::SpringDamper(Particle* a, Particle* b, float k_s, float k_d) {
		assert(k_s > 0.0f);
		assert(k_d > 0.0f);
		assert(a != nullptr);
		assert(b != nullptr);
		m_ks = k_s;
		m_kd = k_d;
		m_a = a;
		m_b = b;
		m_L = glm::distance(a->get_position(), b->get_position());
	}
		
	void SpringDamper::add_force() {
		glm::vec3 f, r, v;
		float f_s;

		r = (m_a->get_position() - m_b->get_position());
		v = (m_a->get_velocity() - m_b->get_velocity());

		f_s = m_ks * (glm::length(r) - m_L) + m_kd * (glm::dot(v, r) / glm::length(r));
		r = glm::normalize(r);
		f = f_s * r;
		//std::cout << "Spring force: " << f_s << std::endl;
		m_b->add_force(f);
		m_a->add_force(-1.0f * f);
	}
		
	void SpringDamper::set_spring_constant(float k_s) {
		assert(k_s > 0.0f);
		m_ks = k_s;
	}
		
	void SpringDamper::set_damper_constant(float k_d) {
		assert(k_d > 0.0f);
		m_kd = k_d;
	}
		
	float SpringDamper::get_spring_constant() {
		return m_ks;
	}
		
	float SpringDamper::get_damper_constant() {
		return m_kd;
	}

	float SpringDamper::get_initial_lenght() {
		return m_L;
	}

	void SpringDamper::make_connection(Particle* a, Particle* b) {
		assert(a != nullptr);
		assert(b != nullptr);
		m_a = a;
		m_b = b;
		m_L = glm::distance(a->get_position(), b->get_position());
	}
	
	void SpringDamper::set_initial_lenght(float L) {
		assert(L > 0.0f);
		m_L = L;
	}

	std::pair<glm::vec3, glm::vec3> SpringDamper::get_ends_positions() {
		return std::make_pair(m_a->get_position(), m_b->get_position());
	}
}