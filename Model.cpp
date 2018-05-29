#include "Model.h"
#include "include/assimp/Importer.hpp"
#include "include/assimp/postprocess.h"
#include <iostream>

using namespace std;
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

void Model::setupShader(GLuint shaderProgram, const wchar_t* texturePath)
{
	// przypisanie tekstur do obiektu
	if (hasTextureCoords())
	{
		GLuint texSamplerLoc = glGetUniformLocation(shaderProgram, "textureSamp");

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			meshes[i]->setupShader(texSamplerLoc, texturePath);
		}
	}
}

void Model::Poly::setupShader(GLuint texSamplerLoc, const wchar_t* texturePath)
{
	//tworzenie tekstur obiektu
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (texturePath != (const wchar_t*)"none")
		loadTexture(GL_TEXTURE_2D, texturePath);
	else
		loadTexture();

	glUniform1i(texSamplerLoc, 0);
	glActiveTexture(GL_TEXTURE0);
}

bool Model::hasTextureCoords()
{
	// okreœlenie wspó³rzêdnych tekstur
	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i]->hasTextures) return true;
	}
	return false;
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

	// jeœli pobrane dane zawieraj¹ wierzcho³ki zostaj¹ dodane do tablicy oraz ich przesuniêcia
	if (mesh->HasPositions()) {
		float *vertices = new float[mesh->mNumVertices * 3];
		float cordX = 0, cordY = 0, cordZ = 0;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;

			cordX += mesh->mVertices[i].x / mesh->mNumVertices;
			cordY += mesh->mVertices[i].y / mesh->mNumVertices;
			cordZ += mesh->mVertices[i].z / mesh->mNumVertices;
		}
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices[i * 3] -= cordX;
			vertices[i * 3 + 1] -= cordY;
			vertices[i * 3 + 2] -= cordZ;
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
		hasTextures = true;
		vector<GLfloat> textureCoords;
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			textureCoords.push_back(mesh->mTextureCoords[0][i].x);
			textureCoords.push_back(mesh->mTextureCoords[0][i].y);
		}

		glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), &textureCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		glGenTextures(1, &vbo[TEXTURE_BUFFER]);
		glBindTexture(GL_TEXTURE_2D, vbo[TEXTURE_BUFFER]);
		glGenerateMipmap(GL_TEXTURE_2D);

		textureCoords.clear();
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
//wczytanie tekstury z obrazka
void Model::Poly::loadTexture(GLuint tex, const wchar_t *filename)
{
	ilInit();

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	ILuint imageName;

	ilGenImages(1, &imageName);
	ilBindImage(imageName);

	if (!ilLoadImage(filename))
	{
		ILenum err = ilGetError();

		ilBindImage(0);
		ilDeleteImages(1, &imageName);

		exit(1);
	}

	glTexImage2D(tex, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	ilBindImage(0);
	ilDeleteImages(1, &imageName);
}


void Model::Poly::loadTexture()
{
	ILubyte* white = new ILubyte();
	white[0] = white[1] = white[2] = 255;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
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
	if (vbo[TEXTURE_BUFFER]) {
		glDeleteTextures(1, &vbo[TEXTURE_BUFFER]);
		glDeleteBuffers(1, &vbo[TEXTURE_BUFFER]);
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

