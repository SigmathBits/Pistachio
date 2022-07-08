// Pistachio
// Quad Shader

#shader vertex
#version 450 core

layout(std140, binding = 0) uniform Camera {
	uniform mat4 ProjectionViewMatrix;
} u_Camera;

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TextureCoords;
layout(location = 3) in uint a_TextureIndex;
layout(location = 4) in float a_TilingScale;
layout(location = 5) in int a_EntityID;

struct Vertex {
	vec4 Colour;
	vec2 TextureCoords;
	float TilingScale;
};

layout (location = 0) out Vertex v_Vertex;
layout (location = 3) flat out uint v_TextureIndex;
layout (location = 4) flat out int v_EntityID;


void main() {
	v_Vertex.Colour = a_Colour;
	v_Vertex.TextureCoords = a_TextureCoords;
	v_TextureIndex = a_TextureIndex;
	v_Vertex.TilingScale = a_TilingScale;
	v_EntityID = a_EntityID;

	gl_Position = u_Camera.ProjectionViewMatrix * a_Position;
}


#shader fragment
#version 450 core

layout (binding = 0) uniform sampler2D u_Textures[32];

struct Vertex {
	vec4 Colour;
	vec2 TextureCoords;
	float TilingScale;
};

layout (location = 0) in Vertex v_Vertex;
layout (location = 3) flat in uint v_TextureIndex;
layout (location = 4) flat in int v_EntityID;

layout(location = 0) out vec4 o_Colour;
layout(location = 1) out int o_EntityID;


void main () {
	vec4 textureColour = texture(u_Textures[v_TextureIndex], v_Vertex.TilingScale * v_Vertex.TextureCoords) * v_Vertex.Colour;

	if (textureColour.a == 0.0f) {
		discard;
	}

	o_Colour = textureColour;
	o_EntityID = v_EntityID;
}
