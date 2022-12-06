#version 330 core

// fullscreen quad input variables
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

// uv coord output to fragment shader
out vec2 uvCoord;

void main() {
    // Task 16: assign the UV layout variable to the UV "out" variable
    uvCoord = uv;

    gl_Position = vec4(position, 1.0);
}
