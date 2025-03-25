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

#include <common/memory.h>

#include <core/fwd.h>

#include <string>
#include <vector>

namespace caspar { namespace gstreamer {

// Main producer factory function
spl::shared_ptr<core::frame_producer> create_producer(const core::frame_producer_dependencies& dependencies,
                                                     const std::vector<std::wstring>&         params);

/**
 * Command handler for GS-prefixed commands (GSPLAY, GSLOAD, etc.)
 * 
 * The template parameter T is unused but required to match the producer_factory signature.
 * This function forwards to the create_producer function after adding the "GSTREAMER_PRODUCER" token.
 */
template <typename T>
spl::shared_ptr<core::frame_producer> create_gs_producer_proxy(
    const core::frame_producer_dependencies& dependencies,
    const std::vector<std::wstring>& params);

}} // namespace caspar::gstreamer