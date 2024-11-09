#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

typedef struct{float x, y, z;} vec3;

float SDF(vec3 position)
{
	float res, temp;
	res = 0;
	temp = position.x - 0.0;
	res += temp*temp;
	temp = position.y - 0.0;
	res += temp*temp;
	temp = position.z - 5.0;
	res += temp*temp;
	res = sqrtf(res) - 1;
	return res;
}

#define NUM_STEP 128
#define MIN_STEP 0.001

char march(vec3 ray)
{
	vec3 position;
	position.x = 0.0;
	position.y = 0.0;
	position.z = 0.0;
	for(int step = 0; step < NUM_STEP; step++)
	{
		float distance = SDF(position);

		if(distance < MIN_STEP) return 1;

		position.x += ray.x * distance;
		position.y += ray.y * distance;
		position.z += ray.z * distance;
	}
	return 0;
}

#define SCREENX 158 
#define SCREENY 36
#define ASPECT_RATIO ((float)SCREENX/(float)SCREENY)/2.2

char screen[SCREENY][SCREENX];
int main(int argc, char* argv[])
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	#pragma omp parallel for num_threads(1)
	for(int j = 0; j < SCREENY; j++)
	{
		for(int i = 0; i < SCREENX; i++)
		{
			// if((i == 79)&&(j == 18))
			// {
			//     printf("\nhere\n");
			// }
			vec3 ray;
			ray.x = ((((float)i)/SCREENX)-0.5)*ASPECT_RATIO;
			ray.y = (0.5-(((float)j)/SCREENY));
			ray.z = 1.0;
			float inv = (ray.x*ray.x)+(ray.y*ray.y)+(ray.z*ray.z);
			inv = 1.0 / sqrtf(inv);
			ray.x *= inv;
			ray.y *= inv;
			ray.z *= inv;

			screen[j][i] = march(ray);
		}
	}
	gettimeofday(&end, NULL);

	printf("%ld\n", end.tv_usec-start.tv_usec);

	gettimeofday(&start, NULL);
	if(argc > 1) return 0;

	for(int j = 0; j < SCREENY; j++)
	{
		for(int i = 0; i < SCREENX; i++)
		{
			printf("%d", screen[j][i]);
		}
		printf("\n");
	}
	gettimeofday(&end, NULL);
	printf("%ld\n", end.tv_usec-start.tv_usec);

	return 0;
}