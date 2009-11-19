

#ifndef HaarFeatures_h
#define HaarFeatures_h


#include "common.h"
#include "vec2D.h"
#include "vec2Di.h"


class HaarFeatures
{
public:
        HaarFeatures();
        //HaarFeatures(const HaarFeatures& hf);
        ~HaarFeatures();

// Operators
        //const HaarFeatures& operator=(const HaarFeatures& hf);

// Operations
        int load(const wchar_t* file, unsigned int object_width, unsigned int object_height);
        void unload();

        int estimate(const vec2Di& integral_image,                                      //-1 ERR not initialized yet
                     unsigned int dx, unsigned int dy,                                  //positive - size of feature vector 
                     const HaarFeatures* pprev = 0);                                 
                                                                                        
// Access
        inline unsigned int get_object_width() const;
        inline unsigned int get_object_height() const;
        inline const std::vector<Feature>& get_features() const;
        inline const vec2D& get_feature_vector() const;

// Inquiry

protected:
private:
        HaarFeatures(const HaarFeatures& hf);
        const HaarFeatures& operator=(const HaarFeatures& hf);        

        unsigned int m_object_width;
        unsigned int m_object_height;
        unsigned int m_object_size;

        std::vector<Feature> m_features;

        vec2D* m_feature_vector;

};

// Inlines
inline unsigned int HaarFeatures::get_object_width() const
{
        return m_object_width;
}

inline unsigned int HaarFeatures::get_object_height() const
{
        return m_object_height;
}

inline const std::vector<Feature>& HaarFeatures::get_features() const
{
        return m_features;
}

inline const vec2D& HaarFeatures::get_feature_vector() const
{
        return *m_feature_vector;
}

#endif HaarFeatures_h



/*
Features File:

Total Features Number

FeatureN Number_of_rects
Left Top Right Bottom (in 1.0 unit lengths)  Rect_sign



3

feature1 2
0.00 0.00 0.50 1.00   1
0.50 0.00 1.00 1.00  -1

feature2 2
0.00 0.00 1.00 0.50   1
0.00 0.50 1.00 1.00  -1

feature3 4
0.00 0.00 0.50 0.50   1
0.50 0.00 1.00 0.50  -1
0.00 0.50 0.50 1.00   1
0.50 0.50 1.00 1.00  -1
*/