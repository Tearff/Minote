#pragma once

#include <string_view>
#include <string>
#include <span>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "VulkanMemoryAllocator/vma.h"
#include "volk/volk.h"
#include "base/hashmap.hpp"
#include "base/svector.hpp"
#include "base/version.hpp"
#include "base/types.hpp"
#include "base/util.hpp"
#include "base/id.hpp"
#include "sys/vk/image.hpp"
#include "sys/window.hpp"
#include "sys/glfw.hpp"
#include "gfx/technique.hpp"
#include "gfx/world.hpp"
#include "gfx/base.hpp"
#include "gfx/mesh.hpp"

#ifndef NDEBUG
#define VK_VALIDATION
#endif //NDEBUG

namespace minote::gfx {

using namespace base;
using namespace base::literals;

struct Engine {

	Engine(sys::Glfw&, sys::Window&, std::string_view name, Version appVersion);
	~Engine();

	void setBackground(glm::vec3 color);
	void setLightSource(glm::vec3 position, glm::vec3 color);
	void setCamera(glm::vec3 eye, glm::vec3 center, glm::vec3 up = {0.0f, 1.0f, 0.0f});

	void enqueueDraw(ID mesh, ID technique, std::span<Instance const> instances,
		Material material, MaterialData const& materialData = {});

	void render();

private:

	struct Swapchain {

		VkSwapchainKHR swapchain = nullptr;
		VkFormat format;
		VkExtent2D extent = {};
		std::vector<sys::vk::Image> color;
		std::vector<VkImageView> colorView;

	};

	struct Present {

		VkRenderPass renderPass;
		std::vector<VkFramebuffer> framebuffer;
		VkDescriptorSet descriptorSet;
		sys::vk::Shader shader;
		VkPipelineLayout layout;
		VkPipeline pipeline;

	};

	struct RenderTargets {

		VkSampleCountFlagBits sampleCount;
		sys::vk::Image msColor;
		VkImageView msColorView;
		sys::vk::Image ssColor;
		VkImageView ssColorView;
		sys::vk::Image depthStencil;
		VkImageView depthStencilView;
		VkRenderPass objectPass;
		VkFramebuffer objectFb;

	};

	struct Frame {

		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
		VkSemaphore renderSemaphore;
		VkSemaphore presentSemaphore;
		VkFence renderFence;

	};

	struct Camera {


		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up;
	};

	struct DelayedOp {

		u64 deadline;
		std::function<void()> func;

	};

	std::string name;
	sys::Window& window;
	u64 frameCounter;
	svector<DelayedOp, 64> delayedOps;

	VkInstance instance;
	std::vector<char const*> instanceExtensions;
#ifdef VK_VALIDATION
	std::vector<char const*> instanceLayers;
	VkDebugUtilsMessengerEXT debugMessenger;
#endif //VK_VALIDATION
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	std::vector<char const*> deviceExtensions;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> surfacePresentModes;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	u32 graphicsQueueFamilyIndex;
	u32 presentQueueFamilyIndex;
	u32 transferQueueFamilyIndex;

	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue transferQueue;
	VmaAllocator allocator;

	PerFrame<Frame> frames;
	VkDescriptorPool descriptorPool;
	VkCommandPool transferCommandPool;
	VkFence transfersFinished;

	Swapchain swapchain;
	Present present;
	RenderTargets targets;

	TechniqueSet techniques;
	MeshBuffer meshes;
	Camera camera;
	World world;

	[[nodiscard]]
	auto uniquePresentQueue() const {
		return (presentQueueFamilyIndex != graphicsQueueFamilyIndex);
	}

	[[nodiscard]]
	auto uniqueTransferQueue() const {
		return (transferQueueFamilyIndex != graphicsQueueFamilyIndex);
	}

	void initInstance(Version appVersion);
	void cleanupInstance();

	void initPhysicalDevice();

	void initDevice();
	void cleanupDevice();

	void initCommands();
	void cleanupCommands();

	void initSwapchain();
	void cleanupSwapchain();

	void initImages();
	void cleanupImages();

	void initFramebuffers();
	void cleanupFramebuffers();

	void initBuffers();
	void cleanupBuffers();

	void initPipelines();
	void cleanupPipelines();

	void createSwapchain(VkSwapchainKHR old = nullptr);
	void destroySwapchain(Swapchain&);
	void recreateSwapchain();

	void createPresentFbs();
	void destroyPresentFbs(Present&);
	void createPresentPipeline();
	void destroyPresentPipeline(Present&);
	void createPresentPipelineDS();
	void destroyPresentPipelineDS(Present&);

	void createTargetImages();
	void destroyTargetImages(RenderTargets&);
	void createTargetFbs();
	void destroyTargetFbs(RenderTargets&);

#ifdef VK_VALIDATION
	static VKAPI_ATTR auto VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severityCode,
		VkDebugUtilsMessageTypeFlagsEXT typeCode,
		VkDebugUtilsMessengerCallbackDataEXT const* data,
		void*) -> VkBool32;
#endif //VK_VALIDATION

};

}
