#ifndef SCENE_CREATION_H_
#define SCENE_CREATION_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <vector>

namespace scene {
class SceneCreation {
public:
	SceneCreation();
	~SceneCreation();
	void render(GLint pos_loc, GLint norm_loc, GLint text_loc);
	void pre_render(GLint pos_loc, GLint norm_loc, GLint text_loc);
	void render_quad(unsigned int quad_number);
	void post_render(GLint pos_loc, GLint norm_loc, GLint text_loc);

protected:
	GLuint m_vertex_id;
	GLuint m_normals_id;
	GLuint m_text_coord_id;
	std::vector<glm::vec3> m_vertex;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_text_coordinates;
	
private:
	unsigned int m_quads_number;
	unsigned int m_triangle_number;
	void create_scene();
	void send_data_to_gpu();
	void add_quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
};
}

#endif