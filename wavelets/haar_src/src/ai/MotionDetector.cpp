

#include "stdafx.h"
#include "motiondetector.h"


#include "common.h"
#include "vec2di.h"
#include "haardetector.h"


MotionDetector::MotionDetector() : m_status(-1), m_last_frame(0), m_motion_vector(0), m_TH(10)
{
}

MotionDetector::~MotionDetector()
{
        close();
}

void MotionDetector::init(unsigned int image_width, unsigned int image_height)
{
        close();
        m_last_frame = new vec2Di(image_height, image_width);
        m_motion_vector = new vec2Di(image_height, image_width);
        m_status = 0;
}

void MotionDetector::close()
{
        if (m_last_frame != 0) {
                delete m_last_frame;
                m_last_frame = 0;
        }
        if (m_motion_vector != 0) {
                delete m_motion_vector;
                m_motion_vector = 0;
        }
        m_status = -1;
}

const vec2Di* MotionDetector::detect(const vec2Di& frame, const HaarDetector& hd)
{
        if (status() < 0)
                return 0;

        //m_last = frame - m_last
        m_last_frame->sub(frame, *m_last_frame);    

        //set to 1.0 faces rects
        RECT r0;
        for (unsigned int i = 0; i < hd.get_detected_objects_number(); i++) {
                const Rect* r = hd.get_detected_object_rect(i);
                unsigned int addx = (r->right - r->left) / 3;
                unsigned int addy = (r->bottom - r->top) / 3;
                r0.left = r->left + addx;
                r0.top = r->top + addy;
                r0.right = r->right - addx;
                r0.bottom = r->bottom - addy;
                m_last_frame->set(UCHAR_MAX, r0);
        }

        m_motion_vector->set(0);
        for (unsigned int y = hd.dy(); y < m_motion_vector->height() - hd.dy(); y++) {
                for (unsigned int x = hd.dx(); x < m_motion_vector->width() - hd.dx(); x++) {
                        if ((unsigned)abs((*m_last_frame)(y, x)) > m_TH)
                                (*m_motion_vector)(y, x) = 1;
                        else
                                (*m_motion_vector)(y, x) = 0;
                }
        }

        *m_last_frame = frame;        

        return m_motion_vector;
}

void MotionDetector::clear_last_frame()
{
        if (m_last_frame != 0)
                m_last_frame->set(0);
}