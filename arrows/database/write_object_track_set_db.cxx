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
 * \brief Implementation of detected object set csv output
 */

#include "write_object_track_set_db.h"

#include <cppdb/frontend.h>

#include <time.h>

namespace kwiver {
namespace arrows {
namespace database {

// -------------------------------------------------------------------------------
class write_object_track_set_db::priv
{
public:
  priv( write_object_track_set_db* parent)
    : m_parent( parent )
    , m_logger( kwiver::vital::get_logger( "write_object_track_set_db" ) )
  { }

  ~priv() { }

  write_object_track_set_db* m_parent;
  kwiver::vital::logger_handle_t m_logger;
  cppdb::session m_conn;
  std::string m_conn_str;
};


// ===============================================================================
write_object_track_set_db
::write_object_track_set_db()
  : d( new write_object_track_set_db::priv( this ) )
{
}


write_object_track_set_db
::~write_object_track_set_db()
{
}


// -------------------------------------------------------------------------------
void
write_object_track_set_db
::set_configuration(vital::config_block_sptr config)
{
  d->m_conn_str = config->get_value<std::string>( "conn_str", "" );
}


// -------------------------------------------------------------------------------
bool
write_object_track_set_db
::check_configuration(vital::config_block_sptr config) const
{
  if( !config->has_value( "conn_str" ) )
  {
    LOG_ERROR( d->m_logger, "missing required value: conn_str" );
    return false;
  }

  return true;
}


// -------------------------------------------------------------------------------
void
write_object_track_set_db
::open(std::string const& filename)
{
  d->m_conn.open( d->m_conn_str );
}


// -------------------------------------------------------------------------------
void
write_object_track_set_db
::close()
{
  d->m_conn.close();
}


// -------------------------------------------------------------------------------
void
write_object_track_set_db
::write_set( const kwiver::vital::object_track_set_sptr set )
{
  cppdb::statement stmt = d->m_conn.create_statement( "INSERT INTO OBJECT_TRACK("
    "TRACK_ID, "
    "FRAME_NUMBER, "
    "IMAGE_BBOX_TL_X, "
    "IMAGE_BBOX_TL_Y, "
    "IMAGE_BBOX_BR_X, "
    "IMAGE_BBOX_BR_Y, "
    "TRACK_CONFIDENCE"
    ") VALUES(?, ?, ?, ?, ?, ?, ?)"
  );

  for( auto trk : set->tracks() )
  {
    for( auto ts_ptr : *trk )
    {
      vital::object_track_state* ts =
        dynamic_cast< vital::object_track_state* >( ts_ptr.get() );

      if( !ts )
      {
        LOG_ERROR( d->m_logger, "MISSED STATE " << trk->id() << " " << trk->size() );
        continue;
      }

      vital::detected_object_sptr det = ts->detection;
      const vital::bounding_box_d empty_box = vital::bounding_box_d( -1, -1, -1, -1 );
      vital::bounding_box_d bbox = ( det ? det->bounding_box() : empty_box );

      stmt.bind(1, trk->id());
      stmt.bind(2, ts->frame());
      stmt.bind(3, bbox.min_x());
      stmt.bind(4, bbox.min_y());
      stmt.bind(5, bbox.max_x());
      stmt.bind(6, bbox.max_y());
      stmt.bind(7, det->confidence());

      stmt.exec();
      stmt.reset();
    }
  }
}

} } } // end namespace
