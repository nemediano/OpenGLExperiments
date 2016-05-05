#version 330

in vec4 Position;
in vec3 Normal;

uniform mat4 PVM;
uniform mat4 NormalMatrix;
uniform mat4 VM;

//Outputs for fragment shader
out vec3 fNormal;
out vec3 fPosition;

void main(void) { 
	gl_Position = PVM * Position;
	fNormal = normalize(vec3(NormalMatrix * vec4(Normal, 1.0)));
	//Position needs to be in view space for lighting
	fPosition = vec3(VM * Position);
}
