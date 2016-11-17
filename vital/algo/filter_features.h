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

#ifndef VITAL_ALGO_FILTER_FEATURES_H_
#define VITAL_ALGO_FILTER_FEATURES_H_

#include <vital/vital_config.h>

#include <utility>
#include <vector>
#include <memory>

#include <vital/algo/algorithm.h>
#include <vital/types/feature_set.h>
#include <vital/types/descriptor_set.h>

/**
 * \file
 * \brief Header defining abstract \link kwiver::vital::algo::filter_features
 *        filter features \endlink algorithm
 */

namespace kwiver {
namespace vital {
namespace algo {

/// \brief Abstract base class for feature set filter algorithms.
class VITAL_ALGO_EXPORT filter_features
  : public kwiver::vital::algorithm_def<filter_features>
{
public:

  /// Return the name of this algorithm.
  static std::string static_type_name() { return "filter_features"; }

  /// Filter a feature set and return a subset of the features
  /**
   * The default implementation call the pure virtual function
   * filter(feature_set_sptr feat, std::vector<unsigned int> &indices) const
   * \param [in] input The feature set to filter
   * \returns a filtered version of the feature set (simple_feature_set)
   */
  virtual kwiver::vital::feature_set_sptr
  filter( kwiver::vital::feature_set_sptr input ) const;

  /// Filter a feature_set and its coresponding descriptor_set
  /**
   * The default implementation calls
   * filter(feature_set_sptr feat, std::vector<unsigned int> &indices) const
   * using with \p feat and then uses the resulting \p indices to construct
   * a simple_descriptor_set with the corresponding descriptors.
   * \param [in] feat The feature set to filter
   * \param [in] descr The parallel descriptor set to filter
   * \returns a pair of the filtered features and descriptors
   */
  virtual std::pair<kwiver::vital::feature_set_sptr, kwiver::vital::descriptor_set_sptr>
  filter( kwiver::vital::feature_set_sptr feat, kwiver::vital::descriptor_set_sptr descr) const;

protected:
  filter_features();

  /// Filter a feature set and return a new feature set with a subset of features
  /**
   * \param [in] feat The input feature set
   * \param [in,out] indices The indices into \p feat of the features retained
   * \return a new feature set containing the subset of features noted by \p indices
   */
  virtual kwiver::vital::feature_set_sptr
  filter(kwiver::vital::feature_set_sptr feat, std::vector<unsigned int> &indices) const = 0;

};

/// type definition for shared pointer to a filter_features algorithm
typedef std::shared_ptr<filter_features> filter_features_sptr;

} } } // end namespace

#endif // VITAL_ALGO_FILTER_FEATURES_H_
