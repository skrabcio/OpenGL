#pragma once

#include "GL/glew.h"

#include "include/assimp/scene.h"
#include "include/assimp/mesh.h"

#include <vector>

class Model
{
private:
	struct Poly
	{
		static enum BUFFERS {
			VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER
		};

		GLuint vao;
		GLuint vbo[4];

		unsigned int elements;

		Poly(aiMesh *mesh);
		~Poly();

		void render();
	};
	std::vector<Poly*> meshes;
	bool loadSuccess = false;

public:
	Model(const char *filename);
	~Model(void);

	void render();
};