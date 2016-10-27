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
 * \brief OCV image_container implementation
 */

#include "image_container.h"

#include <arrows/ocv/mat_image_memory.h>

using namespace kwiver::vital;

namespace kwiver {
namespace arrows {
namespace ocv {

/// Constructor - convert base image container to cv::Mat
image_container
::image_container(const vital::image_container& image_cont)
{
  // testing if image_cont is an ocv image container
  const ocv::image_container* oic =
      dynamic_cast<const ocv::image_container*>(&image_cont);
  if( oic )
  {
    this->data_ = oic->data_;
  }
  else
  {
    this->data_ = vital_to_ocv(image_cont.get_image());
  }
}


/// The size of the image data in bytes
size_t
image_container
::size() const
{
  return data_.rows * data_.step;
}


/// Convert an OpenCV cv::Mat to a VITAL image
image
image_container
::ocv_to_vital(const cv::Mat& img)
{
  image_memory_sptr memory(new mat_image_memory(img));

  return image(memory, img.data + (img.channels() == 3 ? 2 : 0),
               img.cols, img.rows, img.channels(),
               img.elemSize(), img.step, (img.channels() == 3 ? -1 : 1));
}


/// Convert a VITAL image to an OpenCV cv::Mat
cv::Mat
image_container
::vital_to_ocv(const vital::image& img)
{
  // cv::Mat is limited in the image data layouts that it supports.
  // Color channels must be reversed and interleaved (d_step==-1) and the
  // step between columns must equal the number of channels (w_step==depth).
  // If the image does not have these properties we must allocate
  // a new cv::Mat and deep copy the data.  Otherwise, share memory.
  if( ( img.depth() == 1 || ( img.depth() == 3 && img.d_step() == -1 ) ) &&
      img.w_step() == static_cast<ptrdiff_t>(img.depth()) )
  {
    image_memory_sptr memory = img.memory();
    cv::Mat out(static_cast<int>(img.height()), static_cast<int>(img.width()),
                CV_MAKETYPE(CV_8U, static_cast<int>(img.depth())),
                memory->data(), img.h_step());

    // if this VITAL image is already wrapping cv::Mat allocated data,
    // then restore the original cv::Mat reference counter.
    if( mat_image_memory* mat_memory =
          dynamic_cast<mat_image_memory*>(memory.get()) )
    {
      // extract the existing reference counter from the VITAL wrapper
#ifndef KWIVER_HAS_OPENCV_VER_3
      out.refcount = mat_memory->get_ref_counter();
#else
      out.u = mat_memory->get_umatdata();
#endif
      out.addref();
    }
    // TODO use MatAllocator to share memory with image_memory
    return out;
  }

  // allocated a new cv::Mat
  cv::Mat out(static_cast<int>(img.height()), static_cast<int>(img.width()),
              CV_MAKETYPE(CV_8U, static_cast<int>(img.depth())));
  // wrap the new image as a VITAL image (always a shallow copy)
  image new_img = ocv_to_vital(out);
  new_img.copy_from(img);

  return out;
}


/// Extract a cv::Mat from any image container
cv::Mat
image_container_to_ocv_matrix(const vital::image_container& img)
{
  if( const ocv::image_container* c =
          dynamic_cast<const ocv::image_container*>(&img) )
  {
    return c->get_Mat();
  }
  return ocv::image_container::vital_to_ocv(img.get_image());
}

} // end namespace ocv
} // end namespace arrows
} // end namespace kwiver
