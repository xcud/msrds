//---------------------------------------------------------------------------

#ifndef Graph2DH
#define Graph2DH
#include "MatUtils.h"
#include "dynarrays.h"
//#include <math.h>

//---------------------------------------------------------------------------
AnsiString FloatToStr(double x);

// Minimal necessary number of decimals for labeling the interval [Xmin, Xmax]
int MinDec(double Xmin, double Xmax);

// Compact output of real numbers
String CompactFloatToStr(double z, int decimals);

void SetLim(double& x1, double& x2);

class PlotBox2D : public TPaintBox
{
  private:	// User declarations
    double fXmin;
    double fXmax;
    double fYmin;
    double fYmax;
    double rTdH ;
    double rBdH ;
    double rW   ;
    double rLdW ;
    double rRdW ;
    double rH   ;
    int TdH ;
    int BdH ;
    int LdW ;
    int RdW ;
  public:		// User declarations
    __fastcall PlotBox2D(Classes::TComponent* Owner);
    __fastcall virtual ~PlotBox2D(void) { }
    double Xi2r( int iX);
    double Yi2r( int iY);
    void SetLimits(double Xmin, double Xmax, double Ymin, double Ymax);
    int XDec; // X-Label decimals
    int YDec; // Y-Label decimals
    double GetXmax();
    double GetXmin();
    double GetYmin();
    double GetYmax();
    void SetXmin(double Value);
    void SetXmax(double Value);
    void SetYmin(double Value);
    void SetYmax(double Value);
    int Yr2i(double Y) ;
    int Xr2i(double X) ;
    double XYRatio;
    int NYTicks ;
    int NXTicks ;
    bool XCaption;
    bool YCaption;
    String XTitle;
    String YTitle;
    TColor FrameColor;
    int GetPenWidth()  ;
    void SetPenWidth(int Value);
    TColor GetPenColor()  ;
    TColor GetBrushColor();
    void SetPenColor(  TColor Value);
    void SetBrushColor(TColor Value);
    void Circle( double x, double y, int r);
    void MoveToF(double x, double y);
    void LineToF(double x, double y);
    void LineF(double x1, double y1, double x2, double y2);
  void RectangleF(double x1, double y1, double x2, double y2);
    void PlotFrame();
    void PlotArrays(FArr1D& x, FArr1D& y, TColor color);
    void PlotArray(FArr1D& y, TColor color);
};

#endif

