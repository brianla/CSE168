#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include "PointLight.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "MiroWindow.h"

#include "assignment2.h"

void
makeSpiralScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
    g_camera->setEye(Vector3(-3,15,3));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-10, 15, 5));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(250);
    g_scene->addLight(light);

    // create a spiral of spheres
    Material* mat = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
    const int maxI = 200;
    const float a = 0.15f;
    for (int i = 1; i < maxI; ++i)
    {
        float t = i/float(maxI);
        float theta = 4*PI*t;
        float r = a*theta;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float z = 2*(2*PI*a - r);
        Sphere * sphere = new Sphere;
        sphere->setCenter(Vector3(x,y,z));
        sphere->setRadius(r/10);
        sphere->setMaterial(mat);
        g_scene->addObject(sphere);
    }
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}


void setCamera(Scene * g_scene)
{
  g_camera = new Camera;
  // set up the camera
  Vector3 bgColor(0.0f, 0.0f, 0.2f);
  g_camera->setBGColor(bgColor);
  g_camera->setEye(Vector3(7, 8, 5));
  g_camera->setLookAt(Vector3(0.5f, 1, 0));
  g_camera->setUp(Vector3(0, 1, 0));
  g_camera->setFOV(45);
  g_scene->setBGColor(bgColor);

}

void setLights(Scene * g_scene)
{
  // create and place a point light source
  PointLight * light = new PointLight;
  light->setPosition(Vector3(-8, 10, 0));
  light->setColor(Vector3(1, 1, 1));
  light->setWattage(500);
  g_scene->addLight(light);

  /*
  PointLight * light2 = new PointLight;
  light2->setPosition(Vector3(5, 18, 3));
  light2->setColor(Vector3(1, 1, 1));
  light2->setWattage(1000);
  g_scene->addLight(light2);
  */
}

void makeFloor(Scene * g_scene)
{
  // create the floor triangle
  TriangleMesh * floor = new TriangleMesh;
  floor->createSingleTriangle();
  floor->setV1(Vector3(0,  0, 10));
  floor->setV2(Vector3(10, 0, -10));
  floor->setV3(Vector3(-10, 0, -10));
  floor->setN1(Vector3(0, 1, 0));
  floor->setN2(Vector3(0, 1, 0));
  floor->setN3(Vector3(0, 1, 0));

  Triangle* t = new Triangle;
  t->setIndex(0);
  t->setMesh(floor);
  Material * floorMat = new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0), 1.0, 0.0f, Vector3(0.0, 0.0, 0.0), 0, true);
  t->setMaterial(floorMat);
  g_scene->addObject(t);
}

void makeBunnyScene(Scene * g_scene)
{
    Material* mat = new Lambert(Vector3(0.8f,0.4f,0.0f), Vector3(0),0.0f,0.0f,Vector3(0.0f,0.0f,0.0f),0);
    Matrix4x4 transform;
    transform.setColumn4(-1.0f, -0.5f, -1.5f, 1.0f);

    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("bunny.obj", transform);
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < bunny->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(bunny);
        t->setMaterial(mat); 
        g_scene->addObject(t);
    }
}

void makeSphereScene(Scene * g_scene)
{
  Material* mat = new Lambert(Vector3(0.00f, 0.00f, 0.00f), Vector3(0), 0.0f, 1.0f, Vector3(0), 0);
  Matrix4x4 transform;
  transform.setColumn4(-0.5f, 1.0f, 2.4f, 1.0f);

  TriangleMesh * sphere = new TriangleMesh;
  sphere->load("sphere.obj", transform);
  // create all the triangles in the bunny mesh and add to the scene
  for (int i = 0; i < sphere->numTris(); ++i)
  {
    Triangle* t = new Triangle;
    t->setIndex(i);
    t->setMesh(sphere);
    t->setMaterial(mat);
    g_scene->addObject(t);
  }
}
void makeTeapotScene(Scene * g_scene)
{
  Material* mat = new Lambert(Vector3(0.0f, 0.5f, 0.5f), Vector3(0), 0.0f, 0.0f, Vector3(1.0f, 0.9f, 0.3f), 30);

  Matrix4x4 transform;
  transform.setColumn4(1.75f, 0.0f, -1.0f, 1.0f);

  TriangleMesh * teapot = new TriangleMesh;
  teapot->load("teapot.obj", transform);
  // create all the triangles in the teapot mesh and add to the scene
  for (int i = 0; i < teapot->numTris(); ++i)
  {
    Triangle* t = new Triangle;
    t->setIndex(i);
    t->setMesh(teapot);
    t->setMaterial(mat);
    g_scene->addObject(t);
  }
}

int
main(int argc, char*argv[])
{
  //  // create a scene
  //  //makeSpiralScene();
  //g_scene = new Scene;
  //g_image = new Image;
  //g_image->resize(512, 512);
  //
  //setCamera(g_scene);
  //setLights(g_scene);
  //makeFloor(g_scene);
  //makeBunnyScene(g_scene);
  //makeSphereScene(g_scene);
  //makeTeapotScene(g_scene);

  ///*
  //Material* matSp = new Lambert(Vector3(0.00f, 0.00f, 0.00f), Vector3(0), 0.0f, 1.0f, Vector3(0), 0);
  //Sphere * sphere = new Sphere;
  //sphere->setCenter(Vector3(-1, 2, 1));
  //sphere->setRadius(0.5f);
  //sphere->setMaterial(matSp);
  //g_scene->addObject(sphere);
  //*/

  //// let objects do pre-calculations if needed
  //g_scene->preCalc();

	//makeTeapot1Scene();
	//makeBunny20Scene();
	//makeBunny1Scene();
	makeMonsterScene();
	//makeSponzaScene();

    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

