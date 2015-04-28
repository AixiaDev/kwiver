/*ckwg +29
 * Copyright 2015 by Kitware, Inc.
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

#include "kwiver_logger.h"
#include "kwiver_logger_factory.h"

namespace kwiver {

class kwiver_logger::impl
{
public:
  impl( logger_ns::kwiver_logger_factory* f, std::string const& node )
    : m_factory( f ),
      m_loggingNode( node )
  { }


  /// Pointer back to our factory.
  logger_ns::kwiver_logger_factory* m_factory;

  /// Hierarchical logging node.
  std::string m_loggingNode;
};


// ------------------------------------------------------------------
kwiver_logger
::kwiver_logger( logger_ns::kwiver_logger_factory* p, const char * const node )
  : m_impl( new kwiver_logger::impl( p, node ) )
{ }


kwiver_logger
::kwiver_logger( logger_ns::kwiver_logger_factory* p, std::string const& node )
  : m_impl( new kwiver_logger::impl( p, node ) )
{ }


kwiver_logger
::~kwiver_logger()
{ }


// ----------------------------------------------------------------
char const* kwiver_logger
::get_level_string(kwiver_logger::log_level_t lev) const
{
  switch (lev)
  {
  case kwiver_logger::LEVEL_TRACE:  return "TRACE";
  case kwiver_logger::LEVEL_DEBUG:  return "DEBUG";
  case kwiver_logger::LEVEL_INFO:   return "INFO";
  case kwiver_logger::LEVEL_WARN:   return "WARN";
  case kwiver_logger::LEVEL_ERROR:  return "ERROR";
  case kwiver_logger::LEVEL_FATAL:  return "FATAL";

  default:           break;
  } // end switch

  return "<unknown>";
}


// ------------------------------------------------------------------
std::string kwiver_logger
::get_name()
{
  return m_impl->m_loggingNode;
}


// ------------------------------------------------------------------
std::string const& kwiver_logger
::get_factory_name() const
{
  return m_impl->m_factory->get_factory_name();
}

} // end namespace
