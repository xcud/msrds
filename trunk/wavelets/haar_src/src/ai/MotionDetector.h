

#ifndef MotionDetector_h
#define MotionDetector_h


class vec2Di;
class HaarDetector;


class MotionDetector
{
public:
        MotionDetector();
        //MotionDetector(const MotionDetector& md);
        ~MotionDetector();

// Operators
        //const MotionDetector& operator=(const MotionDetector& md);
        void init(unsigned int image_width, unsigned int image_height);
        void close();

        const vec2Di* detect(const vec2Di& frame, const HaarDetector& hd);        //frame 0...255
        void clear_last_frame();

// Operations
// Access
        inline int status() const;
        inline const vec2Di& get_motion_vector() const;
        inline unsigned int threshold() const;
        inline void threshold(unsigned int th);


// Inquiry

protected:
private:
        MotionDetector(const MotionDetector& md);
        const MotionDetector& operator=(const MotionDetector& md);

        int m_status;
        vec2Di* m_last_frame;
        vec2Di* m_motion_vector;
        unsigned int m_TH;

};

// Inlines
inline int MotionDetector::status() const
{
        return m_status;
}

inline const vec2Di& MotionDetector::get_motion_vector() const
{
        return *m_motion_vector;
}

inline unsigned int MotionDetector::threshold() const
{
        return m_TH;
}

inline void MotionDetector::threshold(unsigned int th)
{        
        m_TH = th;
}

#endif MotionDetector_h

