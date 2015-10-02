#include "scenedeferred.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#include "glutils.h"
#include "defines.h"

using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

namespace imat3111
{


SceneDeferred::SceneDeferred() : width(800), height(600) { }


void SceneDeferred::initScene(Camera camera)
{
    compileAndLinkShader();

    gl::Enable(gl::DEPTH_TEST);

	//Set up lighting
	setLightParams(camera);

	
	//Create the plane to represent the ground
	plane = new VBOPlane(10.0,10.0,10,10);


	//A matrix to move the teapot lid upwards
	glm::mat4 lid = glm::mat4(1.0);
	lid *= glm::translate(vec3(0.0,1.0,0.0));

	//Create the teapot with translated lid
	teapot = new VBOTeapot(16,lid);



    // Array for quad to which to render final texture
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    gl::GenBuffers(2, handle);

    gl::BindBuffer(gl::ARRAY_BUFFER, handle[0]);
    gl::BufferData(gl::ARRAY_BUFFER, 6 * 3 * sizeof(float), verts,gl::STATIC_DRAW);

    gl::BindBuffer(gl::ARRAY_BUFFER, handle[1]);
    gl::BufferData(gl::ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, gl::STATIC_DRAW);

    // Set up the vertex array object
    gl::GenVertexArrays( 1, &quad );
    gl::BindVertexArray(quad);

    gl::BindBuffer(gl::ARRAY_BUFFER, handle[0]);
    gl::VertexAttribPointer( (GLuint)0, 3, gl::FLOAT, FALSE, 0, ((GLubyte *)NULL + (0)) );
    gl::EnableVertexAttribArray(0);  // Vertex position

    gl::BindBuffer(gl::ARRAY_BUFFER, handle[1]);
    gl::VertexAttribPointer( (GLuint)2, 2, gl::FLOAT, FALSE, 0, ((GLubyte *)NULL + (0)) );
    gl::EnableVertexAttribArray(2);  // Texture coordinates

    gl::BindVertexArray(0);

    setupFBO(); //Set up framebuffer object(s)

    // Set up the subroutine indexes
    GLuint programHandle = prog.getHandle();
    pass1Index = gl::GetSubroutineIndex( programHandle, gl::FRAGMENT_SHADER, "pass1");
    pass2Index = gl::GetSubroutineIndex( programHandle, gl::FRAGMENT_SHADER, "pass2");

    prog.setUniform("PositionTex", 0);
    prog.setUniform("NormalTex", 1);
    prog.setUniform("ColorTex", 2);
	prog.setUniform("SpecularTex", 2);

}


/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDeferred::setLightParams(Camera camera)
{
	vec3 worldLight = vec3(5.0f,10.0f,20.0f);
   
	prog.setUniform("Light.AmbientIntensity",vec3(0.1f,0.1f,0.1f));	
	prog.setUniform("Light.DiffuseIntensity",vec3(0.8f,0.8f,0.8f));	  
	prog.setUniform("Light.Position",  vec4(worldLight,1.0) );
}



void SceneDeferred::setupFBO()
{
    GLuint depthBuf, posTex, normTex, colorTex, specularTex;

    // Create and bind the FBO
    gl::GenFramebuffers(1, &deferredFBO);
    gl::BindFramebuffer(gl::FRAMEBUFFER, deferredFBO);

    // The depth buffer
    gl::GenRenderbuffers(1, &depthBuf);
    gl::BindRenderbuffer(gl::RENDERBUFFER, depthBuf);
    gl::RenderbufferStorage(gl::RENDERBUFFER, gl::DEPTH_COMPONENT, width, height);

    // The position buffer
    gl::ActiveTexture(gl::TEXTURE0);   // Use texture unit 0
    gl::GenTextures(1, &posTex);
    gl::BindTexture(gl::TEXTURE_2D, posTex);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB32F, width, height, 0, gl::RGB, gl::UNSIGNED_BYTE, NULL);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);

    // The normal buffer
    gl::ActiveTexture(gl::TEXTURE1);
    gl::GenTextures(1, &normTex);
    gl::BindTexture(gl::TEXTURE_2D, normTex);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB32F, width, height, 0, gl::RGB, gl::UNSIGNED_BYTE, NULL);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);

    // The color buffer
    gl::ActiveTexture(gl::TEXTURE2);
    gl::GenTextures(1, &colorTex);
    gl::BindTexture(gl::TEXTURE_2D, colorTex);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, width, height, 0, gl::RGB, gl::UNSIGNED_BYTE, NULL);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);

	// The specular buffer
    gl::ActiveTexture(gl::TEXTURE3);
    gl::GenTextures(1, &specularTex);
    gl::BindTexture(gl::TEXTURE_2D, specularTex);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGBA, width, height, 0, gl::RGBA, gl::UNSIGNED_BYTE, NULL);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);

    // Attach the images to the framebuffer
    gl::FramebufferRenderbuffer(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::RENDERBUFFER, depthBuf);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, posTex, 0);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT1, gl::TEXTURE_2D, normTex, 0);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT2, gl::TEXTURE_2D, colorTex, 0);

 
	GLenum drawBuffers[] = {gl::NONE, gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1,
                       gl::COLOR_ATTACHMENT2, gl::COLOR_ATTACHMENT3, gl::COLOR_ATTACHMENT4};

    gl::DrawBuffers(5, drawBuffers);


    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
}

void SceneDeferred::update( float t )
{
	/*Not used at the moment*/
}

void SceneDeferred::render(Camera camera)
{
    pass1(camera);
    pass2(camera);
}

void SceneDeferred::pass1(Camera camera)
{
    gl::BindFramebuffer(gl::FRAMEBUFFER, deferredFBO);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    gl::Enable(gl::DEPTH_TEST);
    gl::UniformSubroutinesuiv( gl::FRAGMENT_SHADER, 1, &pass1Index);

    prog.setUniform("Material.Kd", 1.0f, 0.6f, 0.0f);
	prog.setUniform("Material.Ks", vec4(0.5f, 0.5f, 0.5f, 150.0f));

    model = mat4(1.0f);
    setMatrices(camera);
    teapot->render();

	prog.setUniform("Material.Kd", 0.0f, 1.0f, 0.0f);
	prog.setUniform("Material.Ks", vec4(0.5f, 0.5f, 0.5f, 150.0f));
    model = mat4(1.0f);
    setMatrices(camera);
    plane->render();

    gl::Finish();
}

void SceneDeferred::pass2(Camera camera)
{
    // Revert to default framebuffer
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);

    gl::UniformSubroutinesuiv( gl::FRAGMENT_SHADER, 1, &pass2Index);

    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    gl::Disable(gl::DEPTH_TEST);

	//Render to quad with no transformations
    mat4 view = mat4(1.0);
    mat4 model = mat4(1.0);
    mat4 projection = mat4(1.0);

	 mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("ViewMatrix", view);
    prog.setUniform("NormalMatrix",
                    mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);

    // Render the quad
    gl::BindVertexArray(quad);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}

void SceneDeferred::setMatrices(Camera camera)
{
    mat4 mv = camera.view() * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", camera.projection() * mv);
	
	vec3 worldLight = vec3(5.0f,10.0f,20.0f);
	
    vec4 LightPos = camera.view() * vec4(worldLight,1.0f);
	prog.setUniform("Light.AmbientIntensity",vec3(0.1f,0.1f,0.1f));	
	prog.setUniform("Light.DiffuseIntensity",vec3(0.8f,0.8f,0.8f));	
	prog.setUniform("Light.SpecularIntensity",vec3(0.3f,0.3f,0.3f));	
	prog.setUniform("Light.Position",  LightPos );



}

void SceneDeferred::resize(Camera camera, int w, int h)
{
    gl::Viewport(0,0,w,h);
    width = w;
    height = h;
}

void SceneDeferred::compileAndLinkShader()
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