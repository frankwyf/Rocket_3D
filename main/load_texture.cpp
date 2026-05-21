#include "load_texture.hpp"
#include <cassert>
#include <stb_image.h>
#include "../support/error.hpp"
#include "rapidobj/rapidobj.hpp"
#include <filesystem>

namespace {
	std::filesystem::path resolve_path(char const* aPath) {
		std::filesystem::path input(aPath);
		if (std::filesystem::exists(input)) {
			return input;
		}

		auto current = std::filesystem::current_path();
		for (auto probe = current; !probe.empty(); probe = probe.parent_path()) {
			auto candidate = probe / input;
			if (std::filesystem::exists(candidate)) {
				return candidate;
			}
			if (probe == probe.root_path()) {
				break;
			}
		}

		return input;
	}
}

GLuint load_texture(char const* aPath) {
	assert(aPath);
	stbi_set_flip_vertically_on_load(true);

	auto resolvedPath = resolve_path(aPath);

	int w, h, channels;
	stbi_uc* ptr = stbi_load(resolvedPath.string().c_str(), &w, &h, &channels, 4);
	if (!ptr) {
		throw Error("Unable to load image '%s' (cwd: %s)\n", aPath, std::filesystem::current_path().string().c_str());
	}
	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
	stbi_image_free(ptr);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 6.f);
	return tex;
}	

std::unordered_map<std::string, Material> load_mtl(char const* aPath) {
	auto resolvedPath = resolve_path(aPath);
	std::ifstream file_open(resolvedPath, std::ios::binary);
	if (!file_open) {
		throw Error("Cannot find the file: %s (cwd: %s)", aPath, std::filesystem::current_path().string().c_str());
	}
	file_open.close();
	auto result = rapidobj::ParseFile(resolvedPath.string());
	if (result.error) {
		throw Error("Can't load OBJ file '%s': '%s'", resolvedPath.string().c_str(), result.error.code.message().c_str());
	}
	std::unordered_map<std::string, Material> materials;
	std::string connection = "assets/";
	for (const auto& this_mat : result.materials) {
		Material material;
		material.Shininess = this_mat.shininess;
		material.ambient_color = Vec3f{ this_mat.ambient[0],this_mat.ambient[1],this_mat.ambient[2] };
		material.diffuse_color = Vec3f{ this_mat.diffuse[0],this_mat.diffuse[1] ,this_mat.diffuse[2] };
		material.spectral_color = Vec3f{ this_mat.specular[0],this_mat.specular[1] ,this_mat.specular[2] };
		material.emissive_color = Vec3f{ this_mat.emission[0], this_mat.emission[1], this_mat.emission[2] };
		material.optical_density = this_mat.ior;
		material.dissolve = this_mat.dissolve;
		material.illumination_model = this_mat.illum;
		if (!this_mat.diffuse_texname.empty()) {
			material.texture = load_texture((connection + (this_mat.diffuse_texname)).c_str());
		}
		else {
			material.texture = 0;
		}
		materials[this_mat.name] = material;
	}
	return materials;
}

