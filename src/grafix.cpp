#include "grafix.h"

double Hue_2_RGB(double v1, double v2, double vH )             //Function Hue_2_RGB
{
   if ( vH >= 1 ) vH -= 1;
   if ( vH < 0 ) vH += 1;
   if ( ( 6.0 * vH ) < 1.0 ) return ( v1 + ( v2 - v1 ) * 6 * vH );
   if ( ( 2.0 * vH ) < 1.0 ) return ( v2 );
   if ( ( 3.0 * vH ) < 2.0 ) return ( v1 + ( v2 - v1 ) * 6 * ( ( 2.0 / 3.0 ) - vH ) );
   return ( v1 );
}
