/** ==============================================
 * @file 		DebugCallbacks.h
 * @date 		2018/03/17 19:50
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 *
 * @brief		This file contains OpenGL callback for handling errors
 ** ==============================================*/
 
#pragma once


#include <GL/glew.h>

#include <iostream>
#include <string>

void GLAPIENTRY debugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
#ifdef _DEBUG
	std::string typeString;
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		typeString = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeString = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeString = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeString = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeString = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
	case GL_DEBUG_TYPE_POP_GROUP:
	case GL_DEBUG_TYPE_OTHER:
		return;
	}
	std::cout << "-------------Opengl debug-----------------" << std::endl;
	std::cout << "Type: " << typeString << std::endl;
	std::cout << "Message: " << message << std::endl;


	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	}
	std::cout << std::endl;
#endif
}