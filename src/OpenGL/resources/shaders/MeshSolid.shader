#shader vertex
# version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 v_normal;
out vec3 v_position;

uniform mat4 u_MVP;
uniform mat4 u_model;

void main(){
	gl_Position = u_MVP * vec4(position, 1.0f);
	v_normal = mat3(transpose(inverse(u_model))) * normal;  
};

#shader fragment
# version 330 core

layout(location = 0) out vec4 color;

in vec3 v_position;
in vec3 v_normal;


uniform vec3 u_lightcolor;
uniform vec3 u_ambientlightcolor;
uniform vec3 u_lightpos;
uniform vec3 u_viewpos;

void main(){
	vec3 objcolor = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambientResult = u_ambientlightcolor * objcolor;

	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(u_lightpos - v_position); 

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseResult = diff * u_lightcolor;

	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_viewpos - v_position);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specularResult = specularStrength * spec * u_lightcolor; 

	color = vec4((ambientResult + diffuseResult + specularResult), 1.0);
};