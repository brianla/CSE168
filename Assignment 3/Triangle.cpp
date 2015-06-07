#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"
#include <iomanip>
#include <algorithm> 

Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}

Vector3 Triangle::min()
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	float x = std::min(std::min(v0.x, v1.x), v2.x);
	float y = std::min(std::min(v0.y, v1.y), v2.y);
	float z = std::min(std::min(v0.z, v1.z), v2.z);

	return Vector3(x, y, z);
}

Vector3 Triangle::max()
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	float x = std::max(std::max(v0.x, v1.x), v2.x);
	float y = std::max(std::max(v0.y, v1.y), v2.y);
	float z = std::max(std::max(v0.z, v1.z), v2.z);

	return Vector3(x, y, z);

}

Vector3 Triangle::center()
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	return (v0 + v1 + v2) / 3.0f;
}

void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r,float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v_a = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v_b = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v_c = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	const Vector3 v_oa = r.o - v_a;			// OA
	const Vector3 v_ab = v_b - v_a;			// AB
	const Vector3 v_ac = v_c - v_a;			// AC

	float det_A = determinant(-r.d, v_ab, v_ac);

	// TODO: Check if this equality works
	if (det_A < 0.00001f && det_A > -0.00001f) return false;
  
	float t = determinant(v_oa, v_ab, v_ac) / det_A;
 // std::cerr << std::setprecision(9) << std::showpoint << std::fixed;
 // std::cerr << t<< std::endl;
  if (t< tMin || t > tMax)
    return false;

	float alpha = determinant(-r.d, v_oa, v_ac) / det_A;
	float beta = determinant(-r.d, v_ab, v_oa) / det_A;

	// Check if ray intersects
	if ((alpha >= 0.000001f) && (beta >= 0.000001f) && (alpha + beta <= 1.0f)) {

		result.t = t;
		result.P = r.o + result.t*r.d;

		// TODO: Barycentric interpolation
		//result.N = cross(v_ab, v_ac);

		TriangleMesh::TupleI3 ni3 = m_mesh->nIndices()[m_index];
		const Vector3 & n_a = m_mesh->normals()[ni3.x]; //normal a of triangle
		const Vector3 & n_b = m_mesh->normals()[ni3.y]; //normal b of triangle
		const Vector3 & n_c = m_mesh->normals()[ni3.z]; //normal c of triangle

		float delta = 1.0f - alpha - beta;

		result.N = n_a * delta + n_b * alpha + n_c * beta;
		result.N.normalize();
		result.material = this->m_material;

		if (m_mesh->texImage().size() == 0) {
			result.T = NULL;
		}
		else {
			int texImageIndex = m_mesh->triToTex().at(m_index);
			TriangleMesh::TexImage* texImage = m_mesh->texImage().at(texImageIndex);

			TriangleMesh::TupleI3 ti2 = m_mesh->tIndices()[m_index];
			const TriangleMesh::VectorR2 & t_a = m_mesh->texCoords()[ti2.x]; //texture point a of triangle
			const TriangleMesh::VectorR2 & t_b = m_mesh->texCoords()[ti2.y]; //texture point b of triangle
			const TriangleMesh::VectorR2 & t_c = m_mesh->texCoords()[ti2.z]; //texture point c of triangle
			
			float u = t_a.x * delta + t_b.x * alpha + t_c.x * beta;
			float v = t_a.y * delta + t_b.y * alpha + t_c.y * beta;
			
			int width = texImage->width;
			int height = texImage->height;
			float x_pos = u * width;
			float y_pos = v * height;

			y_pos = floor(y_pos + 0.5f);
			float loc = y_pos * width + x_pos;

			result.T = texImage->image.at(floor(loc + 0.5f));
		}

		return true;
	}

	// The ray does not intersect
    return false;
}
