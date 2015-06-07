#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"

class Material
{
public:
    Material();
    virtual ~Material();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, HitInfo& hit,
                          Scene& scene, int depth = 0) const;
   
  Vector3 m_kd; //diffuse component
  Vector3 m_ka; //ambient compent
  Vector3 m_ks; //specular compoenet
  //1-kd then reflective.
  float m_refl; //reflection
  float m_refr; //refractive
  float m_sh = 0.0f; //shiny
  bool isFloor;
};

#endif // CSE168_MATERIAL_H_INCLUDED
