#version 330 core
// Berechnet finale Position jedes Vertex im Ausgabebild

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 colorRGB;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 colorVS;

void main(){
    colorVS = colorRGB;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);

}

// Output ist die Vertex-Position in homogenen Koordinaten, vor der perspektivischen
//   Division (Homogenisierung)