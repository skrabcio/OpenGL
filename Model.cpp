#include "Model.h"
#include "include/assimp/Importer.hpp"
#include "include/assimp/postprocess.h"
#include <iostream>

Model::Model(const char* filePath)
{
	Assimp::Importer importer;
	//importowanie danych zawartych w pliku
	const aiScene *scene = importer.ReadFile(filePath, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenNormals);

	// zabezpieczenie czy dane nie s¹ puste, jeœli tak program przerywa dzia³anie
	if (scene == NULL)
		return;

	// jeœli dane s¹ poprawne pobieramy je i generujemy buffor 
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		meshes.push_back(new Model::Poly(scene->mMeshes[i]));
	}
	loadSuccess = true;
}

Model::Poly::Poly(aiMesh *mesh)
{
	// stworzenie pustego VBO
	vbo[VERTEX_BUFFER] = NULL;
	vbo[TEXCOORD_BUFFER] = NULL;
	vbo[NORMAL_BUFFER] = NULL;
	vbo[INDEX_BUFFER] = NULL;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	elements = mesh->mNumFaces * 3;

	// jeœli pobrane dane zawieraj¹ wierzcho³ki zostaj¹ dodane do tablicy 
	if (mesh->HasPositions()) {
		float *vertices = new float[mesh->mNumVertices * 3];
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}

		glGenBuffers(1, &vbo[VERTEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		delete[] vertices;
	}


	// Jeœli model posiada normalne zostaj¹ dodane do kolejnej tablicy 
	if (mesh->HasNormals()) {
		float *normals = new float[mesh->mNumVertices * 3];
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &vbo[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		delete[] normals;
	}

	// Je¿eli model zawiera UV zostaj¹ dodane do tablicy 
	if (mesh->HasTextureCoords(0)) {
		float *textureCoords = new float[mesh->mNumVertices * 2];
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			textureCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			textureCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), textureCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		delete[] textureCoords;
	}

	// je¿eli model posiada œcianki to równie¿ zostaj¹ dodane do tablicy 
	if (mesh->HasFaces()) {
		unsigned int *faces = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			faces[i * 3] = mesh->mFaces[i].mIndices[0];
			faces[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			faces[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &vbo[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), faces, GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);

		delete[] faces;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Zwolnienie pamiêci, za pomoc¹ destruktor, który usuwa bufory
Model::Poly::~Poly() {
	if (vbo[VERTEX_BUFFER]) {
		glDeleteBuffers(1, &vbo[VERTEX_BUFFER]);
	}

	if (vbo[TEXCOORD_BUFFER]) {
		glDeleteBuffers(1, &vbo[TEXCOORD_BUFFER]);
	}

	if (vbo[NORMAL_BUFFER]) {
		glDeleteBuffers(1, &vbo[NORMAL_BUFFER]);
	}

	if (vbo[INDEX_BUFFER]) {
		glDeleteBuffers(1, &vbo[INDEX_BUFFER]);
	}

	glDeleteVertexArrays(1, &vao);
}

// wyrenderowanie siatki modelu
void Model::Poly::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

// Zwolnienie pamiêci prze destruktor, który usuwa elementy siatki obiektu 
Model::~Model(void)
{
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		delete meshes.at(i);
	}
	meshes.clear();
}

// Gdy model zostaje prawid³owo za³adowany, nastêpuje jego wyrenderowanie
void Model::render()
{
	if (!loadSuccess) return;
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		meshes.at(i)->render();
	}
}

