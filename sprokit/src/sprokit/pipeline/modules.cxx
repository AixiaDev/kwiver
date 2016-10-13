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

#include "modules.h"

#if defined(_WIN32) || defined(_WIN64)
#include <sprokit/pipeline/module-paths.h>
#endif

#include "utils.h"

#include <vital/logger/logger.h>
#include <vital/vital_foreach.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/operations.hpp>

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <cstddef>
#include <iostream>

/**
 * \file modules.cxx
 *
 * \brief Implementation of module loading.
 */

namespace sprokit
{

namespace
{

#if defined(_WIN32) || defined(_WIN64)
typedef HMODULE library_t;
typedef FARPROC function_t;
#else
typedef void* library_t;
typedef void* function_t;
#endif
typedef void (*load_module_t)();
typedef std::string lib_suffix_t;
typedef std::string function_name_t;

} // end namespace

static void look_in_directory(module_path_t const& directory);
static void load_from_module(module_path_t const& path);
static bool is_separator(module_path_t::value_type ch);

static function_name_t const process_function_name = function_name_t("register_processes");
static function_name_t const scheduler_function_name = function_name_t("register_schedulers");
static module_path_t const default_module_dirs = module_path_t(DEFAULT_MODULE_PATHS);
static envvar_name_t const sprokit_module_envvar = envvar_name_t("SPROKIT_MODULE_PATH");
static lib_suffix_t const library_suffix = lib_suffix_t(LIBRARY_SUFFIX);

// ------------------------------------------------------------------
module_paths_t
get_module_load_path()
{
  module_paths_t module_dirs;

  envvar_value_t const extra_module_dirs = get_envvar(sprokit_module_envvar);

  if (extra_module_dirs)
  {
    boost::split(module_dirs, *extra_module_dirs, is_separator, boost::token_compress_on);
  }

  module_paths_t module_dirs_tmp;
  boost::split(module_dirs_tmp, default_module_dirs, is_separator, boost::token_compress_on);
  module_dirs.insert(module_dirs.end(), module_dirs_tmp.begin(), module_dirs_tmp.end());

  module_paths_t path_list;

  VITAL_FOREACH (module_path_t const& module_dir, module_dirs)
  {
    path_list.push_back( module_dir );
  }

  return path_list;
}


// ------------------------------------------------------------------
void
load_known_modules()
{
  module_paths_t module_dirs = get_module_load_path();

  VITAL_FOREACH (module_path_t const& module_dir, module_dirs)
  {
    look_in_directory(module_dir);
  }
}


// ------------------------------------------------------------------
void
look_in_directory(module_path_t const& directory)
{
  kwiver::vital::logger_handle_t m_logger(kwiver::vital::get_logger("sprokit.modules"));

  if (directory.empty())
  {
    return;
  }

  if (!boost::filesystem::exists(directory))
  {
    LOG_WARN( m_logger, "Directory \"" << directory.c_str() << "\" does not exist." );
    return;
  }

  if (!boost::filesystem::is_directory(directory))
  {
    LOG_WARN( m_logger, "Path \"" << directory.c_str() << "\" is not a directory." );
    return;
  }

  boost::system::error_code ec;
  boost::filesystem::directory_iterator module_dir_iter(directory, ec);

  while (module_dir_iter != boost::filesystem::directory_iterator())
  {
    boost::filesystem::directory_entry const ent = *module_dir_iter;

    ++module_dir_iter;

    // Check if file ends with allowable suffix. If not, skip entry.
    if (!boost::ends_with(ent.path().native(), library_suffix))
    {
      continue;
    }

    if (ent.status().type() != boost::filesystem::regular_file)
    {
      // This may happen a lot, but it is interesting.
      LOG_DEBUG( m_logger, "Directory entry \"" << ent << "\" is not a regular file." );
      continue;
    }

    load_from_module(ent.path().native());
  }
}

// ----------------------------------------------------------------
/**
 * \brief Load single module from shared object / DLL
 *
 * @param path Name of module to load.
 */
void
load_from_module(module_path_t const& path)
{
  kwiver::vital::logger_handle_t m_logger(kwiver::vital::get_logger("sprokit.modules"));

  library_t library = NULL;

#if defined(_WIN32) || defined(_WIN64)
  library = LoadLibraryW(path.c_str());
#else
  dlerror(); // clear error message
  library = dlopen(path.c_str(), RTLD_GLOBAL | RTLD_LAZY);
#endif

  if (!library)
  {
    LOG_ERROR( m_logger, "Unable to load module: " << path.c_str() );
    return;
  }

  LOG_TRACE( m_logger, "Attempting to load module: " << path.c_str() );

  function_t process_function = NULL;
  function_t scheduler_function = NULL;

#if defined(_WIN32) || defined(_WIN64)
  process_function = GetProcAddress(library, process_function_name.c_str());
  scheduler_function = GetProcAddress(library, scheduler_function_name.c_str());
#else
  process_function = dlsym(library, process_function_name.c_str());
  scheduler_function = dlsym(library, scheduler_function_name.c_str());
#endif

#ifdef __GNUC__
  // See https://trac.osgeo.org/qgis/ticket/234#comment:17
  __extension__
#endif
  load_module_t const process_registrar = reinterpret_cast<load_module_t>(process_function);
#ifdef __GNUC__
  // See https://trac.osgeo.org/qgis/ticket/234#comment:17
  __extension__
#endif
  load_module_t const scheduler_registrar = reinterpret_cast<load_module_t>(scheduler_function);

  bool functions_found = false;

  if (process_registrar)
  {
    LOG_INFO( m_logger, "Loading processes from module " << path.c_str() << " loaded" );

    (*process_registrar)();
    functions_found = true;
  }
  if (scheduler_registrar)
  {
    LOG_INFO( m_logger, "Loading schedulers from module " << path.c_str() << " loaded" );

    (*scheduler_registrar)();
    functions_found = true;
  }

  if (!functions_found)
  {
#if defined(_WIN32) || defined(_WIN64)
    int const ret = FreeLibrary(library);

    if (!ret)
    {
      LOG_WARN( m_logger, "Error returned from freeing loaded library." );
    }
#else
    int const ret = dlclose(library);

    if (ret)
    {
      LOG_WARN( m_logger, "Error returned from closing loaded library." );
    }
#endif
  }
}

bool
is_separator(module_path_t::value_type ch)
{
  module_path_t::value_type const separator =
#if defined(_WIN32) || defined(_WIN64)
    ';';
#else
    ':';
#endif

  return (ch == separator);
}

}
