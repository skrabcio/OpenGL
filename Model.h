#pragma once

#include "GL/glew.h"

#include "include/assimp/scene.h"
#include "include/assimp/mesh.h"
#include <IL/il.h>

#include <vector>

class Model
{
private:
	static enum BUFFERS {
		VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, TEXTURE_BUFFER
	};
	struct Poly
	{

		GLuint vao;
		GLuint vbo[5];

		unsigned int elements;
		bool hasTextures = false;

		Poly(aiMesh *mesh);
		~Poly();

		void setupShader(GLuint texSamplerLoc, const wchar_t* texturePath);
		void loadTexture(GLuint tex, const wchar_t* filename);
		void loadTexture();
		void render();
	};
	std::vector<Poly*> meshes;
	bool loadSuccess= false;
	const aiScene* scene;

public:
	Model(const char *filename);
	~Model(void);
	enum Shader { NonTextured, Textured };
	void setupShader(GLuint shaderProgram, const wchar_t* texturePath = (const wchar_t*)"none");
	bool hasTextureCoords();
	void render();
};