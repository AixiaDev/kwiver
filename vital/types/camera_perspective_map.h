/*ckwg +29
 * Copyright 2018 by Kitware, Inc.
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
 * \brief Header file for a map from frame IDs to cameras
 */

#ifndef VITAL_CAMERA_PERSPECTIVE_MAP_H_
#define VITAL_CAMERA_PERSPECTIVE_MAP_H_

#include "camera_map.h"
#include "camera_perspective.h"

#include <vital/vital_types.h>
#include <vital/vital_config.h>

#include <map>
#include <memory>

namespace kwiver {
namespace vital {

class simple_camera_perspective_map;

typedef std::shared_ptr<simple_camera_perspective_map> simple_camera_perspective_map_sptr;

/// A concrete camera_map that simply wraps a std::map.
class simple_camera_perspective_map :
  public camera_map
{
public:
  typedef std::map< frame_id_t, simple_camera_perspective_sptr > map_simple_camera_perspective_t;

  /// Default Constructor
  simple_camera_perspective_map() { }

  /// Constructor from a std::map of cameras
  explicit simple_camera_perspective_map( map_simple_camera_perspective_t const& cameras )
    : data_( cameras ) { }

  /// Return the number of cameras in the map
  virtual size_t size() const { return data_.size(); }

  /// Return a map from integer IDs to camera shared pointers
  virtual map_camera_t cameras() const
  {
    map_camera_t ret_map;
    for (auto const& d : data_)
    {
      ret_map[d.first] = d.second;
    }
    return ret_map;
  }

  simple_camera_perspective_sptr find(frame_id_t fid)
  {
    auto it = data_.find(fid);
    if (it == data_.end())
    {
      return simple_camera_perspective_sptr();
    }
    else
    {
      return it->second;
    }
  }

  void erase(frame_id_t fid)
  {
    data_.erase(fid);
  }

  void insert(frame_id_t fid, simple_camera_perspective_sptr cam)
  {
    data_[fid] = cam;
  }

  void clear()
  {
    data_.clear();
  }

  void set_from_base_cams(camera_map_sptr base_cams)
  {
    auto base_cams_map = base_cams->cameras();
    set_from_base_camera_map(base_cams_map);
  }

  void set_from_base_camera_map(const camera_map::map_camera_t &base_cams_map)
  {
    clear();
    for (auto &c : base_cams_map)
    {
      auto pc = std::dynamic_pointer_cast<simple_camera_perspective>(c.second);
      if (pc)
      {
        data_[c.first] = pc;
      }
    }
  }

  simple_camera_perspective_map_sptr clone()
  {
    auto the_clone = std::make_shared<simple_camera_perspective_map>();
    for (auto &d : data_)
    {
      the_clone->insert(d.first, std::static_pointer_cast<simple_camera_perspective>(d.second->clone()));
    }
    return the_clone;
  }

  /// return a map from integer IDs to simple perspective camera shared pointers
  virtual map_simple_camera_perspective_t simple_perspective_cameras() const { return data_;  }

protected:
  /// The map from integer IDs to camera shared pointers
  map_simple_camera_perspective_t data_;
};

}} // end namespace vital

#endif // VITAL_CAMERA_PERSPECTIVE_MAP_H_
