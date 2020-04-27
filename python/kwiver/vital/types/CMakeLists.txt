project(vital_python_types)

include_directories(${pybind11_INCLUDE_DIR})

set( vital_python_headers
     image.h
     image_container.h
  )

set( vital_python_sources
     image.cxx
     image_container.cxx
     types_module.cxx
   )

kwiver_add_python_library(types
  vital/types
  ${vital_python_headers}
  ${vital_python_sources}
  )

target_link_libraries(python-vital.types-types
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(activity_types
  vital/types
  activity.h
  activity_type.h
  activity.cxx
  activity_type.cxx
  activity_types_module.cxx
  )

target_link_libraries(python-vital.types-activity_types
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(bounding_box
  vital/types
  bounding_box.cxx)
target_link_libraries(python-vital.types-bounding_box
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(camera
  vital/types
  camera.cxx)
target_link_libraries(python-vital.types-camera
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(camera_intrinsics
  vital/types
  camera_intrinsics.cxx)
target_link_libraries(python-vital.types-camera_intrinsics
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(camera_map
  vital/types
  camera_map.cxx)
target_link_libraries(python-vital.types-camera_map
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(color
  vital/types
  color.cxx)
target_link_libraries(python-vital.types-color
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(covariance
  vital/types
  covariance.cxx)
target_link_libraries(python-vital.types-covariance
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(database_query
  vital/types
  database_query.cxx)
target_link_libraries(python-vital.types-database_query
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(descriptor
  vital/types
  descriptor.cxx)
target_link_libraries(python-vital.types-descriptor
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(descriptor_request
  vital/types
  descriptor_request.cxx)
target_link_libraries(python-vital.types-descriptor_request
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(descriptor_set
  vital/types
  descriptor_set.cxx)
target_link_libraries(python-vital.types-descriptor_set
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(detected_object
  vital/types
  detected_object.cxx)
target_link_libraries(python-vital.types-detected_object
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(detected_object_set
  vital/types
  detected_object_set.cxx)
target_link_libraries(python-vital.types-detected_object_set
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(detected_object_type
  vital/types
  detected_object_type.cxx)
target_link_libraries(python-vital.types-detected_object_type
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(feature
  vital/types
  feature.cxx)
target_link_libraries(python-vital.types-feature
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(feature_set
  vital/types
  feature_set.cxx)
target_link_libraries(python-vital.types-feature_set
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(fundamental_matrix
  vital/types
  fundamental_matrix.cxx)
target_link_libraries(python-vital.types-fundamental_matrix
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(geodesy
  vital/types
  geodesy.cxx)
target_link_libraries(python-vital.types-geodesy
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(geo_MGRS
  vital/types
  geo_MGRS.cxx)
target_link_libraries(python-vital.types-geo_MGRS
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(geo_covariance
  vital/types
  geo_covariance.cxx)
target_link_libraries(python-vital.types-geo_covariance
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(geo_point
  vital/types
  geo_point.cxx)
target_link_libraries(python-vital.types-geo_point
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(geo_polygon
  vital/types
  geo_polygon.cxx)
target_link_libraries(python-vital.types-geo_polygon
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(homography
  vital/types
  homography.cxx)
target_link_libraries(python-vital.types-homography
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(homography_f2f
  vital/types
  homography_f2f.cxx)
target_link_libraries(python-vital.types-homography_f2f
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(homography_f2w
  vital/types
  homography_f2w.cxx)
target_link_libraries(python-vital.types-homography_f2w
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(iqr_feedback
  vital/types
  iqr_feedback.cxx)
target_link_libraries(python-vital.types-iqr_feedback
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(landmark
  vital/types
  landmark.cxx)
target_link_libraries(python-vital.types-landmark
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(local_cartesian
  vital/types
  local_cartesian.cxx)
target_link_libraries(python-vital.types-local_cartesian
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(point
  vital/types
  point.cxx)
target_link_libraries(python-vital.types-point
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(polygon
  vital/types
  polygon.cxx)
target_link_libraries(python-vital.types-polygon
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(query_result
  vital/types
  query_result.cxx)
target_link_libraries(python-vital.types-query_result
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(rotation
  vital/types
  rotation.cxx)
target_link_libraries(python-vital.types-rotation
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(similarity
  vital/types
  similarity.cxx)
target_link_libraries(python-vital.types-similarity
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(timestamp
  vital/types
  timestamp.cxx)
target_link_libraries(python-vital.types-timestamp
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(track
  vital/types
  track.cxx)
target_link_libraries(python-vital.types-track
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(track_descriptor
  vital/types
  track_descriptor.cxx)
target_link_libraries(python-vital.types-track_descriptor
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(track_interval
  vital/types
  track_interval.cxx)
target_link_libraries(python-vital.types-track_interval
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(track_set
  vital/types
  track_set.cxx)
target_link_libraries(python-vital.types-track_set
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(transform_2d
  vital/types
  transform_2d.cxx)
target_link_libraries(python-vital.types-transform_2d
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(uid
  vital/types
  uid.cxx)
target_link_libraries(python-vital.types-uid
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

kwiver_add_python_library(object_track_set
  vital/types
  object_track_set.cxx)
target_link_libraries(python-vital.types-object_track_set
  PUBLIC ${PYTHON_LIBRARIES}
         vital)

if(NOT SKBUILD)
  kwiver_create_python_init(vital/types
    types
    bounding_box
    camera
    camera_intrinsics
    camera_map
    color
    covariance
    database_query
    descriptor
    descriptor_request
    descriptor_set
    detected_object_type
    detected_object
    detected_object_set
    feature
    feature_set
    fundamental_matrix
    geodesy
    geo_covariance
    geo_MGRS
    geo_polygon
    geo_point
    homography
    homography_f2w
    iqr_feedback
    landmark
    local_cartesian
    point
    polygon
    query_result
    rotation
    similarity
    timestamp
    track
    track_descriptor
    track_interval
    track_set
    transform_2d
    uid
    object_track_set
    activity_types
  )
endif()
