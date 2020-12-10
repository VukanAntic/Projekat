//
// Created by vukan on 28.11.20..
//

#ifndef PROJECT_BASE_MYTEXTURE_H
#define PROJECT_BASE_MYTEXTURE_H

#include <iostream>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glad/glad.h>
#include <rg/Shader.h>
#include <learnopengl/filesystem.h>

class MyTexture
{
    unsigned int tex;
public: MyTexture(std::string path){
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load((FileSystem::getPath(path)).c_str(), &width, &height, &nrChannels, 0);
        GLint type;
        if (nrChannels == 1)
            type = GL_RED;
        else if (nrChannels == 3)
            type = GL_RGB;
        else if (nrChannels == 4)
            type = GL_RGBA;
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
            //std::cout << tex << "\n";
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << FileSystem::getPath(path) << "\n";
            std::cout << "Failed to load texture" << std::endl;
            exit(EXIT_FAILURE);
        }
        stbi_image_free(data);

}

void activateTexture(int value){
    glActiveTexture(GL_TEXTURE0 + value);
    glBindTexture(GL_TEXTURE_2D, tex);
}
};

#endif //PROJECT_BASE_MYTEXTURE_H
