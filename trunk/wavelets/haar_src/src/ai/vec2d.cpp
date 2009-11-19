

#include "stdafx.h"
#include "vec2d.h"





/////////////////////////constructors/destructors////////////////////////////////////////////////////
vec2D::vec2D(unsigned int ysize, unsigned int xsize, const float* pdata) : m_width(xsize), m_height(ysize), m_data(0)
{
        init(ysize, xsize);

        if (pdata != 0) {                
                for (unsigned int i = 0; i < height(); i++) 
                        for (unsigned int j = 0; j < width(); j++)                       
                                m_data[i][j] = *pdata++;
        }
        else
                set(0.0f);
}
vec2D::vec2D(const vec2D& v)
{
        init(v.height(), v.width());
        init(v);
}
vec2D::vec2D(const wchar_t* file) : m_width(0), m_height(0), m_data(0)
{
        float val;

        FILE* fp = _wfopen(file, L"rt");
        if (fp) {
                if (fwscanf(fp, L"%d %d", &m_height, &m_width) != 2) {
                        fclose(fp);
                        init(1, 1);
                        return;
                }

                init(m_height, m_width);

                for (unsigned int i = 0; i < height(); i++) {
                        for (unsigned int j = 0; j < width(); j++) {
                                if (fwscanf(fp, L"%f", &val) != 1) {
                                        m_data[i][j] = 0.0f;
                                } else
                                        m_data[i][j] = val;
                        }
                }

                fclose(fp);
        } else
                init(1, 1);
}
vec2D::~vec2D()
{
        if (m_data != 0)
                close();
}
////////////////////////////////////////////////////////////////////////////////


//////////////////////init,free memory//////////////////////////////////////////
void vec2D::init(unsigned int ysize, unsigned int xsize)
{
        m_width = xsize;
        m_height = ysize;
                
        m_data = (float**) malloc(height() * sizeof(float*));         //setup rows
        for (unsigned int i = 0; i < height(); i++) {
                m_data[i] = (float*)_aligned_malloc(width() * sizeof(float), 16);       //setup cols                
        }        
}
void vec2D::init(const vec2D& v)
{
        for (unsigned int i = 0; i < v.height(); i++)
                for (unsigned int j = 0; j < v.width(); j++)
                        (*this)(i, j) = v(i, j);
}
void vec2D::close(void)
{        
        for (unsigned int i = 0; i < height(); i++)
                _aligned_free(m_data[i]);       //delete colums
        free(m_data);           //delete rows

        m_data = 0;
        m_width = 0;
        m_height = 0;
}
//////////////////////init,free memory//////////////////////////////////////////


//////////////////////operations////////////////////////////////////////////////
void vec2D::print(const wchar_t* file) const
{
        const vec2D &v = *this;
        if (file) {
                FILE *fp = _wfopen(file, L"wt");
                if (fp) {
                        fwprintf(fp, L"\n vec: %p\n", this);
                        for (unsigned int i = 0; i < v.height(); i++) {
                                for (unsigned int j = 0; j < v.width(); j++)
                                        fwprintf(fp, L" %g", v(i, j));
                                fwprintf(fp, L"\n");
                        }
                        fclose(fp);
                }
        } else {
                wprintf(L"\n vec: %p\n", this);
                for (unsigned int i = 0; i < v.height(); i++) {
                        for (unsigned int j = 0; j < v.width(); j++)
                                wprintf(L" %g", v(i, j));
                        wprintf(L"\n");
                }
        }
}
void vec2D::set(float scalar)
{
        vec2D &v = *this;
        __m128 mscalar = _mm_load_ps1(&scalar);

        for (unsigned int i = 0; i < v.height(); i++) {
                for (unsigned int j = 0; j < v.width() / 4; j++) 
                        _mm_store_ps(&v(i, j << 2), mscalar);                                        
                if (width() % 4) {
                        for (unsigned int j = width() - (width() % 4); j < width(); j++)
                                v(i, j) = scalar;
                }
        }
}
//set to Rect [ left, right )
//            [ top, bottom )
void vec2D::set(float scalar, RECT& r)                              //0-offset array function
{
        vec2D &v = *this;       

        if (r.top < 0) r.top = 0;
        if (r.left < 0) r.left = 0;
        if ((unsigned int)r.bottom > v.height()) r.bottom = v.height();
        if ((unsigned int)r.right > v.width()) r.right = v.width();

        __m128 mscalar = _mm_load_ps1(&scalar);
        unsigned int width = r.right - r.left;

        for (unsigned int i = (unsigned int)r.top; i < (unsigned int)r.bottom; i++) {
                for (unsigned int j = 0; j < width / 4; j++) {
                        _mm_storeu_ps(&v(i, r.left + (j << 2)), mscalar);      //store unaligned r.left%4 might be nonzero
                }
                if (width % 4) {
                        for (unsigned int j = width - (width % 4); j < width; j++)
                                v(i, r.left + j) = scalar;
                }
        }
}
void vec2D::setrand()
{
        vec2D &v = *this;
        int r;
        srand((unsigned int)time(0));
        for (unsigned int i = 0; i < v.height(); i++) {
                for (unsigned int j = 0; j < v.width(); j++) {
                        r = 0xFFF & rand();
                        r -= 0x800;
                        v(i, j) = (float)r / 2048.0f;
                }
        }
}
bool vec2D::copy(const vec2D& v, int left, int top)                                        //copy [hxw] region from top,left offset A to this
{        
        //v size >= this size !
        vec2D& pv = *this;        

        RECT r;                           //selected rectangle
        r.left = left;                    //left,top coords
        r.top = top;
        r.right = r.left + width();       //right,bottom coords
        r.bottom = r.top + height();

        if ((r.left >= 0 && r.top >= 0) && (r.right <= (int)v.width() && r.bottom <= (int)v.height())) {       //operator()
                for (unsigned int i = 0; i < height(); i++)
                        for (unsigned int j = 0; j < width(); j++)
                                pv(i, j) = v(i + r.top, j + r.left);
        } else { //get
                for (unsigned int i = 0; i < height(); i++)
                        for (unsigned int j = 0; j < width(); j++)
                                pv(i, j) = v.get(i + r.top, j + r.left);
        }

        return true;
}
void vec2D::minmax(float& mn, float& mx) const
{
        const vec2D& v = *this;
        mn = v(0, 0);
        mx = mn;
        for (unsigned int i = 0; i < v.height(); i++) {
                for (unsigned int j = 0; j < v.width(); j++) {
                        if (v(i, j) < mn) 
                                mn = v(i, j);
                        if (v(i, j) > mx) 
                                mx = v(i, j);
                }
        }
}
float vec2D::min_val() const
{
        const vec2D& v = *this;
        float mn = v(0, 0);
        for (unsigned int i = 0; i < v.height(); i++) {
                for (unsigned int j = 0; j < v.width(); j++) {
                        if (v(i, j) < mn) 
                                mn = v(i, j);       
                }
        }
        return mn;
}
float vec2D::max_val() const
{
        const vec2D& v = *this;
        float mx = v(0, 0);
        for (unsigned int i = 0; i < v.height(); i++) {
                for (unsigned int j = 0; j < v.width(); j++) {
                        if (v(i, j) > mx) 
                                mx = v(i, j);
                }
        }
        return mx;
}
//get max value with [x,y] coord from offset dx,dy in rect [sizex,sizey]
void vec2D::max_val(float& mx, int& x, int& y, int sizex, int sizey, int dx, int dy) const
{
        const vec2D& c = *this;

        mx = -FLT_MAX;

        if (dx == 0 || dx < (sizex - 1) / 2)
                dx = (sizex - 1) / 2;
        if (dy == 0 || dy < (sizey - 1) / 2)
                dy = (sizey - 1) / 2;

        x = dx;
        y = dy;

        float tmp = 0.0f;
        for (unsigned int n = dy; n < height() - dy; n++) {
                for (unsigned int m = dx; m < width() - dx; m++) {
                        tmp = 0.0f;
                        for (int i = -(sizey - 1) / 2; i <= (sizey - 1) / 2; i++)
                                for (int j = -(sizex - 1) / 2; j <= (sizex - 1) / 2; j++)
                                        tmp += c(n + i, m + j);
                        if (tmp > mx) {
                                mx = tmp;
                                y = n;
                                x = m;
                        }
                }
        }
}
void vec2D::fliplr()
{
        vec2D& v = *this;
        float tmp;
        for (unsigned int j = 0; j < width() / 2; j++) {
                for (unsigned int i = 0; i < height(); i++) {                        
                        tmp = v(i, j);
                        v(i, j) = v(i, (width() - 1) - j);
                        v(i, (width() - 1) - j) = tmp;
                }
        }
}
void vec2D::flipud()
{
        vec2D& v = *this;
        float tmp;
        for (unsigned int i = 0; i < height() / 2; i++) {
                for (unsigned int j = 0; j < width(); j++) {                        
                        tmp = v(i, j);
                        v(i, j) = v((height() - 1) - i, j);
                        v((height() - 1) - i, j) = tmp;
                }
        }
}
void vec2D::rotate180()
{
        vec2D& c = *this;
        c.fliplr();
        c.flipud();
}
vec2D* vec2D::repmat(unsigned int v, unsigned int h)
{
        const vec2D& c = *this;
        vec2D* m = new vec2D(v * c.height(), h * c.width());

        //repmat horizontally
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int n = 0; n < h; n++) {
                        for (unsigned int j = 0; j < c.width(); j++)
                                (*m)(i, n*c.width() + j) = c(i, j);
                }
        }
        //repmat vertically
        for (unsigned int n = 1; n < v; n++) {
                for (unsigned int i = 0; i < c.height(); i++)
                        for (unsigned int j = 0; j < m->width(); j++)
                                (*m)(n*c.height() + i, j) = (*m)(i, j);
        }       

        return m;
}
vec2D* vec2D::transpose() const
{
        const vec2D& v = *this;
        vec2D* w = new vec2D(v.width(), v.height());
        for (unsigned int i = 0; i < w->height(); i++)
                for (unsigned int j = 0; j < w->width(); j++)
                        (*w)(i, j) = v(j, i);
        return w;
}

void vec2D::add(const vec2D& a, const vec2D& b)      //this = a.+b
{
        vec2D& c = *this;
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_add_ps(_mm_load_ps(a.data(i, (j << 2))), _mm_load_ps(b.data(i, (j << 2)))));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) = a(i, j) + b(i, j);
                }
        }
}
void vec2D::sub(const vec2D& a, const vec2D& b)      //this = a.-b
{
        vec2D& c = *this;
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_sub_ps(_mm_load_ps(a.data(i, (j << 2))), _mm_load_ps(b.data(i, (j << 2)))));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) = a(i, j) - b(i, j);
                }
        }
}
void vec2D::mule(const vec2D& a, const vec2D& b)      //this = a.*b
{
        vec2D& c = *this;
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_mul_ps(_mm_load_ps(a.data(i, (j << 2))), _mm_load_ps(b.data(i, (j << 2)))));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) = a(i, j) * b(i, j);
                }
        }
}
void vec2D::mul(const vec2D& a, const vec2D& b)      //this = a*b
{
        vec2D& c = *this;
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width(); j++) {
                        c(i, j) = 0.0f;
                        for (unsigned int n = 0; n < a.width(); n++)
                                c(i, j) += a(i, n) * b(n, j);
                }
        }
}
void vec2D::mult(const vec2D& a, const vec2D& b)      //this = a*b'
{
        vec2D& c = *this;
        for (unsigned int i = 0; i < c.height(); i++)
                for (unsigned int j = 0; j < c.width(); j++)
                        c(i, j) = dot(a.data(i, 0), b.data(j, 0), a.width());
}
void vec2D::div(const vec2D& a, const vec2D& b)      //this = a./b
{
        vec2D& c = *this;
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_div_ps(_mm_load_ps(a.data(i, (j << 2))), _mm_load_ps(b.data(i, (j << 2)))));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) = a(i, j) / b(i, j);
                }
        }
}
void vec2D::add(float scalar)           //this = this.+scalar   sse optimized
{
        vec2D& c = *this;
        __m128 mscalar = _mm_load_ps1(&scalar);
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_add_ps(_mm_load_ps(&c(i, (j << 2))), mscalar));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) += scalar;
                }
        }
}
void vec2D::sub(float scalar)           //this = this.-scalar   sse optimized
{
        vec2D& c = *this;
        __m128 mscalar = _mm_load_ps1(&scalar);
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_sub_ps(_mm_load_ps(&c(i, (j << 2))), mscalar));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) -= scalar;
                }
        }
}
void vec2D::mul(float scalar)           //this = this.*scalar   sse optimized
{
        vec2D& c = *this;
        __m128 mscalar = _mm_load_ps1(&scalar);
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_mul_ps(_mm_load_ps(&c(i, (j << 2))), mscalar));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) *= scalar;
                }
        }
}
void vec2D::div(float scalar)           //this = this./scalar   sse optimized
{
        vec2D& c = *this;
        __m128 mscalar = _mm_load_ps1(&scalar);
        for (unsigned int i = 0; i < c.height(); i++) {
                for (unsigned int j = 0; j < c.width() / 4; j++)
                        _mm_store_ps(&c(i, (j << 2)), _mm_div_ps(_mm_load_ps(&c(i, (j << 2))), mscalar));
                if ((c.width() % 4) != 0) {
                        for (unsigned int j = c.width() - (c.width() % 4); j < c.width(); j++)
                                c(i, j) /= scalar;
                }
        }
}
float vec2D::prod() const
{
        const vec2D& c = *this;

        float fres = 1.0f;
        __declspec(align(16)) float tmp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        __m128 mres;

        for (unsigned int i = 0; i < height(); i++) {
                for (unsigned int j = 0; j < width() / 4; j++) {
                        if (i == 0)
                                mres = _mm_load_ps(c.data(0, 0));
                        else
                                mres = _mm_mul_ps(mres, _mm_load_ps(c.data(i, (j << 2))));
                }
                if ((width() % 4) != 0) {
                        for (unsigned int j = width() - width() % 4; j < width(); j++)
                                fres *= c(i, j);
                }
        }

        //mres: a,b,c,d
        __m128 m = _mm_movehl_ps(mres, mres);    //m:  c,d,c,d
        mres = _mm_mul_ps(m, mres);              //mres: a*c,b*d,*,*
        _mm_store_ps(tmp, mres);

        return tmp[0] * tmp[1] * fres;
}

void vec2D::inter2(const vec2D& src, vec2D& dst_grdx, vec2D& dst_grdy)                //2d interpolation
{
        vec2D& trg = *this;

        float xrto = float(width() - 1) / float(src.width() - 1);       //trg/src
        float yrto = float(height() - 1) / float(src.height() - 1);

        //arrange before srcx,srcy,frcx,frcy to speed calcs
        for (unsigned int y = 0; y < height(); y++) {
                float srcy = (float)y / yrto;
                float frcy = srcy - float((int)srcy);
                dst_grdy(0, y) = srcy;
                dst_grdy(1, y) = frcy;
        }

        for (unsigned int x = 0; x < width(); x++) {
                float srcx = (float)x / xrto;               //position to take from src vec2D
                float frcx = srcx - float((int)srcx);       //srcx=1.34  frcx=.34
                dst_grdx(0, x) = srcx;
                dst_grdx(1, x) = frcx;
        }
        //arrange before srcx,srcy,frcx,frcy to speed calcs

        for (unsigned int y = 0; y < height(); y++) {
                for (unsigned int x = 0; x < width(); x++) {
                        unsigned int sx = (unsigned int)dst_grdx(0, x);         //x index to source
                        unsigned int sy = (unsigned int)dst_grdy(0, y);         //y index to source
                        trg(y, x) = src(sy, sx) * (1.0f - dst_grdy(1, y)) * (1.0f - dst_grdx(1, x));        //1-frcy 1-frcx

                        if (dst_grdx(1, x) > 0.0f && sx + 1 < src.width())
                                trg(y, x) += src(sy, sx + 1) * (1.0f - dst_grdy(1, y)) * dst_grdx(1, x);        //1-frcy frcx
                        if (dst_grdy(1, y) > 0.0f && sy + 1 < src.height())
                                trg(y, x) += src(sy + 1, sx) * dst_grdy(1, y) * (1.0f - dst_grdx(1, x));        //frcy 1-frcx
                        if ((dst_grdx(1, x) > 0.0f && dst_grdy(1, y) > 0.0f) && (sx + 1 < src.width() && sy + 1 < src.height()))
                                trg(y, x) += src(sy + 1, sx + 1) * dst_grdy(1, y) * dst_grdx(1, x);             //frcy frcx
                }
        }
}

void vec2D::conv2(const vec2D& a, const vec2D& filter, enum CONV type)  //filter should be of odd size
{
        int l = filter.height() / 2;
        int m = filter.width() / 2;        

        vec2D& c = *this;

        if (type == SAME) {      //this size = a size        
                for (int i = 0; i < (int)c.height(); i++) {
                        for (int j = 0; j < (int)c.width(); j++) {
                                float tmp = 0;                        
                                for (int u = i - l; u <= i + l; u++)
                                        for (int v = j - m; v <= j + m; v++) 
                                                tmp += filter((i - u) + l, (j - v) + m) * a.get(u, v);
                                c(i, j) = tmp;                        
                        }
                }
        }
        else if (type == VALID) { //this size = (a size - filter size) + 1        
                for (int i = l; i < (int)a.height() - l; i++) {
                        for (int j = m; j < (int)a.width() - m; j++) {
                                float tmp = 0;                        
                                for (int u = i - l; u <= i + l; u++)
                                        for (int v = j - m; v <= j + m; v++) 
                                                tmp += filter((i - u) + l, (j - v) + m) * a(u, v);
                                c(i - l, j - m) = tmp;                        
                        }
                }
        }
}
void vec2D::conv2(const vec2D& a, const vec2D& re, const vec2D& im)     //filter should be of odd size
{
        int l = (im.height() - 1) / 2;
        int m = (im.width() - 1) / 2;        

        vec2D& c = *this;
        for (int i = 0; i < (int)c.height(); i++) {
                for (int j = 0; j < (int)c.width(); j++) {
                        float imag = 0.0f;
                        float real = 0.0f, tmp;
                        for (int u = i - l; u <= i + l; u++) {
                                for (int v = j - m; v <= j + m; v++) {
                                        tmp = a.get(u, v);
                                        real += re((i - u) + l, (j - v) + m) * tmp;
                                        imag += im((i - u) + l, (j - v) + m) * tmp;
                                }
                        }
                        c(i, j) = sqrt(real * real + imag * imag);  //abs value
                }
        }
}
//this = sum( s[i] [a * u[i]] * v[i])
void vec2D::conv2(const vec2D& a, const std::vector<vec2D>& u,          //col vectors
                                  const std::vector<vec2D>& v,          //row vectors
                                  const vec2D& s,                       //row vector singular values
                                  vec2D& tmp1,                          //tmp1 = [(a.height - u.height + 1) x a.width]
                                  vec2D& tmp2,                          //tmp2 = this = [(a.height - u.height + 1) x a.width - v.width + 1]
                                  enum CONV type)
{                                   
        vec2D& c = *this;
        c.set(0.0);
        for (unsigned int i = 0; i < s.width(); i++) {
                tmp1.conv2(a, u[i], type);
                tmp2.conv2(tmp1, v[i], type);
                tmp2.mul(s(0, i));
                c.add(c, tmp2);
        }
}

void vec2D::normalize(float a, float b)
{
        vec2D& v = *this;

        float mn, mx;
        minmax(mn, mx);

        v.sub(mn);
        v.mul((b - a) / (mx - mn));
        v.add(a);
}
void vec2D::histeq(vec2D &hist)    //hist 1x256 array
{
        vec2D& v = *this;

        hist.set(0.0f);

        for (unsigned int i = 0; i < v.height(); i++)
                for (unsigned int j = 0; j < v.width(); j++)
                        hist(0, (unsigned int)v(i, j))++;

        for (unsigned int j = 1; j < hist.width(); j++)
                hist(0, j) += hist(0, j - 1);

        hist.div(float(width() * height()));

        for (unsigned int i = 0; i < v.height(); i++)
                for (unsigned int j = 0; j < v.width(); j++)
                        v(i, j) = hist(0, (unsigned int)v(i, j));
}
