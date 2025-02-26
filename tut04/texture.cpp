#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../glad/glad.h"

#include "texture.hpp"

unsigned int MakeTexture(const char *filename, TextureOptions *options) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options->wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options->wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options->minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options->magFilter);

  if (options->flip)
    stbi_set_flip_vertically_on_load(true);
  int width, height, nrChannels;
  unsigned char *imageData =
      stbi_load(filename, &width, &height, &nrChannels, 0);
  unsigned int format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
  if (imageData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
                 GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    printf("loaded texture %u from %s width=%d, height=%d, nrChannels=%d\n",
           texture, filename, width, height, nrChannels);
    stbi_image_free(imageData);
  } else {
    printf("failed to load image data.\n");
  }

  return texture;
}
