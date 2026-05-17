#include "simple_mesh.hpp"
#include "../support/error.hpp"
#include "../vmlib/mat44.hpp"



SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN)
{
	aM.vertex_positions.insert(aM.vertex_positions.end(), aN.vertex_positions.begin(), aN.vertex_positions.end());
	aM.vertex_normals .insert(aM.vertex_normals.end(), aN.vertex_normals.begin(), aN.vertex_normals.end());
	aM.vertex_textures.insert(aM.vertex_textures.end(), aN.vertex_textures.begin(), aN.vertex_textures.end());
	aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
	return aM;
}


GLuint bind_vao(SimpleMeshData const& aMeshData) {
	
	// Position VBO
	GLuint positionVBO = 0;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.vertex_positions.size() * sizeof(Vec3f), aMeshData.vertex_positions.data(), GL_STATIC_DRAW);
	
	GLuint colorVBO = 0;
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

	//Normals VBO
	GLuint normalsVBO = 0;
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.vertex_normals.size() * sizeof(Vec3f), aMeshData.vertex_normals.data(), GL_STATIC_DRAW);

	//Textures VBO
	GLuint texturesVBO = 0;
	glGenBuffers(1, &texturesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texturesVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.vertex_textures.size() * sizeof(Vec2f), aMeshData.vertex_textures.data(), GL_STATIC_DRAW);

	//Create vao
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Bind vao and vbos
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(
		0, 
		3, GL_FLOAT, GL_FALSE, 
		0, 
		0
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(
		1,
		3, GL_FLOAT, GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glVertexAttribPointer(
		2,
		3, GL_FLOAT, GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, texturesVBO);
	glVertexAttribPointer(
		3,
		2, GL_FLOAT, GL_FALSE,
		0,
		nullptr
	);
	glEnableVertexAttribArray(3);
	return vao;
}