

#ifndef ObjectMap_h
#define ObjectMap_h

#include "vec2D.h"

class ObjectMap
{
public:
        ObjectMap(unsigned int image_width, unsigned int image_height,
                  unsigned int object_width, unsigned int object_height);
        //ObjectMap(const ObjectMap& om);
        ~ObjectMap();

// Operators
        //const ObjectMap& operator=(const ObjectMap& om);

// Operations
        
// Access
        inline unsigned int get_object_width() const;
        inline unsigned int get_object_height() const;
        inline vec2D& get_object_map() const;
        inline vec2D& get_tmp_object_map() const;

// Inquiry

protected:
private:
        ObjectMap(const ObjectMap& om);
        const ObjectMap& operator=(const ObjectMap& om);


        unsigned int m_object_width;
        unsigned int m_object_height;

        vec2D* m_object_map;
        vec2D* m_tmp_object_map;
        
};

// Inlines
inline unsigned int ObjectMap::get_object_width() const
{
        return m_object_width;
}

inline unsigned int ObjectMap::get_object_height() const
{
        return m_object_height;
}

inline vec2D& ObjectMap::get_object_map() const
{
        return *m_object_map;
}

inline vec2D& ObjectMap::get_tmp_object_map() const
{
        return *m_tmp_object_map;
}


#endif ObjectMap_h

