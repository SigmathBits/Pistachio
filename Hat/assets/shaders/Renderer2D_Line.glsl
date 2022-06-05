// Pistachio
// Line Shader

#shader vertex
#version 450 core

layout(std140, binding = 0) uniform Camera {
	uniform mat4 ProjectionViewMatrix;
} u_Camera;

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in int a_EntityID;

struct Vertex {
	vec4 Colour;
};

layout (location = 0) out Vertex v_Vertex;
layout (location = 1) flat out int v_EntityID;


void main() {
	v_Vertex.Colour = a_Colour;
	v_EntityID = a_EntityID;

	gl_Position = u_Camera.ProjectionViewMatrix * a_Position;
}


#shader fragment
#version 450 core

struct Vertex {
	vec4 Colour;
};

layout (location = 0) in Vertex v_Vertex;
layout (location = 1) flat in int v_EntityID;

layout(location = 0) out vec4 o_Colour;
layout(location = 1) out int o_EntityID;


void main () {
	o_Colour = v_Vertex.Colour;
	o_EntityID = v_EntityID;
}
