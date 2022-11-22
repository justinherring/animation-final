//
//    Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
//    Created by zwood on 2/21/10.
//    Modified by sueda 10/15/15.
//

#include "GLSL.h"
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>

using namespace std;

namespace GLSL {

const char * errorString(GLenum err)
{
	switch(err) {
	case GL_NO_ERROR:
		return "No error";
	case GL_INVALID_ENUM:
		return "Invalid enum";
	case GL_INVALID_VALUE:
		return "Invalid value";
	case GL_INVALID_OPERATION:
		return "Invalid operation";
	case GL_STACK_OVERFLOW:
		return "Stack overflow";
	case GL_STACK_UNDERFLOW:
		return "Stack underflow";
	case GL_OUT_OF_MEMORY:
		return "Out of memory";
	default:
		return "No error";
	}
}

void checkVersion()
{
	int major, minor;
	major = minor = 0;
	const char *verstr = (const char *)glGetString(GL_VERSION);
	
	if((verstr == NULL) || (sscanf(verstr, "%d.%d", &major, &minor) != 2)) {
		printf("Invalid GL_VERSION format %d.%d\n", major, minor);
	}
	if(major < 2) {
		printf("This shader example will not work due to the installed Opengl version, which is %d.%d.\n", major, minor);
		exit(0);
	}
}

void checkError(const char *str)
{
	GLenum glErr = glGetError();
	if(glErr != GL_NO_ERROR) {
		if(str) {
			printf("%s: ", str);
		}
		printf("GL_ERROR = %s.\n", errorString(glErr));
		assert(false);
	}
}

void printShaderInfoLog(GLuint shader)
{
	GLint infologLength = 0;
	GLint charsWritten  = 0;
	GLchar *infoLog = 0;
	
	checkError(GET_FILE_LINE);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
	checkError(GET_FILE_LINE);
	
	if(infologLength > 0) {
		infoLog = (GLchar *)malloc(infologLength);
		if(infoLog == NULL) {
			puts("ERROR: Could not allocate InfoLog buffer");
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		checkError(GET_FILE_LINE);
		printf("Shader InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint program)
{
	GLint infologLength = 0;
	GLint charsWritten  = 0;
	GLchar *infoLog = 0;
	
	checkError(GET_FILE_LINE);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
	checkError(GET_FILE_LINE);
	
	if(infologLength > 0) {
		infoLog = (GLchar *)malloc(infologLength);
		if(infoLog == NULL) {
			puts("ERROR: Could not allocate InfoLog buffer");
			exit(1);
		}
		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		checkError(GET_FILE_LINE);
		printf("Program InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
}
	
char *textFileRead(const char *fn)
{
	FILE *fp;
	char *content = NULL;
	int count = 0;
	if(fn != NULL) {
		fp = fopen(fn,"rt");
		if(fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = (int)ftell(fp);
			rewind(fp);
			if(count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = (int)fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		} else {
			printf("error loading %s\n", fn);
		}
	}
	
	const char *t = content;
	vector<int> v;
	while(*t != '\0') {
		v.push_back(*t);
		t++;
	}
	v.push_back('\0');
	if(!GLSL::validUTF8(v)) {
		printf("%s is not UTF8\n", fn);
	}
	return content;
}

int textFileWrite(const char *fn, const char *s)
{
	FILE *fp;
	int status = 0;
	if(fn != NULL) {
		fp = fopen(fn,"w");
		if(fp != NULL) {
			if(fwrite(s,sizeof(char),strlen(s),fp) == strlen(s)) {
				status = 1;
			}
			fclose(fp);
		}
	}
	return(status);
}

// https://www.tutorialspoint.com/utf-8-validation-in-cplusplus
bool validUTF8(vector<int> &data) {
	int cnt = 0;
	for(int i = 0; i <data.size(); i++){
		int x = data[i];
		if(!cnt) {
			if((x >> 5) == 0b110) {
				cnt = 1;
			} else if((x >> 4) == 0b1110) {
				cnt = 2;
			} else if((x >> 3) == 0b11110) {
				cnt = 3;
			} else if((x >> 7) != 0) return false;
		} else {
			if((x >> 6) != 0b10) return false;
			cnt--;
		}
	}
	return cnt == 0;
}

}
