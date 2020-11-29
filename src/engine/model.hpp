// Minote - engine/model.hpp
// Drawing of 3D mesh data with a number of shading models

#pragma once

#include "sys/opengl/vertexarray.hpp"
#include "sys/opengl/framebuffer.hpp"
#include "sys/opengl/buffer.hpp"
#include "sys/opengl/draw.hpp"
#include "engine/scene.hpp"
#include "store/shaders.hpp"

namespace minote {

// Flat shaded model - no lighting is applied
struct ModelFlat {

	struct Vertex {

		// Vertex position in model space
		vec3 pos = {0.0f, 0.0f, 0.0f};

		// Vertex color, smoothly interpolated
		color4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	};

	struct Instance {

		// Instance tint, multiplied with vertex color
		color4 tint = {1.0f, 1.0f, 1.0f, 1.0f};

		// Instance highlight, blended with fragment color
		// Highlight alpha 0.0 is no highlight, 1.0 is pure highlight color
		color4 highlight = {0.0f, 0.0f, 0.0f, 0.0f};

		// Instance's model space -> world space transform
		mat4 transform = mat4(1.0f);

	};

	// Model name for logging and debugging
	char const* name = nullptr;

	// VBO of static vertex data
	VertexBuffer<Vertex> vertices;

	// VBO of instance data, uploaded every draw
	VertexBuffer<Instance> instances;

	// Vertex and Instance attribute pointers
	VertexArray vao;

	// Cached drawcall data
	Draw<Shaders::Flat> drawcall;

	// Create the model from an array of vertices.
	template<template<copy_constructible, size_t> typename Arr, size_t N>
		requires ArrayContainer<Arr, Vertex, N>
	void create(char const* name, Shaders& shaders,
		Arr<Vertex, N> const& vertices);

	// Free up all resources used by the model.
	void destroy();

	// Draw the model with specified parameters and identity instance.
	void draw(Framebuffer& fb, Scene const& scene, DrawParams const& params);

	// Draw the model with specified parameters and custom instance data.
	void draw(Framebuffer& fb, Scene const& scene, DrawParams const& params,
		Instance const& instance);

	// Draw multiple instances of the model with specified parameters
	// and an array of instance data. Number of instances drawn is the size
	// of the instance data array.
	template<template<copy_constructible, size_t> typename Arr, size_t N>
		requires ArrayContainer<Arr, Instance, N>
	void draw(Framebuffer& fb, Scene const& scene, DrawParams const& params,
		Arr<Instance, N> const& instances);

};

// Phong shaded model - the Phong-Blinn lighting model is used
struct ModelPhong {

	struct Vertex {

		// Vertex position in model space
		vec3 pos = {0.0f, 0.0f, 0.0f};

		// Vertex color, smoothly interpolated
		color4 color = {1.0f, 1.0f, 1.0f, 1.0f};

		// Vertex normal. Can be left empty and generated inside create()
		vec3 normal = {0.0f, 1.0f, 0.0f};

	};

	struct Instance {

		// Instance tint, multiplied with vertex color
		color4 tint = {1.0f, 1.0f, 1.0f, 1.0f};

		// Instance highlight, blended with fragment color
		// Highlight alpha 0.0 is no highlight, 1.0 is pure highlight color
		color4 highlight = {0.0f, 0.0f, 0.0f, 0.0f};

		// Instance's model space -> world space transform
		mat4 transform = mat4(1.0f);

	};

	struct Material {

		// Strength of ambient light (not affected by normal)
		f32 ambient = 0.0f;

		// Strength of diffuse reflection (light diffused from the source)
		f32 diffuse = 0.0f;

		// Strength of specular highlight (light reflected from the source)
		f32 specular = 0.0f;

		// Smoothness of surface (inverse of specular highlight size)
		f32 shine = 1.0f;

	};

	// Model name for logging and debugging
	char const* name = nullptr;

	// VBO of static vertex data
	VertexBuffer<Vertex> vertices;

	// VBO of instance data, uploaded every draw
	VertexBuffer<Instance> instances;

	// Material data, can be modified
	Material material;

	// Vertex and Instance attribute pointers
	VertexArray vao;

	// Cached drawcall data
	Draw<Shaders::Phong> drawcall;

	// Create the model from an array of vertices. Vertex normals can be left
	// blank and automatically generated by setting generateNormals to true.
	template<template<copy_constructible, size_t> typename Arr, size_t N>
	requires ArrayContainer<Arr, Vertex, N>
	void create(char const* name, Shaders& shaders,
		Arr<Vertex, N> const& vertices, Material material, bool generateNormals = false);

	// Free up all resources used by the model.
	void destroy();

	// Draw the model with specified parameters and identity instance.
	void draw(Framebuffer& fb, Scene const& scene, DrawParams const& params);

	// Draw the model with specified parameters and custom instance data.
	void draw(Framebuffer& fb, Scene const& scene, DrawParams const& params,
		Instance const& instance);

	// Draw multiple instances of the model with specified parameters
	// and an array of instance data. Number of instances drawn is the size
	// of the instance data array.
	template<template<copy_constructible, size_t> typename Arr, size_t N>
	requires ArrayContainer<Arr, Instance, N>
	void draw(Framebuffer& fb, Scene const& scene, DrawParams const& params,
		Arr<Instance, N> const& instances);

};

}

#include "model.tpp"
