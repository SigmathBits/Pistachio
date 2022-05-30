#shader vertex
#version 330 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TextureCoords;
layout(location = 3) in float a_TextureIndex;
layout(location = 4) in float a_TilingScale;

out vec4 v_Colour;
out vec2 v_TextureCoords;
out float v_TextureIndex;
out float v_TilingScale;


void main() {
	v_Colour = a_Colour;
	v_TextureCoords = a_TextureCoords;
	v_TextureIndex = a_TextureIndex;
	v_TilingScale = a_TilingScale;

	gl_Position = u_ProjectionViewMatrix * u_Transform * a_Position;
}


#shader fragment
#version 330 core

uniform sampler2D u_Textures[32];

in vec4 v_Colour;
in vec2 v_TextureCoords;
in float v_TextureIndex;
in float v_TilingScale;

layout(location = 0) out vec4 colour;


void main () {
	colour = texture(u_Textures[int(v_TextureIndex)], v_TilingScale * v_TextureCoords) * v_Colour;
}
