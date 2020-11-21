#version 330 core
uniform bool useHighlightColor;
const vec3 hightLightColor = vec3(0.7, 0.7, 0.7);

in vec3 fragmentColor;
out vec4 FragColor;

void main()
{
	vec3 color = useHighlightColor ? hightLightColor * fragmentColor : fragmentColor;

	FragColor = vec4(color, 1.0);
}
