//  ----- interface file  -----
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include "color.h"

using namespace std;

const int DefaultWidth =32;
const int DefaultHeight=24;
const int DefaultDepth =255;

// Class Image:
//   The copy constructor and assignment operator must be
//     overloaded to copy from one variable to another, otherwise
//     the copied variables would share the image data.
//     In this case the only variable that is dynamically
//     allocated is the pixels array.  It is a one
//     dimensional array of size (width*height).
//
//   Functions that _may_* need to use the new[] operator
//     All constructors
//     resize()
//     operator=
//
//   Functionst that _may_* need to use the delete[] operator
//     resize()
//     operator=
//
//   *   Many of these functions can get away without using
//         the new[] or delete[] operators if you have them
//         either distribute the work to a helper function,
//         or use other member functions that can do all of the
//         work.

class Image
{
  private:
    Color *pixels;
    int width, height;
    int depth;

  public:
    //
    //   Constructors use Default values if any given parameter is 
    //     invalid.  Depth, width, and height values less than 1 are
    //     invalid.
    //
    Image();
    // Start with bg as the background color of the image.
    Image(Color bg);
    // Start with an image of width w and height h
    Image(int w, int h);
    // The copy constructor must do a deep copy.
    Image(const Image& img);

    //Destructor
    ~Image();


    // Resize is used to change the width and the height of the image.
    // If w or h <=0, or if they are the same as the current height
    //   and width, then nothing is done.
    // If the image does change size, then the old image will be 
    //   deallocated and a new array of pixels will be allocated.
    void resize(int w, int h);

    // accessors for depth
    // setdepth must check to verify that the requested depth
    // is not < 1.  All values < 1 are invalid.  If an invalid
    // value is passed, then the depth does not change.
    void setdepth(int d);
    int getdepth() const;

    // accessors for width and height
    int getwidth() const;
    int getheight() const;

    // allows access to an arbitray pixel
    // any color can be set, but when output an
    // image always truncates colors to a max of depth
    // NOTE : Consider why in this instance this
    //        function overloading is possible...
    //        The key is the const at the end
    //        of the expression.
    // NOTE : returning a reference from the () operator is in 
    //        general not a good idea as it neglects bounds checking.
    //        In this case it is used because the only bounds checking
    //        is done on output, therefore all values are valid.
    //        (See the insertion operator for more information on
    //         truncation)
    // operator() does not need check to verify that x and y are within
    // the bounds of the image.
    Color &operator() (int x, int y);
    const Color &operator() (int x, int y) const;

    // Make sure that you handle the case of
    //   Image img;
    //   img=img;
    // In this case you do not have to do anything, 
    //   but if your assignment operator does not
    //   recognize that, then you will probably
    //   delete your entire image.
    // The assignment operator must do a deep copy.
    Image & operator=(Image& img);

    // Clear the image and make all pixels the same
    //   color.
    void clear(Color bg=Color());

};



// Extraction and insertion operators for the Image Class.

// The insertion operator outputs a ppm image.  See the spec for
//   the details of the image format.  An example is given below.
// Example:    
//   P3
//   2 5
//   255
//   255 0 0  0 0 0
//   0 0 0  0 0 0
//   0 0 0  0 255 0
//   0 0 0  0 0 0
//   0 0 255  0 0 0
// NOTE:  Here we have an endl after the "P3", height, color depth, 
//        and each row of pixels.  The endl after the "P3", height,
//        and color depth should always be included.  The endl
//        after each of the rows is optional.  We also have an extra 
//        space between each of the Color points.  This is not required,
//        but it is highly suggested as it will help in debugging
//        immensely.
// 
// Another responsibility of the insertion operator is to truncate
//   all of the pixel values between 0 and pixeldepth.
// It does not change the image, but the resulting file will only
//   have colors with components between 0 and pixeldepth.
// This is the only place that checks the pixels to verify
//   their color depth.  By restricting the places which 
//   truncate, we can allow for repeated filters without a 
//   loss of detail.  This also allows for increasing the 
//   color depth after a filter run.
// Examples of pixel truncation
//   Ex...  Depth 255  (r,g,b)
//        (-10, 100, 300) => (  0, 100, 255)
//        (999, 999, 999) => (255, 255, 255)
//        (-99, -99, -99) => (  0,   0,   0)
//   Note : There is _no_ scaling of the colors as this would
//     involve additional computation and complication. If you
//     do not know what color "scaling" means, ignore this comment.
ostream& operator<<(ostream& os, const Image& image);


// The extraction operator will perform the inverse of the 
//   insertion operator.  None of the image's member variables
//   will be updated until all of the data has been successfully
//   read.  If any fail, then the image will remain unchanged.
//   Further if any of the image data is successfully read, but
//   is not correct, then the image is not changed, and if the
//   stream is still in the good state, it is set to the fail state.
//
// Example:
//   P8
//   2 5
//   255
//   255 0 0  0 0 0
//   0 0 0  0 0 0
//   0 0 0  0 255 0
//   0 0 0  0 0 0
//   0 0 255  0 0 0
//   
//   This image should return in the fail state, as it does not have
//   the correct image header.  Here we will need to set the fail state.
//
//   P3
//   2 2
//   255
//   255 0 0  0 0 0
//   0 0 0  0 0 0
//   0 0 0  0 255 0
//   0 0 0  0 0 0
//   0 0 255  0 0 0
//   
//   This image should input properly, and just not use the entire input.
//
//   P3
//   2 9
//   255
//   255 0 0  0 0 0
//   0 0 0  0 0 0
//   0 0 0  0 255 0
//   0 0 0  0 0 0
//   0 0 255  0 0 0
//   
//   This image should fail as it will run out of integers.  It will hit
//   the end of the file, and return with the stream state unchanged.
//   In this example the stream state should not be changed!  It is already
//   in the appropriate state.
//
//   P3
//   -1 9
//   255
//   255 0 0  0 0 0
//   0 0 0  0 0 0
//   0 0 0  0 255 0
//   0 0 0  0 0 0
//   0 0 255  0 0 0
//   
//   This image will fail because after it reads the -1 for the width
//   it will not be able to proceed, and it will return the stream
//   state in the failed state.  In this case it will need to set the 
//   stream to the fail state as -1 _is_ an integer.
//
//
// NOTE : Many of these examples are too extreme!  We are not going
//        to be testing in this great of detail.  We will test :
//  
//          That you check the header, and don't just ignore it.
//          That you don't change the image if there is not
//            enough information to load a full image.
//          Invalid Color data (i.e. 1 0 a)
//        
//        We won't be checking :
//          For negative widths, heights and color depths for the
//            extraction operator(in other cases we will check...).
//          Color depths that don't match the color data.
//          Any cases that examples aren't given for.
//
istream& operator>>(istream& is, Image& image);

#endif  // EFIMAGE_H
