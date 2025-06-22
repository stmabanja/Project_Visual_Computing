#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec3 vertex_color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
out vec3 colorVS;

uniform vec3 lightPos;
out vec3 normal, lightDir;

void main() {
gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0);
colorVS = vertex_color;

// compute normal in view space //
vec4 n = vec4(vertex_normal, 1.0);
mat4 normalMat = transpose(inverse(viewMatrix * modelMatrix));
normal = (normalMat * n).xyz;
// compute light direction in view space //
vec4 lp = vec4(viewMatrix * vec4(lightPos, 1.0));
vec4 P = vec4(viewMatrix * modelMatrix * (vertex, 1.0));
lightDir = (lp - P).xyz;

}