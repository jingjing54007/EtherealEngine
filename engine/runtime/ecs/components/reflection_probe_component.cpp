#include "reflection_probe_component.h"

reflection_probe_component::reflection_probe_component()
{
}

reflection_probe_component::reflection_probe_component(const reflection_probe_component& lightComponent)
{
	_probe = lightComponent._probe;
}

reflection_probe_component::~reflection_probe_component()
{
}

int reflection_probe_component::compute_projected_sphere_rect(irect& rect, const math::vec3& position,
															  const math::transform& view,
															  const math::transform& proj)
{
	if(_probe.type == probe_type::sphere)
	{
		return math::compute_projected_sphere_rect(rect.left, rect.right, rect.top, rect.bottom, position,
												   _probe.sphere_data.range, view, proj);
	}
	else if(_probe.type == probe_type::box)
	{
		float w2 = math::pow(_probe.box_data.extents.x * 2.0f, 2.0f);
		float h2 = math::pow(_probe.box_data.extents.y * 2.0f, 2.0f);
		float l2 = math::pow(_probe.box_data.extents.z * 2.0f, 2.0f);
		float d2 = w2 + h2 + l2;
		float d = math::sqrt(d2);

		return math::compute_projected_sphere_rect(rect.left, rect.right, rect.top, rect.bottom, position, d,
												   view, proj);
	}
	else
	{
		return 1;
	}
}

std::shared_ptr<texture> reflection_probe_component::get_cubemap()
{
	static auto buffer_format =
		gfx::get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER | BGFX_CAPS_FORMAT_TEXTURE_CUBE |
								 BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN,
							 gfx::format_search_flags::FourChannels | gfx::format_search_flags::RequireAlpha);

	static auto flags = gfx::get_default_rt_sampler_flags() | BGFX_TEXTURE_BLIT_DST;

	std::uint16_t size = 256;
	return _render_view[0].get_texture("CUBEMAP", size, true, 1, buffer_format, flags);
}

std::shared_ptr<frame_buffer> reflection_probe_component::get_cubemap_fbo()
{
	return _render_view[0].get_fbo("CUBEMAP", {get_cubemap()});
}

void reflection_probe_component::set_probe(const reflection_probe& probe)
{
	if(probe == _probe)
		return;

	touch();

	_probe = probe;
}
