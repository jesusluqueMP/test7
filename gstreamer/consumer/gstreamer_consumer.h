/*
 * This file is part of CasparCG (www.casparcg.com).
 *
 * CasparCG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CasparCG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CasparCG. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <common/bit_depth.h>
#include <common/memory.h>

#include <core/consumer/frame_consumer.h>
#include <core/video_channel.h>

#include <boost/property_tree/ptree_fwd.hpp>

#include <vector>

namespace caspar { namespace gstreamer {

/**
 * GStreamer consumer for CasparCG
 * 
 * Usage examples:
 *   GSADD 1 FILE output.mp4 -codec:v x264 -bitrate:v 5000
 *   GSADD 1 STREAM rtmp://server/live/stream -codec:v x264 -bitrate:v 3000
 *   GSREMOVE 1 FILE
 */
spl::shared_ptr<core::frame_consumer> create_consumer(const std::vector<std::wstring>&     params,
                                                      const core::video_format_repository& format_repository,
                                                      const std::vector<spl::shared_ptr<core::video_channel>>& channels,
                                                      common::bit_depth                                        depth);

spl::shared_ptr<core::frame_consumer>
create_preconfigured_consumer(const boost::property_tree::wptree&,
                              const core::video_format_repository&                     format_repository,
                              const std::vector<spl::shared_ptr<core::video_channel>>& channels,
                              common::bit_depth                                        depth);

}} // namespace caspar::gstreamer