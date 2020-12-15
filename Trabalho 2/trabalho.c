
//Gustavo André de Sousa Paulino
//g++ trabalho.c -o trabalho -lglut -lGLU -lGL

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "figura.h"

bool click1 = false, click2 = false;
int width = 512, height = 512;
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

void drawPontos(figura* f);


int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); 
    glutInitWindowSize (width, height);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Pobraint");
    init(); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

	glMatrixMode(GL_MODELVIEW);
    click1 = true;
    click2 = true;
}

void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 27:
            exit(0);
        break;
    }
}

void mouse(int button, int state, int x, int y)
{
    int yc = height - y;
    
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                if(yc>=482 && yc <=512){//acessando menu
                    setEstado((x/32)+1);
                }
                else{//desenhando
                    desenho(x, yc, 0);
                }
            }
            break;
        /*
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN) {
                glutPostRedisplay();
            }
            break;
        */
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN) {
                desenho(x, yc, 1);
            }
            break;
        default:
            break;
    }
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    menu();
    drawPontos(figuras);
    glutSwapBuffers();

}

void drawPontos(figura* f){
    ponto* pnt = NULL;
    figura* fig = f;
    
    while(fig != NULL){//percorre figuras
        glColor3f (fig->r, fig->g, fig->b);
        glBegin(GL_POINTS);
        pnt = fig->pontos;
        while(pnt != NULL){//percorre pontos
            glVertex2i(pnt->x,pnt->y);
            pnt = pnt->prox;
        }
        
        fig = fig->prox;
    }
    glEnd();
}

