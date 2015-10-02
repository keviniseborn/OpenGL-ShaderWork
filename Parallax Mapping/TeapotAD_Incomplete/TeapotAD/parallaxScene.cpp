
#include "parallaxScene.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#include "defines.h"

using glm::vec3;


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>


namespace imat3111
{
/////////////////////////////////////////////////////////////////////////////////////////////
// Default constructor
/////////////////////////////////////////////////////////////////////////////////////////////
ParallaxScene::ParallaxScene()
{	

	//load bitmaps and assign them to relevant textures
	Bitmap bmpDiff = Bitmap::bitmapFromFile("cobblesDiffuse.png");
	gTextureDiffuse = new Texture(bmpDiff);

	Bitmap bmpNorm = Bitmap::bitmapFromFile("cobblesNormal.png");
	gTextureNormal = new Texture(bmpNorm);

	Bitmap bmpHeight = Bitmap::bitmapFromFile("cobblesHeight.png");
	gTextureHeight = new Texture(bmpHeight);

	Bitmap bmpSpec = Bitmap::bitmapFromFile("cobblesSpecular.png");
	gTextureSpecular = new Texture(bmpSpec);

	
	//Create the plane to represent the ground
	plane = new VBOMesh("plane.obj", false, true, true);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Initialise the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::initScene(Camera camera)
{
    //|Compile and link the shader  
	compileAndLinkShader();

	gl::Enable(gl::TEXTURE_2D);

	GLfloat pScale = -0.02;
	prog.setUniform("parallaxScale", pScale);
    gl::Enable(gl::DEPTH_TEST);

 	//Set up the lighting
	setLightParams(camera);

	//A matrix to move the teapot lid upwards
	glm::mat4 lid = glm::mat4(1.0);
	lid *= glm::translate(vec3(0.0,1.0,0.0));

	//Create the teapot with translated lid
	teapot = new VBOTeapot(16,lid);

		
	GLint loc;
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D,gTextureDiffuse->object());
	loc = gl::GetUniformLocation(prog.getHandle(),"texDif");
	gl::Uniform1i(loc, 0);

	gl::ActiveTexture(gl::TEXTURE1);
	gl::BindTexture(gl::TEXTURE_2D,gTextureNormal->object());
	loc = gl::GetUniformLocation(prog.getHandle(),"texNorm");
	gl::Uniform1i(loc, 1);

	gl::ActiveTexture(gl::TEXTURE2);
	gl::BindTexture(gl::TEXTURE_2D,gTextureHeight->object());
	loc = gl::GetUniformLocation(prog.getHandle(),"texHeight");
	gl::Uniform1i(loc, 2);

	gl::ActiveTexture(gl::TEXTURE3);
	gl::BindTexture(gl::TEXTURE_2D,gTextureSpecular->object());
	gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGBA, width, height, 0, gl::RGBA, gl::UNSIGNED_BYTE, NULL);
	loc = gl::GetUniformLocation(prog.getHandle(),"texSpec");
	gl::Uniform1i(loc, 3);
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Update not used at present
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::update( float t )
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::setLightParams(Camera camera)
{

	vec3 worldLight = vec3(10.0f, 10.0f, 10.0f);
	vec3 spotPosition = vec3(10.0f, 10.0f, 10.0f);
	vec3 lightTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 lightIntensity = vec3(1.0f, 1.0f, 1.0f);
	float coneAngle = 90.f;

    prog.setUniform("Ld", 1.0f, 1.0f, 1.0f);
  //  prog.setUniform("LightPosition", camera.view() * vec4(worldLight,1.0) );
	prog.setUniform("LightPosition", worldLight );

	prog.setUniform("SpotPosition", spotPosition);
	prog.setUniform("LightTarget", lightTarget);
	prog.setUniform("LightIntensity", lightIntensity);
	prog.setUniform("ConeAngle", coneAngle);


}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::render(Camera camera)
{
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	//First deal with the plane to represent the ground

	//Initialise the model matrix for the plane
	model = mat4(1.0f);
	//Set the matrices for the plane although it is only the model matrix that changes so could be made more efficient
    setMatrices(camera);
	//Set the plane's material properties in the shader and render
	prog.setUniform("Kd", 0.4f, 0.7f, 0.4f);
	prog.setUniform("Ka", 0.2f, 0.2f, 0.2f);
	prog.setUniform("Ls", 0.3f, 0.3f, 0.3f);
	prog.setUniform("Ks", 0.3f, 0.3f, 0.3f);


	

	plane->render(); 
}


/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::setMatrices(Camera camera)
{

    mat4 mv = camera.view() * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", camera.projection() * mv);
	// the correct matrix to transform the normal is the transpose of the inverse of the M matrix
	  mat3 normMat = glm::transpose(glm::inverse(mat3(model)));
	prog.setUniform("M", model);
	//prog.setUniform("NormalMatrix", normMat);
	prog.setUniform("V", camera.view() );
	prog.setUniform("P", camera.projection() );

	prog.setUniform("CamPos", camera.position() );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize the viewport
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::resize(Camera camera,int w, int h)
{
    gl::Viewport(0,0,w,h);
    width = w;
    height = h;
	camera.setAspectRatio((float)w/h);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compile and link the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void ParallaxScene::compileAndLinkShader()
{
   
	try {
    	prog.compileShader("diffuse.vert");
    	prog.compileShader("diffuse.frag");
    	prog.link();
    	prog.validate();
    	prog.use();
    } catch(GLSLProgramException & e) {
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}

}
