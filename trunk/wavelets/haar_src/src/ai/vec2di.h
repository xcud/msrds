

#ifndef VEC2Di_H
#define VEC2Di_H


class vec2Di
{
public:
        vec2Di(unsigned int ysize = 1, unsigned int xsize = 1, const int* pdata = 0);
        vec2Di(const wchar_t* file);
        vec2Di(const vec2Di& v);
        ~vec2Di();

// Operators
        inline const vec2Di& operator=(const vec2Di& v);
        inline int& operator()(unsigned int y, unsigned int x);                         //operator v(y,x) = v.m_data[y][x]
        inline int operator()(unsigned int y, unsigned int x) const;                    //const operator v(y,x) = v.m_data[y][x]

// Operations
        void set(int scalar);
        void set(int scalar, RECT& r);
        void print(const wchar_t* file) const;
        bool copy(const vec2Di& v, int left = 0, int top = 0);                          //copy v's (this_size)[hxw] region from dx,dy offset to this [v>=this in size]
        int min_val() const;
        int max_val() const;
        int dilate(const vec2Di& src, unsigned int w = 3, unsigned int h = 3);          //this = dilate(src)
        int erode(const vec2Di& src, unsigned int w = 3, unsigned int h = 3);           //this = erode(src)
        void and(const vec2Di& v);                                                      //this = this AND v
        void add(const vec2Di& a, const vec2Di& b);                                     //this = a.+b
        void sub(const vec2Di& a, const vec2Di& b);                                     //this = a.-b

// Access
        inline unsigned int width() const;                                      //width of array
        inline unsigned int height() const;                                     //height of array
        inline unsigned int length() const;                                     //total size of array width*height
        inline int get(int y, int x, bool zeropad = true) const;       

// Inquiry
	bool is_size_equal(const vec2Di& v) const;

protected:
private:
        unsigned int m_width;
        unsigned int m_height;
        int** m_data;

        void init(unsigned int ysize, unsigned int xsize);                      //allocate m_data only
        void init(const vec2Di &v);                                              //fill this->m_data with v.m_data[:][:]
        void close(void);                                                       //deallocate m_data
        
};

// Inlines
inline unsigned int vec2Di::width() const
{
        return m_width;
}

inline unsigned int vec2Di::height() const
{
        return m_height;
}

inline unsigned int vec2Di::length() const
{
        return m_width*m_height;
}

inline int& vec2Di::operator()(unsigned int y, unsigned int x)
{
        return m_data[y][x];
}

inline int vec2Di::operator()(unsigned int y, unsigned int x) const
{
        return m_data[y][x];
}

inline const vec2Di& vec2Di::operator=(const vec2Di& v)
{
        if (this == &v) {
                return *this;
        } else if (width() == v.width() && height() == v.height()) {  //equal size arrays?                
                init(v);
        } else {  //create a complete copy from v
                close();
                init(v.height(), v.width());
                init(v);
        }
        return *this;
}

inline int vec2Di::get(int y, int x, bool zeropad) const
{
        if (zeropad == true) {
                if (x < 0 || x > ((int)width() - 1))
                        return 0;
                else if (y < 0 || y > ((int)height() - 1))
                        return 0;
                else
                        return m_data[y][x];
        }
        else {
                if (x < 0) 
                        x = -x;
                else if (x > ((int)width() - 1)) 
                        x = ((int)width() - 1) - (x - ((int)width() - 1));
                if (y < 0) 
                        y = -y;
                else if (y > ((int)height() - 1)) 
                        y = ((int)height() - 1) - (y - ((int)height() - 1));
                return m_data[y][x];
        }        
}


#endif VEC2Di_H

