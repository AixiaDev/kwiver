/*ckwg +29
 * Copyright 2013-2016 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Test PROJ geo map functionality
 */

#include <test_common.h>

#include <arrows/proj/register_algorithms.h>
#include <arrows/proj/geo_map.h>

// test_common.h required things
#define TEST_ARGS ()
DECLARE_TEST_MAP();

// distance epsilons for centimeter accuracy or better
static const double met_epsilon = 0.01;
static const double deg_epsilon = 0.0000001;

int main(int argc, char** argv)
{
  CHECK_ARGS(1);
  testname_t const testname = argv[1];
  RUN_TEST(testname);
}


IMPLEMENT_TEST(factory)
{
  using namespace kwiver::arrows;
  proj::register_algorithms();
  kwiver::vital::algo::geo_map_sptr gmap = kwiver::vital::algo::geo_map::create("proj");
  if (!gmap)
  {
    TEST_ERROR("Unable to create geo_map algorithm of type proj");
  }
  kwiver::vital::algo::geo_map* gmap_ptr = gmap.get();
  if (typeid(*gmap_ptr) != typeid(proj::geo_map))
  {
    TEST_ERROR("Factory method did not construct the correct type");
  }
}


IMPLEMENT_TEST(latlon_to_utm)
{
  kwiver::arrows::proj::geo_map gm;
  double lat = -17.234908,
         lon = 24.000048,
         easting, northing;
  int zone;
  bool is_northp;

  gm.latlon_to_utm(lat, lon, easting, northing, zone, is_northp);
  printf("ret UTM: %.2fE %.2fN %d %d\n", easting, northing, zone, is_northp);

  double expected_easting = 180954.88,
         expected_northing = -1908018.40;
  int expected_zone = 35;
  bool expected_np = true;
  TEST_NEAR("easting", easting, expected_easting, met_epsilon);
  TEST_NEAR("northing", northing, expected_northing, met_epsilon);
  TEST_EQUAL("zone", zone, expected_zone);
  TEST_EQUAL("is north point", is_northp, expected_np);
}

IMPLEMENT_TEST(utm_to_latlon)
{
  kwiver::arrows::proj::geo_map gm;
  double easting = 180954.88,
         northing = -1908018.40,
         lat, lon,
         ex_lat = -17.234908,
         ex_lon = 24.000048;
  int zone = 35;
  bool is_np = true;

  gm.utm_to_latlon(easting, northing, zone, is_np, lat, lon);

  TEST_NEAR("latitude", lat, ex_lat, deg_epsilon);
  TEST_NEAR("longitude", lon, ex_lon, deg_epsilon);
}

IMPLEMENT_TEST(backprojection)
{
  kwiver::arrows::proj::geo_map gm;
  double orig_lat = -17.234908,
         orig_lon = 24.000048,
         orig_easting = 180954.88,
         orig_northing = -1908018.40;
  int orig_zone = 35;
  bool orig_is_np = true;

  double lat, lon, easting, northing;
  int zone;
  bool is_np;

  // lat/lon back projection
  gm.latlon_to_utm(orig_lat, orig_lon,
                   easting, northing, zone, is_np);
  gm.utm_to_latlon(easting, northing, zone, is_np,
                   lat, lon);

  TEST_NEAR("bp_lat", lat, orig_lat, deg_epsilon);
  TEST_NEAR("bp_lon", lon, orig_lon, deg_epsilon);

  // UTM back projection
  gm.utm_to_latlon(orig_easting, orig_northing, orig_zone, orig_is_np,
                   lat, lon);
  gm.latlon_to_utm(lat, lon,
                   easting, northing, zone, is_np);

  TEST_NEAR("bp_easting", easting, orig_easting, met_epsilon);
  TEST_NEAR("bp_northing", northing, orig_northing, met_epsilon);
  TEST_EQUAL("bp_zone", zone, orig_zone);
  TEST_EQUAL("bp_np", is_np, orig_is_np);
}
