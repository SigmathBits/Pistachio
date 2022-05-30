#shader vertex
#version 330 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

layout(location = 0) in vec4 a_Position;

void main() {
	gl_Position = u_ProjectionViewMatrix * u_Transform * a_Position;
}


#shader fragment
#version 330 core

uniform vec4 u_Colour;

layout(location = 0) out vec4 colour;

void main () {
	colour = u_Colour;
}
