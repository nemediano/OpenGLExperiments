#ifndef FACE_H_
#define FACE_H_
#include "Particle.h"
#include "WindSource.h"
namespace physics {
	class Face {
	public:
		Face();
		Face(Particle* a, Particle* b, Particle* c, Particle* d);
		~Face();
		void add_force(WindSource& wind);
		void make_connection(Particle* a, Particle* b, Particle* c, Particle* d);
	private:
		Particle* m_a;
		Particle* m_b;
		Particle* m_c;
		Particle* m_d;
	};
}
#endif