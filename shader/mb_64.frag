#version 330 core
#extension GL_ARB_gpu_shader_fp64 : require

in vec2 fragPos;

out vec4 outColor;

uniform dvec2 pos;
uniform dvec2 scale;

const vec4 zero = vec4(0.0, 0.0, 0.5, 1.0);
const vec4 one = vec4(1.0, 1.0, 1.0, 1.0);

const int max_iterations = 100;
const float max_betrag_2 = 4.0;

void iterate(dvec2 pos, out int iterations, out float betrag_2)
{
	betrag_2 = 0;
	double xx = 0.0;
	double yy = 0.0;
	double xy = 0.0;

	for (iterations = 0; iterations < max_iterations; ++iterations)
	{
		double x = xx - yy + pos.x;
		double y = 2 * xy + pos.y;

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
