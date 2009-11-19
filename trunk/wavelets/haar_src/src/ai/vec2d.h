
#ifndef VEC2D_H
#define VEC2D_H


class vec2D       //16 byte aligned
{
public:
        enum CONV { SAME, VALID };

        vec2D(const wchar_t* file);
        vec2D(unsigned int ysize = 1, unsigned int xsize = 1, 
              const float* pdata = 0);                                          // NxM vector,  data copied in [row1][row2] ... [rowN] form
        vec2D(const vec2D& v);
        ~vec2D();

// Operators
        inline const vec2D& operator=(const vec2D& v);                          //this = v        refs
        inline const vec2D& operator=(const float* pf);                         //this = pf
        inline float& operator()(int y, int x);                                 //operator v(y,x) = v.m_data[y][x]
        inline float operator()(int y, int x) const;                            //const operator v(y,x) = v.m_data[y][x]
        inline float& operator[](unsigned int i);                               //operator v[i] = v.m_data[y][x]  0-offset operator MATLAB order like
        inline float operator[](unsigned int i) const;                          //const operator v[i] = v.m_data[y][x]  0-offset operator MATLAB order like
        inline const vec2D& operator*(const vec2D& b) const;                    //new c = this*b      refs

// Operations
        void print(const wchar_t* file = 0) const;                              //dump contents
        inline float get(int y, int x) const;                                   //get periodic boundary conditions
        void set(float scalar);                                                 //set all array to a scalar
        void set(float scalar, RECT& r);                                        //set selected rect in array to a scalar [ left, right )
        void setrand();                                                         //set all array to rand values
        bool copy(const vec2D& v, int left = 0, int top = 0);                   //copy v's (this_size)[hxw] region from dx,dy offset to this [v>=this in size]
        void minmax(float& mn, float& mx) const;                                //get min max values
        float max_val() const;                                                  //get max value
        float min_val() const;                                                  //get min value
        void max_val(float& mx, int& x, int& y,                                 //get max value with [x,y] coord
                 int sizex = 1, int sizey = 1, int dx = 0, int dy = 0) const;   //from offset dx,dy in rect [sizex,sizey]
        void fliplr();                                                          //flip matrix along vertical axis
        void flipud();                                                          //flip matrix along horizontal axis
        void rotate180();                                                       //rotate matrix by 180'
        vec2D* repmat(unsigned int v, unsigned int h);                          //replicate matrix v time vertically and h times horizontally
        vec2D* transpose() const;                                               //return transposed matrix
               
        void add(const vec2D& a, const vec2D& b);                               //this = a.+b   sse optimized
        void sub(const vec2D& a, const vec2D& b);                               //this = a.-b   sse optimized
        void mul(const vec2D& a, const vec2D& b);                               //this = a*b
        void mult(const vec2D& a, const vec2D& b);                              //this = a*b'   sse optimized
        void div(const vec2D& a, const vec2D& b);                               //this = a./b   sse optimized
        void mule(const vec2D& a, const vec2D& b);                              //this = a.*b   sse optimized
        void add(float scalar);                                                 //this = this.+scalar   sse optimized
        void sub(float scalar);                                                 //this = this.-scalar   sse optimized
        void mul(float scalar);                                                 //this = this.*scalar   sse optimized
        void div(float scalar);                                                 //this = this./scalar   sse optimized
        float prod() const;                                                     //f = x0*x1*x2*...xN  sse optimized        
        void inter2(const vec2D& src, vec2D& dst_grdx, vec2D& dst_grdy);        //biliniar 2d interpolation, grd temp buffers width = this->size height=2
        void conv2(const vec2D& a, const vec2D& filter, enum CONV type = SAME); //this = a*filter  convolution with filter
        void conv2(const vec2D& a, const vec2D& re, const vec2D& im);           //this = convolution with complex filter [re i*im]        
        void conv2(const vec2D& a,                                              //this = sum(s(i) conv2( conv2(a, u[i]), v[i]) ))
                   const std::vector<vec2D>& u,                                 //col vectors                
                   const std::vector<vec2D>& v,                                 //row vectors
                   const vec2D& s,                                              //row vector of singular values
                   vec2D& tmp1,                                                 //VALID tmp1 = [(a.height - u.height + 1) x a.width]
                   vec2D& tmp2,                                                 //VALID tmp2 = this = [(a.height - u.height + 1) x a.width - a,width + 1]
                   enum CONV type = SAME);     
        float dot(const float* a, const float* b, unsigned int size) const;     //a0*b0+a1*b1+a2*b2+ ... An*Bn  sse 16-bit aligned
        
        void normalize(float a, float b);                                       //normalize to a...b range  x1 = x-min
        void histeq(vec2D& hist);                                               //histogram normalization vector in 0...255 range -> to 0...1.0 range        


// Access        
        inline unsigned int width() const;                                      //width of array
        inline unsigned int height() const;                                     //height size of array
        inline unsigned int length() const;                                     //total size of array width*height
        inline const float* data(int y, int x) const;


private:        
        unsigned int m_width;
        unsigned int m_height;
        float** m_data;               //offseted data


        void init(unsigned int ysize, unsigned int xsize);                      //allocate m_data only
        void init(const vec2D &v);                                              //fill this->m_data with v.m_data[:][:]
        void close(void);                                                       //deallocate m_data


};


inline unsigned int vec2D::width() const
{
        return m_width;
}

inline unsigned int vec2D::height() const
{
        return m_height;
}

inline unsigned int vec2D::length() const
{
        return m_width*m_height;
}

inline const vec2D& vec2D::operator=(const float * pf)
{
        vec2D& v = *this;
        for (unsigned int i = 0; i < height(); i++) {
                for (unsigned int j = 0; j < width() / 4; j++) {
                        _mm_store_ps(&v(i, (j << 2)), _mm_loadu_ps(pf));
                        pf += 4;
                }
                if ((v.width() % 4) != 0) {
                        for (unsigned int j = v.width() - (v.width() % 4); j < v.width(); j++)
                                v(i, j) = *pf++;
                }
        }
        return *this;
}

inline const vec2D& vec2D::operator=(const vec2D & v)
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

inline float& vec2D::operator()(int y, int x)
{
        return m_data[y][x];
}

inline float vec2D::operator()(int y, int x) const
{
        return m_data[y][x];
}

inline float& vec2D::operator[](unsigned int i)
{
        return m_data[i % height()][i / height()];     //m_height - MATLAB order operator v[:]
}

inline float vec2D::operator[](unsigned int i) const
{
        return m_data[i % height()][i / height()];
}

inline float vec2D::get(int y, int x) const
{
        if (x < 0) x = -x;
        else if (x > ((int)width() - 1)) x = ((int)width() - 1) - (x - ((int)width() - 1));
        if (y < 0) y = -y;
        else if (y > ((int)height() - 1)) y = ((int)height() - 1) - (y - ((int)height() - 1));

        return m_data[y][x];
}

inline const vec2D& vec2D::operator*(const vec2D& b) const     // c = a*b -> C = this*B
{
        const vec2D& a = *this;        

        vec2D* pc = new vec2D(a.height(), b.width());             //0-offseted
        vec2D& c = *pc;

        for (unsigned int y = 0; y < c.height(); y++) {
                for (unsigned int x = 0; x < c.width(); x++) {
                        for (unsigned int i = 0; i < a.width(); i++)                     //a and b must be with zero offsets
                                c(y, x) += a(y, i) * b(i, x);
                }
        }

        return c;
}

inline const float* vec2D::data(int y, int x) const
{
        return m_data[y] + x;
}

inline float vec2D::dot(const float* a, const float* b, unsigned int size) const
{
        float z = 0.0f, fres = 0.0f;
        __declspec(align(16)) float ftmp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        __m128 mres;

        if ((size / 4) != 0) {
                mres = _mm_load_ss(&z);
                for (unsigned int i = 0; i < size / 4; i++)
                        mres = _mm_add_ps(mres, _mm_mul_ps(_mm_load_ps(&a[4*i]), _mm_load_ps(&b[4*i])));

                //mres = a,b,c,d
                __m128 mv1 = _mm_movelh_ps(mres, mres);     //a,b,a,b
                __m128 mv2 = _mm_movehl_ps(mres, mres);     //c,d,c,d
                mres = _mm_add_ps(mv1, mv2);                //res[0],res[1]

                _mm_store_ps(ftmp, mres);

                fres = ftmp[0] + ftmp[1];
        }

        if ((size % 4) != 0) {
                for (unsigned int i = size - size % 4; i < size; i++)
                        fres += a[i] * b[i];
        }

        return fres;
}


#endif