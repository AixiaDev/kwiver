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
* \brief Implementation of kwiver::arrows::sfm_utils utiltiy functions for
* structure from motion.
*/

#include "sfm_utils.h"

#include <vital/vital_types.h>
#include <vital/types/feature_track_set.h>
#include <arrows/core/metrics.h>

using namespace kwiver::vital;

namespace kwiver {
namespace arrows {
namespace core {

/// Calculate fraction of each image that is covered by landmark projections
frame_coverage_vec
image_coverages(
  std::vector<track_sptr> const& trks,
  kwiver::vital::landmark_map::map_landmark_t const& lms,
  camera_map::map_camera_t const& cams )
{
  const int mask_w(16);
  const int mask_h(16);
  typedef Eigen::Matrix<int, mask_h, mask_w> vis_mask;
  typedef std::map<frame_id_t, vis_mask> frame_map_t;
  frame_map_t frame_masks;
  //calculate feature distribution masks for each candidate image.

  struct im_dims {
    int w;
    int h;
  };

  //Get the dimension of each image.  Each image may be a different size.
  std::map<frame_id_t,im_dims> frame_dims;

  for (auto cam : cams)
  {
    if (cam.second)
    {
      auto simp_cam = std::static_pointer_cast<simple_camera_perspective>(cam.second);
      im_dims dims;
      //this assumes the principal point is at the center of the image.
      dims.w = int(simp_cam->intrinsics()->principal_point().x() * 2.0);
      dims.h = int(simp_cam->intrinsics()->principal_point().y() * 2.0);
      frame_dims[cam.first] = dims;
    }
  }

  for (const track_sptr& t : trks)
  {
    if (lms.find(t->id()) != lms.end())
    {
      for (auto ts : *t)
      {
        auto fid = ts->frame();
        auto fd_it = frame_dims.find(fid);
        if (fd_it == frame_dims.end())
        {
          continue;
        }

        feature_track_state_sptr fts =
          std::dynamic_pointer_cast<feature_track_state>(ts);
        if (!fts || !fts->feature)
        {
          continue;
        }

        if (!fts->inlier)
        {
          continue;
        }

        //calculate the mask image location
        vector_2d x = fts->feature->loc();
        int mask_row = int(std::max<float>(0.0f,
          std::min<float>(mask_h*float(x.y()) / fd_it->second.h, mask_h - 1)));
        int mask_col = int(std::max<float>(0.0f,
          std::min<float>(mask_w*float(x.x()) / fd_it->second.w, mask_w - 1)));

        auto fm_it = frame_masks.find(fid);
        if (fm_it == frame_masks.end())
        {
          vis_mask msk;
          msk.setZero();
          msk(mask_row, mask_col) = 1;
          frame_masks.insert(std::pair<frame_id_t, vis_mask>(fid, msk));
        }
        else
        {
          fm_it->second(mask_row, mask_col) = 1;
        }
      }
    }
  }

  //ok, now we have calculated all the masks.
  frame_coverage_vec ret;
  for (auto fid : frame_dims)
  {
    auto fm_it = frame_masks.find(fid.first);
    if (fm_it == frame_masks.end())
    {
      ret.push_back(coverage_pair(fid.first, 0.0f));
    }
    else
    {
      float coverage = float(fm_it->second.sum()) / float(mask_w*mask_h);
      ret.push_back(coverage_pair(fid.first, coverage));
    }
  }
  return ret;
}

/// remove landmarks with IDs in the set
void
remove_landmarks(const std::set<track_id_t>& to_remove,
  landmark_map::map_landmark_t& lms)
{
  for (const track_id_t& tid : to_remove)
  {
    auto lm_it = lms.find(tid);
    if (lm_it != lms.end())
    {
      lms.erase(lm_it);
    }
  }
}


/// find connected components of cameras
camera_components
connected_camera_components(
  camera_map::map_camera_t const& cams,
  landmark_map::map_landmark_t const& lms,
  feature_track_set_sptr tracks)
{
  auto trks = tracks->tracks();

  camera_components comps;

  for (const track_sptr& t : trks)
  {
    auto lmi = lms.find(t->id());
    if (lmi == lms.end() || !lmi->second)
    {
      // no landmark corresponding to this track
      continue;
    }

    //ok this track has an associated landmark
    const landmark& lm = *lmi->second;

    std::unordered_set<frame_id_t> cam_clique;
    for (auto ts : *t)
    {
      auto fts = std::static_pointer_cast<feature_track_state>(ts);
      if (!fts || !fts->feature)
      {
        // no feature for this track state.
        continue;
      }
      const feature& feat = *fts->feature;
      if (!fts->inlier)
      {
        //outliers don't connect cameras
        continue;
      }
      auto ci = cams.find(ts->frame());
      if (ci == cams.end() || !ci->second)
      {
        // no camera corresponding to this track state
        continue;
      }
      cam_clique.insert(ci->first);
    }
    if (cam_clique.empty())
    {
      continue; // nothing to do if no cameras found viewing this track.
    }

    //which of the existing cliques does cam_clique overlap with?
    std::vector<int> overlapping_comps;
    for (int comp_id = 0; comp_id < comps.size(); ++comp_id)
    {
      auto &cur_comp = comps[comp_id];
      for (auto cn : cam_clique)
      {
        if (cur_comp.find(cn) != cur_comp.end())
        {
          overlapping_comps.push_back(comp_id);
          break; //go onto the next component
        }
      }
    }
    if (overlapping_comps.empty())
    {
      //make a new component
      comps.push_back(cam_clique);
    }
    else
    {
      auto &final_comp = comps[overlapping_comps[0]];
      //add all cameras in cam_clique to final comp
      for (auto cn : cam_clique)
      {
        final_comp.insert(cn);
      }
      //merge all other overlapping components into final_comp
      for (int oc = 1; oc < overlapping_comps.size(); ++oc)
      {
       auto &merged_comp = comps[overlapping_comps[oc]];
        final_comp.insert(merged_comp.begin(), merged_comp.end());
      }
      //remove all merged comps except for final_comp
      for (auto oc_it = overlapping_comps.rbegin();
        oc_it != overlapping_comps.rend(); ++oc_it)
      {
        int comp_idx = *oc_it;
        if (comp_idx == overlapping_comps[0])
        {
          continue;  // we don't erase the final comp which is the first
                     // overlapping comp
        }
        comps.erase(comps.begin() + comp_idx);
      }
    }
  }
  return comps;
}

/// Detect underconstrained landmarks.
std::set<landmark_id_t>
detect_bad_landmarks(
  camera_map::map_camera_t const& cams,
  landmark_map::map_landmark_t const& lms,
  feature_track_set_sptr tracks,
  double triang_cos_ang_thresh,
  double error_tol)
{

  double stdev_bound = 3;

  //returns a set of un-constrained landmarks to be removed from the solution
  kwiver::vital::logger_handle_t logger(kwiver::vital::get_logger("arrows.core.sfm_utils"));
  std::set<landmark_id_t> landmarks_to_remove;

  double ets = error_tol * error_tol;

  auto trks = tracks->tracks();

  size_t num_lm_removed_bad_angle = 0;
  size_t num_lm_found_from_tracks = 0;
  size_t num_unconstrained_landmarks_found = 0;

  bool do_statistical_filter = false;
  if (do_statistical_filter)
  {
    vector_3d mean_lm;
    mean_lm.setZero();
    double n = 0;
    for (auto const &lm : lms)
    {
      n += 1.0;
      mean_lm = (1 / n) * lm.second->loc() + ((n - 1) / n)* mean_lm;
    }
    vector_3d var_lm;
    var_lm.setZero();
    double weight = 1.0 / (n - 1.0);
    for (auto const &lm : lms)
    {
      auto diff = lm.second->loc() - mean_lm;
      var_lm += weight * diff.cwiseProduct(diff);
    }
    auto inv_stdev_lm = var_lm.cwiseSqrt().cwiseInverse();

    for (auto const& lm : lms)
    {
      vector_3d num_stdev = ((lm.second->loc() - mean_lm).cwiseProduct(inv_stdev_lm)).cwiseAbs();
      if (num_stdev.maxCoeff() > stdev_bound)
      {
        landmarks_to_remove.insert(lm.first);
      }
    }
  }

  std::vector<double> depths;
  for (auto const &lm_it: lms)
  {
    if (landmarks_to_remove.find(lm_it.first) != landmarks_to_remove.end())
    {
      //already removed, no need to process further.
      continue;
    }

    //ok this track has an associated landmark

    const auto lm = lm_it.second;
    auto t_id = lm_it.first;

    std::vector<kwiver::vital::simple_camera_perspective> observing_cams;

    auto t = tracks->get_track(t_id);
    if (!t)
    {
      continue;
    }

    for (auto ts : *t)
    {
      auto fts = std::static_pointer_cast<feature_track_state>(ts);
      if (!fts || !fts->feature)
      {
        // no feature for this track state.
        continue;
      }
      const feature& feat = *fts->feature;
      auto ci = cams.find(ts->frame());
      if (ci == cams.end() || !ci->second)
      {
        // no camera corresponding to this track state
        continue;
      }
      const auto cam = std::static_pointer_cast<simple_camera_perspective>(ci->second);

      auto d = cam->depth(lm->loc());
      if (d <= 0)
      {
        fts->inlier = false;
        continue;
      }

      double sq_err = kwiver::arrows::reprojection_error_sqr(*cam, *lm, feat);
      if (sq_err <= ets || error_tol < 0)
      {
        observing_cams.push_back(*cam);
        fts->inlier = true;
        depths.push_back(d);
      }
      else
      {
        fts->inlier = false;
      }
    }

    bool bad_ang = false;
    if (observing_cams.size() < 2)
    {
      ++num_unconstrained_landmarks_found;
      landmarks_to_remove.insert(lm_it.first);
    }
    else
    {
      double cos_ang =
        kwiver::arrows::bundle_angle_max(observing_cams, lm->loc());

      bad_ang = cos_ang > triang_cos_ang_thresh;
      if (bad_ang)
      {
        ++num_lm_removed_bad_angle;
        landmarks_to_remove.insert(lm_it.first);
      }
    }
  }

  size_t num_far_lm_removed = 0;
  std::sort(depths.begin(), depths.end());
  double depth_thresh = -1;
  if (!depths.empty())
  {
    depth_thresh = depths[depths.size()*0.5] * 10;
    for (auto const &lm_it : lms)
    {
      if (landmarks_to_remove.find(lm_it.first) != landmarks_to_remove.end())
      {
        //already removed, no need to process further.
        continue;
      }

      //ok this track has an associated landmark

      const auto lm = lm_it.second;
      auto t_id = lm_it.first;

      std::vector<kwiver::vital::simple_camera_perspective> observing_cams;
      bool lm_too_far = false;

      auto t = tracks->get_track(t_id);
      if (!t)
      {
        continue;
      }
      for (auto ts : *t)
      {
        auto fts = std::static_pointer_cast<feature_track_state>(ts);
        if (!fts || !fts->feature)
        {
          // no feature for this track state.
          continue;
        }
        const feature& feat = *fts->feature;
        auto ci = cams.find(ts->frame());
        if (ci == cams.end() || !ci->second)
        {
          // no camera corresponding to this track state
          continue;
        }
        const auto cam = std::static_pointer_cast<simple_camera_perspective>(ci->second);
        auto d = cam->depth(lm->loc());
        if (d > depth_thresh)
        {
          landmarks_to_remove.insert(lm_it.first);
          ++num_far_lm_removed;
          lm_too_far = true;
          break;
        }
      }
      if (lm_too_far)
      {
        break;
      }
    }
  }

  LOG_DEBUG(logger, "num landmarks " << lms.size() << " num unconstrained " <<
    num_unconstrained_landmarks_found << " found from tracks " <<
    num_lm_found_from_tracks << " removed bad angle " <<
    num_lm_removed_bad_angle << " removed too far " <<
    num_far_lm_removed);

  return landmarks_to_remove;
}

/// detect bad cameras in sfm solution
std::set<frame_id_t>
detect_bad_cameras(
  camera_map::map_camera_t const& cams,
  landmark_map::map_landmark_t const& lms,
  feature_track_set_sptr tracks,
  float coverage_thresh)
{
  std::set<frame_id_t> rem_frames;

  frame_coverage_vec fc = image_coverages(tracks->tracks(), lms, cams);

  for (auto cov : fc)
  {
    if (cov.second < coverage_thresh)
    {
      rem_frames.insert(cov.first);
    }
  }
  return rem_frames;
}

/// clean structure from motion solution
void
clean_cameras_and_landmarks(
  vital::simple_camera_perspective_map& cams_persp,
  landmark_map::map_landmark_t& lms,
  feature_track_set_sptr tracks,
  double triang_cos_ang_thresh,
  std::vector<frame_id_t> &removed_cams,
  const std::set<vital::frame_id_t> &active_cams,
  const std::set<vital::landmark_id_t> &active_lms,
  float image_coverage_threshold,
  double error_tol)
{

  auto cams = cams_persp.cameras();

  landmark_map::map_landmark_t det_lms;
  if (active_lms.empty())
  {
    det_lms = lms;
  }
  else
  {
    for (auto lm_id : active_lms)
    {
      auto it = lms.find(lm_id);
      if (it == lms.end())
      {
        continue;
      }
      det_lms[lm_id] = it->second;
    }
  }

  camera_map::map_camera_t det_cams;
  if (active_cams.empty())
  {
    det_cams = cams;
  }
  else
  {
    for (auto cam_id : active_cams)
    {
      auto it = cams.find(cam_id);
      if (it == cams.end())
      {
        continue;
      }
      det_cams[cam_id] = it->second;
    }
  }



  kwiver::vital::logger_handle_t logger(kwiver::vital::get_logger("arrows.core.sfm_utils"));

  removed_cams.clear();
  //loop until no changes are done to further clean up the solution
  bool keep_cleaning = true;
  while (keep_cleaning)
  {
    keep_cleaning = false;
    std::set<track_id_t> lm_to_remove =
      detect_bad_landmarks(cams, det_lms, tracks, triang_cos_ang_thresh, error_tol);

    if (!lm_to_remove.empty())
    {
      keep_cleaning = true;
      if (logger)
      {
        LOG_DEBUG(logger, "removing " << lm_to_remove.size() <<
          " under constrained landmarks");
      }
    }
    remove_landmarks(lm_to_remove, lms);
    remove_landmarks(lm_to_remove, det_lms);

    std::set<frame_id_t> cams_to_remove =
      detect_bad_cameras(det_cams, det_lms, tracks, image_coverage_threshold);

    for (auto frame_id : cams_to_remove)
    {
      cams_persp.erase(frame_id);
      det_cams[frame_id] = nullptr;
      removed_cams.push_back(frame_id);
      LOG_DEBUG(logger, "removing camera " << frame_id);
    }
  }
}
}
}
}
