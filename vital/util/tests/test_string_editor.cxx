/*ckwg +29
 * Copyright 2016 by Kitware, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
 * \brief test util string_editor class
 */
#include <test_common.h>

#include <vital/util/string_editor.h>


#define TEST_ARGS ( )

DECLARE_TEST_MAP();

// ------------------------------------------------------------------
int
main( int argc, char* argv[] )
{
  CHECK_ARGS( 1 );

  testname_t const testname = argv[1];

  RUN_TEST( testname );
}


// ------------------------------------------------------------------
IMPLEMENT_TEST( test_editor )
{
  kwiver::vital::string_editor se;

  std::string str( "first line\n" );
  std::string astr( str );

  // test empty editor
  se.edit( str );

  TEST_EQUAL( "Empty editor", str, astr );

  se.add( new kwiver::vital::edit_operation::shell_comment() );
  se.add( new kwiver::vital::edit_operation::right_trim() );
  se.add( new kwiver::vital::edit_operation::remove_blank_string() );

  se.edit(str);
  // std::cout << "|" << str << "|\n";
  TEST_EQUAL( "typical line", str, "first line" );

  str = "  \n";
  TEST_EQUAL( "absorb blank line", se.edit(str), false );

  str = "trailing spaces        \n";
  se.edit(str);
  //  std::cout << "|" << str << "|\n";
  TEST_EQUAL( "Trailing spaces", str, "trailing spaces" );

  str = "# comment  \n";
  se.edit(str);
  TEST_EQUAL( "absorb comment line", se.edit(str), false );

  str = "foo bar  # trailing comment  \n";
  se.edit(str);
  TEST_EQUAL( "trailing comment", str, "foo bar" );
  // std::cout << "|" << str << "|\n";
}
