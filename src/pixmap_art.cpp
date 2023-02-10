#include <iostream>
#include "image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
   // Image 1
   Image haverford;
   haverford.load("../images/haverford.jpg");
   haverford = haverford.colorReplace(Pixel(35,64,48), Pixel(0,0,0), 80);
   Image galaxy;
   galaxy.load("../images/galaxy.png");
   galaxy = galaxy.resize(haverford.width(), haverford.height());
   haverford = haverford.lightest(galaxy);
   haverford.save("../art/haverford.png");

   // Image 2
   Image beach;
   beach.load("../images/beach.png");
   beach = beach.sobel();
   beach = beach.grayscale();
   beach = beach.invert();
   beach = beach.colorReplace(Pixel(0, 0, 0), Pixel(0, 0, 255), 240);
   beach.save("../art/beach.png");

   // Image 3
   Image spongebob;
   spongebob.load("../images/spongebob.png");
   int rShift[2] = {-10, 0};
   int gShift[2] = {0, 10};
   int bShift[2] = {10, 0};
   spongebob = spongebob.colorReplace(Pixel(126, 190, 190), Pixel(0, 0, 0), 80);
   spongebob = spongebob.resize(spongebob.width() / 2, spongebob.height() / 2);
   Image fire;
   fire.load("../images/fire.jpg");
   spongebob = spongebob.halftone(rShift, bShift, gShift);
   fire = fire.resize(spongebob.width(), spongebob.height());
   spongebob = spongebob.lightest(fire);
   spongebob.save("../art/spongebob.png");

   Image earth;
   earth.load("../images/earth.png");

   Image rotated;
   rotated = earth.rotate90();
   rotated.save("../art/rotate90.png");

   Image inverted;
   inverted = earth.invert();
   inverted.save("../art/invert.png");

   Image jittered;
   jittered = earth.colorJitter(50);
   jittered.save("../art/colorJitter.png");

   Image channelShifted;
   channelShifted = earth.channelShift(rShift, gShift, bShift);
   channelShifted.save("../art/channelShift.png");

   Image halftoned;
   halftoned = earth.halftone(rShift, gShift, bShift);
   halftoned = halftoned.resize(earth.width(), earth.height());
   halftoned.save("../art/halftone.png");

   Image lightest;
   galaxy.resize(earth.width(), earth.height());
   lightest = earth.lightest(galaxy);
   lightest.save("../art/lightest.png");

   Image colorReplaced;
   colorReplaced = earth.colorReplace(Pixel(0, 0, 0), Pixel(10, 50, 30), 50);
   colorReplaced.save("../art/colorReplace.png");

   Image sobeled;
   sobeled = earth.sobel();
   sobeled.save("../art/sobel.png");

   Image blurred;
   blurred = earth.gaussianBlur(5);
   blurred.save("../art/gaussianBlur.png");


   return 0;
}

