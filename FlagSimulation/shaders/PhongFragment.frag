#version 130
uniform int texture_map_flag;
uniform int light_mode;
uniform int switch_material;
uniform vec4 light_position_in_vs;
uniform vec4 light_direction_in_vs;
uniform vec4 view;      //view direction
uniform sampler2D texture_map;

//From vertex shader
in vec3 position_viewspace;
in vec2 text_coord;
in vec3 fnormal;

vec3 ka = vec3(0.83, 0.69, 0.22);        //ambient material color
vec3 kd = vec3( 1.0, 0.84, 0.0);         //diffuse material color
vec3 ks = vec3( 0.9, 0.9,  0.9);         //specular material color

//Lighting model settings
const vec3 La = vec3(0.3, 0.3, 0.3);   //ambient light color
const vec3 Ld = vec3(0.5, 0.5, 0.5);   //diffuse light color
const vec3 Ls = vec3(0.9, 0.9, 0.9);   //specular light color

const float shininess = 26.0;

//Output to render
out vec4 frag_color;

void set_material(int material);

void main(void) {
   vec4 material_color;
   set_material(switch_material);
   if (texture_map_flag != 0) {
      material_color = texture(texture_map, text_coord);
	  ka = 0.9 * material_color.rgb;
	  kd = material_color.rgb;
   }
   
   //Make sure we are using only normalized vectors
   vec3 n = normalize(fnormal);
   vec3 l;
   vec3 v;
   if (light_mode == 1) {//Assuming directional light
      //We want a vector point to light source so that is -light_direction
      l = normalize(-light_direction_in_vs.xyz);
	  //Since we have directional light we the camera is also directional
	  v = normalize(-view.xyz);
   } else { //Assuming punctual positional light
      l = normalize(light_position_in_vs.xyz - position_viewspace);
	  //Since we are in view space the eye is at the origin so v = eye - p = -p
	  v = normalize(-position_viewspace);
   }
   // the first argument in GLSL reflect function is "incident" vector.
   // so we need to reverse the vector, l.
   vec3 r = normalize(reflect(-l, fnormal));
   
   vec3 ambient_term = ka * La;
   vec3 diffuse_term = kd * Ld * max(0.0, dot(n, l));
   vec3 specular_term = ks * Ls * pow(max(0.0, dot(r, v)), shininess);
   vec4 fcolor = vec4(ambient_term + diffuse_term + specular_term, 1.0);
   
   frag_color = fcolor;
}

void set_material(int material) {
	switch(material) {
	    //Rods
		case 1:
			ka = vec3(0.83, 0.69, 0.22);
			kd = vec3( 1.0, 0.84, 0.0);
			ks = vec3( 0.9, 0.9,  0.9);
		break;
		//Spheres
		case 2:
			ka = vec3(0.89, 0.15, 0.21);
			kd = vec3( 1.0, 0.01, 0.24);
			ks = vec3( 0.9, 0.9,  0.9);
		break;
	}
}