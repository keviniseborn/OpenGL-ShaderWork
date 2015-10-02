#ifndef SCENEDEFERRED_H
#define SCENEDEFERRED_H

#include "gl_core_4_3.hpp"

#include <GLFW/glfw3.h>
#include "scene.h"
#include "glslprogram.h"

#include "vboplane.h"
#include "vboteapot.h"
#include "vbomesh.h"


#include <glm/glm.hpp>
using glm::mat4;

namespace imat3111
{

class SceneDeferred : public Scene
{
private:
    GLSLProgram prog;

    int width, height;
    GLuint deferredFBO;
    GLuint quad, pass1Index, pass2Index;

    VBOPlane *plane;
    VBOMesh *ogre;
    VBOTeapot *teapot;

	mat4 model;

    void setMatrices(Camera camera);
    void compileAndLinkShader();
    void setupFBO();
    void pass1(Camera camera);
    void pass2(Camera camera);

public:
    SceneDeferred();


	void setLightParams(Camera camera); //Setup the lighting

    void initScene(Camera camera); //Initialise the scene

    void update( float t ); //Update the scene

    void render(Camera camera);	//Render the scene

    void resize(Camera camera, int, int); //Resize

};
}

#endif // SCENEDEFERRED_H