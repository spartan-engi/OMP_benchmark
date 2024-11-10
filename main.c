#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

typedef struct{float x, y, z;} vec3;

vec3 vec3m(float x, float y, float z)
{
	vec3 res;
	res.x = x;
	res.y = y;
	res.z = z;
	return res;
}
vec3 vec3add(vec3 v1, vec3 v2)
{
	vec3 res;
	res.x = v1.x + v2.x;
	res.y = v1.y + v2.y;
	res.z = v1.z + v2.z;
	return res;
}
vec3 vec3sub(vec3 v1, vec3 v2)
{
	vec3 res;
	res.x = v1.x - v2.x;
	res.y = v1.y - v2.y;
	res.z = v1.z - v2.z;
	return res;
}
vec3 vec3scale(float scalar, vec3 v)
{
	vec3 res;
	res.x = scalar * v.x;
	res.y = scalar * v.y;
	res.z = scalar * v.z;
	return res;
}
vec3 vec3mult(vec3 v1, vec3 v2)
{
	vec3 res;
	res.x = v1.x * v2.x;
	res.y = v1.y * v2.y;
	res.z = v1.z * v2.z;
	return res;
}
float vec3dot(vec3 v1, vec3 v2)
{
	float res = 0;
	res += v1.x * v2.x;
	res += v1.y * v2.y;
	res += v1.z * v2.z;
	return res;
}


vec3 vec3normalize(vec3 v)
{
	float ilength = 1.0 / sqrtf(vec3dot(v, v));
	return vec3scale(ilength, v);
}



float SDF_Sphere(vec3 position, vec3 center, float radius)
{
	vec3 dif = vec3sub(position, center);
	float d = sqrtf(vec3dot(dif, dif));
	d = d - radius;
	return d;
}
float SDF(vec3 position)
{
	float res = SDF_Sphere(position, vec3m(0, 0, 5), 1);
	return res;
}

#define NUM_STEP 128
#define MIN_STEP 0.001
#define MIN_S_STEP 0.0001
#define EPSILON  0.0001

char march(vec3 start, vec3 ray, vec3 light_origin)
{
	vec3 position = start;
	char colision = 0;
	for(int step = 0; step < NUM_STEP; step++)
	{
		float distance = SDF(position);

		if(distance < MIN_STEP)
		{
			colision = 1;
			break;
		}

		position = vec3add(position, vec3scale(distance, ray));
	}

	if(!colision)	return 0;

	//shadow ray
	vec3 shadow_position = position;
	vec3 to_light = vec3sub(light_origin, shadow_position);
	vec3 light_normal = vec3normalize(to_light);
	shadow_position = vec3add(shadow_position, vec3scale(MIN_STEP, light_normal));
	colision = 0;
	for(int step = 0; step < NUM_STEP; step++)
	{
		float distance = SDF(shadow_position);
		
		to_light = vec3sub(light_origin, shadow_position);
		float light_distance = sqrtf(vec3dot(to_light, to_light));
		if(light_distance < distance)
		{
			colision = 1;
			break;
		}

		if((distance < MIN_S_STEP))
		{
			break;
		}

		shadow_position = vec3add(shadow_position, vec3scale(distance, light_normal));
	}

	// in shadow
	if(!colision)	return 0;


	vec3 normal;
	normal.x = SDF(vec3add(position, vec3m(EPSILON,0,0))) - SDF(vec3sub(position, vec3m(EPSILON,0,0)));
	normal.y = SDF(vec3add(position, vec3m(0,EPSILON,0))) - SDF(vec3sub(position, vec3m(0,EPSILON,0)));
	normal.z = SDF(vec3add(position, vec3m(0,0,EPSILON))) - SDF(vec3sub(position, vec3m(0,0,EPSILON)));
	normal = vec3normalize(normal);

	float temp = vec3dot(normal, light_normal);
	if(temp < 0.0) temp = 0.0;
	temp = 8.0*temp + 1.0;
	char res = (char)temp;
	return res;
}

#define SCREENX 158 
#define SCREENY 36
#define ASPECT_RATIO ((float)SCREENX/(float)SCREENY)/2

const char ASCII_PALLET[10] = " .:-=+*#%@";
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
			vec3 camera = vec3m(0,  0, 0);
			vec3 light  = vec3m(8, 16, -8);
			vec3 ray;
			ray.x = ((((float)i)/SCREENX)-0.5)*ASPECT_RATIO;
			ray.y = (0.5-(((float)j)/SCREENY));
			ray.z = 1.0;
			ray = vec3normalize(ray);

			screen[j][i] = march(camera, ray, light);
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
			printf("%c", ASCII_PALLET[screen[j][i]]);
		}
		printf("\n");
	}
	gettimeofday(&end, NULL);
	printf("%ld\n", end.tv_usec-start.tv_usec);

	return 0;
}