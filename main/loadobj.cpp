#include "loadobj.hpp"
#include "rapidobj/rapidobj.hpp"
#include "../support/error.hpp"
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

// load the obj file
SimpleMeshData load_obj_file(char const* aPath) {
	//Cope with the exception
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
	rapidobj::Triangulate(result);
	SimpleMeshData loaded_data;
	for (auto const& shape : result.shapes) {
		for (size_t i = 0; i < shape.mesh.indices.size(); ++i) {
			auto const& index = shape.mesh.indices[i];

			// vertex position
			loaded_data.vertex_positions.emplace_back(Vec3f{
				result.attributes.positions[index.position_index * 3 + 0],
				result.attributes.positions[index.position_index * 3 + 1],
				result.attributes.positions[index.position_index * 3 + 2]
				}
			);

			// vertex normals (fallback to +Y if source file has no normals)
			if (index.normal_index >= 0 && static_cast<std::size_t>(index.normal_index * 3 + 2) < result.attributes.normals.size())
			{
				loaded_data.vertex_normals.emplace_back(Vec3f{
					result.attributes.normals[index.normal_index * 3 + 0],
					result.attributes.normals[index.normal_index * 3 + 1],
					result.attributes.normals[index.normal_index * 3 + 2]
					}
				);
			}
			else
			{
				loaded_data.vertex_normals.emplace_back(Vec3f{ 0.f, 1.f, 0.f });
			}

			//vertex texture (fallback to 0,0 if source file has no UV)
			if (index.texcoord_index >= 0 && static_cast<std::size_t>(index.texcoord_index * 2 + 1) < result.attributes.texcoords.size())
			{
				loaded_data.vertex_textures.emplace_back(Vec2f{
					result.attributes.texcoords[index.texcoord_index * 2 + 0],
					result.attributes.texcoords[index.texcoord_index * 2 + 1]
					}
				);
			}
			else
			{
				loaded_data.vertex_textures.emplace_back(Vec2f{ 0.f, 0.f });
			}

			Vec3f color = { 1.f, 1.f, 1.f };
			if (!result.materials.empty())
			{
				auto const triIdx = i / 3;
				if (triIdx < shape.mesh.material_ids.size())
				{
					int matIdx = shape.mesh.material_ids[triIdx];
					if (matIdx >= 0 && static_cast<std::size_t>(matIdx) < result.materials.size())
					{
						auto const& mat = result.materials[matIdx];
						color = Vec3f{ mat.ambient[0], mat.ambient[1], mat.ambient[2] };
					}
				}
			}

			loaded_data.colors.emplace_back(color);
		}
	}
	return loaded_data;
}