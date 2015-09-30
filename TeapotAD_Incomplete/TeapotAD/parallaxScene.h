#ifndef PARALLAXSCENE_H
#define PARALLAXSCENE_H

#include "gl_core_4_3.hpp"

#include <GLFW/glfw3.h>
#include "scene.h"
#include "glslprogram.h"

#include "vboteapot.h"
#include "vboplane.h"

#include "VBOMesh.h"
#include "Texture.h"

#include <glm/glm.hpp>

using glm::mat4;


namespace imat3111
{

class ParallaxScene : public Scene
{
private:
    GLSLProgram prog;

    int width, height;
 
	VBOTeapot *teapot;  //Teapot VBO
	VBOMesh *plane;  //Plane VBO

	VBOMesh *ogre;
	Texture *gTextureDiffuse;
	Texture *gTextureNormal;
	Texture *gTextureHeight;
	Texture *gTextureSpecular;
	
	GLuint programHandle;

    mat4 model; //Model matrix


    void setMatrices(Camera camera); //Set the camera matrices

    void compileAndLinkShader(); //Compile and link the shader

public:
    ParallaxScene(); //Constructor

	void setLightParams(Camera camera); //Setup the lighting

    void initScene(Camera camera); //Initialise the scene

    void update( float t ); //Update the scene

    void render(Camera camera);	//Render the scene

    void resize(Camera camera, int, int); //Resize
};

}

#endif // ParallaxScene_H
