

#include "stdafx.h"
#include "objectmap.h"


ObjectMap::ObjectMap(unsigned int image_width, unsigned int image_height,
                     unsigned int object_width, unsigned int object_height) : m_object_width(object_width), 
                                                                              m_object_height(object_height)
{
        m_object_map = new vec2D(image_height, image_width);
        m_tmp_object_map = new vec2D(image_height, image_width);
}
        
ObjectMap::~ObjectMap()
{
        delete m_object_map;
        delete m_tmp_object_map;
}
