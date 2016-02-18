#version 330 core

in vec2 fragPos;

out vec4 outColor;

uniform vec2 pos;
uniform vec2 scale;

const vec4 zero = vec4(0.0, 0.0, 0.5, 1.0);
const vec4 one = vec4(1.0, 1.0, 1.0, 1.0);

const int max_iterations = 100;
const float max_betrag_2 = 4.0;

void iterate(vec2 pos, out int iterations, out float betrag_2)
{
	betrag_2 = 0;
	float xx = 0.0;
	float yy = 0.0;
	float xy = 0.0;

	for (iterations = 0; iterations < max_iterations; ++iterations)
	{
		float x = xx - yy + pos.x;
		float y = 2 * xy + pos.y;

		xx = x * x;
		yy = y * y;
		xy = x * y;

		betrag_2 = float(xx + yy);

		if (betrag_2 > max_betrag_2)
			return;
	}
}

void main()
{
	int iterations;
	float betrag_2;

	iterate(pos + scale * fragPos, iterations, betrag_2);

	if (iterations == max_iterations) {
		outColor = vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		float c = iterations - log(log(betrag_2) / log(4.0)) / log(2.0);
		outColor = mix(zero, one, c / 100.0);
	}
}
