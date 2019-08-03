#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iomanip>

class Shader {
public:

    static char* textFileRead(char* fn) {
        FILE *fp;
        char *content = NULL;

        int count=0;

        if (fn != NULL) {
            fp = fopen(fn,"rt");

            if (fp != NULL) {

                fseek(fp, 0, SEEK_END);
                count = ftell(fp);
                rewind(fp);

                if (count > 0) {
                    content = (char *)malloc(sizeof(char) * (count+1));
                    count = fread(content,sizeof(char),count,fp);
                    content[count] = '\0';
                }
                fclose(fp);
            }
        }

        if (content == NULL) {
            fprintf(stderr, "ERROR: could not load in file %s\n", fn);
            exit(1);
        }
        return content;
    }

    static void printShaderLog(GLuint prog) {
        GLint infoLogLength = 0;
        GLsizei charsWritten  = 0;
        GLchar *infoLog;

        glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength > 0){
            infoLog = (char *) malloc(infoLogLength);
            glGetShaderInfoLog(prog, infoLogLength, &charsWritten, infoLog);
            printf("%s\n",infoLog);
            free(infoLog);
        }
    }

    static void printProgramLog(GLuint shad) {
        GLint infoLogLength = 0;
        GLsizei charsWritten  = 0;
        GLchar *infoLog;

        glGetProgramiv(shad, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength > 0){
            infoLog = (char *) malloc(infoLogLength);
            glGetProgramInfoLog(shad, infoLogLength, &charsWritten, infoLog);
            printf("%s\n",infoLog);
            free(infoLog);
        }
    }

    static GLuint setShaders(char * vert, char * frag) {//vert and frag
        GLuint v,f, pro;
        char *vs,*fs;

        v = glCreateShader(GL_VERTEX_SHADER);
        f = glCreateShader(GL_FRAGMENT_SHADER);

        vs = textFileRead(vert);
        fs = textFileRead(frag);

        const char * vv = vs;
        const char * ff = fs;

        glShaderSource(v, 1, &vv,NULL);
        glShaderSource(f, 1, &ff,NULL);

        free(vs);free(fs);

        glCompileShader(v);
        glCompileShader(f);

        printShaderLog(v);
        printShaderLog(f);

        pro = glCreateProgram();
        glAttachShader(pro,v);
        glAttachShader(pro,f);

        glLinkProgram(pro);
        printProgramLog(pro);

        return(pro);
    }

};

#endif // SHADER_H
