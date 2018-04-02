#include "Model.h"
#include <assert.h>

using namespace RubberSpace;

/////////////// Model

Model::Model(string fileName)
	: m_numMeshes(0)
{
	loadModel(fileName);
}

void Model::addMesh(Mesh* mesh)
{
	m_meshes.push_back(mesh);
}

Model::~Model()
{
	for (int i = 0; i< m_numMeshes; i++)
	{
		delete m_meshes[i];
	}
	
	delete m_file;
}

void Model::render()
{
	for (int i = 0; i < m_numMeshes; i++)
	{
		m_meshes[i]->render();
	}
}


void Model::render(unsigned int mesh)
{
	m_meshes[mesh]->render();
}


/////////////// Mesh


Mesh::Mesh(vec3* vertices, vec2* uvs, vec3* normals, int* indices, int vc, int fc,
	int* origIndices, int* vLinks, int ovc, int linkcnt)
	: m_vc(vc)
	, m_fc(fc)
	, m_vertices(new float[vc*3])
	, m_normals(new float[vc*3])
	, m_uvs(uvs ? new float[vc*2] : NULL)
	, m_tangents(NULL)
	, m_indices(new unsigned int[fc*3])
	, m_origIndices(NULL)
	, m_linkCount(linkcnt)
	, m_origVc(ovc)
{
	LOG("\tNew mesh: %i vertices, %i faces, %i vertex links", vc, fc, linkcnt);

	for (int i = 0; i < vc; i++)
	{
		m_vertices[i * 3 + 0] = vertices[i].x;
		m_vertices[i * 3 + 1] = vertices[i].y;
		m_vertices[i * 3 + 2] = vertices[i].z;

		if (normals)
		{
			m_normals[i * 3 + 0] = normals[i].x;
			m_normals[i * 3 + 1] = normals[i].y;
			m_normals[i * 3 + 2] = normals[i].z;
		}
		
		if (uvs)
		{
			m_uvs[i * 2 + 0] = uvs[i].x;
			m_uvs[i * 2 + 1] = uvs[i].y;
		}
	}
	
	for (int i = 0 ; i < fc * 3; i++)
	{
		m_indices[i] = indices[i];
	}
	
	if (linkcnt > 0)
	{
		m_origIndices = new unsigned int[fc * 3];
		m_vLinks = new unsigned int[linkcnt * 2];
	
		for (int i = 0; i < fc * 3; i++)
		{
			m_origIndices[i] = origIndices[i];
		}
		
		for (int i = 0; i < linkcnt * 2; i++)
		{
			m_vLinks[i] = vLinks[i];
		}
	}
	
	if (!normals) 
	{
		calculateNormals();
		LOG(", calculated normals");
	}
	
	calculateTangents();
	generateVBOs();
	
	LOG(" ... done\n");
}


Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_pos_fbo);
	glDeleteBuffers(1, &m_norm_fbo);
	if (m_uvs) glDeleteBuffers(1, &m_uvs_fbo);
	glDeleteBuffers(1, &m_index_fbo);

	delete[] m_vertices;
	delete[] m_normals;
	if (m_uvs != NULL) delete[] m_uvs;
	if (m_origIndices != NULL) delete[] m_origIndices;
	if (m_vLinks != NULL) delete[] m_vLinks;
	//delete[] m_tangents;

}

void Mesh::render()
{
    glEnableClientState(GL_VERTEX_ARRAY);
  	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_pos_fbo);
  	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_norm_fbo);
	glNormalPointer(GL_FLOAT, 0, 0);
  
	if (m_uvs)
	{
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_uvs_fbo);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}
	
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_index_fbo);
	glDrawElements(GL_TRIANGLES, m_fc * 3, GL_UNSIGNED_INT, (GLvoid*)NULL);
  
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
  	glDisableClientState(GL_VERTEX_ARRAY);
  	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  	glDisableClientState(GL_NORMAL_ARRAY);

}




void Mesh::calculateNormals()
{
//	float *v;
//	unsigned short *i;
//	int x;
//	float *n=malloc(sizeof(float)*vc*3);

//	v = vertices;
//	if (obj->orig_indices) i=obj->orig_indices;
//	else i=obj->indices;
//	i=indices;

	unsigned int* ind;
	
	if (m_origIndices) ind = m_origIndices;
	else ind = m_indices;

	for (int i = 0; i < m_vc * 3; i++) m_normals[i] = 0.0;

	for (int i = 0; i < m_fc; i++)
	{
		unsigned int i1, i2, i3;
		float fnorm[3];
		float* v1;
		float* v2;
		float* v3;
		
		i1 = ind[i * 3 + 0] * 3;
		i2 = ind[i * 3 + 1] * 3;
		i3 = ind[i * 3 + 2] * 3;

		v1 = m_vertices + i1;
		v2 = m_vertices + i2;
		v3 = m_vertices + i3;
		
		fnorm[0] = (v1[1]-v3[1]) * (v2[2]-v3[2]) - (v1[2]-v3[2]) * (v2[1]-v3[1]);
		fnorm[1] = (v1[2]-v3[2]) * (v2[0]-v3[0]) - (v1[0]-v3[0]) * (v2[2]-v3[2]);
		fnorm[2] = (v1[0]-v3[0]) * (v2[1]-v3[1]) - (v1[1]-v3[1]) * (v2[0]-v3[0]);

		float len = 1.0f / sqrt(fnorm[0]*fnorm[0] + fnorm[1]*fnorm[1] + fnorm[2]*fnorm[2]);
		fnorm[0] *= len;
		fnorm[1] *= len;
		fnorm[2] *= len;

		m_normals[i1 + 0] += fnorm[0]; m_normals[i1 + 1] += fnorm[1]; m_normals[i1 + 2] += fnorm[2];
		m_normals[i2 + 0] += fnorm[0]; m_normals[i2 + 1] += fnorm[1]; m_normals[i2 + 2] += fnorm[2];
		m_normals[i3 + 0] += fnorm[0]; m_normals[i3 + 1] += fnorm[1]; m_normals[i3 + 2] += fnorm[2];
	}

	if (m_origIndices)
	{
		for (int i = 0; i < m_linkCount; i++)
		{
			unsigned int l1, l2;

			l1 = m_vLinks[i * 2 + 0];
			l2 = m_vLinks[i * 2 + 1];
			m_normals[l1] = m_normals[l2];
		}
	}

	for (int i = 0; i < m_vc; i++)
	{
		float* n = m_normals + i * 3;
		float len = 1.0f / sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
		n[0] *= len;
		n[1] *= len;
		n[2] *= len;
	}
}


void Mesh::calculateTangents()
{
}


void Mesh::generateVBOs()
{
    glGenBuffersARB(1, &m_pos_fbo);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_pos_fbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * m_vc, m_vertices, GL_STATIC_DRAW_ARB);
	
    glGenBuffersARB(1, &m_norm_fbo);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_norm_fbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * m_vc, m_normals, GL_STATIC_DRAW_ARB);

	if (m_uvs)
	{
	    glGenBuffersARB(1, &m_uvs_fbo);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_uvs_fbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 2 * m_vc, m_uvs, GL_STATIC_DRAW_ARB);
	}
	
	glGenBuffersARB(1, &m_index_fbo);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_index_fbo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_fc * 3, m_indices, GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);	
}






/**************************************************************************
********  old pos code, contains globals and non-threadsafe code  *********
***************************************************************************/

static inline int getline(unsigned char *src, char *dest, int *datapos, int filesize)
{
	static char temp[1000];
	int x = *datapos;

	memset(temp, 0, 1000);

	while (src[*datapos] != 10) 
	{
		temp[(*datapos)-x] = src[*datapos];
		(*datapos)++;
		assert((*datapos)-x < 999);
		if (*datapos >= filesize) return 0;
	}

	sprintf(dest, "%s", temp);
	(*datapos)++;
	return 1;
}


static inline void substr(char *sourke, char *dest, int len) 
{
	for (int x = 0; x < len; x++) 
	{
		dest[x] = sourke[x];
	}
	dest[len] = 0;
}


/**********************************************
*********** vertex splitting stuff  ***********
**********************************************/

typedef struct {
  int vi, ti, ni;
  int newind;
} Hasa;

static Hasa hasataulu[MAX_VERTS*MAX_ATTACH];

static inline int hashcheck(int vi, int ti, int ni, int* vertexpos, int* muthas, int* muthapos) 
{
	for (int x = 0; x < MAX_ATTACH; x++) {
		int i = vi * MAX_ATTACH + x;

		if (hasataulu[i].vi == vi && hasataulu[i].ti == ti && hasataulu[i].ni == ni) 
		{
			return hasataulu[i].newind;
		}

		if (hasataulu[i].vi == -1) 
		{
			hasataulu[i].vi = vi;
			hasataulu[i].ti = ti;
			hasataulu[i].ni = ni;
			hasataulu[i].newind = (*vertexpos);

			if (x > 0) 
			{
				muthas[*muthapos + 0] = *vertexpos;
				muthas[*muthapos + 1] = hasataulu[vi*MAX_ATTACH].newind;
				(*muthapos) += 2;
				assert((*muthapos) <= MAX_FACES * 2);
			}

			(*vertexpos)++;
			assert((*vertexpos) <= MAX_VERTS);
			
			return hasataulu[i].newind;
		}
	}

	return -666;
}


static inline int new_vertex(int vi, int ti, int ni, vec3* verts, vec2* uvs, vec3* norms,
	vec3* vertsx, vec2 *uvsx, vec3 *normsx, int* vertexpos, int* muthas, int* muthapos)
{
	assert(vi>=0 && ti>=0 && ni>=0);
	assert(vi <= MAX_VERTS && ti <= MAX_VERTS && ni <= MAX_VERTS);

 	int newind = hashcheck(vi, ti, ni, vertexpos, muthas, muthapos);
	assert(newind != -666);
	assert(newind >= 0 && newind <= MAX_VERTS);
	assert(*vertexpos < MAX_VERTS);
	
	verts[newind] = vertsx[vi];
	uvs[newind] = uvsx[ti];
	norms[newind] = normsx[ni];

	return newind;
}

 
#define VERTEXPART 1
#define TEXCOORDPART 2
#define FACEPART 3
#define NORMALPART 4


void Model::loadModel(string fileName)
{
	char rivi[1000];
	char subrivi[1000];

	int numgroups = 0;
	int current_group = 0;
	int current_part = 0;
	int indexminus_v = 1;
	int indexminus_t = 1;
	int indexminus_n = 1;
	int datapos = 0;
	int vertexpos = 0;
	//	int orig_vc = 0;
	
	//static char *tempbuf;
	static vec3 *temp_vlist;
	static vec3 *temp_vlist_x;
	static int *temp_flist;
	static vec2 *temp_tlist;
	static vec2 *temp_tlist_x;
	static vec3 *temp_nlist;
	static vec3 *temp_nlist_x;
	static int *orig_flist;
	static int *muthas;

	//int filesize=0;
	int muthapos = 0;
	int orig_fpos = 0;
	int vertexpos_x = 0;
	int texturepos_x = 0;
	int normalpos_x = 0;
	int facepos = 0;
	int linenum = 1;

	m_file = new Datafile(fileName);

	// count number of groups
	while (1)
	{
		if (!getline(m_file->getData(), rivi, &datapos, m_file->getLenght())) break;
		substr(rivi, subrivi, 2);
		if (strcmp(subrivi, "g ") == 0) numgroups++;
		if (strcmp(subrivi, "o ") == 0) numgroups++;
	}
	datapos = 0;  

	if (numgroups < 1) 
	{
		LOG("\tNo groups found, assuming 1\n");
		numgroups = 1;
	}
	else LOG("\tFound %i groups\n", numgroups);

	m_numMeshes = numgroups;

	temp_vlist = new vec3[MAX_VERTS];
	temp_vlist_x = new vec3[MAX_VERTS];
	temp_tlist = new vec2[MAX_VERTS];
	temp_tlist_x = new vec2[MAX_VERTS];
	temp_nlist = new vec3[MAX_VERTS];
	temp_nlist_x = new vec3[MAX_VERTS];
	temp_flist = new int[MAX_FACES*3];
	orig_flist = new int[MAX_FACES*3];
	muthas = new int[MAX_FACES*2];

	// reset
	for (int x = 0; x<MAX_VERTS*MAX_ATTACH; x++) 
	{
		hasataulu[x].vi = hasataulu[x].ti = hasataulu[x].ni = -1;
	}
	orig_fpos = 0;
	muthapos = 0;
	vertexpos_x = 0;
	texturepos_x = 0;
 	vertexpos = 0;
	facepos = 0;
	normalpos_x = 0;

	while (1) 
	{
		if (!getline(m_file->getData(), rivi, &datapos, m_file->getLenght())) break;
		linenum++;
		substr(rivi, subrivi, 2);

		if (strcmp(subrivi, "v ") == 0) 
		{
			float xx=0, yy=0, zz=0;

			// next group, maybe, if not then fuck OBJ
			if (current_part == FACEPART) 
			{   
   				Mesh* mesh = new Mesh(temp_vlist, texturepos_x ? temp_tlist : 0, normalpos_x ? temp_nlist : 0, 
   					temp_flist, vertexpos, facepos, muthapos ? orig_flist : 0, muthapos ? muthas : 0, muthapos ? facepos : 0, muthapos);
				addMesh(mesh);
      
				indexminus_v += vertexpos_x;
				indexminus_t += texturepos_x;
				indexminus_n += normalpos_x;

				// reset
				for (int x=0; x<MAX_VERTS*MAX_ATTACH; x++) 
				{
					hasataulu[x].vi=hasataulu[x].ti=hasataulu[x].ni = -1;
				}
				
				orig_fpos = 0;
				muthapos = 0;
				vertexpos_x = 0;
				texturepos_x = 0;
				vertexpos = 0;
				facepos = 0;
				normalpos_x = 0;
				
				current_part = VERTEXPART;
				current_group++;

				// data can be interleaved, we don't support that kind of shit				
				assert(current_group < numgroups);
			}

			sscanf(rivi, "v %f %f %f", &xx, &yy, &zz);
			temp_vlist_x[vertexpos_x].x = xx;
			temp_vlist_x[vertexpos_x].y = yy;
			temp_vlist_x[vertexpos_x].z = zz;
			vertexpos_x++;
			
			assert(vertexpos_x < MAX_VERTS);

		} 
		else if (strcmp(subrivi, "vt") == 0) 
		{
			// texcoord
			float uu=0, vv=0;

			current_part = TEXCOORDPART;
			sscanf(rivi, "vt %f %f", &uu, &vv);
			temp_tlist_x[texturepos_x].x = uu;
			temp_tlist_x[texturepos_x].y = vv;
			texturepos_x++;
			
			assert(texturepos_x < MAX_VERTS);
      
		} 
		else if (strcmp(subrivi, "vn") == 0) 
		{
			// normal
			float nx=0, ny=0, nz=0;

			current_part = NORMALPART;
			sscanf(rivi, "vn %f %f %f", &nx, &ny, &nz);
			temp_nlist_x[normalpos_x].x = nx;
			temp_nlist_x[normalpos_x].y = ny;
			temp_nlist_x[normalpos_x].z = nz;
			normalpos_x++;
			
			assert(normalpos_x < MAX_VERTS);

		} 
		else if (strcmp(subrivi, "f ") == 0) 
		{
			// face
			static int v1, v2, v3;
			static int t1, t2, t3;
			static int n1, n2, n3;
			static int vv1, vv2, vv3;

			current_part = FACEPART;			

			if (texturepos_x && normalpos_x) // texturecoords+normals
			{ 
				sscanf(rivi, "f %i/%i/%i %i/%i/%i %i/%i/%i", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			} 
			else if (texturepos_x)  // texcoords
			{
				sscanf(rivi, "f %i/%i %i/%i %i/%i", &v1, &t1, &v2, &t2, &v3, &t3);
				n1 = n2 = n3 = indexminus_n;
			} 
			else if (normalpos_x)  // normals
			{
				sscanf(rivi, "f %i//%i %i//%i %i//%i", &v1, &n1, &v2, &n2, &v3, &n3);
				t1 = t2 = t3 = indexminus_t;
			} 
			else // just vertices
			{
				sscanf(rivi, "f %i %i %i", &v1, &v2, &v3);
				t1 = t2 = t3 = indexminus_t;
				n1 = n2 = n3 = indexminus_n;
			}
      
			v1 -= indexminus_v;
			v2 -= indexminus_v;
			v3 -= indexminus_v;
			t1 -= indexminus_t;
			t2 -= indexminus_t;
			t3 -= indexminus_t;
			n1 -= indexminus_n;
			n2 -= indexminus_n;
			n3 -= indexminus_n;

			assert(v1 >= 0 && v2 >= 0 && v3 >= 0);
			assert(v1 < vertexpos_x && v2 < vertexpos_x && v3 < vertexpos_x);

			if (texturepos_x) 
			{
				assert(t1 >= 0 && t2 >= 0 && t3 >= 0);
				assert(t1 < texturepos_x && t2 < texturepos_x && t3 < texturepos_x);
			}
			
			if (normalpos_x) 
			{
				assert(n1 >= 0 && n2 >= 0 && n3 >= 0);
				assert(n1 < normalpos_x && n2 < normalpos_x && n3 < normalpos_x);
			}
						
			vv1 = new_vertex(v1, t1, n1, temp_vlist, temp_tlist, temp_nlist,
					temp_vlist_x, temp_tlist_x, temp_nlist_x, &vertexpos, muthas, &muthapos);
			vv2 = new_vertex(v2, t2, n2, temp_vlist, temp_tlist, temp_nlist,
					temp_vlist_x, temp_tlist_x, temp_nlist_x, &vertexpos, muthas, &muthapos);
			vv3 = new_vertex(v3, t3, n3, temp_vlist, temp_tlist, temp_nlist,
					temp_vlist_x, temp_tlist_x, temp_nlist_x, &vertexpos, muthas, &muthapos);

			assert(vv1 >= 0 && vv2 >= 0 && vv3 >= 0);
			assert(vv1 < MAX_VERTS && vv2 < MAX_VERTS && vv3 < MAX_VERTS);
			assert(orig_fpos >= 0 && orig_fpos < MAX_FACES);

			int i1, i2, i3;
			i1 = hasataulu[v1 * MAX_ATTACH].newind;
			i2 = hasataulu[v2 * MAX_ATTACH].newind;
			i3 = hasataulu[v3 * MAX_ATTACH].newind;
						
			assert(i1 >= 0 && i2 >= 0 && i3 >= 0);
			assert(i1 < MAX_FACES && i2 < MAX_FACES && i3 < MAX_FACES);

			orig_flist[orig_fpos * 3 + 0] = i1;
			orig_flist[orig_fpos * 3 + 1] = i2;
			orig_flist[orig_fpos * 3 + 2] = i3;
			orig_fpos++;

			temp_flist[facepos * 3 + 0] = vv1;
			temp_flist[facepos * 3 + 1] = vv2;
			temp_flist[facepos * 3 + 2] = vv3;
			facepos++;
			assert(facepos < MAX_FACES);

		}
	}

	Mesh* mesh = new Mesh(temp_vlist, texturepos_x ? temp_tlist : 0, normalpos_x ? temp_nlist : 0, 
		temp_flist, vertexpos, facepos, muthapos ? orig_flist : 0, muthapos ? muthas : 0, muthapos ? facepos : 0, muthapos);
	addMesh(mesh);

	delete[] temp_vlist_x;
	delete[] temp_vlist;
	delete[] temp_flist;
	delete[] temp_tlist;
	delete[] temp_tlist_x;
	delete[] temp_nlist;
	delete[] temp_nlist_x;
	delete[] orig_flist;
	delete[] muthas;
}
