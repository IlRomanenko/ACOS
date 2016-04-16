#pragma once
#define _USE_MATH_DEFINES
#include <GL/freeglut.h>
#include <cmath>
using namespace std;

enum constants
{
    Window_Width = 800,
    Window_Height = 800,
    Map_Width = 800,   //may be different from Window_Width
    Map_Height = Map_Width, //as Map_Width

};

struct initializer_thread_data
{
    int argc;
    char **argv;
    initializer_thread_data(int _argc, char** _argv) : argc(_argc), argv(_argv) { }
};

void render();

namespace visualizer
{
#define glPush(); glPushMatrix(); {
#define glPop(); } glPopMatrix();


    void SetColor(double red, double green, double blue)
    {
        glColor3d(red, green, blue);
    }

    void DrawRectangle(double x, double y, double width, double height)
    {
        glBegin(GL_POLYGON);
        glVertex2d(x, y);
        glVertex2d(x + width, y);
        glVertex2d(x + width, y + height);
        glVertex2d(x, y + height);
        glEnd();
    }

    void DrawCircle(double x, double y, double radius, int edges = 30)
    {
        glPush();
        glTranslated(x, y, 0);
        glBegin(GL_POLYGON);

        double angle = 2 * M_PI / edges;
        for (int i = 0; i < edges; i++)
            glVertex2d(cos(angle * i) * radius, sin(angle * i) * radius);

        glEnd();
        glPop();
    }

    void DrawLine(double x1, double y1, double x2, double y2)
    {
        glBegin(GL_POLYGON);
        glVertex2d(x1, y1);
        glVertex2d(x2, y2);
        glEnd();
    }

    void timer_redisplay(int)
    {
        glutPostRedisplay();
        glutTimerFunc(16, timer_redisplay, 0);
    }

    void* InitializeViz(void *data)
    {
        initializer_thread_data* cur_data = (initializer_thread_data*)data;
        int argc = cur_data->argc;
        char **argv = cur_data->argv;
        glutInit(&argc, argv);
        glutInitWindowSize(Window_Width, Window_Height);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
        glutCreateWindow("Visualizer");
        glOrtho(0, Map_Width, 0, Map_Height, -1, 1);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glutDisplayFunc(render);
        glutTimerFunc(100, timer_redisplay, 0);
        glutMainLoop();
        delete cur_data;
        return NULL;
    }
};