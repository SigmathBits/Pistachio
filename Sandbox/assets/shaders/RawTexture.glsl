// Basic texture shader

#shader vertex
#version 330 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoords;

out vec2 v_TextureCoords;

void main() {
	v_TextureCoords = a_TextureCoords;

	gl_Position = u_ProjectionViewMatrix * u_Transform * a_Position;
}


#shader fragment
#version 330 core

uniform sampler2D u_Texture;

in vec2 v_TextureCoords;

layout(location = 0) out vec4 colour;

void main () {
	colour = texture(u_Texture, v_TextureCoords);
}
