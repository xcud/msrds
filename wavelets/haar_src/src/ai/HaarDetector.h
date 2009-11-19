

#ifndef HaarDetector_h
#define HaarDetector_h


#include "common.h"
#include "vec2di.h"
#include "objectmap.h"
#include "aiclassifier.h"


class HaarDetector
{
public:
        HaarDetector();
        //HaarDetector(const HaarDetector& hd);
        ~HaarDetector();

// Operators
        //const HaarDetector& operator=(const HaarDetector& hd);

// Operations        
        void add_object_size(unsigned int object_width, unsigned int object_height);
        void clear_object_sizes();
        int load_skin_filter(const wchar_t* fname);
        void unload_skin_filter();
        int add_ai_classifier(const wchar_t* classifier_file, const wchar_t* features_file);
        void clear_ai_classifiers();        
        int init(unsigned int image_width, unsigned int image_height);
        void close();
        
        int detect_objects(const vec2Di* y, 
                           char** r = 0, char** g = 0, char** b = 0,
                           const vec2Di* search_mask = 0);

// Access
        inline int status() const;
        inline unsigned int dx() const;
        inline unsigned int dy() const;
        inline unsigned int get_image_width() const;
        inline unsigned int get_image_height() const;
        inline const vec2Di& get_integral_image() const;                
        inline float detection_sensitivity() const;
        inline void detection_sensitivity(float th);
        inline unsigned int get_detected_objects_number() const;
        inline const Rect* get_detected_object_rect(unsigned int i) const; 
        inline const vec2Di* get_detected_object(unsigned int i) const;        

        //for debugging purposes
        inline const vec2Di* get_search_mask() const;
        inline const ObjectMap* get_object_map(unsigned int i) const;                

// Inquiry

protected:
private:
        HaarDetector(const HaarDetector& hd);
        const HaarDetector& operator=(const HaarDetector& hd);


        int m_status;

        unsigned int m_dx;              //search offset, (min objsize / 2)
        unsigned int m_dy;

        std::vector<ObjectSize> m_object_sizes;

        unsigned int m_image_width;
        unsigned int m_image_height;

        const vec2Di* m_pimage;
        vec2Di* m_integral_image;        
        
        std::vector<ObjectMap *> m_object_maps;
        std::vector<AiClassifier *> m_ai_classifiers;
        
        float m_detection_sensitivity;  //object detection sensitivity 0 - 25

        std::vector<ObjRect> m_detected_objects;
        
        void clear_detected_objects();
        void compute_integral_image(const vec2Di& image);        
        void run_classifiers();
        unsigned int search_faces();
        bool is_foundobject_overlaps(const ObjRect& or) const;        
        void erase_object_rect(const Rect& r);
        void erase_object_rect(unsigned int x, unsigned int y);


        float m_skin_amount;            //-1.0f - skin filter absent; 0.0f - 1.0f skin amount
        float m_motion_amount;          //-1.0f - no motion estimation; 0.0f - 1.0f motion amount
        vec2Di* m_search_mask;
        vec2Di* m_tmp_search_mask;
        AiClassifier* m_skin_filter;
        void estimate_motion_percent(const vec2Di* search_mask);
        void skin_filter(char** r, char** g, char** b, const vec2Di* search_mask = 0);        
        
};

// Inlines
inline int HaarDetector::status() const
{
        return m_status;
}

inline unsigned int HaarDetector::dx() const
{
        return m_dx;
}

inline unsigned int HaarDetector::dy() const
{
        return m_dy;
}

inline unsigned int HaarDetector::get_image_width() const
{
        return m_image_width;
}

inline unsigned int HaarDetector::get_image_height() const
{
        return m_image_height;
}

inline const vec2Di& HaarDetector::get_integral_image() const
{
        return *m_integral_image;
}

inline float HaarDetector::detection_sensitivity() const
{
        return m_detection_sensitivity;
}

inline void HaarDetector::detection_sensitivity(float th)
{
        if (th < 0.0)
                th = 0.0;
        else if (th > 25.0)
                th = 25.0;
        m_detection_sensitivity = th;
}

inline unsigned int HaarDetector::get_detected_objects_number() const
{
        return m_detected_objects.size();
}

inline const Rect* HaarDetector::get_detected_object_rect(unsigned int i) const
{
        if (get_detected_objects_number() > 0)
                return &m_detected_objects[i % get_detected_objects_number()].rect;
        else
                return 0;
}

inline const vec2Di* HaarDetector::get_detected_object(unsigned int i) const
{
        if (get_detected_objects_number() > 0)
                return m_detected_objects[i % get_detected_objects_number()].object;
        else
                return 0;
}

inline const vec2Di* HaarDetector::get_search_mask() const
{
        return m_search_mask;
}

inline const ObjectMap* HaarDetector::get_object_map(unsigned int i) const
{
        if (m_object_maps.size() > 0)
                return m_object_maps[i % m_object_maps.size()];
        else 
                return 0;
}

#endif HaarDetector_h

