#ifndef GLOBALS_H_
#define GLOBALS_H_

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>


#include "scene/Flag.h"
extern bool texture_map_flag;
extern bool texture_animation;
extern bool rotate_animation;
extern bool light_mode_flag;
extern bool fish_animation;
extern int switch_material;

//Mouse manipulation
extern bool mouse_dragging;
extern glm::vec2 mouse_start_drag;

extern glm::vec3 camera_position;
extern glm::vec3 camera_center;
extern glm::vec3 camera_up;
extern glm::vec2 camera_pan;

extern glm::quat camera_base_rotation;
extern glm::quat camera_new_rotation;

extern float field_of_view_y;
enum CAM_TYPE{ ROTATION, PAN, NONE };
extern CAM_TYPE mode;

extern float seconds;
extern const float TAU;

extern int VM_loc;
extern int P_loc;
extern int time_loc;
extern int texture_map_flag_loc;
extern int light_mode_flag_loc;
extern int fish_animation_loc;
extern int rotate_animation_loc;
extern int view_vector_loc;
extern int light_position_in_vs_loc;
extern int light_direction_in_vs_loc;
extern int tex_loc;
extern int switch_material_loc;

extern scene::Flag* flag;

#endif