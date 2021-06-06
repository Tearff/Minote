#pragma once

#include "vuk/RenderGraph.hpp"
#include "vuk/Context.hpp"
#include "gfx/indirect.hpp"
#include "gfx/sky.hpp"
#include "gfx/ibl.hpp"

namespace minote::gfx {

struct Forward {
	
	vuk::Extent2D size;
	
	Forward(vuk::PerThreadContext&, vuk::Extent2D targetSize);
	
	auto zPrepass(vuk::Buffer world, Indirect&, Meshes&) -> vuk::RenderGraph;
	
	auto draw(vuk::Buffer world, Indirect&, Meshes&) -> vuk::RenderGraph;
	
	auto resolve() -> vuk::RenderGraph;
	
private:
	
	inline static bool pipelinesCreated = false;
	
};

}
