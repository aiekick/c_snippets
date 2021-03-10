#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <stdlib.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define DBL_EPSILON 2.2204460492503131e-16
#define IS_DOUBLE_EQUAL(a, b) abs(a - b) < DBL_EPSILON
#define IS_DOUBLE_ZERO(a) IS_DOUBLE_EQUAL(a, 0.0)

#define dmin(a, b) (((a)<(b)) ? (a) : (b))
#define dmax(a, b) (((a)>(b)) ? (a) : (b))

int printJulia(
		int sx, // width
		int sy, // height
		double px, // julia param x
		double py, // julia param y
		int iterations, // count iterations
		const char* charset,
		size_t countChars) // charset (ASCII art)
{
	if (sx == 0 ||
		sy == 0 ||
		iterations == 0 ||
		charset == 0 ||
		countChars == 0U)
		return 0;

	const size_t schar = sizeof(char);

	const size_t charsetLength = strlen(charset);
	const double zoom = 1.4;
	const double threshold = 0.14;

	int bufSize = (int)sizeof(char) * sx * sy;
	char* buffer = malloc(bufSize + 1);
	memset(buffer, 0, bufSize);

	double timer = 0.0;

	double sum = 0.0;
	double count = 0;

#ifdef _OPENMP
	while(timer < 10)
	{
		double ppx = sin(timer += 0.01) * px;
		double start = omp_get_wtime();
		{
			double mns = zoom / dmin((double)sx, (double)sy);
#pragma default(none) omp parallel for
#endif
			for (int t = 0; t < bufSize; t++)
			{
				double x = (double)(t % sx);
				double y = floor((double)t / (double)sy);

				// current pixel
				double zx = (x * 2.0 - (double)sx) * mns;
				double zy = (y * 2.0 - (double)sy) * mns;

				// fractal iteration
				double d = 0.0;
				for (int i = 0; i < iterations; i++)
				{
					d = zx * zx + zy * zy;
					if (d < 4.0)                        // 4 mean 2 * 2 (opt fix with dot instead of length
					{
						double tx = zx;                 // tmp x
						zx = zx * zx - zy * zy - ppx;    // julia zx
						zy = 2.0 * tx * zy - py;        // julia zy
					}
				}

				// distance field formula
				double df = dmax(dmin(threshold / (d + 1e-5) + sqrt(d) * log(d), 1.0), 0.0); // clamp df to 0.0 > 1.0
				int artIdx = (int)floor(df * (double)(charsetLength - 1U)); // char from art
				char c = charset[artIdx];
				buffer[t] = c;

				if (x == sx - 1)
					buffer[t] = '\n';
			}
#ifdef _OPENMP
		}
		buffer[bufSize] = '\0';
		system("cls");
		printf("%s", buffer);

		double end = omp_get_wtime();
		sum += end - start;
		count++;
	}
#endif

	sum /= count;
	printf("Mean Work took %f seconds\n", sum);
	free(buffer);

	return 1;
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		printf("julia width height param_x param_y iterations charset\n");
		return 0;
	}

#ifdef _OPENMP
	int threadID = omp_get_thread_num();
	printf("thread id : %i\n", threadID);
	omp_set_num_threads(4);
#endif

	int sx = 100; // size x
	int sy = 75; // size y
	double py = 1.0;
	double px = 0.3;
	int iterations = 10; // fractal iteration count
	char charset[127 + 1];
	memset(charset, 0, 128);
	size_t charsetLen = 0U;

	if (argc > 1) sx = atoi(argv[1]);
	if (argc > 2) sy = atoi(argv[2]);
	if (argc > 3) px = atof(argv[3]);
	if (argc > 4) py = atof(argv[4]);
	if (argc > 5) iterations = atoi(argv[5]);
	if (argc > 6)
	{
		charsetLen = dmin((int)strlen(argv[6]), 127);
		if (charsetLen)
		{
			memcpy(charset, argv[6], charsetLen);
			charset[charsetLen + 1] = '\0';
		}
	}

	int err = printJulia(sx, sy, px, py, iterations, charset, charsetLen);
	if (err == 0)
	{
		printf("error, maybe some parameters are missing or are wrong");
	}

	return err;
}
