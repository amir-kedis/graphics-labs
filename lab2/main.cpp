// =============================================================>
// This lab is following Dr. Yahia recorded labs on CMP24 Channel
//
// Lab 2 objectives:
//  * sending data from a shader to another shader (vars)
//  * streaming data from cpu to gpu (uniforms)
//  * sending data from the cpu to gpu as buffers
//  * elements buffer to removes duplicates from buffer arrays.
//
//  NOTE: vendors are in lab 1 since they didn't change.
//
// Comments are written by Amir Kedis
// =============================================================>

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

// ==========================================================
// STRUCTS
// ==========================================================

struct Vec3
{
    float x, y, z;
};

struct Color
{
    unsigned char r, g, b, a;
};

struct Vertex
{
    Vec3 position;
    Color color;
};

// ==========================================================
// FUNCTIONS
// ==========================================================

GLuint load_shader(const std::string &path, GLenum shader_type)
{
    std::ifstream file(path);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char *sourceCodeCStr = sourceCode.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &sourceCodeCStr, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
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

// ==========================================================
// MAIN
// ==========================================================

int main(int, char **)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to init GLFW\n";
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to Load OpenGL\n";
        glfwTerminate();
        glfwDestroyWindow(window);
        exit(1);
    }

    GLuint program = glCreateProgram();
    GLuint vs = load_shader("assets/shaders/simple.vert", GL_VERTEX_SHADER);
    GLuint fs = load_shader("assets/shaders/simple.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Here we create the data that we will allocate 
    // at the GPU level. Vertex is a custom struct we
    // made containing Position and color of vertices.
    Vertex vertices[] = {
        {{-0.5f, -0.5f, 0.0f}, {255, 0, 0, 255}},
        {{+0.5f, -0.5f, 0.0f}, {0, 255, 0, 255}},
        {{-0.5f, 0.5f, 0.0f}, {0, 0, 255, 255}},
        {{+0.5f, 0.5f, 0.0f}, {255, 255, 0, 255}},
    };

    // To not repeat vertices we make an elements array
    // containing indicies of vertices 
    // (0,1,2) => a triangle using the first 3 vertices in the array
    uint16_t elements[] = {0, 1, 2, 3, 1, 2};

    // Allocation the buffer at the gpu
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    // NOTE: you need to bind the buffer before running any command that uses it.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // glBufferData is an example of that
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // NOTE: you could put the elements on the same vertext array 
    // but it will be more complicated to use them.
    GLuint element_buffer;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    // Usage of uniforms
    // simple 1d data streamed from cpu to gpu
    GLuint time_loc = glGetUniformLocation(program, "time");

    GLuint position_loc = 0; // glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(
        position_loc,   // where it exist on gpu
        3,              // size (number of element)
        GL_FLOAT,       // type (size of element)
        false,          // normalized or not (divided by type max)
        sizeof(Vertex), // stride (how much you jump tell next element)
        0               // offset from the start of the object
    );

    GLuint color_loc = 1; // glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(color_loc);
    glVertexAttribPointer(
        color_loc,
        4,
        GL_UNSIGNED_BYTE,
        true,
        sizeof(Vertex),
        (void *)offsetof(Vertex, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        glClearColor(0.8, 0.5, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vertex_array);

        // streams the uniform data
        glUniform1f(time_loc, time);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)(0));

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
