#version 330

uniform sampler2D texture_map; //texture map
uniform int texture_option; //Use texture or color

//Inputs from vertex shader
in vec4 vColor;
in vec3 vNormal;
in vec2 vTextCoord;

out vec4 fragcolor;

void main(void) {
	vec4 text_color = texture(texture_map, vTextCoord);
	fragcolor = mix(vColor, text_color, texture_option);
}
