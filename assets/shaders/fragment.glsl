#version 330 core
// Berechnet die Farbe jedes Pixels im Ausgabebild

in vec3 colorVS;

//fragment shader output
out vec4 color;

uniform float time;

void main(){


    vec3 animatedColor = vec3(sin(time) + 2.0, sin(time) + 2.0, sin(time) + 2.0);
    color = vec4(animatedColor * colorVS, 1.0);
}
