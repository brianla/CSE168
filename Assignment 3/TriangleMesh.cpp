#include "TriangleMesh.h"
#include "Triangle.h"
#include "Scene.h"


TriangleMesh::TriangleMesh() :
    m_normals(0),
    m_vertices(0),
    m_texCoords(0),
    m_normalIndices(0),
    m_vertexIndices(0),
    m_texCoordIndices(0)
{

}

TriangleMesh::~TriangleMesh()
{
    delete [] m_normals;
    delete [] m_vertices;
    delete [] m_texCoords;
    delete [] m_normalIndices;
    delete [] m_vertexIndices;
    delete [] m_texCoordIndices;

	for (int i = 0; i < m_texImages.size(); i++) {
		std::vector<Vector3*> tex = m_texImages[i]->image;
		for (int j = 0; j < tex.size(); j++) {
			delete tex[j];
		}

		delete m_texImages[i];
	}
}
