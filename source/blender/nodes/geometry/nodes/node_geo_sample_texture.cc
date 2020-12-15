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
 */

#include "BLI_compiler_attrs.h"

#include "DNA_texture_types.h"

#include "BKE_texture.h"

#include "RE_texture.h"

#include "node_geometry_util.hh"

static bNodeSocketTemplate geo_node_sample_texture_in[] = {
    {SOCK_GEOMETRY, N_("Geometry")},
    {SOCK_STRING, N_("Result")},
    {-1, ""},
};

static bNodeSocketTemplate geo_node_sample_texture_out[] = {
    {SOCK_GEOMETRY, N_("Geometry")},
    {-1, ""},
};

namespace blender::nodes {

static void execute_on_component(GeometryComponent &component, const GeoNodeExecParams &params)
{
  const bNode &node = params.node();
  Tex *texture = reinterpret_cast<Tex *>(node.id);
  const GeometryNodeSampleTextureMode mode = static_cast<GeometryNodeSampleTextureMode>(
      node.custom1);
  const std::string result_attribute_name = params.get_input<std::string>("Result");

  if (texture == nullptr) {
    return;
  }

  WriteAttributePtr attribute_out = component.attribute_try_ensure_for_write(
      result_attribute_name, ATTR_DOMAIN_POINT, CD_PROP_COLOR);
  if (!attribute_out) {
    return;
  }

  MutableSpan<Color4f> colors = attribute_out->get_span().typed<Color4f>();

  const int tot_points = component.attribute_domain_size(ATTR_DOMAIN_POINT);
  if (mode == GEO_NODE_SAMPLE_TEXTURE_MODE_POSITION) {
    Float3ReadAttribute attribute_position = component.attribute_get_for_read<float3>(
        "position", ATTR_DOMAIN_POINT, {0, 0, 0});

    for (const int i : IndexRange(tot_points)) {
      TexResult texture_result = {0};
      const float3 position = attribute_position[i];
      BKE_texture_get_value(nullptr, texture, position, &texture_result, false);
      colors[i] = {texture_result.tr, texture_result.tg, texture_result.tb, texture_result.ta};
    }
  }
  if (mode == GEO_NODE_SAMPLE_TEXTURE_MODE_UV) {
    colors.fill({0, 0, 0, 1});
  }

  attribute_out->apply_span();
}

static void geo_node_sample_texture_exec(GeoNodeExecParams params)
{
  GeometrySet geometry_set = params.extract_input<GeometrySet>("Geometry");

  if (geometry_set.has<MeshComponent>()) {
    execute_on_component(geometry_set.get_component_for_write<MeshComponent>(), params);
  }
  if (geometry_set.has<PointCloudComponent>()) {
    execute_on_component(geometry_set.get_component_for_write<PointCloudComponent>(), params);
  }

  params.set_output("Geometry", geometry_set);
}
}  // namespace blender::nodes

void register_node_type_geo_sample_texture()
{
  static bNodeType ntype;

  geo_node_type_base(&ntype, GEO_NODE_SAMPLE_TEXTURE, "Sample Texture", NODE_CLASS_GEOMETRY, 0);
  node_type_socket_templates(&ntype, geo_node_sample_texture_in, geo_node_sample_texture_out);
  ntype.geometry_node_execute = blender::nodes::geo_node_sample_texture_exec;
  nodeRegisterType(&ntype);
}