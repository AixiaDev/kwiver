/*ckwg +29
 * Copyright 2013-2016 by Kitware, Inc.
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
 * \brief Implementation of the core match_features_homography algorithm
 */

#include "match_features_homography.h"

#include <iostream>

#include <vital/exceptions/algorithm.h>
#include <vital/types/homography.h>
#include <vital/types/match_set.h>

using namespace kwiver::vital;

namespace kwiver {
namespace arrows {
namespace core {

/// Private implementation class
class match_features_homography::priv
{
public:
  /// Constructor
  priv()
  : inlier_scale(10.0),
    min_required_inlier_count(0),
    min_required_inlier_percent(0.0),
    m_logger( vital::get_logger( "arrows.core.match_features_homography" ))
  {
  }

  priv(const priv& other)
  : inlier_scale(other.inlier_scale),
    min_required_inlier_count(other.min_required_inlier_count),
    min_required_inlier_percent(other.min_required_inlier_percent),
    m_logger( vital::get_logger( "arrows.core.match_features_homography" ))
  {
  }

  // the scale of inlier points
  double inlier_scale;

  // min inlier count required to make any matches
  int min_required_inlier_count;

  // min inlier percent required to make any matches
  double min_required_inlier_percent;
  /// Logger handle
  vital::logger_handle_t m_logger;
};


/// Constructor
match_features_homography
::match_features_homography()
: d_(new priv)
{
}


/// Copy Constructor
match_features_homography
::match_features_homography(const match_features_homography& other)
: d_(new priv(*other.d_)),
  matcher1_(!other.matcher1_ ? algo::match_features_sptr()
                             : other.matcher1_->clone()),
  matcher2_(!other.matcher2_ ? algo::match_features_sptr()
                             : other.matcher2_->clone()),
  h_estimator_(!other.h_estimator_ ? algo::estimate_homography_sptr()
                                   : other.h_estimator_->clone()),
  feature_filter_(!other.feature_filter_ ? algo::filter_features_sptr()
                                         : other.feature_filter_->clone())
{
}


/// Destructor
match_features_homography
::~match_features_homography()
{
}


/// Get this alg's \link vital::config_block configuration block \endlink
vital::config_block_sptr
match_features_homography
::get_configuration() const
{
  vital::config_block_sptr config = algorithm::get_configuration();
  config->set_value("inlier_scale", d_->inlier_scale,
                    "The acceptable error distance (in pixels) between warped "
                    "and measured points to be considered an inlier match.");
  config->set_value("min_required_inlier_count", d_->min_required_inlier_count,
                    "The minimum required inlier point count. If there are less "
                    "than this many inliers, no matches will be output.");
  config->set_value("min_required_inlier_percent", d_->min_required_inlier_percent,
                    "The minimum required percentage of inlier points. If the "
                    "percentage of points considered inliers is less than this "
                    "amount, no matches will be output.");

  // nested algorithm configurations
  vital::algo::estimate_homography::get_nested_algo_configuration("homography_estimator",
                                                     config, h_estimator_);
  vital::algo::match_features::get_nested_algo_configuration("feature_matcher1", config,
                                                      matcher1_);
  vital::algo::match_features::get_nested_algo_configuration("feature_matcher2", config,
                                                      matcher2_);
  vital::algo::filter_features::get_nested_algo_configuration("filter_features", config,
                                                          feature_filter_);

  return config;
}


void
match_features_homography
::set_configuration(vital::config_block_sptr in_config)
{
  // Starting with our generated config_block to ensure that assumed values are present
  // An alternative is to check for key presence before performing a get_value() call.
  vital::config_block_sptr config = this->get_configuration();
  config->merge_config(in_config);

  // Set nested algorithm configurations
  vital::algo::estimate_homography::set_nested_algo_configuration("homography_estimator",
                                                     config, h_estimator_);
  vital::algo::match_features::set_nested_algo_configuration("feature_matcher1", config,
                                                      matcher1_);
  vital::algo::match_features::set_nested_algo_configuration("feature_matcher2", config,
                                                      matcher2_);
  vital::algo::filter_features::set_nested_algo_configuration("filter_features", config,
                                                          feature_filter_);

  // Other parameters
  d_->inlier_scale = config->get_value<double>("inlier_scale");
  d_->min_required_inlier_count = config->get_value<int>("min_required_inlier_count");
  d_->min_required_inlier_percent = config->get_value<double>("min_required_inlier_percent");
}

bool
match_features_homography
::check_configuration(vital::config_block_sptr config) const
{
  bool config_valid = true;
  // this algorithm is optional
  if (config->has_value("filter_features") &&
      config->get_value<std::string>("filter_features") != "" &&
      !vital::algo::filter_features::check_nested_algo_configuration("filter_features", config))
  {
    config_valid = false;
  }
  // this algorithm is optional
  if (config->has_value("feature_matcher2") &&
      config->get_value<std::string>("feature_matcher2") != "" &&
      !vital::algo::match_features::check_nested_algo_configuration("feature_matcher2", config))
  {
    config_valid = false;
  }
  return (
    vital::algo::estimate_homography::check_nested_algo_configuration("homography_estimator", config)
    &&
    vital::algo::match_features::check_nested_algo_configuration("feature_matcher1", config)
    &&
    config_valid
  );
}


/// Match one set of features and corresponding descriptors to another
match_set_sptr
match_features_homography
::match(feature_set_sptr feat1, descriptor_set_sptr desc1,
        feature_set_sptr feat2, descriptor_set_sptr desc2) const
{
  if( !matcher1_ || !h_estimator_ )
  {
    return match_set_sptr();
  }

  // filter features if a filter_features is set
  feature_set_sptr src_feat;
  descriptor_set_sptr src_desc;
  if (feature_filter_.get())
  {
    std::pair<feature_set_sptr, descriptor_set_sptr> ret = feature_filter_->filter(feat1, desc1);
    src_feat = ret.first;
    src_desc = ret.second;
  }
  else
  {
    src_feat = feat1;
    src_desc = desc1;
  }

  // compute the initial matches
  match_set_sptr init_matches = matcher1_->match(src_feat, src_desc, feat2, desc2);

  // estimate a homography from the initial matches
  std::vector<bool> inliers;
  homography_sptr H = h_estimator_->estimate(feat1, feat2, init_matches,
                                             inliers, d_->inlier_scale);
  int inlier_count = static_cast<int>(std::count(inliers.begin(), inliers.end(), true));
  LOG_INFO(d_->m_logger, "inlier ratio: " << inlier_count << "/" << inliers.size());

  // verify matching criteria are met
  if( !inlier_count || inlier_count < d_->min_required_inlier_count ||
      static_cast<double>(inlier_count)/inliers.size() < d_->min_required_inlier_percent )
  {
    return match_set_sptr(new simple_match_set());
  }

  if( !matcher2_ )
  {
    // return the subset of inlier matches
    std::vector<vital::match> m = init_matches->matches();
    std::vector<vital::match> inlier_m;
    for( unsigned int i=0; i<inliers.size(); ++i )
    {
      if( inliers[i] )
      {
        inlier_m.push_back(m[i]);
      }
    }

    return match_set_sptr(new simple_match_set(inlier_m));
  }

  //deep copy and warp the original (non filtered) points
  const std::vector<feature_sptr> &feat1_vec = feat1->features();
  std::vector<feature_sptr> warped_feat1;
  warped_feat1.reserve(feat1_vec.size());
  homography_<double> Hd(*H);
  for (unsigned int i = 0; i < feat1_vec.size(); i++)
  {
    feature_<double> f(*feat1_vec[i]);
    f.set_loc(Hd.map_point(f.get_loc()));
    warped_feat1.push_back(std::make_shared<feature_<double> >(f));
  }

  feature_set_sptr warped_feat1_set =
    std::make_shared<simple_feature_set>(
      simple_feature_set(warped_feat1));

  return matcher2_->match(warped_feat1_set, desc1, feat2, desc2);
}


} // end namespace core
} // end namespace arrows
} // end namespace kwiver
