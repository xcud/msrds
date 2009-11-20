

#ifndef MBIOR97_H
#define MBIOR97_H


class BaseFWT2D;

class mBior97 : public BaseFWT2D    //generic 2D fwt
{
public:
        mBior97();


private:
        static const float tH[9];       //filters coeffs to BaseFWT2D vec1D
        static const float tG[9];
        static const float H[9];
        static const float G[9];

        inline float conv(const float* f, const char* c) const;                           //MMX 16bit aligned convolution sum(f0*c0 + f1*c1 + f2*c2 + f3*c3)
        inline float convr(const float* f, const char* c) const;                          //MMX 16bit aligned convolution sum(f3*c0 + f2*c1 + f1*c2 + f0*c3)
        inline void mmxround4(char* dest, __m128 mf) const;                               //dest <- mf
        inline void mmxround4TH(char* dest, __m128 mf, unsigned int t = 0) const;         //dest <- mf  denoise t ... 4

        void transrows(char** dest, char** sour, unsigned int w, unsigned int h) const;
        void transcols(char** dest, char** sour, unsigned int w, unsigned int h) const;
        void synthrows(char** dest, char** sour, unsigned int w, unsigned int h) const;
        void synthcols(char** dest, char** sour, unsigned int w, unsigned int h) const;
};


inline float mBior97::conv(const float* f, const char* c) const
{
        float out[1] = { 0.0f };

        const __m64* pc = (__m64 *)c;
        __m128 n = _mm_cvtpi8_ps(*pc);
        __m128 m = _mm_load_ps(f);

        __m128 res = _mm_mul_ps(m, n);

        //res = a,b,c,d
        m = _mm_movelh_ps(res, res);     //a,b,a,b
        n = _mm_movehl_ps(res, res);     //c,d,c,d
        m = _mm_add_ps(m, n);            //r0,r1,r0,r1
        n = _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1)); //r1,r0,r1,r0
        res = _mm_add_ss(m, n);          //r1+r0

        _mm_store_ss(out, res);

        _mm_empty();

        return out[0];
}
inline float mBior97::convr(const float* f, const char* c) const
{
        float out[1] = { 0.0f };

        const __m64 *pc = (__m64 *)c;
        __m128 n = _mm_cvtpi8_ps(*pc);
        __m128 m = _mm_loadr_ps(f);

        __m128 res = _mm_mul_ps(m, n);

        //res = a,b,c,d
        m = _mm_movelh_ps(res, res);     //a,b,a,b
        n = _mm_movehl_ps(res, res);     //c,d,c,d
        m = _mm_add_ps(m, n);            //r0,r1,r0,r1
        n = _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1)); //r1,r0,r1,r0
        res = _mm_add_ss(m, n);               //r1+r0

        _mm_store_ss(out, res);

        _mm_empty();

        return out[0];
}

inline void mBior97::mmxround4(char* dest, __m128 mf) const
{
        char tmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        __m64 *mtmp = (__m64 *)tmp;

        *mtmp = _mm_cvtps_pi8(mf);

        for (unsigned int i = 0; i < 4; i++)
                dest[i] = tmp[i];

        _mm_empty();
}

inline void mBior97::mmxround4TH(char* dest, __m128 mf, unsigned int t) const
{
        char tmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        __m64 *mtmp = (__m64 *)tmp;

        *mtmp = _mm_cvtps_pi8(mf);

        int th = (int)getTH();
        for (unsigned int i = 0; i < 4; i++) {
                if ((th != 0) && (i >= t)) {                        //denoise
                        if (tmp[i] > 0) {
                                if (tmp[i] <= th)
                                        tmp[i] = 0;
                        } else {
                                if (tmp[i] >= -th)
                                        tmp[i] = 0;
                        }
                }
                dest[i] = tmp[i];
        }

        _mm_empty();
}


#endif


/*
  usage:
   mBior97 fwt;
    fwt.init(width,height);    //init buffers
     fwt.trans(data,3,30);     //fwt transform from data buffer J=3,TH=30, data is intact after fwt
     fwt.synth(data);          //fwt synthesis to data buffer
     ...
     ...  //keep going multiple times transforming and synthesing
     ...
    fwt.close();



*/

/*
tH
 -4  0.026749
 -3  -0.016864
 -2  -0.078223
 -1  0.266864
  0  0.602949
  1  0.266864
  2  -0.078223
  3  -0.016864
  4  0.026749

tG
 -4  0.000000
 -3  0.000000
 -2  0.045636
 -1  -0.028772
  0  -0.295636
  1  0.557544
  2  -0.295636
  3  -0.028772
  4  0.045636

H
 -3  -0.045636
 -2  -0.028772
 -1  0.295636
  0  0.557544
  1  0.295636
  2  -0.028772
  3  -0.045636
  4  0.000000
  5  0.000000

G
 -3  0.026749
 -2  0.016864
 -1  -0.078223
  0  -0.266864
  1  0.602949
  2  -0.266864
  3  -0.078223
  4  0.016864
  5  0.026749


H2m
 -2  -0.028772
  0  0.557544
  2  -0.028772
  4  0.000000

H2m1
 -3  -0.045636
 -1  0.295636
  1  0.295636
  3  -0.045636
  5  0.000000

G2m
 -2  0.016864
  0  -0.266864
  2  -0.266864
  4  0.016864

G2m1
 -3  0.026749
 -1  -0.078223
  1  0.602949
  3  -0.078223
  5  0.026749
*/