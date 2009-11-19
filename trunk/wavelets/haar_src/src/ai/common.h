

#ifndef Common_h
#define Common_h


typedef struct _objectsize
{
        unsigned int width;
        unsigned int height;
} ObjectSize;


typedef struct _rect    //[left,top  right,bottom)
{
        int left;       //inclusive lower bound
        int top;
        int right;      //exclusive upper bound
        int bottom;        
        int sign;
} Rect;


typedef struct _feature
{
        std::wstring name;
        std::vector<Rect> rects;
} Feature;


#include "vec2di.h"
typedef struct _objrect {
        Rect rect;          //face rectangle
        unsigned int x;     //[x,y] face center coords
        unsigned int y;
        float radius;       //object radius
        vec2Di* object;      
} ObjRect, *PObjRect;


#endif Common_h

