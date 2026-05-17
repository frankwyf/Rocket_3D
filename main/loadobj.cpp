#include "loadobj.hpp"
#include "rapidobj/rapidobj.hpp"
#include "../support/error.hpp"

// load the obj file
SimpleMeshData load_obj_file(char const* aPath) {
	//Cope with the exception
	std::ifstream file_open(aPath, std::ios::binary);
	if (!file_open) {
		throw Error("Cannot find the file: %s", aPath);
	}
	file_open.close();
	auto result = rapidobj::ParseFile(aPath);
	if (result.error) {
		throw Error("Can't load OBJ file '%s': '%s'", aPath, result.error.code.message().c_str());
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

			// vertex normals
			loaded_data.vertex_normals.emplace_back(Vec3f{
				result.attributes.normals[index.normal_index * 3 + 0],
				result.attributes.normals[index.normal_index * 3 + 1],
				result.attributes.normals[index.normal_index * 3 + 2]
				}
			);

			//vertex texture
			loaded_data.vertex_textures.emplace_back(Vec2f{
				result.attributes.texcoords[index.texcoord_index * 2 + 0],
				result.attributes.texcoords[index.texcoord_index * 2 + 1]
				}
			);

			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			loaded_data.colors.emplace_back(Vec3f{	
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});
		}
	}
	return loaded_data;
}