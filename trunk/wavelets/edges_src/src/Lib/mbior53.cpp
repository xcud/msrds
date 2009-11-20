

#include "stdafx.h"

#include "vec1d.h"
#include "basefwt.h"
#include "mbior53.h"


//bior53 filter////////////////////////////////////////////////////////////////////////////////////////
//  -2        -1       0        1         2      [ 3        4        5 ]
const float mBior53::tH[8] = { -0.125f, 0.2500f, 0.7500f, 0.2500f, -0.125f, 0.0000f, 0.0000f, 0.0000f };
//   0         1        2       [3]
const float mBior53::tG[4] = { -0.250f, 0.5000f, -0.250f, 0.0000f };
//   -1        0        1      [2        3        4        5]
const float mBior53::H[7] = { 0.2500f, 0.5000f, 0.2500f, 0.0000f, 0.0000f, 0.0000f, 0.0000f };
//  [-2]      -1        0       1        2        3        [4       5]
const float mBior53::G[8] = { 0.0000f, -0.125f, -0.250f, 0.7500f, -0.250f, -0.125f, 0.0000f, 0.0000f };

//bior53 filter////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////constructors/destructors///////////////////////////////////////////////////////////////////
mBior53::mBior53() : BaseFWT2D(L"bior53", tH, 8, 2, tG, 4, 0, H, 7, 1, G, 8, 2)
{
}
///////////////////////////////////constructors/destructors///////////////////////////////////////////////////////////////////









///////////////////////////////////////////////transforms/////////////////////////////////////////////////////////////////////
void mBior53::transrows(char** dest, char** sour, unsigned int w, unsigned int h) const
{
        char srck0[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        //int n;
        float s, d;
        unsigned int w2 = w / 2;

        const vec1D& tH = gettH();
        const vec1D& tG = gettG();

        for (unsigned int y = 0; y < h; y++) {

                //k=0
                unsigned int k = 0;
                srck0[0] = sour[y][2];
                srck0[1] = sour[y][1];
                srck0[2] = sour[y][0];
                srck0[3] = sour[y][1];

                s = conv(tH.data(), srck0);
                s += tH[2] * (float)sour[y][2*k + 2];
                d = conv(tG.data(), &sour[y][2*k]);

                dest[y][k] = mmxround(s);
                dest[y][k+w2] = mmxroundTH(d);


                //k=1, k<w2-1
                for (k = 1; k < w2 - 1; k++) {

                        s = conv(tH.data(), &sour[y][2*k - 2]);
                        s += tH[2] * (float)sour[y][2*k + 2];
                        d = conv(tG.data(), &sour[y][2*k]);

                        dest[y][k] = mmxround(s);
                        dest[y][k+w2] = mmxroundTH(d);
                }


                //k=w2-1
                k = w2 - 1;
                srck0[0] = sour[y][2*k];
                srck0[1] = sour[y][2*k+1];
                srck0[2] = sour[y][2*k];
                srck0[3] = sour[y][2*k-1];

                s = conv(tH.data(), &sour[y][2*k - 2]);
                s += tH[2] * (float)sour[y][2*k];
                d = conv(tG.data(), srck0);

                dest[y][k] = mmxround(s);
                dest[y][k+w2] = mmxroundTH(d);

        }
}
void mBior53::transcols(char** dest, char** sour, unsigned int w, unsigned int h) const
{
        float fz = 0.0f;

        int n;
        float s, d;
        __m128 ms, md;
        unsigned int h2 = h / 2;

        const vec1D& tH = gettH();
        const vec1D& tG = gettG();

        for (unsigned int x = 0; x < w / 4; x++) {   //x<w/4   x = 4*x
                for (unsigned int k = 0; k < h2; k++) {

                        ms = _mm_load_ss(&fz);
                        md = ms;

                        for (int m = -2; m <= 2; m++) {
                                n = 2 * k + m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                ms = _mm_add_ps(ms, _mm_mul_ps(_mm_load_ps1(tH.data(m)), _mm_cvtpi8_ps(*(__m64 *)(&sour[n][4*x]))));
                        }
                        for (int m = 0; m <= 2; m++) {
                                n = 2 * k + m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                md = _mm_add_ps(md, _mm_mul_ps(_mm_load_ps1(tG.data(m)), _mm_cvtpi8_ps(*(__m64 *)(&sour[n][4*x]))));
                        }

                        if (4*x < w / 2) {
                                if ((w / 2) - (4*x) >= 4)
                                        mmxround4(&dest[k][4*x], ms);
                                else
                                        mmxround4TH(&dest[k][4*x], ms, (w / 2) - (4*x));   //skip first from LL part 10/2-4=1 [lo] o o o o * | * * * o o [hi]
                        } else
                                mmxround4TH(&dest[k][4*x], ms);

                        mmxround4TH(&dest[k+h2][4*x], md);
                }
        }
        _mm_empty();


        //odd remainder
        for (unsigned int x = w - (w % 4); x < w; x++) {
                for (unsigned int k = 0; k < h2; k++) {
                        s = 0;
                        d = 0;
                        for (int m = -2; m <= 2; m++) {
                                n = 2 * k + m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                s += tH[m] * float(sour[n][x]);
                        }
                        for (int m = 0; m <= 2; m++) {
                                n = 2 * k + m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                d += tG[m] * float(sour[n][x]);
                        }

                        if (x < w / 2)
                                dest[k][x] = mmxround(s);
                        else
                                dest[k][x] = mmxroundTH(s); 	        //is this needed? hi band were TH'ed on transrows

                        dest[k+h2][x] = mmxroundTH(d);          //is this needed? hi band were TH'ed on transrows on x>w/2
                }
        }
}
///////////////////////////////////////////////transforms/////////////////////////////////////////////////////////////////////







//////////////////////////////////////////////synths//////////////////////////////////////////////////////////////////////////
void mBior53::synthrows(char** dest, char** sour, unsigned int w, unsigned int h) const      //w,h of the LO part
{
        char srclo[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        char srchi[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        //int n;
        float s2k, s2k1;

        const vec1D& H2m = getH2m();
        const vec1D& G2m = getG2m();
        const vec1D& H2m1 = getH2m1();
        const vec1D& G2m1 = getG2m1();

        for (unsigned int y = 0; y < 2*h; y++) {

                unsigned int k;

                //k = [0; 2)
                for (k = 0; k < 2; k++) {
                        s2k = 0;
                        s2k1 = 0;

                        if (k == 0) {
                                srclo[0] = sour[y][2];
                                srclo[1] = sour[y][1];
                                srclo[2] = sour[y][0];
                                srclo[3] = sour[y][1];
                                srchi[0] = sour[y][2+w];
                                srchi[1] = sour[y][1+w];
                                srchi[2] = sour[y][0+w];
                                srchi[3] = sour[y][1+w];
                        }
                        if (k == 1) {
                                srclo[0] = sour[y][1];
                                srclo[1] = sour[y][0];
                                srclo[2] = sour[y][1];
                                srclo[3] = sour[y][2];
                                srchi[0] = sour[y][1+w];
                                srchi[1] = sour[y][0+w];
                                srchi[2] = sour[y][1+w];
                                srchi[3] = sour[y][2+w];
                        }

                        s2k += H2m[0] * float(sour[y][k]);      //s2k even H
                        s2k += convr(G2m.data(), srchi);

                        s2k1 += convr(H2m1.data(), srclo);
                        s2k1 += convr(G2m1.data(), srchi);

                        dest[y][2*k] = mmxround(2.0f * s2k);
                        dest[y][2*k+1] = mmxround(2.0f * s2k1);
                }


                //k = [2; w-1)
                for (k = 2; k < w - 1; k++) {
                        s2k = 0;
                        s2k1 = 0;

                        s2k += H2m[0] * float(sour[y][k]);              //s2k even H
                        s2k += convr(G2m.data(), &sour[y][k-2+w]);      //s2k even G

                        s2k1 += convr(H2m1.data(), &sour[y][k-2]);       //s2k1 odd H
                        s2k1 += convr(G2m1.data(), &sour[y][k-2+w]);     //s2k1 odd G

                        dest[y][2*k] = mmxround(2.0f * s2k);
                        dest[y][2*k+1] = mmxround(2.0f * s2k1);
                }


                //k = w-1
                k = w - 1;

                srclo[0] = sour[y][k-2];
                srclo[1] = sour[y][k-1];
                srclo[2] = sour[y][k];
                srclo[3] = sour[y][k-1];
                srchi[0] = sour[y][k-2+w];
                srchi[1] = sour[y][k-1+w];
                srchi[2] = sour[y][k+w];
                srchi[3] = sour[y][k-1+w];

                s2k = H2m[0] * float(sour[y][k]);          //s2k even H
                s2k += convr(G2m.data(), srchi);           //s2k even G

                s2k1 = convr(H2m1.data(), srclo);          //s2k1 odd H
                s2k1 += convr(G2m1.data(), srchi);         //s2k1 odd G

                dest[y][2*k] = mmxround(2.0f * s2k);
                dest[y][2*k+1] = mmxround(2.0f * s2k1);
        }
}

void mBior53::synthcols(char** dest, char** sour, unsigned int w, unsigned int h) const     //w,h of the LO part
{
        float fz = 0.0f;
        float mul2 = 2.0f;

        int n;
        float s2k, s2k1;
        __m128 ms2k, ms2k1;
        unsigned int w2 = 2 * w;

        const vec1D& H2m = getH2m();
        const vec1D& G2m = getG2m();
        const vec1D& H2m1 = getH2m1();
        const vec1D& G2m1 = getG2m1();

        for (unsigned int x = 0; x < w2 / 4; x++) {      //x<w2/2   x = 4*x
                for (unsigned int k = 0; k < h; k++) {

                        ms2k = _mm_load_ss(&fz);
                        ms2k1 = ms2k;

                        for (int m = 0; m <= 0; m++) {                             //s2k even H
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                ms2k = _mm_add_ps(ms2k, _mm_mul_ps(_mm_load_ps1(H2m.data(m)), _mm_cvtpi8_ps(*(__m64 *)(&sour[n][4*x]))));
                        }
                        for (int m = 0; m <= 1; m++) {                             //s2k even G
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                ms2k = _mm_add_ps(ms2k, _mm_mul_ps(_mm_load_ps1(G2m.data(m)), _mm_cvtpi8_ps(*(__m64 *)(&sour[n+h][4*x]))));
                        }

                        for (int m = -1; m <= 0; m++) {                           //s2k1 odd H
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                ms2k1 = _mm_add_ps(ms2k1, _mm_mul_ps(_mm_load_ps1(H2m1.data(m)), _mm_cvtpi8_ps(*(__m64 *)(&sour[n][4*x]))));
                        }
                        for (int m = -1; m <= 1; m++) {                          //s2k1 odd G
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                ms2k1 = _mm_add_ps(ms2k1, _mm_mul_ps(_mm_load_ps1(G2m1.data(m)), _mm_cvtpi8_ps(*(__m64 *)(&sour[n+h][4*x]))));
                        }

                        __m128 mmul2 = _mm_load_ps1(&mul2);

                        mmxround4(&dest[2*k][4*x], _mm_mul_ps(ms2k, mmul2));
                        mmxround4(&dest[2*k+1][4*x], _mm_mul_ps(ms2k1, mmul2));
                }
        }
        _mm_empty();


        //odd remainder
        for (unsigned int x = w2 - (w2 % 4); x < w2; x++) {
                for (unsigned int k = 0; k < h; k++) {

                        s2k = 0;
                        s2k1 = 0;

                        for (int m = H2m.first(); m <= H2m.last(); m++) {        //s2k even H
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                s2k += H2m[m] * float(sour[n][x]);
                        }
                        for (int m = G2m.first(); m <= G2m.last(); m++) {        //s2k even G
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                s2k += G2m[m] * float(sour[n+h][x]);
                        }

                        for (int m = H2m1.first(); m <= H2m1.last(); m++) {     //s2k1 odd H
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                s2k1 += H2m1[m] * float(sour[n][x]);
                        }
                        for (int m = G2m1.first(); m <= G2m1.last(); m++) {      //s2k1 odd G
                                n = k - m;
                                if (n < 0) n = 0 - n;
                                if (n >= (int)h) n -= 2 * (1 + n - h);
                                s2k1 += G2m1[m] * float(sour[n+h][x]);
                        }

                        dest[2*k][x] = mmxround(2.0f * s2k);
                        dest[2*k+1][x] = mmxround(2.0f * s2k1);
                }
        }
}
//////////////////////////////////////////////synths//////////////////////////////////////////////////////////////////////////

