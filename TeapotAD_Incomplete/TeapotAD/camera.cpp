#define _USE_MATH_DEFINES
#include <cmath>


#include "camera.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_core_4_3.hpp"

#include "defines.h"
#include <iostream>
using namespace imat3111;

static const float MaxVerticalAngle = 85.0f;

static inline float RadiansToDegrees(float radians) {
	return radians * 180.0f / (float)M_PI;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////////////////////
Camera::Camera() 

{
	reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Get the position
/////////////////////////////////////////////////////////////////////////////////////////////
const glm::vec3& Camera::position() const
{
	return _position;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set the position
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::setPosition(const glm::vec3& position)
{
	_position = position;
}




/////////////////////////////////////////////////////////////////////////////////////////////
// Get the fieldOfView
/////////////////////////////////////////////////////////////////////////////////////////////
float Camera::fieldOfView() const
{
	return _fieldOfView;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set the fieldOfView
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::setFieldOfView(float fieldOfView)
{
	assert(fieldOfView>0.0f && fieldOfView <180.0f);
	_fieldOfView = fieldOfView;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Get the aspectRatio
/////////////////////////////////////////////////////////////////////////////////////////////
float Camera::aspectRatio() const
{
	return _aspectRatio;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set the aspectRatio
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::setAspectRatio(float aspectRatio)
{
	assert(aspectRatio >0.0f);
	_aspectRatio = aspectRatio;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Get the nearPlane
/////////////////////////////////////////////////////////////////////////////////////////////
float Camera::nearPlane() const
{
	return _nearPlane;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set the nearPlane
/////////////////////////////////////////////////////////////////////////////////////////////
float Camera::farPlane() const
{
	return _farPlane;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set the nearPlane and the farPlane
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::setNearAndFarPlanes(float nearPlane, float farPlane)
{
	assert(nearPlane > 0.0f);
	assert(farPlane > nearPlane);
	_nearPlane = nearPlane;
	_farPlane = farPlane;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Rotate the camera
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::rotate(const float x, const float y)
{
	//Update the current _pitch and _yaw; i.e. the current x and y rotations
	_pitch += y;
	_yaw += x;

	if ((_pitch>360) || (_pitch<-360)) _pitch=0;
	if ((_yaw>360) || (_yaw<-360)) _yaw=0;

	/*TO DO */
	/* The job here is to change the value of camera _position
	so it rotates by x and y around the camera _target location.
	This involves creating a matrix that will translate the scene 
	models to the target location, rotate them and then undo the 
	translation. Finally applying this matrix to change the camera
	_position. 
	*/

	glm::vec4 position = glm::vec4 (_position, 1.0);

	glm::mat4 translate = glm::mat4 (1.0, 0.0, 0.0, -_target.x,
		0.0, 1.0, 0.0, -_target.y,
		0.0, 0.0, 1.0, -_target.z,
		0.0, 0.0, 0.0, 1.0);

	glm::mat4 translate2 = glm::mat4 (1.0, 0.0, 0.0, _target.x,
		0.0, 1.0, 0.0, _target.y,
		0.0, 0.0, 1.0, _target.z,
		0.0, 0.0, 0.0, 1.0);

	glm::mat4 xrotate = glm::rotate(glm::mat4(1.0), x, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 yrotate = glm::rotate(glm::mat4(1.0), y, glm::vec3(1.0, 0.0, 0.0));

	glm::mat4 rotate = xrotate * yrotate;

	position = translate2 * rotate * translate * position;

	_position.x = position.x;
	_position.y = position.y;
	_position.z = position.z;

	//Finally call update()
	update();
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Pan the camera
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::pan(const float x, const float y)
{
	/*TO DO */
	/* The job here is to update the value of camera _position and 
	it's _target location so they move but stay in position relative 
	to one another. We also need to take account of the fact that the
	scene may already have been rotated using camera rotate but we
	still want the camera to pan left/right and up/down in parallel
	to the current view.
	So, although panning is fundamentally a translate of camera 
	_position and _target, we will need to modify this with the 
	current rotation held by the camera's _pitch and _yaw attributes.
	*/

	float xSpeed = 0.05;
	float ySpeed = 0.005;

	glm::vec3 panVector(x,-y,0);
	glm::vec3 vector;

	vector = _position - _target;

	glm::vec3 right = glm::cross(vector, glm::vec3(0,1,0));
	glm::vec3 up = glm::cross(vector,right);

	_position += right * -x * xSpeed;
	_target += right * -x * xSpeed;

	_position += up * y * ySpeed;
	_target += up * y * ySpeed;

	//Finally call update()
	update();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Zoom the camera
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::zoom(const float z)
{
	/*TO DO */
	/* This is almost the same as the Pan but the translation is
	in the z-direction rather than x and y.
	Again we need to update the value of camera _position and 
	it's _target location so they move but stay in position relative 
	to one another. And, again, we also need to take account of the 
	fact that the scene may already have been rotated using camera rotate but we
	still want the camera to zoom in and out parallel to the current view.
	So, although zooming is fundamentally a translate of camera 
	_position and _target, we will need to modify this with the 
	current rotation held by the camera's _pitch and _yaw attributes.
	*/

	float speed = 0.05;

	_position -= _forward * -z * speed;

	//Finally call update()
	update();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Update the camera
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::update()
{

	_forward = _target - _position;


	double dArray[16] = {0.0};

	/*
	const float *pSource = (const float*)glm::value_ptr(_view);
	for (int i = 0; i < 16; ++i)
	dArray[i] = pSource[i];

	std::cout << "view " << dArray[0] << "   " << dArray[1]  << "   " << dArray[2] << "   " << dArray[3] << "\n";
	std::cout << dArray[4] << "   " << dArray[5]  << "   " << dArray[6] << "   " << dArray[7] << "\n";
	std::cout << dArray[8] << "   " << dArray[9]  << "   " << dArray[10] << "   " << dArray[11] << "\n";
	std::cout << dArray[12] << "   " << dArray[13]  << "   " << dArray[14] << "   " << dArray[15] << "\n";
	*/

	_right = glm::cross(_up,_forward);
	glm::vec3 up = glm::normalize(glm::cross(_forward,_right));
	_view = glm::lookAt(_position,_target,up);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Reset the camera
/////////////////////////////////////////////////////////////////////////////////////////////
void Camera::reset(void)
{
	_pitch = 0.0f;
	_yaw = 0.0f;

	_position= glm::vec3(0.0f,10.0f,30.0f);
	_up= glm::vec3(0.0f,-1.0f,0.0f);
	_target = glm::vec3(0.0f,0.0f,0.0f);
	_right = glm::cross(_up,_target);
	_fieldOfView =50.0f;
	_nearPlane = 0.01f;
	_farPlane = 100.0f;
	_aspectRatio = 4.0f/3.0f;
	_forward = _target - _pitch;

	_view = glm::lookAt(_position, glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f));
	_projection = glm::perspective(_fieldOfView,_aspectRatio,_nearPlane,_farPlane);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Return the camera View matrix
/////////////////////////////////////////////////////////////////////////////////////////////
glm::mat4 Camera::view() 
{

	this->update();
	return _view;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Return the camera Projection matrix
/////////////////////////////////////////////////////////////////////////////////////////////
glm::mat4 Camera::projection() 
{
	this->update();
	return _projection;


}


