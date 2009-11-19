

#include "stdafx.h"
#include "vec2Di.h"



vec2Di::vec2Di(unsigned int ysize, unsigned int xsize, const int* pdata) : m_width(xsize), m_height(ysize), m_data(0)
{
        init(ysize, xsize);

        if (pdata != 0) {                
                for (unsigned int i = 0; i < height(); i++) 
                        for (unsigned int j = 0; j < width(); j++)                       
                                m_data[i][j] = *pdata++;
        }
        else
                set(0);
}
vec2Di::vec2Di(const wchar_t* file) : m_width(0), m_height(0)
{
        int val;

        FILE* fp = _wfopen(file, L"rt");
        if (fp) {
                if (fwscanf(fp, L"%d %d", &m_height, &m_width) != 2) {
                        fclose(fp);
                        init(1, 1);
                        return;
                }

                init(m_height, m_width);

                for (unsigned int y = 0; y < height(); y++) {
                        for (unsigned int x = 0; x < width(); x++) {
                                if (fwscanf(fp, L"%d", &val) != 1) {
                                        m_data[y][x] = 0;
                                } else
                                        m_data[y][x] = val;
                        }
                }

                fclose(fp);
        } else {
                init(1, 1);
                set(0);
        }
}
vec2Di::vec2Di(const vec2Di& v)
{
        init(v.height(), v.width());
        init(v);
}
vec2Di::~vec2Di()
{
        if (m_data != 0)
                close();
}

void vec2Di::init(unsigned int ysize, unsigned int xsize)
{
        m_width = xsize;
        m_height = ysize;
                
        m_data = (int **)malloc(height() * sizeof(int *));         //setup rows
        for (unsigned int i = 0; i < height(); i++) 
                m_data[i] = (int *)malloc(width() * sizeof(int));       //setup cols                                
}
void vec2Di::init(const vec2Di& v)
{
        for (unsigned int i = 0; i < v.height(); i++)
                for (unsigned int j = 0; j < v.width(); j++)
                        (*this)(i, j) = v(i, j);
}
void vec2Di::close(void)
{        
        for (unsigned int i = 0; i < height(); i++)
                free(m_data[i]);       //delete colums
        free(m_data);           //delete rows

        m_data = 0;
        m_width = 0;
        m_height = 0;
}

void vec2Di::print(const wchar_t* file) const
{
        const vec2Di& v = *this;
        wchar_t format[32] = L"%4d";

        int mx = max_val();
        int mn = min_val();
        if (abs(mn) > mx)
                mx = abs(mn);

        if (mx >= 10000)
                wcscpy(format, L"%7d");
        else if (mx >= 10000)
                wcscpy(format, L"%6d");
        else if (mx >= 1000)
                wcscpy(format, L"%5d");
        else if (mx >= 100)
                wcscpy(format, L"%4d");
        else if(mx >= 10)
                wcscpy(format, L"%3d");
        else if(mx < 10)
                wcscpy(format, L"%2d");
        
        if (file) {
                FILE *fp = _wfopen(file, L"wt");
                if (fp != 0) {
                        fwprintf(fp, L"\n vec: %p\n", this);
                        for (unsigned int i = 0; i < height(); i++) {
                                for (unsigned int j = 0; j < width(); j++)
                                        fwprintf(fp, format, v(i, j));
                                fwprintf(fp, L"\n");
                        }
                        fclose(fp);
                }
        } else {
                wprintf(L"\n vec: %p\n", this);
                for (unsigned int i = 0; i < height(); i++) {
                        for (unsigned int j = 0; j < width(); j++)
                                wprintf(format, v(i, j));
                        wprintf(L"\n");
                }
        }
}

void vec2Di::set(int scalar)
{
        vec2Di& v = *this;        
        for (unsigned int i = 0; i < v.height(); i++)
                for (unsigned int j = 0; j < v.width(); j++) 
                        v(i, j) = scalar;                        
}
//set to Rect [ left, right )
//            [ top, bottom )
void vec2Di::set(int scalar, RECT& r)
{
        vec2Di &v = *this;

        if (r.top < 0) r.top = 0;
        if (r.left < 0) r.left = 0;
        if ((unsigned int)r.bottom > v.height()) r.bottom = v.height();
        if ((unsigned int)r.right > v.width()) r.right = v.width();
        
        unsigned int width = r.right - r.left;

        for (unsigned int i = (unsigned int)r.top; i < (unsigned int)r.bottom; i++) 
                for (unsigned int j = 0; j < width; j++)
                        v(i, r.left + j) = scalar;                                                
}

bool vec2Di::copy(const vec2Di& v, int left, int top)                                        //copy [hxw] region from top,left offset A to this
{        
        //v size >= this size !
        vec2Di& pv = *this;        

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

int vec2Di::min_val() const
{
        const vec2Di& v = *this;
        int mn = v(0, 0);
        for (unsigned int i = 0; i < height(); i++) {
                for (unsigned int j = 0; j < width(); j++) {
                        if (v(i, j) < mn) 
                                mn = v(i, j);
                }
        }
        return mn;
}

int vec2Di::max_val() const
{
        const vec2Di& v = *this;
        int mx = v(0, 0);
        for (unsigned int i = 0; i < height(); i++) {
                for (unsigned int j = 0; j < width(); j++) {
                        if (v(i, j) > mx) 
                                mx = v(i, j);
                }
        }
        return mx;
}

int vec2Di::dilate(const vec2Di& src, unsigned int w, unsigned int h)
{
        if (w < 2) w = 2;
        if (h < 2) h = 2;

        int strel_w = (int)w;
        int strel_h = (int)h;

        vec2Di& trg = *this;
        int val;

        if (trg.is_size_equal(src) == false)
                return -1;

        for (unsigned int y = 0; y < height(); y++) {
                for (unsigned int x = 0; x < width(); x++) {
                        val = INT_MIN;
                        for (int j = -strel_h / 2; j <= strel_h / 2; j++) {
                                for (int i = -strel_w / 2; i <= strel_w / 2; i++) {
                                        if (y + j < 0 || y + j > height() - 1)
                                                continue;
                                        if (x + i < 0 || x + i > width() - 1)
                                                continue;
                                        if (val < src(y + j, x + i))
                                                val = src(y + j, x + i);
                                }
                        }
                        trg(y, x) = val;
                }
        }

        return 0;
}

int vec2Di::erode(const vec2Di& src, unsigned int w, unsigned int h)
{
        if (w < 2) w = 2;
        if (h < 2) h = 2;

        int strel_w = (int)w;
        int strel_h = (int)h;

        vec2Di& trg = *this;
        int val;

        if (trg.is_size_equal(src) == false)
                return -1;

        for (unsigned int y = 0; y < height(); y++) {
                for (unsigned int x = 0; x < width(); x++) {
                        val = INT_MAX;
                        for (int j = -strel_h / 2; j <= strel_h / 2; j++) {
                                for (int i = -strel_w / 2; i <= strel_w / 2; i++) {
                                        if (y + j < 0 || y + j > height() - 1)
                                                continue;
                                        if (x + i < 0 || x + i > width() - 1)
                                                continue;
                                        if (val > src(y + j, x + i))
                                                val = src(y + j, x + i);
                                }
                        }
                        trg(y, x) = val;
                }
        }

        return 0;
}

bool vec2Di::is_size_equal(const vec2Di& v) const
{
        const vec2Di& src = *this;
        if (src.width() == v.width() && src.height() == v.height())
                return true;
        else
                return false;
}

void vec2Di::and(const vec2Di& v)
{
        vec2Di& pv = *this;
        for (unsigned int i = 0; i < height(); i++)
                for (unsigned int j = 0; j < width(); j++)
                        pv(i, j) &= v(i, j);
}

void vec2Di::add(const vec2Di& a, const vec2Di& b)
{
        vec2Di& c = *this;
        for (unsigned int i = 0; i < height(); i++)
                for (unsigned int j = 0; j < width(); j++)
                        c(i, j) = a(i, j) + b(i, j);
}

void vec2Di::sub(const vec2Di& a, const vec2Di& b)
{
        vec2Di& c = *this;
        for (unsigned int i = 0; i < height(); i++)
                for (unsigned int j = 0; j < width(); j++)
                        c(i, j) = a(i, j) - b(i, j);
}


