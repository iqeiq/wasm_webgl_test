#ifndef _INC_APP_HPP_
#define _INC_APP_HPP_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "lifegame.hpp"

#define LOGD(mes) std::cout << "[DEBUG] " << mes << std::endl
#define LOGE(mes) std::cerr << "[ERROR] " << mes << std::endl


class App {
public:
  App() :lifegame(1024) {
    const int tmp = 1024 - 2;
    for(int i = 0; i < tmp; ++i) {
      //lifegame.born(rand() % tmp + 1, rand() % tmp + 1);
      lifegame.born(i + 1, 512);
      //lifegame.born(i + 1, i + 2);
    }
  }
  int initGL(const int width, const int height);
  void draw();
  void updateFrame();
private:
  GLuint compileShader(GLenum type, const char *source);
  GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader, const char * vertexPositionName);
    
  struct _uniform { GLint tex; };
  GLuint program;
  SDL_Surface* screen;
  _uniform uniform;
  LifeGame lifegame;
};

void App::updateFrame() {
  glClear(GL_COLOR_BUFFER_BIT);
  lifegame.step();
  draw();
}

int App::initGL(const int width, const int height) {
	//initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    LOGE("SDL_Init" << SDL_GetError());
    return 0;
  }
  screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
  if (screen == NULL) {
	  LOGE("SDL_SetVideoMode" << SDL_GetError());
		return 0;
	}
	
	//SDL initialised successfully, now load shaders and geometry
	const char vs_src[] =
		"attribute vec4 vPos; \n"
		"void main() {\n"
		"   gl_Position = vPos; \n"
		"}\n";

	const char fs_src[] =
		"precision mediump float; \n"
    "uniform sampler2D tex;\n"
    "void main() {  \n"
    "  vec4 color = texture2D(tex, gl_FragCoord.xy / 640.0); \n"
    "  color.a = 1.0;"
		"  gl_FragColor = vec4(color); \n"
		"}\n";

	//load vertex and fragment shaders
	auto vs = compileShader(GL_VERTEX_SHADER, vs_src);
	auto fs = compileShader(GL_FRAGMENT_SHADER, fs_src);
	linkProgram(vs, fs, "vPos");

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glViewport(0, 0, width, height);

  uniform.tex = glGetUniformLocation(program, "tex");  
  GLuint texid;
  glGenTextures(1, &texid);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  
  glBindTexture(GL_TEXTURE_2D, texid);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lifegame.N, lifegame.N, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, lifegame.buffer);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  return 1;
}

void App::draw() {
  glUseProgram(program);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lifegame.N, lifegame.N, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, lifegame.buffer);
  
  static GLfloat vert[] = {
    -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f
  };
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vert);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


GLuint App::compileShader(GLenum type, const char *source) {
	auto shader = glCreateShader(type);
	if (shader == 0) {
		LOGE("glCreateShader");
		return 0;
	}

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(!status) {
    LOGE("glCompileShader");
    
    GLsizei bufSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
    if (bufSize > 1) {
      GLchar *infoLog = (GLchar *)malloc(bufSize);
      if (infoLog != NULL) {
        GLsizei length;
        glGetShaderInfoLog(shader, bufSize, &length, infoLog);
        LOGE(infoLog);
        free(infoLog);
      }         
    }
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint App::linkProgram(GLuint vs, GLuint fs, const char* vPosName) {
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glBindAttribLocation(program, 0, vPosName);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		LOGE("glLinkProgram");
		glDeleteProgram(program);
		return 0;
  }
  
  return 1;
}

#endif
