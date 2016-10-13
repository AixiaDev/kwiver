/*ckwg +29
 * Copyright 2011-2013 by Kitware, Inc.
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

#include <test_common.h>

#include <vital/config/config_block.h>
#include <vital/vital_foreach.h>

#include <sprokit/pipeline/edge.h>
#include <sprokit/pipeline/modules.h>
#include <sprokit/pipeline/process.h>
#include <sprokit/pipeline/process_exception.h>
#include <sprokit/pipeline/process_registry.h>
#include <sprokit/pipeline/process_registry_exception.h>
#include <sprokit/pipeline/types.h>

#include <boost/make_shared.hpp>

static void test_process(sprokit::process::type_t const& type);

int
main()
{
  try
  {
    sprokit::load_known_modules();
  }
  catch (sprokit::process_type_already_exists_exception const& e)
  {
    TEST_ERROR("Duplicate process names: " << e.what());
  }
  catch (sprokit::pipeline_exception const& e)
  {
    TEST_ERROR("Failed to load modules: " << e.what());
  }
  catch (std::exception const& e)
  {
    TEST_ERROR("Unexpected exception when loading modules: " << e.what());

    return EXIT_FAILURE;
  }

  sprokit::process_registry_t const reg = sprokit::process_registry::self();

  sprokit::process::types_t const types = reg->types();

  VITAL_FOREACH (sprokit::process::type_t const& type, types)
  {
    //@note The adapter processes cause the tests to fail because they
    // behave differently than other processes. This check handles the
    // current set or problems, it is not a universal solution.
    //
    // A better approach is to add an indication in the metadata to
    // indicate a process is suitable for use in tests. There are
    // processes that have undesirable side effects that should not be
    // used here.
    if (type.find( "adapter" ) != std::string::npos)
    {
      continue;
    }

    try
    {
      std::cout << "------------- testing process type: " << type << std::endl;
      test_process(type);
    }
    catch (std::exception const& e)
    {
      TEST_ERROR("Unexpected exception when testing "
                 "type \'" << type << "\': " << e.what());
    }
  }

  return EXIT_SUCCESS;
}

static void test_process_properties(sprokit::process_t const process);
static void test_process_configuration(sprokit::process_t const process);
static void test_process_input_ports(sprokit::process_t const process);
static void test_process_output_ports(sprokit::process_t const process);

static void test_process_invalid_configuration(sprokit::process_t const process);
static void test_process_invalid_input_port(sprokit::process_t const process);
static void test_process_invalid_output_port(sprokit::process_t const process);

void
test_process(sprokit::process::type_t const& type)
{
  static sprokit::process::name_t const expected_name = sprokit::process::name_t("expected_name");

  sprokit::process_registry_t const reg = sprokit::process_registry::self();

  if (reg->description(type).empty())
  {
    TEST_ERROR("The description is empty");
  }

  sprokit::process_t const process = reg->create_process(type, expected_name);

  if (!process)
  {
    TEST_ERROR("Received NULL process (" << type << ")");

    return;
  }

  if (process->name() != expected_name)
  {
    TEST_ERROR("Name (" << process->name() << ") "
               "does not match expected name: " << expected_name);
  }

  if (process->type() != type)
  {
    TEST_ERROR("Type (" << process->type() << ") "
               "does not match registry type: " << type);
  }

  test_process_properties(process);
  test_process_configuration(process);
  test_process_input_ports(process);
  test_process_output_ports(process);

  test_process_invalid_configuration(process);
  test_process_invalid_input_port(process);
  test_process_invalid_output_port(process);
}

void
test_process_properties(sprokit::process_t const process)
{
  sprokit::process::properties_t const consts = process->properties();

  (void)consts;

  /// \todo Test for conflicting properties.
}

void
test_process_configuration(sprokit::process_t const process)
{
  kwiver::vital::config_block_keys_t const keys = process->available_config();

  VITAL_FOREACH (kwiver::vital::config_block_key_t const& key, keys)
  {
    try
    {
      process->config_info(key);
    }
    catch (sprokit::unknown_configuration_value_exception const& e)
    {
      TEST_ERROR("Failed to get a default for "
                 << process->type() << kwiver::vital::config_block::block_sep << key
                 << ": " << e.what());
    }
    catch (std::exception const& e)
    {
      TEST_ERROR("Unexpected exception when querying for default "
                 "(" << process->type() << kwiver::vital::config_block::block_sep
                 << key << "): " << e.what());
    }
  }
}

void
test_process_input_ports(sprokit::process_t const process)
{
  static kwiver::vital::config_block_sptr const config = kwiver::vital::config_block::empty_config();

  sprokit::process::ports_t const ports = process->input_ports();

  VITAL_FOREACH (sprokit::process::port_t const& port, ports)
  {
    sprokit::process::port_info_t info;

    try
    {
      info = process->input_port_info(port);
    }
    catch (sprokit::no_such_port_exception const& e)
    {
      TEST_ERROR("Failed to get a info for input port "
                 << process->type() << "." << port << ": " << e.what());
    }
    catch (std::exception const& e)
    {
      TEST_ERROR("Unexpected exception when querying for input port info "
                 "(" << process->type() << "." << port << "): " << e.what());
    }

    sprokit::process::port_flags_t const& flags = info->flags;

    bool const is_const = (0 != flags.count(sprokit::process::flag_output_const));

    if (is_const)
    {
      TEST_ERROR("Const flag on input port "
                 "(" << process->type() << "." << port << ")");
    }

    bool const is_shared = (0 != flags.count(sprokit::process::flag_output_shared));

    if (is_shared)
    {
      TEST_ERROR("Shared flag on input port "
                 "(" << process->type() << "." << port << ")");
    }

    sprokit::process::port_type_t const& type = info->type;

    bool const is_data_dependent = (type == sprokit::process::type_data_dependent);

    if (is_data_dependent)
    {
      TEST_ERROR("Data-dependent input port "
                 "(" << process->type() << "." << port << ")");
    }

    sprokit::process::port_description_t const& description = info->description;

    if (description.empty())
    {
      TEST_ERROR("Description empty on input port "
                 "(" << process->type() << "." << port << ")");
    }

    sprokit::edge_t edge = boost::make_shared<sprokit::edge>(config);

    process->connect_input_port(port, edge);

    EXPECT_EXCEPTION(sprokit::port_reconnect_exception,
                     process->connect_input_port(port, edge),
                     "connecting to an input port a second time");
  }
}

void
test_process_output_ports(sprokit::process_t const process)
{
  static kwiver::vital::config_block_sptr const config = kwiver::vital::config_block::empty_config();

  sprokit::process::ports_t const ports = process->output_ports();

  VITAL_FOREACH (sprokit::process::port_t const& port, ports)
  {
    sprokit::process::port_info_t info;

    try
    {
      info = process->output_port_info(port);
    }
    catch (sprokit::no_such_port_exception const& e)
    {
      TEST_ERROR("Failed to get a info for output port "
                 << process->type() << "." << port << ": " << e.what());
    }
    catch (std::exception const& e)
    {
      TEST_ERROR("Unexpected exception when querying for output port info "
                 "(" << process->type() << "." << port << "): " << e.what());
    }

    sprokit::process::port_flags_t const& flags = info->flags;

    bool const is_mutable = (0 != flags.count(sprokit::process::flag_input_mutable));

    if (is_mutable)
    {
      TEST_ERROR("Mutable flag on output port "
                 "(" << process->type() << "." << port << ")");
    }

    bool const is_nodep = (0 != flags.count(sprokit::process::flag_input_nodep));

    if (is_nodep)
    {
      TEST_ERROR("No dependency flag on output port "
                 "(" << process->type() << "." << port << ")");
    }

    sprokit::process::port_description_t const& description = info->description;

    if (description.empty())
    {
      TEST_ERROR("Description empty on output port "
                 "(" << process->type() << "." << port << ")");
    }

    sprokit::edge_t edge1 = boost::make_shared<sprokit::edge>(config);
    sprokit::edge_t edge2 = boost::make_shared<sprokit::edge>(config);

    process->connect_output_port(port, edge1);
    process->connect_output_port(port, edge2);
  }
}

void
test_process_invalid_configuration(sprokit::process_t const process)
{
  kwiver::vital::config_block_key_t const non_existent_config = kwiver::vital::config_block_key_t("does_not_exist");

  EXPECT_EXCEPTION(sprokit::unknown_configuration_value_exception,
                   process->config_info(non_existent_config),
                   "requesting the information for a non-existent config");
}

void
test_process_invalid_input_port(sprokit::process_t const process)
{
  static sprokit::process::port_t const non_existent_port = sprokit::process::port_t("does_not_exist");
  static kwiver::vital::config_block_sptr const config = kwiver::vital::config_block::empty_config();

  EXPECT_EXCEPTION(sprokit::no_such_port_exception,
                   process->input_port_info(non_existent_port),
                   "requesting the info for a non-existent input port");

  sprokit::edge_t edge = boost::make_shared<sprokit::edge>(config);

  EXPECT_EXCEPTION(sprokit::no_such_port_exception,
                   process->connect_input_port(non_existent_port, edge),
                   "requesting a connection to a non-existent input port");
}

void
test_process_invalid_output_port(sprokit::process_t const process)
{
  static sprokit::process::port_t const non_existent_port = sprokit::process::port_t("does_not_exist");
  static kwiver::vital::config_block_sptr const config = kwiver::vital::config_block::empty_config();

  // Output ports.
  EXPECT_EXCEPTION(sprokit::no_such_port_exception,
                   process->output_port_info(non_existent_port),
                   "requesting the info for a non-existent output port");

  sprokit::edge_t edge = boost::make_shared<sprokit::edge>(config);

  EXPECT_EXCEPTION(sprokit::no_such_port_exception,
                   process->connect_output_port(non_existent_port, edge),
                   "requesting a connection to a non-existent output port");
}
