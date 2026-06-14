#version 460 core
struct VertexData {
    float position[3];
    float color[3];
    float tex[2];	
};

layout(binding = 0, std430) readonly buffer ssbo1 {
    VertexData data[];
};

vec3 getPosition(int index) {
    return vec3(
        data[index].position[0], 
        data[index].position[1], 
        data[index].position[2]
    );
}

vec3 getColor(int index) {
    return vec3(
        data[index].color[0], 
        data[index].color[1], 
        data[index].color[2]
    );
}

vec2 getTexCoord(int index) {
    return vec2(
        data[index].tex[0], 
        data[index].tex[1]
    );
}

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(getPosition(gl_VertexID), 1.0);
    ourColor = getColor(gl_VertexID);
    TexCoord = getTexCoord(gl_VertexID);
}
