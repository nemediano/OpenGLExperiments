#ifndef SPHERE_SUBDIVISION_H
#define SPHERE_SUBDIVISION_H

//Program management
void exit_glut();
void init_OpenGL();
void init_program();
void create_glut_window();
void create_glut_callbacks();
void draw_gui();

//Scene creation
void create_sphere();
void start_thetrahedra();
void start_icosahedra();
void subdivide_face(glm::vec3& p_0, glm::vec3& p_1, glm::vec3& p_2, const size_t& level);
void create_indexed_mesh();
void pass_light_and_material();

//Only callback function outside interface
void display();

#endif