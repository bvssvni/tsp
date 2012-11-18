
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

#include "tsp.h"

func(tsp, point_Distance);

func(tsp, route_Copy);
func(tsp, route_Print);
func(tsp, route_AddCity);
func(tsp, route_RemoveLastCity);
func(tsp, route_New);

func(tsp, map_Shuffle);

double tsp_point_Distance(point a, point b)
{
	double dx = b.x - a.x;
	double dy = b.y - a.y;
	return sqrt(dx * dx + dy * dy);
}

route *tsp_route_New(int n)
{
	int bytes = sizeof(route) + sizeof(int)*n;
	route *r = malloc(bytes);
	memset(r, 0, bytes);
	r->cap = n;
	r->len = 0;
	return r;
}

void tsp_route_Copy(route *dst, route *src)
{
	memcpy(dst, src, sizeof(route) + sizeof(int)*src->len);
}

void tsp_route_AddCity(route *r, int city)
{
	r->cities[r->len++] = city;
}

void tsp_route_RemoveLastCity(route *r)
{
	r->cities[--r->len] = 0;
}

void tsp_route_Print(route *r)
{
	int i;
	for (i = 0; i < r->len; i++) {
		printf("%i, ", r->cities[i]);
	}
	printf("\n");
}

void random_pos(double *x, double *y)
{
	*x = (double)rand() / RAND_MAX;
	*y = (double)rand() / RAND_MAX;
}

map *tsp_map_Random(int n)
{
	map *m = malloc(sizeof(map) + sizeof(point)*n);
	m->len = n;
	
	int i;
	for (i = 0; i < n; i++) {
		point *p = m->cities+i;
		random_pos(&p->x, &p->y);
	}
	
	return m;
}

void tsp_map_Shuffle(map *m)
{
	int a = rand() % m->len;
	int b = rand() % m->len;
	
	point tmp = m->cities[a];
	m->cities[a] = m->cities[b];
	m->cities[b] = tmp;
}

map *tsp_map_Circle(int n)
{
	static const float PI = 3.141592653589793;
	map *m = malloc(sizeof(map) + sizeof(point)*n);
	m->len = n;
	
	int i;
	double angle;
	for (i = 0; i < n; i++) {
		angle = 2 * PI * i / n;
		point *p = m->cities+i;
		p->x = 0.5 + 0.5 * cos(angle);
		p->y = 0.5 + 0.5 * sin(angle);
	}
	
	for (i = 0; i < 100; i++) map_Shuffle(m);
	
	return m;
}

map *tsp_map_Square(void)
{
	int n = 4;
	map *m = malloc(sizeof(map) + sizeof(point)*n);
	m->len = n;
	
	m->cities[0] = (point){0, 0};
	m->cities[1] = (point){1, 0};
	m->cities[2] = (point){0, 1};
	m->cities[3] = (point){1, 1};
	
	return m;
}

void tsp_map_Print(const map *m)
{
	int i;
	for (i = 0; i < m->len; i++) {
		point p = m->cities[i];
		printf("%lg, %lg, \n", p.x, p.y);
	}
}


void compare(route *min, travel *tmin, route *r, travel *tr, int verbose)
{
	static int count = 0;
	
	// We have visited all cities.
	if (tmin->distance == 0 || tr->distance < tmin->distance) {
		*tmin = *tr;
		route_Copy(min, r);
		
		if (verbose) {
			printf("%i: ", ++count);
			route_Print(r);
			fflush(stdout);
		}
	}
}


void search(map *m, int indices[], double weights[], route *min, travel *tmin, route *r, travel *tr, int verbose)
{
	int lastCityIndex = r->cities[r->len-1];
	point lastPos = m->cities[lastCityIndex];
	double dist;
	double oldDist = tr->distance;
	
	if (r->len == m->len) {
		
		dist = point_Distance(lastPos, m->cities[0]);
		tr->distance += dist;
		compare(min, tmin, r, tr, verbose);
		tr->distance = oldDist;
		return;
	}
	
	int i, j;
	for (j = 0; j < m->len; j++) {
		i = indices[lastCityIndex*m->len+j];
		
		if ((tr->visited >> i) & 1) continue;
		
		dist = weights[lastCityIndex*m->len+i];
		
		if (tmin->distance != 0 && oldDist + dist >= tmin->distance) continue;
		
		tr->distance += dist;
		tr->visited |= 1ULL << i;
		route_AddCity(r, i);
		
		search(m, indices, weights, min, tmin, r, tr, verbose);
		
		route_RemoveLastCity(r);
		tr->distance = oldDist;
		tr->visited &= ~(1ULL << i);
	}
}

#define swap(a, b) do {__typeof__(a) tmp = a; a = b; b = tmp;} while (0);

// Sorts indices rather than moving the values.
void bubble_sort_indices(int n, double values[], int res[])
{
	int i, j;
	
	for (i = 0; i < n; i++) {
		for (j = i+1; j < n; j++) {
			if (values[res[i]] > values[res[j]]) {
				swap(res[i], res[j]);
				i--;
				break;
			}
		}
	}
}

int tsp_route_Equal(route *a, route *b)
{
	int n = a->len;
	int i;
	int notsameway = 0;
	for (i = 0; i < n; i++) {
		if (a->cities[i] != b->cities[i]) {notsameway = 1; break;}
	}
	if (!notsameway) return 1;
	
	int j;
	for (i = 0; i < a->len; i++) {
		j = n-i == n ? 0 : n-i;
		if (a->cities[i] != b->cities[j]) return 0;
	}
	
	return 1;
}


route *tsp_route_TSP(map *m, int verbose)
{
	assert(m->len <= 64);
	
	// Sort distances by nodes to use greedy algorithm.
	int n = m->len;
	int sq = n * n;
	
	double *distances = malloc(sq*sizeof(double));
	int *indices = malloc(sq*sizeof(int));
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			distances[i*n+j] = point_Distance(m->cities[i], m->cities[j]);
			indices[i*n+j] = j;
		}
		
		bubble_sort_indices(n, distances+i*n, indices+i*n);
	}
	
	route *r = route_New(n);
	travel tr = {};
	
	route *min = route_New(n);
	travel tmin = {};
	
	r->len = 1;
	r->cities[0] = 0;
	tr.visited = 1;
	
	search(m, indices, distances, min, &tmin, r, &tr, verbose);
	
	free(r);
	free(distances);
	free(indices);
	
	return min;
}

