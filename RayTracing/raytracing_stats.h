#ifndef _RAYTRACING_STATS_H_
#define _RAYTRACING_STATS_H_

#include <time.h>
#include <limits.h>

#include "vectors.h"

// ====================================================================
// ====================================================================

// This class only contains static variables and static member
// functions so there is no need to call the constructor, destructor
// etc.  It's just a wrapper for the global statistics data.

class RayTracingStats {

public:

  // To be called before computation begins
  static void Initialize(int w,int h, int nx, int ny, int nz,
			 const Vec3f &min, const Vec3f &max) {
    width = w;
    height = h;
    num_x = nx; if (num_x <= 0) num_x = 0;
    num_y = ny; if (num_y <= 0) num_y = 0;
    num_z = nz; if (num_z <= 0) num_z = 0;
    min_bounds = min;
    max_bounds = max;
    start_time = time(NULL);
    num_nonshadow_rays = 0;
    num_shadow_rays = 0;
    num_intersections = 0;
    num_grid_cells_traversed = 0;
  }

  // Call for each ray,  RayTracer::TraceRay()
  static void IncrementNumNonShadowRays() { num_nonshadow_rays++; }

  // Call for each shadow ray
  static void IncrementNumShadowRays() { num_shadow_rays++; }

  // Add this to each Object3D primitive's intersect routine (not group and transform
  static void IncrementNumIntersections() {    
    //if (num_intersections >= ULONG_LONG_MAX)
    //  printf ("warning...  overflow on intersection counter\n");
    num_intersections++; }

  // Call each time a new cell is entered by a ray
  static void IncrementNumGridCellsTraversed() { num_grid_cells_traversed++; }

  // Call when you're all done
  static void PrintStatistics();

private:

  // TIMING VARIABLES
  // use long longs to try to avoid overflow problems,
  // but with really big scenes we will still have problems.
  static int width;
  static int height;
  static int num_x;
  static int num_y;
  static int num_z;
  static Vec3f min_bounds;
  static Vec3f max_bounds;
  static unsigned long long start_time;
  static unsigned long long num_nonshadow_rays;
  static unsigned long long num_shadow_rays;
  static unsigned long long num_intersections;
  static unsigned long long num_grid_cells_traversed;  
};

// ====================================================================
// ====================================================================

#endif
