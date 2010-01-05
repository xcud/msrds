
#ifndef CVLIB_h
#define CVLIB_h


void cvInit(unsigned int image_width, unsigned int image_height,
            unsigned int face_width, unsigned int face_height, double zoom);

int cvInitAI(const wchar_t* face_detector,
             const wchar_t* projection_matrix,
             const wchar_t* skin_filter = 0,
             const wchar_t* preface_filter = 0);              //0-OK, <0-err

void cvSetScales(const double* scales, unsigned int size);     //scales = {0.9, 0.8, 0.7, ... N} N = size

const wchar_t* cvInfo();         //get fdetect setup params

int cvDetect(const unsigned char *pBGR);  

float cvMotionAmount();
float cvSkinAmount();
const vec2D* cvGetFace(unsigned int i);
unsigned int cvGetFacesNumber();
void cvFaceRect(unsigned int i, RECT& rect);                                //get face rectangle


void tic();
__int64 toc();

#endif
