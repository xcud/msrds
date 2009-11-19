

#include "stdafx.h"
#include "HaarDetector.h"


HaarDetector::HaarDetector() : m_status(-1), m_dx(0), m_dy(0),
                               m_image_width(0), m_image_height(0), 
                               m_integral_image(0),                                
                               m_detection_sensitivity((float(5) * 0.9f) + (float(20) * 0.07f)),
                               m_search_mask(0), m_tmp_search_mask(0), m_skin_filter(0),
                               m_motion_amount(-1.0f), m_skin_amount(-1.0f)
{
}        
HaarDetector::~HaarDetector()
{
        close();
        unload_skin_filter();
        clear_ai_classifiers();
        clear_detected_objects();
        clear_object_sizes();
}

void HaarDetector::add_object_size(unsigned int object_width, unsigned int object_height)
{
        ObjectSize osize;
        osize.width = object_width;
        osize.height = object_height;
        m_object_sizes.push_back(osize);

        osize = m_object_sizes[0];
        m_dx = osize.width / 2;
        m_dy = osize.height / 2;
        for (unsigned int i = 1; i < (unsigned int)m_object_sizes.size(); i++) {
                const ObjectSize& posize = m_object_sizes[i];
                if (posize.width < osize.width) {
                        osize.width = posize.width;
                        m_dx = osize.width / 2;
                }
                if (posize.height < osize.height) {
                        osize.height = posize.height;
                        m_dy = osize.height / 2;
                }                
        }
}
void HaarDetector::clear_object_sizes()
{
        m_object_sizes.clear();
}

int HaarDetector::load_skin_filter(const wchar_t* fname)
{
        unload_skin_filter();
        m_skin_filter = new AiClassifier(fname); 
        if (m_skin_filter->status() != AiClassifier::CLASSIFIER)
                return m_skin_filter->status();
        if (m_skin_filter->get_input_dimension() != 3) {
                unload_skin_filter();
                return -1;
        }
        return 0;
}
void HaarDetector::unload_skin_filter()
{
        if (m_skin_filter != 0) {
                delete m_skin_filter;
                m_skin_filter = 0;        
        }
}

int HaarDetector::add_ai_classifier(const wchar_t* classifier_file, const wchar_t* features_file)
{
        if (m_object_sizes.size() == 0)
                return -1;

        AiClassifier* pai_classifier = new AiClassifier(classifier_file, features_file, m_object_sizes);
        if (pai_classifier->status() != (AiClassifier::CLASSIFIER | AiClassifier::FEATURE_EXTRACTOR)) {
                delete pai_classifier;
                return -2;
        }
        else {
                m_ai_classifiers.push_back(pai_classifier);                
                return 0;
        }
}
void HaarDetector::clear_ai_classifiers()
{
        for (unsigned int i = 0; i < (unsigned int)m_ai_classifiers.size(); i++) {
                AiClassifier* pai_classifier = m_ai_classifiers[i];
                delete pai_classifier;
        }
        m_ai_classifiers.clear();
        m_status = -1;
}

int HaarDetector::init(unsigned int image_width, unsigned int image_height)
{        
        if (m_object_sizes.size() == 0)
                return -1;
        if (m_ai_classifiers.size() == 0)            
                return -2;

        AiClassifier* pai = m_ai_classifiers[m_ai_classifiers.size() - 1];
        if (pai->ai_type() != AiClassifier::SIGMOID_ANN)
                return -3;


        m_image_width = image_width;
        m_image_height = image_height;

        m_integral_image = new vec2Di(get_image_height(), get_image_width());

        for (unsigned int i = 0; i < (unsigned int)m_object_sizes.size(); i++) {
                const ObjectSize& osize = m_object_sizes[i];
                ObjectMap* omap = new ObjectMap(get_image_width(), get_image_height(),
                                                osize.width, osize.height);
                m_object_maps.push_back(omap);
        }

        m_search_mask = new vec2Di(get_image_height(), get_image_width());
        m_tmp_search_mask = new vec2Di(get_image_height(), get_image_width());
        
        m_status = 0;

        return 0;
}
void HaarDetector::close()
{
        if (m_integral_image != 0) {
                delete m_integral_image;
                m_integral_image = 0;
        }

        for (unsigned int i = 0; i < (unsigned int)m_object_maps.size(); i++) {         
                ObjectMap* omap = m_object_maps[i];
                delete omap;
        }
        m_object_maps.clear();        

        if (m_search_mask != 0) {
                delete m_search_mask;
                delete m_tmp_search_mask;
                m_search_mask = 0;
                m_tmp_search_mask = 0;
        }

        m_status = -1;
}

void HaarDetector::clear_detected_objects()
{
        for (unsigned int i = 0; i < get_detected_objects_number(); i++)
                delete m_detected_objects[i].object;
        m_detected_objects.clear();
}

void HaarDetector::compute_integral_image(const vec2Di& image)
{
        if (status() < 0)
                return;

        //copy image -> m_integral_image
        vec2Di& v = *m_integral_image;
        v = image;

        for (unsigned int i = 1; i < get_image_height(); i++)
                v(i, 0) += v(i - 1, 0);
        for (unsigned int j = 1; j < get_image_width(); j++)
                v(0, j) += v(0, j - 1);

        for (unsigned int i = 1; i < get_image_height(); i++) 
                for (unsigned int j = 1; j < get_image_width(); j++) 
                        v(i, j) += (v(i, j - 1) + v(i - 1, j)) - v(i - 1, j - 1);                                                
}


int HaarDetector::detect_objects(const vec2Di* y, 
                                 char** r, char** g, char** b,
                                 const vec2Di* search_mask)
{
        if (status() < 0)
                return status();
        
        estimate_motion_percent(search_mask);
                
        m_search_mask->set(1);
        if ((r != 0 && g != 0 && b != 0) && m_skin_filter != 0)
                skin_filter(r, g, b, search_mask);        
        if (search_mask != 0)
                m_search_mask->and(*search_mask);                        

        m_pimage = y;
        compute_integral_image(*y);

        run_classifiers();
        return search_faces();        
}


void HaarDetector::run_classifiers()
{
        float oval = 0.0f;
        for (unsigned int i = 0; i < (unsigned int)m_object_maps.size(); i++) {
                const ObjectMap* pom = m_object_maps[i];
                unsigned int dx = pom->get_object_width() / 2;
                unsigned int dy = pom->get_object_height() / 2;
                vec2D& omap = pom->get_object_map();
                omap.set(0.0f);
                for (unsigned int y = dy; y < omap.height() - dy; y++) {
                        for (unsigned int x = dx; x < omap.width() - dx; x++) {
                                //negotiate with motion & skin detector out
                                if ((*m_search_mask)(y, x) == 0)
                                        continue;

                                for (unsigned int j = 0; j < (unsigned int)m_ai_classifiers.size(); j++) {                                        
                                        const AiClassifier* pprev_ai = 0;     //previous stage ai classifier
                                        if (j > 0)
                                                pprev_ai = m_ai_classifiers[j - 1];
                                        AiClassifier* pai = m_ai_classifiers[j];                                        
                                        int cls = pai->classify(*m_integral_image, i, x - dx, y - dy, &oval, pprev_ai);
                                        if (j != (unsigned int)m_ai_classifiers.size() - 1) {   //first stages classifiers
                                                if (cls < 0) {
                                                        omap(y, x) = 0.07f;
                                                        break;
                                                }                                        
                                        }
                                        else    //last classification stage: ANN with sigmoid out layer
                                                omap(y, x) = oval;                                                                                
                                }
                        }
                }
                //debug
                //omap.print(L"object_map.txt");
        }
}

unsigned int HaarDetector::search_faces()
{
        clear_detected_objects();

        //debug out only
        for (unsigned int i = 0; i < (unsigned int)m_object_maps.size(); i++) {
                ObjectMap* pom = m_object_maps[i];                
                pom->get_tmp_object_map().copy(pom->get_object_map());
        }
        //debug out only

        ObjectMap* pom = 0;
        int tmpx = 0, tmpy = 0;
        unsigned int x = 0, y = 0, index = 0;
        float max = 0.0f, tmpmx = 0.0f;        
        while (true) {
                float max = -FLT_MAX;
                index = 0;
                for (unsigned int i = 0; i < (unsigned int)m_object_maps.size(); i++) {
                        pom = m_object_maps[i];
                        unsigned int dx = pom->get_object_width() / 2;
                        unsigned int dy = pom->get_object_height() / 2;
                        pom->get_object_map().max_val(tmpmx, tmpx, tmpy, 5, 5, dx, dy);
                        if (tmpmx > max) {
                                index = i;
                                max = tmpmx;
                                x = tmpx;
                                y = tmpy;
                        }
                }
                
                pom = m_object_maps[index];                
                if (max > detection_sensitivity()) {
                        unsigned int w2 = pom->get_object_width() / 2;
                        unsigned int h2 = pom->get_object_height() / 2;
                        Rect rect;
                        rect.left = x - w2;
                        rect.top = y - h2;
                        rect.right = x + w2;
                        rect.bottom = y + h2;
                        ObjRect obj_rect;
                        obj_rect.rect = rect;
                        obj_rect.x = x;
                        obj_rect.y = y;
                        obj_rect.radius = sqrt(pow(float(w2), 2.0f) + pow(float(h2), 2.0f));
                        if (is_foundobject_overlaps(obj_rect) == false) {
                                obj_rect.object = new vec2Di(pom->get_object_height(), pom->get_object_width());
                                obj_rect.object->copy(*m_pimage, x - w2, y - h2);                                
                                m_detected_objects.push_back(obj_rect);
                                erase_object_rect(rect);            //erase found face region  [left, right)
                        } else {
                                erase_object_rect(x, y);                                                                
                        }
                }
                else
                        break;                
        }

        return (unsigned int)m_detected_objects.size();
}

bool HaarDetector::is_foundobject_overlaps(const ObjRect& or) const
{
        for (unsigned int i = 0; i < (unsigned int)m_detected_objects.size(); i++) {
                const ObjRect* por = &m_detected_objects[i];
                //euclid distance between centers
                float dist = sqrt(pow((float(por->x) - float(or.x)), 2)
                                   + pow((float(por->y) - float(or.y)), 2));
                if (dist <= 0.90f * (or.radius + por->radius))
                        return true;
        }
        return false;
}

void HaarDetector::erase_object_rect(const Rect& r)
{
        RECT zr;
        zr.left = r.left;
        zr.top = r.top;
        zr.right = r.right + 1;
        zr.bottom = r.bottom + 1;
        for (unsigned int i = 0; i < (unsigned int)m_object_maps.size(); i++) {
                ObjectMap* pom = m_object_maps[i];
                pom->get_object_map().set(0.0f, zr);
        }
}

void HaarDetector::erase_object_rect(unsigned int x, unsigned int y)
{                
        for (unsigned int i = 0; i < (unsigned int)m_object_maps.size(); i++) {
                ObjectMap* pom = m_object_maps[i];
                RECT zr;
                zr.left = x - 2;
                zr.top = y - 2;
                zr.right = x + 3;
                zr.bottom = y + 3;
                pom->get_object_map().set(0.0f, zr);                
        }
}


void HaarDetector::estimate_motion_percent(const vec2Di* search_mask)
{
        if (search_mask == 0)
                m_motion_amount = -1.0f;
        else {
                unsigned int motion_pixels = 0;
                unsigned int total_pixels = 0;
                for (unsigned int y = 0; y < search_mask->height(); y++) {
                        for (unsigned int x = 0; x < search_mask->width(); x++) {
                                total_pixels++;
                                if ((*search_mask)(y, x) == 1)
                                        motion_pixels++;
                        }
                }
                m_motion_amount = float(motion_pixels) / float(total_pixels);
        }
}

/*
        check if (m_skin_filter != 0 && m_skin_filter->status() == 0) before call
*/
void HaarDetector::skin_filter(char** r, char** g, char** b, const vec2Di* search_mask)
{
        float ivec[3] = {0.0f, 0.0f, 0.0f};    //0.0 ... 1.0f range
        float ovec = 0.0f;

        m_search_mask->set(0);

        unsigned int skin_pixels = 0;
        unsigned int total_pixels = 0;
        for (unsigned int y = dy(); y < m_search_mask->height() - dy(); y++) {
                for (unsigned int x = dx(); x < m_search_mask->width() - dx(); x++) {
                        total_pixels++;

                        if (search_mask != 0 && ((*search_mask)(y, x) == 0))                                 
                                continue;

                        ivec[0] = (float)((int)r[y][x] + 128) / 255.0f;
                        ivec[1] = (float)((int)g[y][x] + 128) / 255.0f;
                        ivec[2] = (float)((int)b[y][x] + 128) / 255.0f;
                        if (m_skin_filter->classify(ivec, &ovec) >= 0) {
                                (*m_search_mask)(y, x) = 1;
                                skin_pixels++;
                        }
                }
        }
        m_skin_amount = float(skin_pixels) / float(total_pixels);

        m_tmp_search_mask->dilate(*m_search_mask, 5, 5);
        m_search_mask->erode(*m_tmp_search_mask, 5, 5);
}

