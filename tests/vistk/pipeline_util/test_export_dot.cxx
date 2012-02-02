/*ckwg +5
 * Copyright 2011 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include <test_common.h>

#include <vistk/pipeline_util/export_dot.h>
#include <vistk/pipeline_util/export_dot_exception.h>
#include <vistk/pipeline_util/pipe_bakery.h>

#include <vistk/utilities/path.h>

#include <vistk/pipeline/modules.h>

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include <cstdlib>

static std::string const pipe_ext = ".pipe";

static void run_test(std::string const& test_name, vistk::path_t const& pipe_file);

int
main(int argc, char* argv[])
{
  if (argc != 3)
  {
    TEST_ERROR("Expected two arguments");

    return EXIT_FAILURE;
  }

  std::string const test_name = argv[1];
  vistk::path_t const pipe_dir = argv[2];

  vistk::path_t const pipe_file = pipe_dir / (test_name + pipe_ext);

  try
  {
    run_test(test_name, pipe_file);
  }
  catch (std::exception& e)
  {
    TEST_ERROR("Unexpected exception: " << e.what());

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static void test_pipeline_null(vistk::path_t const& pipe_file);
static void test_simple_pipeline(vistk::path_t const& pipe_file);
static void test_simple_group_pipeline(vistk::path_t const& pipe_file);

void
run_test(std::string const& test_name, vistk::path_t const& pipe_file)
{
  if (test_name == "pipeline_null")
  {
    test_pipeline_null(pipe_file);
  }
  else if (test_name == "simple_pipeline")
  {
    test_simple_pipeline(pipe_file);
  }
  else if (test_name == "simple_group_pipeline")
  {
    test_simple_group_pipeline(pipe_file);
  }
  else
  {
    TEST_ERROR("Unknown test: " << test_name);
  }
}

void
test_pipeline_null(vistk::path_t const& /*pipe_file*/)
{
  vistk::pipeline_t pipeline;

  std::ostringstream sstr;

  EXPECT_EXCEPTION(vistk::null_pipeline_export_dot_exception,
                   vistk::export_dot(sstr, pipeline, "(unnamed)"),
                   "exporting a NULL pipeline to dot");
}

void
test_simple_pipeline(vistk::path_t const& pipe_file)
{
  vistk::load_known_modules();

  vistk::pipeline_t const pipeline = vistk::bake_pipe_from_file(pipe_file);

  std::ostringstream sstr;

  vistk::export_dot(sstr, pipeline, "(unnamed)");
}

void
test_simple_group_pipeline(vistk::path_t const& pipe_file)
{
  vistk::load_known_modules();

  vistk::pipeline_t const pipeline = vistk::bake_pipe_from_file(pipe_file);

  std::ostringstream sstr;

  vistk::export_dot(sstr, pipeline, "(unnamed)");
}
