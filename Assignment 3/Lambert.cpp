#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "Worley.h"
#include "Perlin.h"
#include <algorithm>

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka, float refl, float refr, Vector3 specular, float sh, bool fl) :
m_kd(kd), m_ka(ka)
{
  m_refl = refl;
  m_refr = refr;
  m_ks = specular;
  m_sh = sh;
  isFloor = fl;
//  m_refr = 1.0f - de - refl;
}

Lambert::~Lambert()
{
}
using namespace std;


Vector3 Lambert::phongModel(PointLight pLight, Ray ray, HitInfo hit) const
{
  // phong model
  Vector3 l = pLight.position() - hit.P;

  // the inverse-squared falloff
  float falloff = l.length2();

  // normalize the light direction
  l /= sqrt(falloff);
  float nDotL = dot(hit.N, l);
  float nDotR = dot(hit.N, ray.d);
  // Calculate specular component
  Vector3 reflect = ray.d - (hit.N * (2.0f * nDotR));

  Vector3 specResult = pLight.color();
  specResult *= m_ks;
  
  if (dot(l, reflect)> 0)
    return powf(dot(reflect, l), m_sh) / nDotL * std::max(0.0f, nDotL / falloff * pLight.wattage() / PI)  * specResult;
  else
    return Vector3(0);
}

Vector3 Lambert::specularReflection(Ray ray, HitInfo hit, Scene& scene, int depth) const
{
  {
    //if its coming in then it is negative. going out it is positive
    //for each secondary rays
    Vector3 reflect = ray.d - (hit.N * (2.0f * dot(ray.d, hit.N)));
    reflect.normalize();

    Vector3 origin = hit.P + reflect * 0.001f;

    Ray secondaryRay(origin, reflect);
    HitInfo hitInfo;
    //find intersect
    if (scene.trace(hitInfo, secondaryRay) == false)
    {
      return scene.getBGColor()*m_refl;
    }
    return hitInfo.material->shade(secondaryRay, hitInfo, scene, depth + 1)*m_refl;
  }
  // add the ambient component
}


Vector3 Lambert::specularRefraction(Ray ray, HitInfo &hit, Scene& scene, int depth ) const
{
    float epislon = 0.001f;
    float const WATER = 1.3333f;
    float const AIR = 1.0003f;
    /* Work out the correct n1 and n2 depending on the incident vector's direction relative to the normal. */

    float dDotN = dot(ray.d, hit.N);
    float n1, n2;
    Vector3 origin = hit.P;
    if (dDotN > 0)
    {
      n1 = WATER;
      n2 = AIR;

      hit.N= -hit.N;
    }
    else
    {
      n2 = WATER;
      n1 = AIR;
    }

    //calculation for totla internal reflection check
    float z = pow(n1 / n2, 2.0f) * (1.0f - pow(dDotN, 2.0f));

    // Check for total internal reflection. 
    if (z > 1.0f)
    {
      // Total internal reflection occurred. 
      origin = origin + hit.N * epislon;
      //didn't trace a ray
      return specularReflection(ray,hit,scene,depth) * m_refr;
    }
    else
    {
      z = 1.0f - z;
      float t = sqrt(z);
      
      // Refraction. 
      origin = origin - hit.N * epislon;
      Vector3 refractRay = ray.d * (n1 / n2) + hit.N * ((n1 / n2) * dDotN - t);

      Ray secondaryRay(origin, refractRay);
      HitInfo hitInfo;
      //find intersect
      if (scene.trace(hitInfo, secondaryRay) == false)
      {
        return scene.getBGColor()*m_refr;
      }
      return hitInfo.material->shade(secondaryRay, hitInfo, scene, depth + 1)*m_refr;
    }
}

Vector3 Lambert::raytraceRecurssion(Ray ray, HitInfo hit, Scene& scene, int depth) const
{
  Vector3 L = Vector3(0);
  if (depth == MAXDEPTH)
    return L;
  HitInfo hitInfo = hit;
  //float contribution = 1.0f - m_de;
  if (m_refr > 0)
    L += specularRefraction(ray, hitInfo, scene, depth);
  if (m_refl > 0)
    L += specularReflection(ray, hitInfo, scene, depth);
  return L;
}

  Vector3 marbleTexture(Vector3 point, Vector3 color)
  {
    float sc = 30.0f;

    float x = point.x * sc;
    float y = point.y * sc;
    float z = point.z * sc;
    float noiseCoef = 0;

    for (int level = 1; level < 10; level++) {
      noiseCoef += (1.0f / level) * fabsf(PerlinNoise::noise(
        level * 0.05 * x,
        level * 0.05 * y,
        level * 0.05 * z
        ));
    }
    noiseCoef = 0.5f * sinf((x + y) * 0.05f + noiseCoef) + 0.5f;

    return color * noiseCoef;
  }

  float InvSqrt(float x)
  {
    float xhalf = 0.5f*x;
    int i = *(int*)&x; // get bits for floating value
    i = 0x5f375a86 - (i >> 1); // gives initial guess y0
    x = *(float*)&i; // convert bits back to float
    x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
    return x;
  }


Vector3
Lambert::shade(const Ray& ray, HitInfo& hit, Scene& scene, int depth) const 
{
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

	const Vector3 viewDir = -ray.d; // d is a unit vector

	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
	{
		PointLight* pLight = *lightIter;

		Vector3 l = pLight->position() - hit.P;

		// the inverse-squared falloff
		float falloff = l.length2();

		// normalize the light direction
		l /= sqrt(falloff);

		// get the diffuse component
		float nDotL = dot(hit.N, l);
		Vector3 result = pLight->color();

		Vector3 diffuse;

		if (hit.T == NULL) {
			diffuse = m_kd;
		}
		else {
			diffuse = *hit.T;
		}
		result *= diffuse;

		L += std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result;
	}

	// add the ambient component
	L += m_ka;

	return L;

  //  Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
  //  Vector3 viewDir = -ray.d; // d is a unit vector
  //  
  //  Vector3 floor = m_kd;
  //  if (isFloor)
  //  {
  //    long maxOrder = 5;
  //    float at[3] = { hit.P[0], hit.P[1], hit.P[2] };
  //    float *F = new float[maxOrder];
  //    float(*delta)[3] = new float[maxOrder][3];
  //    unsigned long *ID = new unsigned long[maxOrder];

  //    WorleyNoise::noise3D(at, maxOrder, F, delta, ID);
  //    floor = Vector3(F[1] - F[0]);


  //  }
  //  const Lights *lightlist = scene.lights();
  //  
  //  // loop over all of the lights
  //  Lights::const_iterator lightIter;
  //  for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
  //  {
  //      PointLight* pLight = *lightIter;
  //  
  //      Vector3 l = pLight->position() - hit.P;

  //      // the inverse-squared falloff
  //      float falloff = l.length2();
  //      
  //      // normalize the light direction
  //      l /= sqrt(falloff);

  //   if (pLight->wattage() == 0 || dot(hit.N, l) <= 0)
  //        continue;
  //      //Find if it is in shadow or not
  //      HitInfo shHitInfo;
  //      //Create shadow ray towards light
  //      //find intersect
		//if (DO_SHADOW && scene.trace(shHitInfo, Ray(hit.P + l * 0.001f, l)))
  //      {
  //        continue;
  //      }
  //        // get the diffuse component
  //        float nDotL = dot(hit.N, l);
  //        Vector3 result = pLight->color();

		//  Vector3 diffuse;

		//  if (hit.T == NULL) {
		//	  diffuse = m_kd;
		//  }
		//  else {
		//	  diffuse = *hit.T;
		//  }

  //        if (isFloor && floor.x > 0.03f)
  //        {
  //          result += floor;
		//	result *= marbleTexture((hit.P + floor), diffuse);
  //        }
  //        else
		//	  result *= diffuse;

  //       L += std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result;
  //       L += phongModel(*pLight, ray, hit);
  //  }
  //  // add the ambient component
  //  L += m_ka;
  //  L += raytraceRecurssion(ray, hit, scene, depth);
  //  return L;
}

