// Pistachio
// Renderer2D Circle Shader

#shader vertex
#version 450 core

layout(std140, binding = 0) uniform Camera {
	uniform mat4 ProjectionViewMatrix;
} u_Camera;

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Colour;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Blur;
layout(location = 5) in int a_EntityID;

struct Vertex {
	vec3 LocalPosition;
	vec4 Colour;
	float Thickness;
	float Blur;
};

layout (location = 0) out Vertex v_Vertex;
layout (location = 4) flat out int v_EntityID;


void main() {
	v_Vertex.LocalPosition = a_LocalPosition;
	v_Vertex.Colour = a_Colour;
	v_Vertex.Thickness = a_Thickness;
	v_Vertex.Blur = a_Blur;
	v_EntityID = a_EntityID;

	gl_Position = u_Camera.ProjectionViewMatrix * a_Position;
}


#shader fragment
#version 450 core

layout (binding = 0) uniform sampler2D u_Textures[32];

struct Vertex {
	vec3 LocalPosition;
	vec4 Colour;
	float Thickness;
	float Blur;
};

layout (location = 0) in Vertex v_Vertex;
layout (location = 4) flat in int v_EntityID;

layout(location = 0) out vec4 o_Colour;
layout(location = 1) out int o_EntityID;


void main () {
	float distance = 1.0f - length(v_Vertex.LocalPosition);
	float circleAlpha = smoothstep(0.0f, v_Vertex.Blur, distance);
	circleAlpha *= smoothstep(v_Vertex.Thickness + v_Vertex.Blur, v_Vertex.Thickness, distance);
	
	if (circleAlpha <= 0.0f) {
		discard;
	}

	o_Colour = v_Vertex.Colour;
	o_Colour.a *= circleAlpha;

	o_EntityID = v_EntityID;
}
