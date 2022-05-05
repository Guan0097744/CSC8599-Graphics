#version 330 core
out vec4 fragColor;

in vec3 ourColor;

void main()
{
	//fragColor = vec4(0.5f, 0.8f, 0.1f, 1.0f);
	//fragColor = color;
	fragColor = vec4(ourColor, 1.0);
}