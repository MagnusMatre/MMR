#shader vertex
# version 330 core

layout(location = 0) in vec4 position;

uniform mat4 u_MVP;

void main(){
	gl_Position = u_MVP * position;
};

#shader fragment
# version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_lightcolor;
uniform vec3 u_objectcolor;

void main(){
	color = vec4(u_lightcolor * u_objectcolor, 1.0);
};