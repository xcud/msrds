

#ifndef AiClassifier_h
#define AiClassifier_h


#include "common.h"
#include "vec2di.h"
#include "annlayer.h"
#include "annetwork.h"
#include "svm.h"
#include "haarfeatures.h"


class AiClassifier
{
public:
        enum Status { ERR = -1, CLASSIFIER = 1, FEATURE_EXTRACTOR = 2 };
        enum AiType { UNKNOWN, SVM, ANN, SIGMOID_ANN, TANH_ANN };

        AiClassifier(const wchar_t* classifier_file);
        AiClassifier(const wchar_t* classifier_file,
                     const wchar_t* features_file, const std::vector<ObjectSize>& objsizes);
        //AiClassifier(const AiClassifier& classifier);
        ~AiClassifier();

// Operators
        //const AiClassifier& operator=(const AiClassifier& classifier);

// Operations        
        inline int classify(const float* x, float* y);
        inline int classify(const vec2Di& integral_image, unsigned int obj_index, 
                            unsigned int dx, unsigned int dy, 
                            float* out, 
                            const AiClassifier* pprev = 0);

// Access        
        inline int status() const;
        inline AiType ai_type() const;
        inline unsigned int get_input_dimension() const;
        inline const HaarFeatures* get_features(unsigned int obj_index) const;

// Inquiry
        

protected:
private:
        AiClassifier(const AiClassifier& classifier);
        const AiClassifier& operator=(const AiClassifier& classifier);

        int m_status;
        enum AiType m_ai_type;
        
        ANNetwork* m_ann;
        SVMachine* m_svm;  

        std::vector<HaarFeatures *> m_features;


        inline int sign(float x) const;
};

// Inlines
inline int AiClassifier::status() const
{
        return m_status;
}

inline AiClassifier::AiType AiClassifier::ai_type() const
{
        return m_ai_type;
}

inline unsigned int AiClassifier::get_input_dimension() const
{
        switch (m_ai_type) {
        case SVM:
                return m_svm->dimension();                
        case ANN:
        case SIGMOID_ANN:
        case TANH_ANN:
                return m_ann->dimension();                
        default:
                return 0;
        }        
}

inline const HaarFeatures* AiClassifier::get_features(unsigned int obj_index) const
{
        return m_features[obj_index];
}

inline int AiClassifier::sign(float x) const
{
        return (x >= 0.0f) ? 1 : -1;
}

inline int AiClassifier::classify(const float* x, float* y)
{
        if (m_status == ERR)
                return 0;

        double dy;
        int s = 0;        

        switch (m_ai_type) {
        case SVM:
                s = m_svm->classify(x, dy);
                y[0] = float(dy);
                return s;

        case ANN:        
        case TANH_ANN:
                m_ann->classify(x, y);
                s = sign(y[0]);
                return s;

        case SIGMOID_ANN:
                m_ann->classify(x, y);        
                s = sign(y[0] - 0.5f);
                return s;

        default:
                *y = 0.0f;
                return 0;
        }                       
}

inline int AiClassifier::classify(const vec2Di& integral_image, unsigned int obj_index, 
                                  unsigned int dx, unsigned int dy, 
                                  float* out,
                                  const AiClassifier* pprev)
{        
        if (m_status != (CLASSIFIER | FEATURE_EXTRACTOR))
                return 0;

        const HaarFeatures* pprev_features = 0;
        if (pprev != 0)
                pprev_features = pprev->get_features(obj_index);
       
        HaarFeatures* phf = m_features[obj_index];
        if (phf->estimate(integral_image, dx, dy, pprev_features) <= 0)
                return 0;

        const float* x = phf->get_feature_vector().data(0, 0);
        
        return classify(x, out);
}

#endif AIClassifier_h

