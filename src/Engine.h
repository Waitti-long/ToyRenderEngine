#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "RenderingProgram.h"
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#define numVAOs 1
#define numVBOs 2

class Engine {
public:
    explicit Engine() = default;

    void Init(GLFWwindow *window) {
        rendering_program_.Create().VertexShader("../glsl/vertex.glsl").FragmentShader(
                "../glsl/fragment.glsl").Link();

        SetUpMat();
        SetUpVertices();
    }

    void SetUpMat() {
        cameraX = 0.0f;
        cameraY = 0.0f;
        cameraZ = 8.0f;
        cubeLocX = 0.0f;
        cubeLocY = -2.0f;
        cubeLocZ = 0.0f;
    }

    void SetUpVertices() {
        float vertex_positions[108] = {
                -1.0f, 1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f, 1.0f, -1.0f
        };

        glGenVertexArrays(numVAOs, vao);
        glBindVertexArray(vao[0]);
        glGenBuffers(numVBOs, vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
    }

    void Draw(double dt) {
        glUseProgram(rendering_program_.program());

        UpdateMat();
        UpdateUniformMat4fv("mv_matrix", mvMat);
        UpdateUniformMat4fv("proj_matrix", pMat);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void UpdateMat() {
        glfwGetFramebufferSize(window_, &width, &height);
        aspect = (float) width / (float) height;
        pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

        vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
        mvMat = vMat * mMat;
    }


    void UpdateUniform1f(const std::string &name, double value) {
        GLuint loc = glGetUniformLocation(rendering_program_.program(), name.c_str());
        glProgramUniform1f(rendering_program_.program(), loc, value);
    }

    void UpdateUniformMat4fv(const std::string &name, const glm::mat4 &value) {
        GLuint loc = glGetUniformLocation(rendering_program_.program(), name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Clear() {
        glClear(GL_DEPTH_BUFFER_BIT);
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
            Draw(dt);
            glfwSwapBuffers(window_);
            glfwPollEvents();
        }

        // close
        glfwDestroyWindow(window_);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

private:
    GLFWwindow *window_ = nullptr;
    RenderingProgram rendering_program_{};
    double last_time_{};

    GLuint vao[numVAOs]{};
    GLuint vbo[numVBOs]{};
    float cameraX, cameraY, cameraZ;
    float cubeLocX, cubeLocY, cubeLocZ;
    GLuint mvLoc, projLoc;
    int width, height;
    float aspect;
    glm::mat4 pMat, vMat, mMat, mvMat;
};
