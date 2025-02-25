// =============================================================>
// This lab is following Dr. Yahia recorded labs on CMP24 Channel
// 
// Lab 1 objectives:
//  * Project setup with cmake
//  * Opening a window using "glfw"
//  * Using "glad" to load OpenGL
//  * W a simple (vert, and frag) shaders
//  * Loading shaders from external files
//  * Drawing first Triangle using shaders
//
// Comments are written by Amir Kedis
// =============================================================>

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

// external vendors (libs) linked by cmake
#include <glad/gl.h>
#include <GLFW/glfw3.h>


// NOTE: any think OpenGL returns is a `GLuint`.
// Think of it as an id for an object in memory
GLuint load_shader(const std::string &path, GLenum shader_type)
{
    // load a file and turn it to C-style string
    std::ifstream file(path);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char *sourceCodeCStr = sourceCode.c_str();


    // Creates a shader -> add text to shader as source -> compile the shader
    GLuint shader = glCreateShader(shader_type); 
    glShaderSource(shader, 1, &sourceCodeCStr, nullptr);
    glCompileShader(shader);


    // we need to check manually for errors as GL won't throw an error
    GLint status;
    // gets if it failed or not
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        // Get the length of the log 
        // -> then allocate a string/buffer to get it
        // -> Get the log and put in the bufffer
        // -> log the error and free the buffer 
        // -> make the general cleanup
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);

        std::cerr << "ERROR IN: " << path << std::endl;
        std::cerr << logStr << std::endl;

        delete[] logStr;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int main(int, char **)
{
    // load "glfw"
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        exit(1);
    }

    // give windows some info about our project
    // NOTE: when I did'nt put those the window wouldn't open in my machine
    //       using linux on wayland
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    // Create the window
    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to init GLFW\n";
        glfwTerminate();
        exit(1);
    }

    // Create the context (further loading of opengl and many will use this context)
    glfwMakeContextCurrent(window);

    // Tell gald to load open gl from the driver 
    // ("userpointer" in gald => ProcAddress)
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to Load OpenGL\n";
        glfwTerminate();
        glfwDestroyWindow(window);
        exit(1);
    }

    // create a program (wrapper for multiple shaders, buffers, etc)
    GLuint program = glCreateProgram();
    GLuint vs = load_shader("assets/shaders/simple.vert", GL_VERTEX_SHADER);
    GLuint fs = load_shader("assets/shaders/simple.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program); // does the actual functionality

    // once the shaders are in the program, you don't need these objects anymore.
    glDeleteShader(vs);
    glDeleteShader(fs);

    // NOT used yet but vital for opengl to work.
    // more on them next time
    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);

    // main program loop
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        // Clears the screen with a changing color
        glClearColor(0.5 * sinf(time) + 0.5, 0.5 * sinf(time + 1) + 0.5, 0.5 * sinf(time + 2) + 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // draws the shader 
        glUseProgram(program);
        glBindVertexArray(vertex_array);
        // Specifically from our vertices
        // draw 3 as triangles
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // as it sounds but nsessray now to be able to close the window.
        glfwPollEvents();
        // draws (moves backbuffer -> frontbuffer)
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
