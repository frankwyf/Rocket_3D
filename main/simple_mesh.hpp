#ifndef SIMPLE_MESH_HPP_19B23D82_71E9_40C4_8B0B_BDB7826CE84C
#define SIMPLE_MESH_HPP_19B23D82_71E9_40C4_8B0B_BDB7826CE84C

#include <glad.h>
#include <vector>
#include <string>

#include "../vmlib/vec2.hpp"
#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

struct SimpleMeshData
{
	std::vector<Vec3f> vertex_positions;
	std::vector<Vec3f> vertex_normals;
	std::vector<Vec2f> vertex_textures;
	std::vector<Vec3f> colors;
};

GLuint bind_vao(SimpleMeshData const& aMeshData);
GLuint load_texture(char const* aPath);
SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN);
#endif // !SIMPLE_MESH_HPP_19B23D82-71E9-40C4-8B0B-BDB7826CE84C
