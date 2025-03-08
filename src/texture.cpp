#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #include "../glad/glad.h"

#include "texture.hpp"

unsigned int MakeTexture(const char *filename, TextureOptions *options) {
  unsigned int texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  stbi_set_flip_vertically_on_load(options->flip);
  int width, height, nrChannels;
  unsigned char *imageData =
      stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!imageData) {
    stbi_image_free(imageData);
    printf("failed to load image data %s.\n", filename);
    return texture;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options->wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options->wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options->minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options->magFilter);

  GLenum format = (nrChannels == 3)   ? GL_RGB
                  : (nrChannels == 4) ? GL_RGBA
                                      : GL_RED;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
               GL_UNSIGNED_BYTE, imageData);
  glGenerateMipmap(GL_TEXTURE_2D);

  printf("loaded texture %u from %s width=%d, height=%d, nrChannels=%d\n",
         texture, filename, width, height, nrChannels);
  stbi_image_free(imageData);

  return texture;
}
