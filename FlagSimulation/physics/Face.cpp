#include <glm/glm.hpp>
#include "Face.h"
namespace physics {

	Face::Face() : m_a(nullptr), m_b(nullptr), m_c(nullptr), m_d(nullptr) {
	}

	Face::Face(Particle* a, Particle* b, Particle* c, Particle* d) {
		assert(a != nullptr);
		assert(b != nullptr);
		assert(c != nullptr);
		assert(d != nullptr);
		m_a = a;
		m_b = b;
		m_c = c;
		m_d = d;
	}

	Face::~Face() {
	}

	void Face::add_force(WindSource& wind) {
		//For the triangle ACB
		glm::vec3 centroid = (1.0f / 3.0f) * (m_a->get_position() + m_b->get_position() + m_c->get_position());
		glm::vec3 f_w = wind.get_force_vector(centroid);
		glm::vec3 cross_product = glm::cross(m_c->get_position() - m_a->get_position(), m_b->get_position() - m_a->get_position());
		float area = 0.5f * glm::length(cross_product);
		glm::vec3 n = glm::normalize(cross_product);
		glm::vec3 r = glm::normalize(f_w);
		glm::vec3 force = (glm::dot(n, r) / area) * f_w;
		m_a->add_force(0.333f * force);
		m_b->add_force(0.333f * force);
		m_c->add_force(0.333f * force);

		//For the triangle BCD
		centroid = (1.0f / 3.0f) * (m_d->get_position() + m_b->get_position() + m_c->get_position());
		f_w = wind.get_force_vector(centroid);
		cross_product = glm::cross(m_c->get_position() - m_d->get_position(), m_b->get_position() - m_d->get_position());
		area = 0.5f * glm::length(cross_product);
		n = glm::normalize(cross_product);
		r = glm::normalize(f_w);
		force = (glm::dot(n, r) / area) * f_w;
		m_d->add_force(0.333f * force);
		m_b->add_force(0.333f * force);
		m_c->add_force(0.333f * force);
	}

	void Face::make_connection(Particle* a, Particle* b, Particle* c, Particle* d) {
		assert(a != nullptr);
		assert(b != nullptr);
		assert(c != nullptr);
		assert(d != nullptr);
		m_a = a;
		m_b = b;
		m_c = c;
		m_d = d;
	}
}
