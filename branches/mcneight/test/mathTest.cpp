/*
 * mathTest.cpp
 * Test of proper C99 compliant overloading of math functions, as well
 * as test for precision of variable types.
 *
 * For the square root of 2, the first 50 digits after the decimal
 * point (according to http://www.rossi.com/sqr2.htm) are:
 *                 1.41421356237309504880168872420969807856967187537694
 **************
 * When testing with g++ v3.2 on Solaris 8, the following is output:
Pertinent values for the floating point data types:
     (values for float, double, and long double, respectively.)

     sizeof(datatype) in bytes:          4              8              16
     decimal digit precision:            6             15              33
     minimum exponent of 10:           -37           -307           -4931
     maximum exponent of 10:            38            308            4932
     minimum positive value:  1.175494e-38  2.225074e-308  3.362103e-4932
     maximum positive value:  3.402823e+38  1.797693e+308  1.189731e+4932

PI:                3.141592653589793
(float)PI:         3.141592741012573
(double)PI:        3.141592653589793
(long double)PI:   3.141592653589793115997963468544185

square root of 2 = 1.41421356237309504880168872420969807856967187537694
float =            1.414213538169861
double =           1.414213562373095
long double =      1.414213562373095048801688724209698

 **************
 * For g++ 2.95.3 on Solaris 8 (which is not correct), it outputs:
Pertinent values for the floating point data types:
     (values for float, double, and long double, respectively.)

     sizeof(datatype) in bytes:          4              8              16
     decimal digit precision:            6             15              33
     minimum exponent of 10:           -37           -307           -4931
     maximum exponent of 10:            38            308            4932
     minimum positive value:  1.175494e-38  2.225074e-308  3.362103e-4932
     maximum positive value:  3.402823e+38  1.797693e+308  1.189731e+4932

PI:                3.141592653589793115997963468544185161590576171875
(float)PI:         3.1415927410125732421875
(double)PI:        3.141592653589793115997963468544185161590576171875
(long double)PI:   3.141592653589793115997963468544185161590576171875

square root of 2 = 1.41421356237309504880168872420969807856967187537694
float =            1.41421353816986083984375
double =           1.4142135623730951454746218587388284504413604736328125
long double =      1.4142135623730951454746218587388284504413604736328125

Incorrect overload with long double data type!

 **************
 * For g++ 3.2 under Cygwin on Windows 2000, it outputs:

Pertinent values for the floating point data types:
     (values for float, double, and long double, respectively.)

     sizeof(datatype) in bytes:          4              8              12
     decimal digit precision:            6             15              18
     minimum exponent of 10:           -37           -307           -4931
     maximum exponent of 10:            38            308            4932
     minimum positive value:  1.175494e-38  2.225074e-308  3.362103e-4932
     maximum positive value:  3.402823e+38  1.797693e+308  1.189731e+4932

PI:                3.141592653589793
(float)PI:         3.141592741012573
(double)PI:        3.141592653589793
(long double)PI:   3.141592653589793116

square root of 2 = 1.41421356237309504880168872420969807856967187537694
float =            1.414213538169861
double =           1.414213562373095
long double =      1.414213562373095049

 **************
 * This test fails both Visual Studio .NET and CodeWarrior 8 for Mac with
 * (sizeof(double) == sizeof(long double))
 * 
 * The comparison tests between g++ v3.2 and g++ v2.95.3 are
 * repeatable on Solaris 8, as well as HP/UX 11 and Red Hat Linux 7.2.
 * 
 * When these math functions are properly overloaded, explicit calls that
 * are required in C (defined in C99) are not required in C++. For example:
 * float          sqrtf(float);
 * double         sqrt(double);
 * long double    sqrtl(long double);
 * in C are all replaced by a single overloaded function, sqrt(), in C++.
 *
 */

#include <cfloat>		// for max and min floating point values
#include <cmath>
#include <cstdio>		// for printf()
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main()
{
  int		sf = 0, sd = 0, sld = 0;
  float         xf = 2, yf = 0;
  double        xd = 2, yd = 0;
  long double   xl = 2, yl = 0;
  ostringstream yo;

  cout.precision(70);
  yo.precision(70);
  
  // Thank you, Professor Flanigan for the code
  // and Professor Brehob for helping me to understand what it means
  
  sf = sizeof(float);
  sd = sizeof(double);
  sld = sizeof(long double);

  printf("\nPertinent values for the floating point data types:\n");
  printf("     (values for float, double, and long double, respectively.)\n\n");
  printf("     sizeof(datatype) in bytes:   %8i       %8i        %8i\n",sf,sd,sld);
  printf("     decimal digit precision:     %8d       %8d        %8d\n",FLT_DIG,DBL_DIG,LDBL_DIG);
  printf("     minimum exponent of 10:      %8d       %8d        %8d\n",FLT_MIN_10_EXP,DBL_MIN_10_EXP,LDBL_MIN_10_EXP);
  printf("     maximum exponent of 10:      %8d       %8d        %8d\n",FLT_MAX_10_EXP,DBL_MAX_10_EXP,LDBL_MAX_10_EXP);
  printf("     minimum positive value:  %1.6e  %1.6e  %1.6Le\n",FLT_MIN,DBL_MIN,LDBL_MIN);
  printf("     maximum positive value:  %1.6e  %1.6e  %1.6Le\n",FLT_MAX,DBL_MAX,LDBL_MAX);

  // Determine that sizeof(float) != sizeof(double) != sizeof(long double)
  if (sf == sd)
  {
    // Test is invalid: all three data types are not distinct
    cout << "Test Halted: sizeof(float) == sizeof(double)" << endl;
    exit(1);
  }
  else if (sd == sld)
  {
    // Test is invalid: all three data types are not distinct
    cout << "Test Halted: sizeof(double) == sizeof(long double)" << endl;
    exit(1);
  }

#if defined (M_PIl)
  #define PI M_PIl
#elif defined (M_PI)
  #define PI M_PI
#endif

#ifdef PI
  cout << endl;
  
  yf = PI;
  yd = PI;
  yl = PI;
  
  cout << "PI:                " << PI << endl;
  cout << "(float)PI:         " << yf << endl;
  cout << "(double)PI:        " << yd << endl;
  cout << "(long double)PI:   " << yl << endl;
#endif
  
  yf = sqrt(xf);
  yd = sqrt(xd);
  yl = sqrt(xl);

  // If properly overloaded, sqrt() should produce 3 different
  // results, depending on what type gets passed into it.

  cout << endl;
  cout << "square root of 2 = 1.41421356237309504880168872420969807856967187537694" << endl;
  cout << "float =            " << yf << endl;
  cout << "double =           " << yd << endl;
  cout << "long double =      " << yl << endl;

  yo << yl;
  //cout << yo.str() << endl;
  if ((yo.str() == "1.414213562373095048801688724209698") && (sld == 16))
  {
    return 0;
  }
  else if ((yo.str() == "1.414213562373095049") && (sld == 12))
  {
    return 0;
  }
  else
  {
    cout << endl << "Incorrect overload with long double data type!" << endl;
    return 1;
  }
}
