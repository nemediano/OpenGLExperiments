#include "WindSource.h"
namespace physics {
	WindSource::WindSource() : m_mean(0.0f), m_standar_deviation(1.0f), m_position(glm::vec3(0.0f, 0.0f, 1.0f)) {
		update_force();
	}

	WindSource::WindSource(glm::vec3 position) : m_mean(0.0f), m_standar_deviation(1.0f), m_position(position) {
		update_force();
	}

	WindSource::WindSource(glm::vec3 position, float mean, float stdv) : m_position(position) {
		set_mean(mean);
		set_stdv(stdv);
		update_force();
	}

	float WindSource::get_mean() {
		return m_mean;
	}

	float WindSource::get_stdv() {
		return m_standar_deviation;
	}

	void WindSource::set_mean(float mean) {
		assert(mean > 0.0f);
		m_mean = mean;
	}

	void WindSource::set_stdv(float stdv) {
		assert(stdv > 0.0f);
		m_standar_deviation = stdv;
	}

	WindSource::~WindSource() {

	}
	
	void WindSource::update_force() {
		std::normal_distribution<float> distribution(m_mean, m_standar_deviation);
		m_force_magnitude = distribution(m_generator);
	}
	
	glm::vec3 WindSource::get_force_vector(glm::vec3 triangle_center) {
		glm::vec3 r = glm::normalize(triangle_center - m_position);
		return m_force_magnitude * r;
	}
}