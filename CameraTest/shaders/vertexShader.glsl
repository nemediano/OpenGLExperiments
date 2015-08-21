#version 330

in vec4 Position;
in vec2 TextCoord;
in vec3 Normal;

uniform mat4 PVM;
uniform vec4 Color;

out vec4 vColor;
out vec3 vNormal;
out vec2 vTextCoord;

void main(void) { 
	gl_Position = PVM * Position; 
	vNormal = Normal;
	vColor = Color;
	vTextCoord = TextCoord;
}
