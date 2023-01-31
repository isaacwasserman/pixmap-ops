// Copyright 2021, Aline Normoyle, alinen

#include "image.h"

#include <cassert>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <cstring>
#include <string>
#include <cmath>
#include <time.h>

namespace agl {

Pixel::Pixel(){
  this->r = 0;
  this->g = 0;
  this->b = 0;
}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b){
  this->r = r;
  this->g = g;
  this->b = b;
}

Pixel Pixel::operator=(const Pixel& other){
  this->r = other.r;
  this->g = other.g;
  this->b = other.b;
  return *this;
}

bool Pixel::operator==(const Pixel& other){
  return this->r == other.r && this->g == other.g && this->b == other.b;
}

Pixel Pixel::operator*(const Pixel& other){
  return Pixel(this->r * other.r, this->g * other.g, this->b * other.b);
}

Pixel Pixel::operator*(const int& operand){
  return Pixel(this->r * operand, this->g * operand, this->b * operand);
}

Pixel Pixel::operator*(const float& operand){
  return Pixel(std::round(this->r * operand), std::round(this->g * operand), std::round(this->b * operand));
}

Pixel Pixel::operator/(const float& operand){
  return Pixel(std::round(this->r / operand), std::round(this->g / operand), std::round(this->b / operand));
}

Pixel Pixel::operator+(const Pixel& other){
  return Pixel(std::min(255, this->r + other.r), std::min(255, this->g + other.g), std::min(255, this->b + other.b));
}

std::ostream& operator<<(std::ostream& stream, const Pixel& p) {
  stream << "(" << (int) p.r << ", " << (int) p.g << ", " << (int) p.b << ")";
  return stream;
}

Image::Image() {}

Image::Image(int width, int height) {
  mWidth = width;
  mHeight = height;
  mChannels = 3;
  mData = new unsigned char[width * height * mChannels];
}

Image::Image(const Image& orig) {
  mWidth = orig.mWidth;
  mHeight = orig.mHeight;
  mChannels = orig.mChannels;
  mData = new unsigned char[mWidth * mHeight * mChannels];
  memcpy(mData, orig.data(), mWidth * mHeight * mChannels);
}

Image& Image::operator=(const Image& orig) {
  if (this != &orig) {
    mWidth = orig.mWidth;
    mHeight = orig.mHeight;
    mChannels = orig.mChannels;
    mData = new unsigned char[mWidth * mHeight * mChannels];
    memcpy(mData, orig.data(), mWidth * mHeight * mChannels);
  }
  return *this;
}

Image::~Image() {
  if (mData != NULL) {
    delete[] mData;
  }
}

int Image::width() const { return mWidth; }

int Image::height() const { return mHeight; }

unsigned char* Image::data() const { return mData; }

void Image::set(int width, int height, unsigned char* data) {}

bool Image::load(const std::string& filename, bool flip) {
  int width, height, channels;
  mData = stbi_load(filename.c_str(), &width, &height, &channels, 3);
  mWidth = width;
  mHeight = height;
  mChannels = channels;
  if (mData != NULL) {
    return true;
  } else {
    return false;
  }
}

bool Image::save(const std::string& filename, bool flip) const {
  std::string ext = filename.substr(filename.find_last_of(".") + 1);
  for (int i = 0; i < ext.length(); i++) {
    ext[i] = std::tolower(ext[i]);
  }
  if (ext == "png"){
    return stbi_write_png(filename.c_str(), mWidth, mHeight, mChannels, mData, mWidth * mChannels);
  }
  else if(ext == "jpg" || ext == "jpeg"){
    return stbi_write_jpg(filename.c_str(), mWidth, mHeight, mChannels, mData, 90);
  }
  else if(ext == "bmp") {
    return stbi_write_bmp(filename.c_str(), mWidth, mHeight, mChannels, mData);
  }
  else if(ext == "tga") {
    return stbi_write_tga(filename.c_str(), mWidth, mHeight, mChannels, mData);
  }
  else if(ext == "hdr") {
    return stbi_write_hdr(filename.c_str(), mWidth, mHeight, mChannels, (float *) mData);
  }
  else {
    std::cerr << "Error: " << ext << " is not a valid file type." << std::endl;
    return false;
  }
}

Pixel Image::get(int row, int col) const {
  int i = (row * mWidth + col) * 3;
  return Pixel{mData[i], mData[i + 1], mData[i + 2]};
}

Pixel Image::get_rel(float yPercent, float xPercent, std::string method) const {
  if(method == "nearest"){
    int row = (int) std::lround(yPercent * mHeight);
    int col = (int) std::lround(xPercent * mWidth);
    return get(row, col);
  }
  else if(method == "bilinear"){
    float y = yPercent * mHeight;
    float x = xPercent * mWidth;

    float x1 = std::floor(x);
    float x2 = std::ceil(x);
    float y1 = std::floor(y);
    float y2 = std::ceil(y);

    Pixel Q11 = get(y1, x1);
    Pixel Q12 = get(y1, x2);
    Pixel Q21 = get(y2, x1);
    Pixel Q22 = get(y2, x2);

    Pixel P1;
    Pixel P2;
    if(x1 == x2){
      P1 = Q11;
      P2 = Q21;
    }
    else {
      P1 = (Q11 * (x2 - x)) + (Q12 * (x - x1));
      P2 = (Q21 * (x2 - x)) + (Q22 * (x - x1));
    }

    Pixel P;
    if(y1 == y2){
      P = P1;
    }
    else {
      P = (P1 * (y2 - y)) + (P2 * (y - y1));
    }

    return P;
  }
  else {
    std::cerr << "Error: \"" << method << "\" is not a sampling method" << std::endl;
    return Pixel{0, 0, 0};
  }
}

void Image::set(int row, int col, const Pixel& color) {
  int i = (row * mWidth + col) * 3;
  mData[i] = color.r;
  mData[i + 1] = color.g;
  mData[i + 2] = color.b;
}

Pixel Image::get(int i) const { return Pixel{0, 0, 0}; }

void Image::set(int i, const Pixel& c) {}

Image Image::resize(int w, int h) const {
  Image result(w, h);
  for(int row = 0; row < h; row++){
    for(int col = 0; col < w; col++){
      Pixel p = get_rel((float) row / h, (float) col / w, "bilinear");
      result.set(row, col, p);
    }
  }
  return result;
}

Image Image::flipHorizontal() const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      result.set(row, col, get(row, mWidth - col - 1));
    }
  }
  return result;
}

Image Image::flipVertical() const {
  Image result(mWidth, mHeight);
  for(int col = 0; col < mWidth; col++){
    for(int row = 0; row < mHeight; row++){
      result.set(row, col, get(mHeight - row - 1, col));
    }
  }
  return result;
}

Image Image::rotate90() const {
  Image result(mHeight, mWidth);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      result.set(col, mHeight - row - 1, get(row, col));
    }
  }
  return result;
}

Image Image::subimage(int startx, int starty, int w, int h) const {
  Image sub(w, h);
  for(int row = starty; row < starty + h; row++){
    for(int col = startx; col < startx + w; col++){
      sub.set(row - starty, col - startx, get(row, col));
    }
  }
  return sub;
}

void Image::replace(const Image& image, int startx, int starty) {
  for(int row = starty; row < starty + image.mHeight; row++){
    for(int col = startx; col < startx + image.mWidth; col++){
      set(row, col, image.get(row - starty, col - startx));
    }
  }
}

Image Image::swirl() const {
  Image result(0, 0);
  return result;
}

Image Image::add(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::subtract(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::multiply(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::difference(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::lightest(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::darkest(const Image& other) const {
  Image result(0, 0);

  return result;
}

Image Image::gammaCorrect(float gamma) const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      float r_tmp = p.r / 255.0;
      float g_tmp = p.g / 255.0;
      float b_tmp = p.b / 255.0;
      r_tmp = std::pow(r_tmp, 1/gamma);
      g_tmp = std::pow(g_tmp, 1/gamma);
      b_tmp = std::pow(b_tmp, 1/gamma);
      p.r = r_tmp * 255.0;
      p.g = g_tmp * 255.0;
      p.b = b_tmp * 255.0;
      result.set(row, col, p);
    }
  }
  return result;
}

Image Image::alphaBlend(const Image& other, float alpha) const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p1 = get(row, col);
      Pixel p2 = other.get(row, col);
      p1 = p1 * (1 - alpha) + p2 * alpha;
      result.set(row, col, p1);
    }
  }
  return result;
}

Image Image::invert() const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      p.r = 255 - p.r;
      p.g = 255 - p.g;
      p.b = 255 - p.b;
      result.set(row, col, p);
    }
  }
  return result;
}

Image Image::grayscale() const {
  Image result(mWidth, mHeight);
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      float value = 0.3 * p.r + 0.59 * p.g + 0.11 * p.b;
      p.r = value;
      p.g = value;
      p.b = value;
      result.set(row, col, p);
    }
  }
  return result;
}

Image Image::colorJitter(int size) const {
  Image image(mWidth, mHeight);
  srand(time(NULL));
  Pixel delta = Pixel((rand() % 255), (rand() % 255), (rand() % 255));
  delta = (delta / 255.0) * size;
  for(int row = 0; row < mHeight; row++){
    for(int col = 0; col < mWidth; col++){
      Pixel p = get(row, col);
      p = p + delta;
      image.set(row, col, p);
    }
  }
  return image;
}

Image Image::bitmap(int size) const {
  Image image(0, 0);

  return image;
}

void Image::fill(const Pixel& c) {}

}  // namespace agl
