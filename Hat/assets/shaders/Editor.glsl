#shader vertex
#version 450 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TextureCoords;
layout(location = 3) in uint a_TextureIndex;
layout(location = 4) in float a_TilingScale;
layout(location = 5) in int a_EntityID;

out vec4 v_Colour;
out vec2 v_TextureCoords;
flat out uint v_TextureIndex;
out float v_TilingScale;
flat out int v_EntityID;


void main() {
	v_Colour = a_Colour;
	v_TextureCoords = a_TextureCoords;
	v_TextureIndex = a_TextureIndex;
	v_TilingScale = a_TilingScale;
	v_EntityID = a_EntityID;

	gl_Position = u_ProjectionViewMatrix * u_Transform * a_Position;
}


#shader fragment
#version 450 core

uniform sampler2D u_Textures[32];

in vec4 v_Colour;
in vec2 v_TextureCoords;
flat in uint v_TextureIndex;
in float v_TilingScale;
flat in int v_EntityID;

layout(location = 0) out vec4 colour;
layout(location = 1) out int entityID;


void main () {
	colour = texture(u_Textures[v_TextureIndex], v_TilingScale * v_TextureCoords) * v_Colour;
	entityID = v_EntityID;
}
