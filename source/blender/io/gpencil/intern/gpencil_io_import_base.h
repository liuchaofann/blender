/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2020 Blender Foundation
 * All rights reserved.
 */
#pragma once

/** \file
 * \ingroup bgpencil
 */
#include <list>
#include <string>

#include "BLI_path_util.h"

#include "DNA_defs.h"

#include "gpencil_io.h"
#include "gpencil_io_base.h"

struct ARegion;
struct Depsgraph;
struct Main;
struct Object;
struct RegionView3D;
struct Scene;

struct bGPdata;
struct bGPDlayer;
struct bGPDframe;
struct bGPDstroke;

namespace blender::io::gpencil {

class GpencilImporter : public GpencilIO {

 public:
  GpencilImporter(const struct GpencilIOParams *iparams);
  virtual bool read(void) = 0;

  struct Object *create_object(void);
  int32_t create_material(const char *name, const bool stroke, const bool fill);

 protected:
 private:
};

}  // namespace blender::io::gpencil
