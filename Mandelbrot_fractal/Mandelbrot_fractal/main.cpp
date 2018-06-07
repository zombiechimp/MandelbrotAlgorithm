//
//  main.cpp
//  Mandelbrot_fractal
//
//  Created by Tea Jakić on 06/06/2018.
//  Copyright © 2018 Tea Jakić. All rights reserved.
//


#include <GLUT/glut.h>
#include <pthread.h>
#include <math.h>
#include <vector>
#include <stack>
#include <array>

double xmin = 0;
double xmax = 599;
double ymin = 0;
double ymax = 599;
double umin = -2;
double umax = 1;
double vmin = -1.2;
double vmax = 1.2;

int maxLimit = 1000;

GLuint height = 600;
GLuint width = 600;

void reshape(int width, int height);
void display();
void renderScene();
void mouse_pressed( const int button, const int state, const int x, const int y);
void keyReleased(const unsigned char key, int x, int y);

bool div_test1 = true;
bool colour = true;


std::stack<std::array<double, 4>> arg_stack;

typedef struct{
    double x;
    double y;
}vertex;



vertex current_point;


typedef struct {
    double re;
    double im;
}complex;

int divergence_test(complex c, int limit){
    complex z;
    z.re = 0; z.im = 0;
    for(int i = 1; i <= limit; i++){
        double next_re = z.re*z.re - z.im*z.im +c.re;
        double next_im = 2*z.re*z.im + c.im;
        z.re = next_re;
        z.im = next_im;
        double modul2 = z.re*z.re + z.im*z.im;
        if(modul2 > 4) return i;
    }
    return -1;
}

int divergence_test2(complex c, int limit){
    complex z;
    z.re = 0; z.im = 0;
    for(int i = 1; i <= limit; i++){
        double next_re = z.re*z.re*z.re - z.im*z.im*z.re - 2*z.re*z.im*z.im +c.re;
        double next_im = 3*z.re*z.re*z.im -z.im*z.im*z.im + c.im;
        z.re = next_re;
        z.im = next_im;
        double modul2 = z.re*z.re + z.im*z.im;
        if(modul2 > 4) return i;
    }
    return -1;
}

void black_and_white(int n){
    if(n == -1){
        glColor3f(0,0,0);
    }else{
        glColor3f(1,1,1);
    }
    return;
}

void colourScheme2(int n){
    if(n==-1){
        glColor3f(0,0,0);
    }else if(maxLimit < 16){
        int r = (int)((n-1)/(double)(maxLimit-1))*255 + 0.5;
        int g = 255 - r;
        int b = ((n-1)%(maxLimit/2))*255/(maxLimit/2);
        glColor3f((float)(r/(float)255), (float)(g/(float)255), (float)(b/(float)255));
    }else{
        int lim = maxLimit < 32 ? maxLimit:32;
        int r = (n-1)*255/lim;
        int g = ((n-1)%(lim/4))*255/(lim/4);
        int b = ((n-1)%(lim/8)) * 255/(lim/8);
        glColor3f((float)(r/(float)255), (float)(g/(float)255), (float)(b/(float)255));
    }
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Mandelbrot fractal");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_pressed);
    glutKeyboardUpFunc(keyReleased);
    glutMainLoop();
    
    
    
    return 0;
}


void display(){
    
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //crtanje scene
    renderScene();

    glFlush();
    glutSwapBuffers();
}



void reshape(int width, int height){
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width-1, 0, height-1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderScene(){
    glPointSize(1);
    glBegin(GL_POINTS);
    for(int y = ymin; y <=ymax; y++ ){
        for(int x = xmin; x <= xmax; x++){
            complex c;
            c.re = (x-xmin)/(double)(xmax-xmin)*(umax-umin) + umin;
            c.im = (y-ymin)/(double)(ymax-ymin)*(vmax-vmin) + vmin;
            int n;
            if(div_test1)n = divergence_test(c, maxLimit);
            else n = divergence_test2(c, maxLimit);
            if(colour) colourScheme2(n);
            else black_and_white(n);
            glVertex2i(x, y);
        }
    }
    glEnd();
    
}

void mouse_pressed(const int button, const int state, const int x, const int y){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        
        std::array<double, 4> cont = {umin, umax, vmin, vmax};
        arg_stack.push(cont);
        auto zz = arg_stack.top();
        double u = ((x - xmin) / (xmax-xmin)*(umax-umin) + umin);;
        double w = umax - umin;
        double h = vmax - xmin;
        umin = u - w/16.0;
        umax = u + w/16.0;
        double v = ((y-ymin)/(double)(ymax-ymin)*(vmax-vmin) + vmin);
        vmin = v - h/16.0;
        vmax = v + h/16.0;
        

        
        
        glutPostRedisplay();
    }
}



void keyReleased(const unsigned char key, int x, int y){
    if (key == '1') div_test1 = true;
    else if (key == '2' ) div_test1 = false;
    else if (key == 'b')colour = false;
    else if (key == 'c')colour = true;
    else if (key == 'x'){
        if(!arg_stack.empty()){
            auto cont = arg_stack.top();
            umin = cont[0]; umax = cont[1]; vmin = cont[2]; vmax = cont[3];
            arg_stack.pop();
        }

        
    }
    else if (key == 27){
        if(!arg_stack.empty()){
            while(arg_stack.size() > 1){
                arg_stack.pop();
            }
            auto cont = arg_stack.top();
            umin = cont[0]; umax = cont[1]; vmin = cont[2]; vmax = cont[3];
            arg_stack.pop();
        }
    }
    glutPostRedisplay();
}









