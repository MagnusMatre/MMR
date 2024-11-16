#shader vertex
# version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_MVP;

void main(){
	gl_Position = u_MVP * vec4(position, 1.0f);
};

#shader fragment
# version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_wireframecolor;

void main(){
    if (gl_FrontFacing) {
        color = vec4(u_wireframecolor, 1.0f);
    } else {
        discard; // Discard back-facing fragments
    }
};

