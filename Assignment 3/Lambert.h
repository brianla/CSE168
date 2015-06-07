#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"
#include "Camera.h"
const int MAXDEPTH = 5;
const bool DO_SHADOW = true;

class Lambert : public Material
{
public:
  Lambert(const Vector3 & kd = Vector3(1),
    const Vector3 & ka = Vector3(0),
    float refl = 0.0f,
    float refr = 0.0f,
    Vector3 specular = Vector3(0),
    float sh = 0.0f, bool fl = false);

    virtual ~Lambert();
    Camera camera;

    const Vector3 & kd() const {return m_kd;}
    const Vector3 & ka() const {return m_ka;}

    void setKd(const Vector3 & kd) {m_kd = kd;}
    void setKa(const Vector3 & ka) {m_ka = ka;}

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, HitInfo& hit,
                          Scene& scene, int depth = 0) const;
    
  Vector3 phongModel(PointLight pLight, Ray ray, HitInfo hit) const;
  Vector3 specularRefraction(Ray ray, HitInfo &hit, Scene& scene, int depth) const;
  Vector3 specularReflection(Ray ray, HitInfo hit, Scene& scene, int depth) const;

  Vector3 raytraceRecurssion(Ray ray, HitInfo hit, Scene& scene, int depth ) const;
protected:
    Vector3 m_kd;
    Vector3 m_ka;
};

#endif // CSE168_LAMBERT_H_INCLUDED
