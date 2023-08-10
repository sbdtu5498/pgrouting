/*PGR-GNU*****************************************************************
File: d_rt.h

Copyright (c) 2023 pgRouting developers
Mail: project@pgrouting.org

Copyright (c) 2023 Sanskar Bhushan
Mail: sbdtu5498@gmail.com

------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
********************************************************************PGR-GNU*/
/*! @file */

#ifndef INCLUDE_C_TYPES_D_RT_H_
#define INCLUDE_C_TYPES_D_RT_H_
#pragma once

/* for int64_t */
#ifdef __cplusplus
#   include <cstdint>
#else
#   include <stdint.h>
#endif

struct D_rt {
    double value;
};

#endif  // INCLUDE_C_TYPES_D_RT_H_

