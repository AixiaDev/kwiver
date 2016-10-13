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

/**
 * \file
 * \brief Common C Interface Utilities
 */

#ifndef VITAL_C_COMMON_H_
#define VITAL_C_COMMON_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#include <vital/bindings/c/vital_c_export.h>


/// Simple string structure
typedef struct {
  size_t length;
  char *str;
} vital_string_t;

/// Allocate a new vital string structure
VITAL_C_EXPORT
vital_string_t* vital_string_new(size_t length, char const* s);

/// Free an allocated string structure
VITAL_C_EXPORT
void vital_string_free( vital_string_t *s );


/// Common function for freeing string lists
VITAL_C_EXPORT
void vital_common_free_string_list( size_t length, char **keys );


/// Other free functions
VITAL_C_EXPORT void vital_free_pointer( void *thing );
VITAL_C_EXPORT void vital_free_double_pointer( size_t length, void **things );


#ifdef __cplusplus
}
#endif

#endif // VITAL_C_COMMON_H_
