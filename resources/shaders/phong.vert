#version 330 core

// from VBO
layout(location = 0) in vec3 objSpacePos;
layout(location = 1) in vec3 objSpaceNorm;

// model matrix for object, and transpose-inverse matrix for converting normals
uniform mat4 model;
uniform mat3 normModel;

// projection * view matrix
uniform mat4 projView;

// to fragment shader (for phong)
out vec3 worldSpacePos;
out vec3 worldSpaceNorm;

void main() {
    // calculate world space position and normal
    worldSpacePos = vec3(model * vec4(objSpacePos, 1.0));
    worldSpaceNorm = normalize(normModel * objSpaceNorm);

    // set gl_Position to the object space position transformed to clip space
    gl_Position = projView * vec4(worldSpacePos, 1.0);
}
