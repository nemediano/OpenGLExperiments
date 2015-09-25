#version 150

in vec4 vPosition;
in vec3 vColor;
uniform mat4 VM;
uniform mat4 Proj;

out vec4 color;

void main() {
    gl_Position = Proj * VM * vPosition;
	//gl_Position = vPosition;
	color = vec4(vColor, 1.0);
}
