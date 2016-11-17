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

#ifndef VITAL_ALGO_ANALYZE_TRACKS_H_
#define VITAL_ALGO_ANALYZE_TRACKS_H_

#include <vital/vital_config.h>

#include <vital/algo/algorithm.h>
#include <vital/types/image_container.h>
#include <vital/types/track_set.h>

#include <ostream>
#include <memory>

/**
 * \file
 * \brief Header defining abstract \link kwiver::vital::algo::analyze_tracks track
 *        analyzer \endlink algorithm
 */

namespace kwiver {
namespace vital {
namespace algo {

/// Abstract base class for writing out human readable track statistics.
class VITAL_ALGO_EXPORT analyze_tracks
  : public kwiver::vital::algorithm_def<analyze_tracks>
{
public:

  typedef std::ostream stream_t;

  /// Return the name of this algorithm.
  static std::string static_type_name() { return "analyze_tracks"; }

  /// Output various information about the tracks stored in the input set.
  /**
   * \param [in] track_set the tracks to analyze
   * \param [in] stream an output stream to write data onto
   */
  virtual void
  print_info(kwiver::vital::track_set_sptr track_set,
             stream_t& stream = std::cout) const = 0;

protected:
  analyze_tracks();

};

typedef std::shared_ptr<analyze_tracks> analyze_tracks_sptr;

} } } // end namespace

#endif // VITAL_ALGO_ANALYZE_TRACKS_H_
