#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>


float SDF(float position[3])
{
	float res, temp;
	res = 0;
	temp = position[0] - 0.0;
	res += temp*temp;
	temp = position[1] - 0.0;
	res += temp*temp;
	temp = position[2] - 5.0;
	res += temp*temp;
	res = sqrtf(res) - 1;
	return res;
}

#define NUM_STEP 128
#define MIN_STEP 0.001

char march(float ray[3])
{
	float position[3];
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	for(int step = 0; step < NUM_STEP; step++)
	{
		float distance = SDF(position);

		if(distance < MIN_STEP) return 1;

		position[0] += ray[0] * distance;
		position[1] += ray[1] * distance;
		position[2] += ray[2] * distance;
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
			float ray[3];
			ray[0] = ((((float)i)/SCREENX)-0.5)*ASPECT_RATIO;
			ray[1] = (0.5-(((float)j)/SCREENY));
			ray[2] = 1.0;
			float inv = (ray[0]*ray[0])+(ray[1]*ray[1])+(ray[2]*ray[2]);
			inv = 1.0 / sqrtf(inv);
			ray[0] *= inv;
			ray[1] *= inv;
			ray[2] *= inv;

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