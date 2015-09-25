#ifndef WINDSOURCE_H_
#define WINDSOURCE_H_
#include <glm/glm.hpp>
#include <random>

namespace physics {
	class WindSource {
	public:
	   WindSource();
	   WindSource(glm::vec3 position);
	   WindSource(glm::vec3 position, float mean, float stdv);
	   float get_mean();
	   float get_stdv();
	   void set_mean(float mean);
	   void set_stdv(float stdv);
	   ~WindSource();
	   void update_force();
	   glm::vec3 get_force_vector(glm::vec3 triangle_center);
	private:
       glm::vec3 m_position;
	   float m_force_magnitude;
	   std::mt19937 m_generator;
	   float m_mean;
	   float m_standar_deviation;
	};
}
#endif