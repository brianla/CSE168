#ifndef CSE168_TRIANGLE_MESH_H_INCLUDED
#define CSE168_TRIANGLE_MESH_H_INCLUDED

#include "Matrix4x4.h"
#include <unordered_map>

class TriangleMesh
{
public:
    TriangleMesh();
    ~TriangleMesh();

    // load from an OBJ file
    bool load(char* file, const Matrix4x4& ctm = Matrix4x4());

    // for single triangles
    void createSingleTriangle();
    inline void setV1(const Vector3& v) {m_vertices[0] = v;}
    inline void setV2(const Vector3& v) {m_vertices[1] = v;}
    inline void setV3(const Vector3& v) {m_vertices[2] = v;}
    inline void setN1(const Vector3& n) {m_normals[0] = n;}
    inline void setN2(const Vector3& n) {m_normals[1] = n;}
    inline void setN3(const Vector3& n) {m_normals[2] = n;}

    struct TupleI3
    {
        unsigned int x, y, z;
    };

    struct VectorR2
    {
        float x, y;
    };

	struct TexImage
	{
		int width, height;
		std::vector<Vector3*> image;
	};

    Vector3* vertices()     {return m_vertices;}
    Vector3* normals()      {return m_normals;}
	VectorR2* texCoords()	{return m_texCoords;}
    TupleI3* vIndices()     {return m_vertexIndices;}
    TupleI3* nIndices()     {return m_normalIndices;}
	TupleI3* tIndices()		{return m_texCoordIndices;}
	int numTris()           { return m_numTris; }

	std::unordered_map<std::string, int> texMap() { return m_textureMap; }
	std::vector<TexImage*> texImage() { return m_texImages; }
	std::vector<int> triToTex()	{ return m_triToTexture; }

protected:
    void loadObj(FILE* fp, const Matrix4x4& ctm);
	void decodeTwoSteps(const char* filename, int& width, int& height, std::vector<Vector3*>& image);

    Vector3* m_normals;
    Vector3* m_vertices;
    VectorR2* m_texCoords;

    TupleI3* m_normalIndices;
    TupleI3* m_vertexIndices;
    TupleI3* m_texCoordIndices;
    unsigned int m_numTris;

	std::unordered_map<std::string, int> m_textureMap;	// file names to index of texture images
	std::vector<TexImage*> m_texImages;			// texture images
	std::vector<int> m_triToTexture;			// which triangles belong to which image
};


#endif // CSE168_TRIANGLE_MESH_H_INCLUDED
