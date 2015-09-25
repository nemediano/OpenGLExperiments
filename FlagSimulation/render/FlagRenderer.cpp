#include "FlagRenderer.h"
namespace renderer {
	FlagRenderer::FlagRenderer(Flag* flag) {
		assert(flag != nullptr);
		//Grab pointer
		m_flag_ptr = flag;
		//Create buffers
		m_num_vertex = m_flag_ptr->get_width() * m_flag_ptr->get_height();
		m_num_faces = 2 * (m_flag_ptr->get_width() - 1) * (m_flag_ptr->get_height() - 1);
		m_vertices = new float[m_num_vertex * 3];
		m_normals = new float[m_num_vertex * 3];
		m_text_coordinates = new float[m_num_vertex * 2];
		m_indices = new int [m_num_faces * 3];
	}

	FlagRenderer::~FlagRenderer() {
		if (m_vertices != nullptr) {
			delete[] m_vertices;
			m_vertices = nullptr;
		}

		if (m_normals != nullptr) {
			delete[] m_normals;
			m_normals = nullptr;
		}

		if (m_text_coordinates != nullptr) {
			delete[] m_text_coordinates;
			m_text_coordinates = nullptr;
		}

		if (m_indices != nullptr) {
			delete[] m_indices;
			m_indices = nullptr;
		}

		m_num_vertex = 0;
		m_num_faces = 0;
	}

	void FlagRenderer::render_flag() {

	}
}