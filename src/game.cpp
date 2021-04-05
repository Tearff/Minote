#include "game.hpp"

#include "config.hpp"

#include <exception>
#include <stdexcept>
#include <vector>
#include <memory>
#include <span>
#include "sqlite3.h"
#include "base/util.hpp"
#include "base/log.hpp"
#include "gfx/engine.hpp"
#include "mapper.hpp"
//#include "playstate.hpp"
#include "config.hpp"
#include "main.hpp"

#include "GLFW/glfw3.h"

namespace minote {

using namespace base;

constexpr auto UpdateTick = 1_s / 120;

void game(sys::Glfw&, sys::Window& window) try {

	// *** Initialization ***

	auto mapper = Mapper();
	auto engine = gfx::Engine(window, AppVersion);
	engine.setup();

	{
		auto* assets = (sqlite3*)(nullptr);
		if (auto result = sqlite3_open_v2(AssetsPath, &assets, SQLITE_OPEN_READONLY, nullptr); result != SQLITE_OK) {
			sqlite3_close(assets);
			throw std::runtime_error(fmt::format(R"(Failed to open database "{}": {})", AssetsPath, sqlite3_errstr(result)));
		}
		defer {
			if (auto result = sqlite3_close(assets); result != SQLITE_OK)
				L.warn(R"(Failed to close database "{}": {})", AssetsPath, sqlite3_errstr(result));
		};

		{
			auto modelsQuery = (sqlite3_stmt*)(nullptr);
			if (auto result = sqlite3_prepare_v2(assets, "SELECT * from models", -1, &modelsQuery, nullptr); result != SQLITE_OK)
				throw std::runtime_error(fmt::format(R"(Failed to query database "{}": {})", AssetsPath, sqlite3_errstr(result)));
			defer { sqlite3_finalize(modelsQuery); };
			if (sqlite3_column_count(modelsQuery) != 2)
				throw std::runtime_error(fmt::format(R"(Invalid number of columns in table "models" in database "{}")", AssetsPath));

			auto result = SQLITE_OK;
			while (result = sqlite3_step(modelsQuery), result != SQLITE_DONE) {
				if (result != SQLITE_ROW)
					throw std::runtime_error(fmt::format(R"(Failed to query database "{}": {})", AssetsPath, sqlite3_errstr(result)));
				if (sqlite3_column_type(modelsQuery, 0) != SQLITE_TEXT)
					throw std::runtime_error(fmt::format(R"(Invalid type in column 0 of table "models" in database "{}")", AssetsPath));
				if (sqlite3_column_type(modelsQuery, 1) != SQLITE_BLOB)
					throw std::runtime_error(fmt::format(R"(Invalid type in column 1 of table "models" in database "{}")", AssetsPath));

				auto name = (char const*)(sqlite3_column_text(modelsQuery, 0));
				auto nameLen = sqlite3_column_bytes(modelsQuery, 0);
				auto model = (u8 const*)(sqlite3_column_blob(modelsQuery, 1));
				auto modelLen = sqlite3_column_bytes(modelsQuery, 1);
				engine.addModel(std::string_view(name, nameLen), std::span(model, modelLen));
			}
		}
	}

//	PlayState play;

	// *** Main loop ***

	auto nextUpdate = sys::Glfw::getTime();

	auto lightSource = glm::vec3(6.0f, 12.0f, -6.0f);

	while (!window.isClosing()) {
		// Input
		mapper.collectKeyInputs(window);

		// Logic
		auto updateActions = std::vector<Mapper::Action>();
		updateActions.reserve(16);
		while (nextUpdate <= sys::Glfw::getTime()) {
			updateActions.clear();
			mapper.processActions([&](auto const& action) {
				if (action.timestamp > nextUpdate) return false;

#if IMGUI
				if (action.state == Mapper::Action::State::Pressed && ImGui::GetIO().WantCaptureKeyboard)
					return false;
#endif //IMGUI

				updateActions.push_back(action);

				// Interpret quit events here for now
				using Action = Mapper::Action::Type;
				if (action.type == Action::Back)
					window.requestClose();

				return true;
			});

//			play.tick(updateActions);
			nextUpdate += UpdateTick;
		}

		// Graphics
		engine.setBackground({0.4f, 0.4f, 0.4f});
		engine.setLightSource(lightSource, {1.0f, 1.0f, 1.0f});
		engine.setCamera({std::sin(glfwGetTime() / 4.0) * 24.0f, std::sin(glfwGetTime() / 3.3) * 4.0f + 8.0f, std::cos(glfwGetTime() / 4.0) * 24.0f}, {0.0f, 4.0f, 0.0f});

		auto centerTransform = make_translate({-0.5f, -0.5f, -0.5f});
		auto rotateTransform = make_rotate(glm::radians(-90.0f), {1.0f, 0.0f, 0.0f});
		auto rotateTransformAnim = make_rotate(f32(glm::radians(f64(sys::Glfw::getTime().count()) / 20000000.0)), {0.0f, 1.0f, 0.0f});

		engine.enqueue("block"_id, std::array{
			gfx::Engine::Instance{
				.transform = make_translate({0.0f, -1.0f, 0.0f}) * make_scale({16.0f, 2.0f, 16.0f}) * rotateTransform * centerTransform,
				.tint = {0.9f, 0.9f, 1.0f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({-4.0f, 1.0f, -4.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.9f, 0.1f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({4.0f, 1.0f, -4.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.9f, 0.1f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({-4.0f, 1.0f, 4.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.9f, 0.1f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({4.0f, 1.0f, 4.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.9f, 0.1f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({2.0f, 1.0f, 0.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.1f, 0.5f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({2.0f, 2.75f, 0.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.1f, 0.7f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({2.0f, 4.5f, 0.0f}) * make_scale({2.0f, 2.0f, 2.0f}) * rotateTransform * centerTransform,
				.tint = {0.1f, 0.9f, 0.1f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
			gfx::Engine::Instance{
				.transform = make_translate({-2.0f, 1.5f, 0.0f}) * make_scale({3.0f, 3.0f, 3.0f}) * rotateTransformAnim * rotateTransform * centerTransform,
				.tint = {0.2f, 0.9f, 0.5f, 1.0f},
				.ambient = 0.1f,
				.diffuse = 1.0f,
				.specular = 0.4f,
				.shine = 24.0f,
			},
		});

#if IMGUI
		ImGui::ShowDemoWindow();
#endif //IMGUI
		engine.render();
	}

} catch (std::exception const& e) {
	L.crit("Unhandled exception on game thread: {}", e.what());
	L.crit("Cannot recover, shutting down. Please report this error to the developer");
	window.requestClose();
}

}
