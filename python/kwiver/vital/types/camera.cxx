/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
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

#include <vital/io/camera_io.h>
#include <vital/types/camera_perspective.h>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

namespace py = pybind11;
namespace kv = kwiver::vital;

bool
camera_eq(std::shared_ptr<kv::simple_camera_perspective> self,
          std::shared_ptr<kv::simple_camera_perspective> other)
{
  return ((self->get_center() - other->get_center()).isMuchSmallerThan(0.001) &&
          (self->get_center_covar().matrix() == other->get_center_covar().matrix()) &&
          (self->get_rotation().matrix() == other->get_rotation().matrix()));
}

bool
camera_ne(std::shared_ptr<kv::simple_camera_perspective> self,
          std::shared_ptr<kv::simple_camera_perspective> other)
{
  return !camera_eq(self, other);
}

PYBIND11_MODULE(camera, m)
{

  py::class_<kv::simple_camera_perspective,
             std::shared_ptr<kv::simple_camera_perspective> >(m, "Camera")
  .def(py::init<>())
  .def(py::init([](kv::vector_3d const& vec)
                  {
                    kwiver::vital::simple_camera_perspective ret_cam(vec, kwiver::vital::rotation_<double>());
                    return ret_cam;
                  }))
  .def(py::init([](kv::vector_3d const& vec, kv::rotation_d const& rotation)
                  {
                    kwiver::vital::simple_camera_perspective ret_cam(vec, rotation);
                    return ret_cam;
                  }))
  .def(py::init([](kv::vector_3d const& vec, kv::rotation_d const& rotation, kv::simple_camera_intrinsics c_int)
                  {
                    kwiver::vital::simple_camera_perspective ret_cam(vec, rotation, c_int);
                    return ret_cam;
                  }))
  .def("as_matrix", &kv::simple_camera_perspective::as_matrix)
  .def("as_string", [](kv::simple_camera_perspective &self)
                      {
                        std::ostringstream ss;
                        ss << self;
                        return ss.str();
                      })
  .def_static("from_string", [](std::string str)
                      {
                        kv::simple_camera_perspective self;
                        std::istringstream ss(str);
                        ss >> self;
                        return self;
                      })
  .def("clone_look_at", &kv::simple_camera_perspective::clone_look_at,
    py::arg("stare_point"), py::arg("up_direction")=Eigen::Matrix<double,3,1>::UnitZ())
  .def("project", &kv::simple_camera_perspective::project,
    py::arg("pt"))
  .def("depth", &kv::simple_camera_perspective::depth)
  .def("write_krtd_file", [](kv::simple_camera_perspective &self, std::string path)
                                {
                                  kv::write_krtd_file(self, path);
                                })
  .def_static("from_krtd_file", [](std::string path)
                                {
                                  kv::camera_perspective_sptr c(kv::read_krtd_file(path));
                                  return c;
                                })
  .def_property("center", &kv::simple_camera_perspective::center,
                          &kv::simple_camera_perspective::set_center)
  .def_property("covariance", &kv::simple_camera_perspective::get_center_covar,
                              &kv::simple_camera_perspective::set_center_covar)
  .def_property("translation", &kv::simple_camera_perspective::translation,
                               &kv::simple_camera_perspective::set_translation)
  .def_property("rotation", &kv::simple_camera_perspective::get_rotation,
                            &kv::simple_camera_perspective::set_rotation)
  .def("__eq__", &camera_eq,
    py::arg("other"))
  .def("__ne__", &camera_ne,
    py::arg("other"))
  ;

}
