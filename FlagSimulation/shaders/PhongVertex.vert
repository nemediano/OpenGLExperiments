#version 130
uniform float time;
uniform mat4 VM;
uniform mat4 P;

in vec3 pos_attrib;
in vec2 tex_coord_attrib;
in vec3 normal_attrib;

out vec2 text_coord;
out vec3 fnormal;
out vec3 position_viewspace;

mat4 rotationMatrix(vec3 rot_axis, float angle);

void main(void){
   vec4 position;
   position = vec4(pos_attrib, 1.0);
   mat3 rot_VM = mat3(VM);
   text_coord = tex_coord_attrib;
   fnormal =  rot_VM * normal_attrib;
   position_viewspace = (VM * position).xyz;
   gl_Position = P * VM * position;
}
