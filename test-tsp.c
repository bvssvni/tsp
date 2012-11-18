#if 0
#!/bin/bash
#Compile and run with "bash test-tsp.c"
gcc -o test-tsp test-tsp.c tsp.c -Wall -Wfatal-errors -O3
if [ "$?" = "0" ]; then
	time ./test-tsp
fi
exit
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "tsp.h"

func(tsp, point_Distance);

func(tsp, route_New);
func(tsp, route_Copy);
func(tsp, route_AddCity);
func(tsp, route_RemoveLastCity);
func(tsp, route_Print);
func(tsp, route_TSP);

func(tsp, map_Random);
func(tsp, map_Shuffle);
func(tsp, map_Circle);
func(tsp, map_Print);

void test_random_map(void)
{
	srand(4309093);
	map *m = map_Random(3);
	map_Print(m);
	free(m);
}

void test_point_Distance(void)
{
	point a = {0, 4};
	point b = {3, 0};
	assert(point_Distance(a, b) == 5);
}

void test_route_Copy_1(void)
{
	route *a = route_New(3);
	route *b = route_New(3);
	
	assert(a->len == 0);
	
	route_AddCity(b, 1);
	route_AddCity(b, 2);
	route_AddCity(b, 3);
	
	route_Copy(a, b);
	
	assert(a->cities[0] == 1);
	assert(a->cities[1] == 2);
	assert(a->cities[2] == 3);
	
	free(a);
	free(b);
}

void test_search_1(void)
{
	srand(time(NULL));
	// srand(239092);
	map *m = map_Random(15);
	
	map_Print(m);
	
	int verbose = 1;
	route *r = route_TSP(m, verbose);
	
	route_Print(r);
	
	free(r);
	free(m);
}

int main(int argc, char *argv[])
{
	test_point_Distance();
	test_route_Copy_1();
	
	test_search_1();
	
	return 0;
}
