#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

using namespace std;

struct Mat4 {
    float m[4][4] = {};
};

Mat4 identity() {
    Mat4 res;
    for (int i = 0; i < 4; i++) res.m[i][i] = 1.0f;
    return res;
}

Mat4 multiply(const Mat4& a, const Mat4& b) {
    Mat4 res;
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            for (int k = 0; k < 4; k++)
                res.m[r][c] += a.m[r][k] * b.m[k][c];
    return res;
}

Mat4 translate(float x, float y, float z) {
    Mat4 res = identity();
    res.m[0][3] = x;
    res.m[1][3] = y;
    res.m[2][3] = z;
    return res;
}

Mat4 rotateX(float a) {
    Mat4 res = identity();
    res.m[1][1] = cos(a);  res.m[1][2] = -sin(a);
    res.m[2][1] = sin(a);  res.m[2][2] = cos(a);
    return res;
}

Mat4 rotateY(float a) {
    Mat4 res = identity();
    res.m[0][0] = cos(a);  res.m[0][2] = sin(a);
    res.m[2][0] = -sin(a); res.m[2][2] = cos(a);
    return res;
}

Mat4 perspective(float fovRad, float aspect, float nearZ, float farZ) {
    Mat4 res;
    float f = 1.0f / tan(fovRad / 2.0f);
    res.m[0][0] = f / aspect;
    res.m[1][1] = f;
    res.m[2][2] = (farZ + nearZ) / (nearZ - farZ);
    res.m[2][3] = (2.0f * farZ * nearZ) / (nearZ - farZ);
    res.m[3][2] = -1.0f;
    return res;
}

int main() {
    // 1. Ініціалізація GLFW
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Створення вікна та контексту OpenGL
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. Завантаження функцій OpenGL через GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 4. Геометрія та буфери (тільки після GLAD)
    float vertices[] = {
        //x     y      z        r      g       b
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f, //0
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f, //1
         0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f, //2
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f, //3
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f, //4
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f, //5
         0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f, //6
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f  //7
    };

    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 4, 7,  7, 3, 0,
        1, 5, 6,  6, 2, 1,
        3, 2, 6,  6, 7, 3,
        0, 1, 5,  5, 4, 0
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 5. Шейдери
    const char* vertexShaderSource = R"(#version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 ourColor;
        uniform mat4 mvp;
        void main() {
            gl_Position = mvp * vec4(aPos, 1.0);
            ourColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(#version 330 core
        out vec4 FragColor;
        in vec3 ourColor;
        void main() {
            FragColor = vec4(ourColor, 1.0);
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 6. Цикл рендерингу
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float t = (float)glfwGetTime();
        Mat4 model = multiply(rotateY(t), rotateX(t * 0.7f));
        Mat4 view  = translate(0.0f, 0.0f, -3.0f);
        Mat4 proj  = perspective(45.0f * 3.1415926f / 180.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        Mat4 mvp   = multiply(proj, multiply(view, model));

        int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
        glUniformMatrix4fv(mvpLoc, 1, GL_TRUE, &mvp.m[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}