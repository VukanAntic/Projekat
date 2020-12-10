//
// Created by vukan on 11.11.20..
//
#ifndef PROJECT_BASE_VERTEX_H
#define PROJECT_BASE_VERTEX_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

class Vertex
{
    unsigned int VBO, VAO, EBO; // EBO nam dozvoljava da koristimo iste tacke za razlicite trouglove
public:
    Vertex(){
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = { //position -> 3, color -> 3, texture -> 2
                0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
                0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
                -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom left
                -0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // top left

                0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
                0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
                -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom left
                -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f  // top left
        };

        unsigned int indices[] = {
                0, 1, 3, // first triangle // first square // i ovde ima pattern
                1, 2, 3,  // second triangle

                0, 1, 4, // first triangle // second square
                1, 4, 5, // second

                2, 3, 7, // first triangle // third square
                2, 6, 7, // second

                4, 5, 6, // first // fourth // ovde ima pattern
                6, 7, 4, // second

                1, 2, 5,
                2, 6, 5,
//
                0, 3, 4,
                4, 3, 7,

        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //texture position
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);
    }

    void deleteVAO(){
        glDeleteVertexArrays(1, &VAO);
    }
    void deleteVBO(){
        glDeleteBuffers(1, &VBO);
    }
    void deleteEBO(){
        glDeleteBuffers(1, &EBO);
    }
    void bindVAO(){
        glBindVertexArray(VAO);
    }
};

#endif //PROJECT_BASE_VERTEX_H
