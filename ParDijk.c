#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#define N 2048
#define SOURCE 1
#define MAXINT 9999999

void dijkstra ( int **graph, int source );

int main(int argc, char **argv) {
int i, j;
int r = N, c = N;
char fn[255];
FILE *fp;
int** graph = (int*)malloc(r * sizeof(int));
   for (i = 0; i < r; i++)
    graph[i] = (int*)malloc(c * sizeof(int));
int threads;
printf("Enter number of threads: ");
scanf("%d", &threads);
omp_set_num_threads(threads);
double time_start, time_end;
struct timeval tv;
struct timezone tz;
gettimeofday(&tv, &tz);
time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
strcpy(fn, "input2048.txt");
fp = fopen(fn, "r");
if ((fp = fopen(fn, "r")) == NULL) {
printf("Can't open input file: %s\n\n", fn);
exit(1);
}

for (i = 0; i < N; i++) {
for (j = 0; j < N; j++) {
fscanf(fp, "%d", &graph[i][j]);
//if (graph[i][j] == 9999999) printf("%4s", "INT");
//else printf("%4d", graph[i][j]);
}
//printf("\n");
}
dijkstra(graph, SOURCE);
gettimeofday(&tv, &tz);
time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
printf("Nodes: %d\n", N);
printf("time cost is %1f\n", time_end - time_start);
return 0;
}

void dijkstra(int **graph, int source){
int visited[N];
int i;
int md;
int distance[N]; 
int mv;
int my_first; /* The first vertex thats stored in one thread locally. */
int my_id; /* ID for threads */
int my_last; /* The last vertex that stores in one thread locally. */
int my_md; /* local minimum distance */
int my_mv; /* local minimum vertex */
int my_step; /* local vertex that is at the minimum distance from the source */
int nth; /* number of threads */

for (i = 0; i < N; i++) {
visited[i] = 0;
distance[i] = graph[source][i];
}
visited[source] = 1;

# pragma omp parallel private (my_first, my_id, my_last, my_md, my_mv, my_step) \
shared (visited, md, distance, mv, nth, graph)
{
my_id = omp_get_thread_num ( );
nth = omp_get_num_threads ( );
my_first = (my_id * N) / nth;
my_last = ((my_id + 1) * N) / nth - 1;
for (my_step = 1; my_step < N; my_step++) {
# pragma omp single
{
md = MAXINT;
mv = -1;
}
int k;
my_md = MAXINT;
my_mv = -1;
 /* Each thread finds the minimum distance vertex of the graph */
 /* my_first aur my_last har thread ka alag hay coz of unique threadIDs */
 /* To har thread different portions/vertex pe kaam kar raha hay */
for (k = my_first; k <= my_last; k++) {
if (!visited[k] && distance[k] < my_md) {
my_md = distance[k];
my_mv = k;
}
/* Har thread ne apna minimum distance nikaal lya */
}
/* 'critical' specifies that code is only to be executed on by one thread at a time, because we need to determine the minimum of all the my_md here. */
# pragma omp critical
{
if (my_md < md) {
md = my_md;
mv = my_mv;
}
/* Har thread k md compare hoay and sab se chota le lya. mark that vertex as well  */
}
/* 'barrier' identifies a synchronization point at which threads in a parallel
* region will wait until all other threads in this section reach the same point. So
* that md and mv have the correct value. */

# pragma omp barrier
# pragma omp single
{
/* Mark all visited Vertices */
if (mv != - 1){
visited[mv] = 1;
}
}
/* Now for the Vertices that are left */
# pragma omp barrier
if (mv != -1){
int j;
for (j = my_first; j <= my_last; j++) {
if (!visited[j] && graph[mv][j] < MAXINT &&
distance[mv] + graph[mv][j] < distance[j]) {
distance[j] = distance[mv] + graph[mv][j];
}
}
}
#pragma omp barrier
}	
}
printf("\nThe distance to vertices from source:\n");
for (i = 0; i < N; i++) {
printf("%d ", distance[i]);
}
printf("\n");
}
