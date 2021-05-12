#pragma once


#include "vcl/vcl.hpp"

// Compute the interpolated position p(t) given a time t and the set of key_positions and key_frame
vcl::vec3 const interpolation(float t, vcl::buffer<vcl::vec3> const& key_positions, vcl::buffer<float> const& key_times);

/** Compute the linear interpolation p(t) between p1 at time t1 and p2 at time t2*/
vcl::vec3 linear_interpolation(float t, float t1, float t2, const vcl::vec3& p1, const vcl::vec3& p2);

/** Compute the cardinal spline interpolation p(t) with the polygon [p0,p1,p2,p3] at time [t0,t1,t2,t3]
*  - Assume t \in [t1,t2] */
vcl::vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, vcl::vec3 const& p0, vcl::vec3 const& p1, vcl::vec3 const& p2, vcl::vec3 const& p3, float K);

/** Find the index k such that intervals[k] < t < intervals[k+1]
* - Assume intervals is a sorted array of N time values
* - Assume t \in [ intervals[0], intervals[N-1] [       */
size_t find_index_of_interval(float t, vcl::buffer<float> const& intervals);
