/*ckwg +29
 * Copyright 2013-2014 by Kitware, Inc.
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
 * \brief Header for \link kwiver::vital::track track \endlink objects
 */

#ifndef VITAL_TRACK_H_
#define VITAL_TRACK_H_

#include "descriptor.h"
#include "feature.h"

#include <vital/vital_export.h>
#include <vital/vital_config.h>
#include <vital/vital_types.h>

#include <vector>
#include <set>
#include <memory>

namespace kwiver {
namespace vital {

/// A representation of a feature track.
/**
 * A track is a sequence of feature points and their descriptors
 * that represent a sequence of observations of the same world location.
 * A track is required to construct a landmark in 3D.
 */
class VITAL_EXPORT track
{
public:
  /// A structure to hold the state of a track on a given frame
  struct track_state
  {
    /// Constructor
    track_state( frame_id_t       frame,
                 feature_sptr     feature,
                 descriptor_sptr  descriptor )
      : frame_id( frame ),
      feat( feature ),
      desc( descriptor ) { }

    /// The frame identifier (i.e. frame number)
    frame_id_t frame_id;
    /// The feature detected on frame \a frame_id
    feature_sptr feat;
    /// The descriptor extracted on frame \a frame_id
    descriptor_sptr desc;
  };

  /// convenience type for the const iterator of the track state vector
  typedef std::vector< track_state >::const_iterator history_const_itr;

  /// Default Constructor
  track();

  /// Copy Constructor
  track( const track& other );

  ~track() VITAL_DEFAULT_DTOR

  /// Construct a track from a single track state
  explicit track( const track_state& ts );

  /// Access the track identification number
  track_id_t id() const { return id_; }

  /// Set the track identification number
  void set_id( track_id_t id ) { id_ = id; }

  /// Access the first frame number covered by this track
  frame_id_t first_frame() const;

  /// Access the last frame number covered by this track
  frame_id_t last_frame() const;

  /// Append a track state.
  /**
   * The added track state must have a frame_id greater than
   * the last frame in the history.
   *
   * \returns true if successful, false not correctly ordered
   * \param state track state to add to this track.
   */
  bool append( const track_state& state );

  /// Append the history contents of another track.
  /**
   * The first state of the input track must contain a frame number
   * greater than the last state of this track.
   *
   * \returns true if successful, false not correctly ordered
   */
  bool append( const track& to_append );

  /// Insert a track state.
  /**
   * The added track state must have a frame_id that is not already
   * present in the track history.
   *
   * \returns true if successful, false if already a state on this frame
   * \param state track state to add to this track.
   */
  bool insert( const track_state& state );

  /// Access a const iterator to the start of the history
  history_const_itr begin() const { return history_.begin(); }

  /// Access a const iterator to the end of the history
  history_const_itr end() const { return history_.end(); }

  /// Find the track state iterator matching \a frame
  /**
   *  \param [in] frame the frame number to access
   *  \return an iterator at the frame if found, or end() if not
   */
  history_const_itr find( frame_id_t frame ) const;

  /// Return the set of all frame IDs covered by this track
  std::set< frame_id_t > all_frame_ids() const;

  /// Return the number of states in the track.
  size_t size() const { return history_.size(); }

  /// Return whether or not this track has any states.
  bool empty() const { return history_.empty(); }


protected:
  /// The ordered array of track states
  std::vector< track_state > history_;
  /// The unique track identification number
  track_id_t id_;
};


/// Shared pointer for general track type
typedef std::shared_ptr< track > track_sptr;

} } // end namespace vital

#endif // VITAL_TRACK_H_
