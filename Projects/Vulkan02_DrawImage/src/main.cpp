#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


using namespace std;


int main() {
  int width = 512;
  int height = 512;
  int channels = 4;
  vector<uint8_t> img(width * height * channels, 0xff);

  stbi_write_png("result.png", width, height, channels, img.data(), width * channels);
  stbi_write_jpg("result.jpg", width, height, channels, img.data(), 100);

  cout << "It Works!" << endl;
  return 0;
}
