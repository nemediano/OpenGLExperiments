#ifndef FLAG_RENDERER_H_
#define FLAG_RENDERER_H_
#include "../scene/Flag.h"
using namespace scene;
namespace renderer {
	class FlagRenderer {
	public:
		FlagRenderer(Flag* flag);
		~FlagRenderer();
		void update_flag();
		void render_flag();
	private:
		float* m_vertices;
		float* m_normals;
		float* m_text_coordinates;
		int* m_indices;
		int m_num_vertex;
		int m_num_faces;
		Flag* m_flag_ptr;
	};
}
#endif