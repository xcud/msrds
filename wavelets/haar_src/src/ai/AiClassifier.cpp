

#include "stdafx.h"
#include "aiclassifier.h"


AiClassifier::AiClassifier(const wchar_t* classifier_file) : m_status(ERR), m_ai_type(UNKNOWN),
                                                             m_ann(0), m_svm(0)
{
        //load classifier
        m_svm = new SVMachine(classifier_file);
        if(m_svm->status() == 0) {
                m_ai_type = SVM;
                m_status = CLASSIFIER;
                return;
        }
        m_ann = new ANNetwork(classifier_file);
        if (m_ann->status() == 0) {   
                if (m_ann->activation_function() == AnnLayer::SIGMOID)
                        m_ai_type = SIGMOID_ANN;
                else if (m_ann->activation_function() == AnnLayer::TANH)
                        m_ai_type = TANH_ANN;
                else
                        m_ai_type = ANN;
                m_status = CLASSIFIER;
                return;
        }
}
AiClassifier::AiClassifier(const wchar_t* classifier_file,
                           const wchar_t* features_file, 
                           const std::vector<ObjectSize>& objsizes) : m_status(ERR), m_ai_type(UNKNOWN),
                                                                      m_ann(0), m_svm(0)
{               
        //load classifier
        m_svm = new SVMachine(classifier_file);
        if(m_svm->status() == 0) {
                m_ai_type = SVM;
                m_status = CLASSIFIER;        
        }
        m_ann = new ANNetwork(classifier_file);
        if (m_ann->status() == 0) {   
                if (m_ann->activation_function() == AnnLayer::SIGMOID)
                        m_ai_type = SIGMOID_ANN;
                else if (m_ann->activation_function() == AnnLayer::TANH)
                        m_ai_type = TANH_ANN;
                else
                        m_ai_type = ANN;
                m_status = CLASSIFIER;                
        }
        
        if (m_status != CLASSIFIER) 
                return;
        
        //load features file
        for (unsigned int i = 0; i < (unsigned int)objsizes.size(); i++) {
                ObjectSize osize = objsizes[i];
                HaarFeatures* phf = new HaarFeatures();                
                if (phf->load(features_file, osize.width, osize.height) != 0 ||
                    phf->get_feature_vector().length() != get_input_dimension()) {
                        delete phf;
                        m_status = ERR;
                        return;
                }
                else                         
                        m_features.push_back(phf);                
        }

        m_status = CLASSIFIER | FEATURE_EXTRACTOR;
}

AiClassifier::~AiClassifier()
{
        if (m_ann != 0) delete m_ann;
        if (m_svm != 0) delete m_svm;

        for (unsigned int i = 0; i < (unsigned int)m_features.size(); i++) {
                HaarFeatures* phf = m_features[i];
                delete phf;
        }
        m_features.clear();
}
