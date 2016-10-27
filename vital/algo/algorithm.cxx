/*ckwg +29
 * Copyright 2014-2015 by Kitware, Inc.
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
 * \brief base algorithm function implementations
 */

#include "algorithm.h"

#include <vital/vital_foreach.h>
#include <vital/logger/logger.h>

#include <sstream>
#include <algorithm>

namespace kwiver {
namespace vital {

// ------------------------------------------------------------------
algorithm
::algorithm()
  : m_logger( kwiver::vital::get_logger( "vital.algorithm" ) )
{
}


// ------------------------------------------------------------------
void
algorithm
::attach_logger( std::string const& name )
{
  m_logger = kwiver::vital::get_logger( name );
}


// ------------------------------------------------------------------
/// Get this alg's \link kwiver::vital::config_block configuration block \endlink
config_block_sptr
algorithm
::get_configuration() const
{
  return config_block::empty_config( this->type_name() );
}


// ------------------------------------------------------------------
algorithm_sptr
algorithm
::create( const std::string&  type_name,
          const std::string&  impl_name )
{
  std::string qualified_name = type_name + ":" + impl_name;
  algorithm_sptr inst = registrar::instance().find< algorithm > ( qualified_name );

  if ( ! inst )
  {
    return inst;
  }
  return inst->base_clone();
}


// ------------------------------------------------------------------
std::vector< std::string >
algorithm
::registered_names( const std::string& type_name )
{
  if ( type_name == "" )
  {
    return registrar::instance().registered_names< algorithm > ();
  }

  std::vector< std::string > type_reg_names;
  const std::string prefix = type_name + ":";
  VITAL_FOREACH( std::string qual_name,
        registrar::instance().registered_names< algorithm > () )
  {
    // if prefix is a prefix of qual_name, add it to the vector
    if ( ( qual_name.length() >= prefix.length() ) &&
         std::equal( prefix.begin(), prefix.end(), qual_name.begin() ) )
    {
      type_reg_names.push_back( qual_name.substr( prefix.length() ) );
    }
  }
  return type_reg_names;
}


// ------------------------------------------------------------------
bool
algorithm
::has_type_name( const std::string& type_name )
{
  std::vector< std::string > valid_names = algorithm::registered_names( type_name );

  return ! valid_names.empty();
}


// ------------------------------------------------------------------
bool
algorithm
::has_impl_name( const std::string& type_name,
                 const std::string& impl_name )
{
  std::vector< std::string > valid_names = algorithm::registered_names( type_name );

  return std::find( valid_names.begin(), valid_names.end(), impl_name ) != valid_names.end();
}


// ------------------------------------------------------------------
/// Helper function for properly getting a nested algorithm's configuration
void
algorithm
::get_nested_algo_configuration( std::string const& type_name,
                                 std::string const& name,
                                 config_block_sptr  config,
                                 algorithm_sptr     nested_algo )
{
  config_block_description_t type_comment =
    "Algorithm to use for '" + name + "'.\n"
    "Must be one of the following options:";
  std::string tmp_d;

  VITAL_FOREACH( std::string reg_name, algorithm::registered_names( type_name ) )
  {
    type_comment += "\n\t- " + reg_name;
    std::string qualified_name = type_name + ":" + reg_name;
    tmp_d = registrar::instance().find< algorithm > ( qualified_name )->description();
    if ( tmp_d != "" )
    {
      type_comment += " :: " + tmp_d;
    }
  }

  if ( nested_algo )
  {
    config->set_value( name + config_block::block_sep + "type",
                       nested_algo->impl_name(),
                       type_comment );
    config->subblock_view( name + config_block::block_sep + nested_algo->impl_name() )
      ->merge_config( nested_algo->get_configuration() );
  }
  else if ( ! config->has_value( name + config_block::block_sep + "type" ) )
  {
    config->set_value( name + config_block::block_sep + "type",
                       "",
                       type_comment );
  }
}


// ------------------------------------------------------------------
/// Helper method for properly setting a nested algorithm's configuration
void
algorithm
::set_nested_algo_configuration( std::string const& type_name,
                                 std::string const& name,
                                 config_block_sptr  config,
                                 algorithm_sptr&    nested_algo )
{
  static  kwiver::vital::logger_handle_t logger = kwiver::vital::get_logger( "vital.algorithm" );
  const std::string type_key = name + config_block::block_sep + "type";

  if ( config->has_value( type_key ) )
  {
   const std::string iname = config->get_value< std::string > ( type_key );
    if ( algorithm::has_impl_name( type_name, iname ) )
    {
      nested_algo = algorithm::create( type_name, iname );
      nested_algo->set_configuration(
        config->subblock_view( name + config_block::block_sep + iname )
                                    );
    }
    else
    {
      LOG_DEBUG( logger, "Could not find implementation \"" << iname
                 << "\" for \"" << type_name <<"\"." );
    }
  }
  else
  {
    LOG_DEBUG( logger, "Config item \"" << type_key
               << "\" not found for \"" << type_name << "\"." );
  }
}

// ------------------------------------------------------------------
/// Helper method for checking that basic nested algorithm configuration is valid
bool
algorithm
::check_nested_algo_configuration( std::string const& type_name,
                                   std::string const& name,
                                   config_block_sptr  config )
{
  static  kwiver::vital::logger_handle_t logger = kwiver::vital::get_logger( "vital.algorithm" );
  const std::string type_key = name + config_block::block_sep + "type";

  if ( ! config->has_value( type_key ) )
  {
    LOG_WARN( logger, "Configuration Failure: missing value: " << type_key );
    return false;
  }

  const std::string iname = config->get_value< std::string > ( type_key );
  if ( ! algorithm::has_impl_name( type_name, iname ) )
  {
    std::stringstream msg;
    msg << "Configuration Failure: invalid option\n"
        << "   " << type_key << " = " << iname << "\n"
        << "   valid options are";
    VITAL_FOREACH( std::string reg_name, algorithm::registered_names( type_name ) )
    {
      msg << "\n      " << reg_name;
    }

    LOG_WARN( logger, msg.str() );
    return false;
  }

  // retursively check the configuration of the sub-algorithm
  const std::string qualified_name = type_name + ":" + iname;
  if ( ! registrar::instance().find< algorithm > ( qualified_name )->check_configuration(
         config->subblock_view( name + config_block::block_sep + iname ) ) )
  {
    LOG_WARN( logger,  "Configuration Failure Backtrace: "
              << name + config_block::block_sep + iname );
    return false;
  }
  return true;
}


}
}     // end namespace
