#ifndef LOAD_TEXTURE_HPP_37D0A444_F6F8_4E48_9268_1ECB068A6D7D
#define LOAD_TEXTURE_HPP_37D0A444_F6F8_4E48_9268_1ECB068A6D7D

#include <glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "../vmlib/vec3.hpp"
struct Material{
	float Shininess; // Ns
	Vec3f ambient_color; // Ka
	Vec3f diffuse_color; // Kd
	Vec3f spectral_color; // Ks
	Vec3f emissive_color; // Ke
	float optical_density; // Ni
	float dissolve; // d
	unsigned int illumination_model; // illum
	GLuint texture;
};

std::unordered_map<std::string, Material> load_mtl(char const* aPath);
GLuint load_texture(char const* aPath);
#endif // !LOAD_TEXTURE_HPP_37D0A444_F6F8_4E48_9268_1ECB068A6D7D
