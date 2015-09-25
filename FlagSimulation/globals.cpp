#include "globals.h"

bool texture_map_flag;
bool texture_animation;
bool rotate_animation;
bool light_mode_flag;
bool fish_animation;
int switch_material;

//Mouse manipulation 
bool mouse_dragging;
glm::vec2 mouse_start_drag;

//Camera parameters
glm::vec3 camera_position;
glm::vec3 camera_center;
glm::vec3 camera_up;
glm::vec2 camera_pan;

glm::quat camera_base_rotation;
glm::quat camera_new_rotation;

float field_of_view_y;
//enum CAM_TYPE{ ROTATION, PAN, NONE };
CAM_TYPE mode;

float seconds;

int VM_loc = -1;
int P_loc = -1;
int time_loc = -1;
int texture_map_flag_loc = -1;
int light_mode_flag_loc = -1;
int fish_animation_loc = -1;
int rotate_animation_loc = -1;
int view_vector_loc = -1;
int light_position_in_vs_loc = -1;
int light_direction_in_vs_loc = -1;
int tex_loc = -1;
int switch_material_loc = -1;

scene::Flag* flag = nullptr;

const float TAU = 6.28318f;