#ifndef MODEL_H
#define MODEL_H

#include "Rubber.h"
#include "Datafile.h"
#include <string>
#include <vector>

using std::string;
using std::vector;
using glm::vec3;
using glm::vec2;

namespace RubberSpace
{

#define MAX_VERTS 1000000
#define MAX_FACES 2000000
#define MAX_ATTACH 16


class Mesh
{
public:
	Mesh(vec3* vertices, vec2* uvs, vec3* normals, int* indices, int vc, int fc,
	int* origIndices, int* vLinks, int ovc, int linkcnt);
		
	~Mesh();
	void render();

private:
	void calculateNormals();
	void calculateTangents();
	void generateVBOs();
	
	int m_vc, m_fc;
	float* m_vertices;
	float* m_normals;
	float* m_uvs;
	float* m_tangents;
	unsigned int* m_indices;
	unsigned int* m_origIndices;
	unsigned int* m_vLinks;
	int m_linkCount, m_origVc;
	
	GLuint m_pos_fbo;
	GLuint m_norm_fbo;
	GLuint m_uvs_fbo;
	GLuint m_index_fbo;

};


class Model
{
public:
	Model(string filename);
	~Model();
	void render();
	void render(unsigned int mesh);	

private:
	void loadModel(string fileName);
	void addMesh(Mesh* mesh);
	
	int m_numMeshes;
	vector<Mesh*> m_meshes;
	Datafile* m_file;
};

}

#endif 
