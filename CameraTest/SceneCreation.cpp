#include "SceneCreation.h"
#include <vector>

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

namespace scene {
	SceneCreation::SceneCreation() {
		glGenBuffers(1, &m_vertex_id);
		glGenBuffers(1, &m_normals_id);
		glGenBuffers(1, &m_text_coord_id);
		m_triangle_number = 0;
		m_quads_number = 0;
		create_scene();
		send_data_to_gpu();
	}

	SceneCreation::~SceneCreation() {
		m_normals.clear();
		m_vertex.clear();
		m_text_coordinates.clear();
	}

	void SceneCreation::render(GLint pos_loc, GLint norm_loc, GLint text_loc) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_id);
		//int stride = (3 + 3 + 2) * sizeof(float);
		if (pos_loc != -1) {
			glEnableVertexAttribArray(pos_loc);
			glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		}

		if (norm_loc != -1) {
			glEnableVertexAttribArray(norm_loc);
			glVertexAttribPointer(norm_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		}

		if (text_loc != -1) {
			glEnableVertexAttribArray(text_loc);
			glVertexAttribPointer(text_loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		}
		glDrawArrays(GL_TRIANGLES, 0, m_triangle_number * 3);

		if (pos_loc != -1) {
			glDisableVertexAttribArray(pos_loc);
		}

		if (norm_loc != -1) {
			glDisableVertexAttribArray(norm_loc);
		}
		
		if (text_loc != -1) {
			glDisableVertexAttribArray(text_loc);
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SceneCreation::create_scene() {
		//Floor
		glm::vec3 p1 = glm::vec3(-1.0f, -0.5f, -1.0f);
		glm::vec3 p2 = glm::vec3(-1.0f, -0.5f, 1.0f);
		glm::vec3 p3 = glm::vec3( 1.0f, -0.5f, 1.0f);
		glm::vec3 p4 = glm::vec3( 1.0f, -0.5f, -1.0f);
		add_quad(p1, p2, p3, p4);
		//First quad row
		//Left
		p1 = glm::vec3(-1.0f, -0.5f, 0.0f);
		p2 = glm::vec3(-0.6f, -0.5f, 0.0f);
		p3 = glm::vec3(-0.6f, -0.1f, 0.0f);
		p4 = glm::vec3(-1.0f, -0.1f, 0.0f);
		add_quad(p1, p2, p3, p4);
		//Center
		p1 = glm::vec3(-0.2f, -0.5f, 0.0f);
		p2 = glm::vec3( 0.2f, -0.5f, 0.0f);
		p3 = glm::vec3( 0.2f, -0.1f, 0.0f);
		p4 = glm::vec3(-0.2f, -0.1f, 0.0f);
		add_quad(p1, p2, p3, p4);
		//Right
		p1 = glm::vec3( 0.6f, -0.5f, 0.0f);
		p2 = glm::vec3( 1.0f, -0.5f, 0.0f);
		p3 = glm::vec3( 1.0f, -0.1f, 0.0f);
		p4 = glm::vec3( 0.6f, -0.1f, 0.0f);
		add_quad(p1, p2, p3, p4);

		//Second quad row
		//Left
		p1 = glm::vec3(-1.0f, -0.5f, -0.5f);
		p2 = glm::vec3(-0.6f, -0.5f, -0.5f);
		p3 = glm::vec3(-0.6f, -0.1f, -0.5f);
		p4 = glm::vec3(-1.0f, -0.1f, -0.5f);
		add_quad(p1, p2, p3, p4);
		//Center
		p1 = glm::vec3(-0.2f, -0.5f, -0.5f);
		p2 = glm::vec3( 0.2f, -0.5f, -0.5f);
		p3 = glm::vec3( 0.2f, -0.1f, -0.5f);
		p4 = glm::vec3(-0.2f, -0.1f, -0.5f);
		add_quad(p1, p2, p3, p4);
		//Right
		p1 = glm::vec3( 0.6f, -0.5f, -0.5f);
		p2 = glm::vec3( 1.0f, -0.5f, -0.5f);
		p3 = glm::vec3( 1.0f, -0.1f, -0.5f);
		p4 = glm::vec3( 0.6f, -0.1f, -0.5f);
		add_quad(p1, p2, p3, p4);

		//Third quad row
		//Left
		p1 = glm::vec3(-1.0f, -0.5f, 0.5f);
		p2 = glm::vec3(-0.6f, -0.5f, 0.5f);
		p3 = glm::vec3(-0.6f, -0.1f, 0.5f);
		p4 = glm::vec3(-1.0f, -0.1f, 0.5f);
		add_quad(p1, p2, p3, p4);
		//Center
		p1 = glm::vec3(-0.2f, -0.5f, 0.5f);
		p2 = glm::vec3( 0.2f, -0.5f, 0.5f);
		p3 = glm::vec3( 0.2f, -0.1f, 0.5f);
		p4 = glm::vec3(-0.2f, -0.1f, 0.5f);
		add_quad(p1, p2, p3, p4);
		//Right
		p1 = glm::vec3( 0.6f, -0.5f, 0.5f);
		p2 = glm::vec3( 1.0f, -0.5f, 0.5f);
		p3 = glm::vec3( 1.0f, -0.1f, 0.5f);
		p4 = glm::vec3( 0.6f, -0.1f, 0.5f);
		add_quad(p1, p2, p3, p4);
	}

	void SceneCreation::send_data_to_gpu() {
		//Send vertex coordinates data
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_id);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * m_vertex.size(), &m_vertex[0], GL_STATIC_DRAW);
		//Send normals coordinates data
		glBindBuffer(GL_ARRAY_BUFFER, m_normals_id);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);
		//Send texture coordinates
		glBindBuffer(GL_ARRAY_BUFFER, m_text_coord_id);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * m_text_coordinates.size(), &m_text_coordinates[0], GL_STATIC_DRAW);

	}

	void SceneCreation::add_quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
		//Quick and dirty just add two triangles
		glm::vec3 n = glm::normalize(0.5f * (glm::cross(p3 - p2, p1 - p2) + glm::cross(p1 - p4, p3 - p4)));

		//First triangle
		m_vertex.push_back(p1);
		m_normals.push_back(n);
		m_text_coordinates.push_back(glm::vec2(0.0f, 0.0f));

		m_vertex.push_back(p2);
		m_normals.push_back(n);
		m_text_coordinates.push_back(glm::vec2(1.0f, 0.0f));

		m_vertex.push_back(p3);
		m_normals.push_back(n);
		m_text_coordinates.push_back(glm::vec2(1.0f, 1.0f));

		m_triangle_number++;
		
		//Second triangle
		m_vertex.push_back(p1);
		m_normals.push_back(n);
		m_text_coordinates.push_back(glm::vec2(0.0f, 0.0f));
		
		m_vertex.push_back(p3);
		m_normals.push_back(n);
		m_text_coordinates.push_back(glm::vec2(1.0f, 1.0f));
		
		m_vertex.push_back(p4);
		m_normals.push_back(n);
		m_text_coordinates.push_back(glm::vec2(0.0f, 1.0f));
		
		m_triangle_number++;

		m_quads_number++;
	}

	void SceneCreation::pre_render(GLint pos_loc, GLint norm_loc, GLint text_loc) {
		
		if (pos_loc != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vertex_id);
			glEnableVertexAttribArray(pos_loc);
			glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		}
		
		if (norm_loc != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, m_normals_id);
			glEnableVertexAttribArray(norm_loc);
			glVertexAttribPointer(norm_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		}

		if (text_loc != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, m_text_coord_id);
			glEnableVertexAttribArray(text_loc);
			glVertexAttribPointer(text_loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		}
		
	}

	void SceneCreation::render_quad(unsigned int quad_number) {
		assert(quad_number > 0);
		assert(quad_number <= m_quads_number);
		glDrawArrays(GL_TRIANGLES, 6 * (quad_number - 1), 6);
	}

	void SceneCreation::post_render(GLint pos_loc, GLint norm_loc, GLint text_loc) {
		if (pos_loc != -1) {
			glDisableVertexAttribArray(pos_loc);
		}

		if (norm_loc != -1) {
			glDisableVertexAttribArray(norm_loc);
		}

		if (text_loc != -1) {
			glDisableVertexAttribArray(text_loc);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}