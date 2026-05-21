#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D container;
uniform sampler2D awesomeface;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(container, TexCoord), texture(awesomeface, TexCoord), 0.2);
}
