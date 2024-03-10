#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "RenderingProgram.h"
#include <iostream>

#define numVAOs 1

class Engine {
public:
    explicit Engine() = default;

    void Init(GLFWwindow *window) {
        rendering_program_.Create().VertexShader("../glsl/vertex.glsl").FragmentShader(
                "../glsl/fragment.glsl").Link();
        glGenVertexArrays(numVAOs, vao);
        glBindVertexArray(vao[0]);
    }

    void UpdateGL(double dt) {
        glUseProgram(rendering_program_.program());
        Update(dt);
    }

    void Update(double dt) {
        if (x_ > 1.0f) inc_ = -1.0f;
        else if (x_ < -1.0f) inc_ = 1.0f;
        x_ += inc_ * dt;
        thea_ += 10.0f * dt;
        UpdateUniform1f("offset", x_);
        UpdateUniform1f("rotate", thea_);
    }

    void UpdateUniform1f(const std::string &name, double value) {
        GLuint loc = glGetUniformLocation(rendering_program_.program(), name.c_str());
        glProgramUniform1f(rendering_program_.program(), loc, value);
    }

    void Draw() {
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window_);
    }

    void Clear() {
        glClear(GL_DEPTH_BUFFER);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Start() {
        // Init glfw
        if (!glfwInit()) {
            exit(EXIT_FAILURE);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        window_ = glfwCreateWindow(600, 600, "gllearn", nullptr, nullptr);
        glfwMakeContextCurrent(window_);

        // Init glew
        if (glewInit() != GLEW_OK) {
            exit(EXIT_FAILURE);
        }
        glfwSwapInterval(1);
        Init(window_);

        last_time_ = glfwGetTime();
        // loop
        while (!glfwWindowShouldClose(window_)) {
            double time = glfwGetTime();
            double dt = time - last_time_;
            last_time_ = time;
            Clear();
            UpdateGL(dt);
            Draw();
            glfwPollEvents();
        }

        // close
        glfwDestroyWindow(window_);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

private:
    GLFWwindow *window_ = nullptr;
    RenderingProgram rendering_program_;
    GLuint vao[numVAOs]{};
    double last_time_{};

    float x_ = 0.0;
    float inc_ = 1.0f;
    float thea_ = 0.0f;
};
