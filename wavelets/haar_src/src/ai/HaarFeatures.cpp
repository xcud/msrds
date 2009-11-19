

#include "stdafx.h"
#include "HaarFeatures.h"


HaarFeatures::HaarFeatures() : m_object_width(0), m_object_height(0), m_object_size(0),
                               m_feature_vector(0)
{
}
HaarFeatures::~HaarFeatures()
{
        unload();
}

int HaarFeatures::load(const wchar_t* file, unsigned int object_width, unsigned int object_height)
{
        unload();

        FILE* fp = _wfopen(file, L"rt");
        if (fp == 0)
                return -1;

        unsigned int nfeatures;
        if (fwscanf(fp, L"%d", &nfeatures) != 1)
                return -2;

        m_feature_vector = new vec2D(1, nfeatures);

        for (unsigned int i = 0; i < nfeatures; i++) {
                wchar_t str[256] = L"";
                unsigned int nrects;
                if (fwscanf(fp, L"%s %d", str, &nrects) != 2) {
                        unload();
                        return -3;
                }
                Feature feature;
                feature.name = std::wstring(str);
                for (unsigned int j = 0; j < nrects; j++) {
                        Rect rect;
                        float coords[4] = {0.0f, 0.0f, 0.0f, 0.0f};                        
                        if (fwscanf(fp, L"%g %g %g %g %d", &coords[0], &coords[1],
                                                           &coords[2], &coords[3], &rect.sign) != 5) {
                                unload();
                                return -4;
                        }
                        rect.left = int((float)object_width * coords[0]);
                        rect.top = int((float)object_height * coords[1]);
                        rect.right = int((float)object_width * coords[2]);
                        rect.bottom = int((float)object_height * coords[3]);
                        feature.rects.push_back(rect);
                }
                m_features.push_back(feature);
        }

        fclose(fp);

        m_object_width = object_width;
        m_object_height = object_height;
        m_object_size = m_object_width * m_object_height;

        return 0;
}

void HaarFeatures::unload()
{
        m_features.clear();
        m_object_width = 0;
        m_object_height = 0; 
        m_object_size = 0;
        if (m_feature_vector != 0) {
                delete m_feature_vector;
                m_feature_vector = 0;
        }
}

int HaarFeatures::estimate(const vec2Di& integral_image, 
                           unsigned int dx, unsigned int dy,
                           const HaarFeatures* pprev)
{
        if (m_feature_vector == 0)
                return -1;

        m_feature_vector->set(0.0f);

        unsigned int index = 0;
        if (pprev != 0)
                index = pprev->get_feature_vector().length();

        for (unsigned int i = index; i < (unsigned int)m_features.size(); i++) {
                int sum = 0;
                Feature& feature = m_features[i];
                for (unsigned int j = 0; j < (unsigned int)feature.rects.size(); j++) {
                        Rect& rect = feature.rects[j];
                        Rect coords;                        
                        coords.left = (dx + rect.left) - 1;
                        coords.top = (dy + rect.top) - 1;
                        coords.right = (dx + rect.right) - 1;
                        coords.bottom = (dy + rect.bottom) - 1;
                        unsigned int A = integral_image.get(coords.top, coords.left);
                        unsigned int AB = integral_image.get(coords.top, coords.right);
                        unsigned int AC = integral_image.get(coords.bottom, coords.left);
                        unsigned int ABCD = integral_image.get(coords.bottom, coords.right);
                        unsigned int D = ABCD + A - (AB + AC);
                        if (rect.sign > 0)
                                sum += D;
                        else
                                sum -= D;
                }
                (*m_feature_vector)(0, i) = (float)sum;
        }

        m_feature_vector->div(float(m_object_size) * 255.0f);

        for (unsigned int i = 0; i < index; i++)
                (*m_feature_vector)(0, i) = pprev->get_feature_vector()(0, i);

        return m_feature_vector->length();
}
