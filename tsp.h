/*
 Tsp - Accurate Traveling Salesman Problem solver in C for up to 64 cities.
 BSD license.
 by Sven Nilsen, 2012
 http://www.cutoutpro.com
 
 Version: 0.000
 Angular degrees version notation
 http://isprogrammingeasy.blogspot.no/2012/08/angular-degrees-versioning-notation.html
 
 */
/*
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 */

/*
 
 TSP - Traveling Salesman Problem Solver
 
 The point of this library is to have working example code for other projects.
 
 This an greedy algorithm that solves the TSP accurately.
 It does not work for cities above 64, because uint64_t is used as buffer
 to keep track of which cities are visited.
 
 The advantage of using 64 bit "history" is one can add constraints
 in form of bitmasks and use Boolean Algebra.
 Such modifications require changing the code.
 
 The weights are sorted using 2D index map before the search.
 The 2D index map allows the weight matrix to be sorted by row while
 still not moving the weights around.
 Bubble sort is used since the sorting time is insignificant compared
 to the time of searching.
 
 Weights are calculated from tsp_point_t structures, which together
 with tsp_point_Distance can be modified to fit other dimensions.
 
 To test the algoritm, a special case of 15 cities was chosen because of
 the appropriate running time, not too slow or too fast.
 
 Greedy 15 cities (OSX, Intel Dual-Core Duo 2.8 GHz)
 
 [0m7.267s,0m5.218s,0m3.348s,0m2.691s,0m6.628s,0m7.546s,0m2.447s,0m5.817s, 0m19.831s,1m8.839s,0m16.419s,0m9.295s,0m8.944s,0m9.849s,0m22.368s,0m34.774s, 0m6.360s,0m17.358s,0m39.259s,0m7.528s,0m7.251s,0m4.361s,0m19.342s,0m22.227s, 0m5.322s,0m2.772s,0m1.729s,0m4.488s,0m5.289s,0m11.298s,1m3.836s,0m3.349s, 0m30.092s,0m15.774s,0m4.678s,1m4.463s,0m7.954s,0m13.670s,0m55.830s,0m11.461s, 0m34.180s,0m4.629s,0m7.947s,0m16.564s,0m10.044s,0m1.690s,0m1.577s,0m3.167s, 0m14.915s,0m20.391s,0m7.929s,0m9.789s,0m6.115s,0m8.148s,0m32.121s,0m7.986s]
 
 15.074357142857139 avg
 1.577 min
 68.839 max
 
 In comparison, without sorting the weights the solving of 15 cities takes
 180 seconds in worst case (by experiment).
 
 */

#ifndef TSP_GUARD
#define TSP_GUARD

typedef struct tsp_point point;
typedef struct tsp_route route;
typedef struct tsp_map map;
typedef struct tsp_travel travel;

typedef struct tsp_point
{
	double x, y;
} tsp_point_t;

typedef struct tsp_map
{
	int len;
	point cities[];
} tsp_map_t;

typedef struct tsp_route
{
	int len;
	int cap;
	int cities[];
} tsp_route_t;

typedef struct tsp_travel
{
	int depth;
	uint64_t visited;
	double distance;
} tsp_travel_t;

double tsp_point_Distance(point a, point b);

route *tsp_route_New(int n);
void tsp_route_Copy(route *dst, route *src);
void tsp_route_AddCity(route *r, int city);
void tsp_route_RemoveLastCity(route *r);
void tsp_route_Print(route *r);
int tsp_route_Equal(route *a, route *b);
route *tsp_route_TSP(map *m, int verbose);

map *tsp_map_Random(int n);
void tsp_map_Shuffle(map *m);
map *tsp_map_Circle(int n);
map *tsp_map_Square(void);
void tsp_map_Print(const map *m);


#endif

// Paste this on end of header to support namespace import.
#ifndef FUNC_GUARD
#define FUNC_GUARD
#define func(ns, n) static __typeof__(ns##_##n) * const n = ns##_##n
#endif