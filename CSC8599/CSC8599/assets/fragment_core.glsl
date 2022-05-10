#version 330 core
out vec4 fragColor;

//in vec4 color;
in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;

void main()
{
	//fragColor = vec4(1.0f, 0.2f, 0.6f, 1.0f);
	//fragColor = color;
	//fragColor = vec4(ourColor, 1.0);
	fragColor = texture(texture1, texCoord);
}