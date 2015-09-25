#include "Particle.h"
namespace physics {
	
	Particle::Particle () : m_position(glm::vec3()), m_velocity(glm::vec3()), m_force(glm::vec3()), m_mass(1.0f), m_fixed(false) {
	}

	void Particle::fix() {
		m_fixed = true;
	}

	void Particle::release() {
		m_fixed = false;
	}

	bool Particle::fixed() {
		return m_fixed;
	}

	Particle::Particle (glm::vec3& p, glm::vec3& v, glm::vec3& f, float& m) : m_position(p), m_velocity(v), m_force(f), m_fixed(false) {
		assert(m > 0.0f);
		m_mass = m;
	}

	Particle::Particle(glm::vec3& p, glm::vec3& v, float& m) : m_position(p), m_velocity(v), m_fixed(false) {
		assert(m > 0.0f);
		m_mass = m;
	}

	Particle::Particle(glm::vec3& p, float& m) : m_position(p), m_fixed(false) {
		assert(m > 0.0f);
		m_mass = m;
	}

	Particle::Particle (glm::vec3& p, glm::vec3& v, glm::vec3& f) : m_position(p), m_velocity(v), m_force(f), m_mass(1.0f), m_fixed(false) {
	}

	Particle::Particle (glm::vec3& p, glm::vec3& v) : m_position(p), m_velocity(v), m_mass(1.0f), m_fixed(false) {
	}

	Particle::Particle (glm::vec3& p) : m_position(p), m_mass(1.0f), m_fixed(false) {
	}

	Particle::~Particle () {
	}

	void Particle::add_force(glm::vec3 f) {
		m_force += f;
	}

	void Particle::add_velocity(glm::vec3 v) {
		m_velocity += v;
	}

	void Particle::add_position(glm::vec3 p) {
		m_position += p;
	}

	void Particle::set_position(glm::vec3 p) {
		m_position = p;
	}
	
	void Particle::set_velocity(glm::vec3 v) {
		m_velocity = v;
	}
	
	void Particle::set_force(glm::vec3 f) {
		m_force = f;
	}
	
	void Particle::set_mass(float& m) {
		assert(m > 0.0f);
		m_mass = m;
	}
	
	float Particle::get_mass() {
		return m_mass;
	}
	
	glm::vec3 Particle::get_position() {
		return m_position;
	}

	glm::vec3 Particle::get_velocity() {
		return m_velocity;
	}
	
	glm::vec3 Particle::get_force() {
		return m_force;
	}
}