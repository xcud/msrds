//---------------------------------------------------------------------------
// created:  20041102 by N.V.Shokhirev
// modified: 20041117 by N.V.Shokhirev
//---------------------------------------------------------------------------

#pragma hdrstop

#include "uRainbow.h"

//---------------------------------------------------------------------------



int rainbow(double t)
{
  int r = 255;
  int g = r << 8;
  int b = r << 16;
  int i;

  if( (t < 0.0) || (t > 1.0) ) return 0;
  double tt = t*5.0;
  if( tt < 1.0) { i = int( 255*tt ); return (r + (i << 8) ); }
  else if ( tt < 2.0) { i = int( 255*(2.0-tt) ); return (g + i ); }
  else if ( tt < 3.0) { i = int( 255*(tt-2.0) ); return (g + (i << 16) ); }
  else if ( tt < 4.0) { i = int( 255*(4.0-tt) ); return (b + (i << 8) ); }
  else { i = int( 255*(tt-4.0) ); return (b + i ); }
}

int rainbow1(double t)
{
  int r = 255;
  int g = r << 8;
  int b = r << 16;
  int i;

  if( (t < 0.0) || (t > 1.0) ) return 0;
  double tt = t*6.0;
  if( tt < 1.0) { i = int( 255*tt ); return (r + (i << 8) ); }
  else if ( tt < 2.0) { i = int( 255*(2.0-tt) ); return (g + i ); }
  else if ( tt < 3.0) { i = int( 255*(tt-2.0) ); return (g + (i << 16) ); }
  else if ( tt < 4.0) { i = int( 255*(4.0-tt) ); return (b + (i << 8) ); }
  else if ( tt < 5.0) { i = int( 255*(tt-4.0) ); return (b + i ); }
  else { i = int( 255*(6.0-tt) ); return (r + (i << 16) ); }
}

int PositiveNegative(RGB2 pair, double t)
{
  int c, cc, s1, s2, s;

  switch (pair)
  {
    case RG: { s1 =  0; s2 =  8; s = 16; }; break;
    case RB: { s1 =  0; s2 = 16; s =  8; }; break;
    case GR: { s1 =  8; s2 =  0; s = 16; }; break;
    case GB: { s1 =  8; s2 = 16; s =  0; }; break;
    case BR: { s1 = 16; s2 =  0; s =  8; }; break;
    case BG: { s1 = 16; s2 =  8; s =  0; }; break;
    default: ;
  }


  if(abs(t)>1.0) { c = int(255.0/abs(t)); cc = 0; }  // c = R
  else { c = 255; cc = int(255.0*(1.0-abs(t))); }    // c = B
  
  if (t>0.0) return ( (c << s1) + (cc << s) );
  else return ( (c << s2) + (cc << s) );
}

int Transition(RGB2 pair, double t)
{
  int c, cc, s1, s2, s;

  switch (pair)
  {
    case RG: { s1 =  0; s2 =  8; }; break;
    case RB: { s1 =  0; s2 = 16; }; break;
    case GR: { s1 =  8; s2 =  0; }; break;
    case GB: { s1 =  8; s2 = 16; }; break;
    case BR: { s1 = 16; s2 =  0; }; break;
    case BG: { s1 = 16; s2 =  8; }; break;
    default: ;
  }


  if(abs(t)>1.0) { c = int(255.0/abs(t)); cc = 0; }  // c = R
  else { c = 255; cc = int(255.0*(1.0-abs(t))); }    // c = B
  
  if (t>0.0) return ( (c << s1) + (cc << s2) );
  else return ( (c << s2) + (cc << s1) );
}


