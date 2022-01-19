#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>


extern int width, height; 
extern GLFWwindow* window; 

void initGL() {

    /* Initialize the library */
    if (!glfwInit()) {
        throw "Failed to initialize";
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "|  Pong  |", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw "Failed to create window";
    }


    /* Make the window's context current */
    glfwShowWindow(window);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 60hz Monitor -> 60 fps (sync with pc hz) 

    glewInit();

    glOrtho(0.f, width, height, 0.f, 0.f, 1.f);

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
}