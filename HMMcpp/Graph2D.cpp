//---------------------------------------------------------------------------
// N.V.Shokhirev
// created:  20041020
// modified: 20051020
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Graph2D.h"
#include <math.h>
#include "MatUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

AnsiString FloatToStr(double x) { AnsiString s = x;  return s; };

// Minimal necessary number of decimals for labeling the interval [Xmin, Xmax]
int MinDec(double Xmin, double Xmax)
{
  return ( 1 + round0(-log10(fabs(Xmax-Xmin)/max1(fabs(Xmax),fabs(Xmin)))) );
};

// Compact output of real numbers
String CompactFloatToStr(double z, int decimals)
{
const double eps = 1.0e-8;
  double x;
  int exponent;
  String s, ss, result;

  if (z < 0.0) { s = "-"; }
  else { s = " "; };

  x = fabs(z);
  if (x < eps) { result = s+"0.0"; }
  else
  {
    exponent = floor(log10(x));
    x = pow10(-decimals)*round1(x*pow10(decimals-exponent));

    if (x >9.999)
    {
      x = 0.1*x;
      exponent++;
    };
    ss = IntToStr(exponent);

    if (exponent < 0) { ss = "e"+ss; }
    else { ss = "e+"+ss; };

    result = s+FloatToStr(x)+ss;
  };
  return result;
};

void SetLim(double& x1,double& x2)
{
  double xmax = max1( abs(x1), abs(x2) );

  if ( xmax < SafeMinReal ) // x1 = x2 = 0
  {
     x2 = x1 +1.0;
     return;
  }
//  if ( (x2-x1) < SafeMacheps*xmax ) // x1 = x2
  if ( (x2-x1) < xmax/1024.0 ) // x1 = x2
  {
    x1 -= xmax/1024.0;
    x2 += xmax/1024.0;
  };
};


__fastcall PlotBox2D::PlotBox2D(Classes::TComponent* Owner): TPaintBox(Owner)
{
//  Parent := aParent;
  Align = alClient;
  FrameColor = clBlack;
  XCaption = true;
  YCaption = true;
  NXTicks = 3;
  NYTicks = 3;
  XTitle = "X Axis";
  YTitle = "Y Axis";
  XYRatio = -1.0;
  SetLimits(0.0,1.0,0.0,1.0);
}

double PlotBox2D::Xi2r( int iX)
{
  double X;
  X = double(iX);
  return (X*(fXmax-fXmin)- rLdW*fXmax-rRdW*fXmin+ rW*fXmin)/(rW-rLdW-rRdW);
};

double PlotBox2D::Yi2r( int iY)
{
  double Y;
  Y = double(iY);
  return (Y*(fYmax - fYmin)-rH*fYmax+rBdH*fYmax+rTdH*fYmin)/(rBdH+rTdH-rH);
};

void PlotBox2D::SetLimits(double Xmin, double Xmax, double Ymin, double Ymax)
{
  fXmin = Xmin;
  fXmax = Xmax;
  fYmin = Ymin;
  fYmax = Ymax;

  SetLim(fXmin, fXmax);
  SetLim(fYmin, fYmax);

  XDec = MinDec(fXmin, fXmax);
  YDec = MinDec(fYmin, fYmax);
};

void PlotBox2D::Circle( double x, double y, int r)
{
  this->Canvas->Ellipse(Xr2i(x)-r, Yr2i(y)-r, Xr2i(x)+r, Yr2i(y)+r);
};

void PlotBox2D::MoveToF(double x, double y)
{
  this->Canvas->MoveTo(Xr2i(x), Yr2i(y));
};

void PlotBox2D::LineToF(double x, double y)
{
  this->Canvas->LineTo(Xr2i(x), Yr2i(y));
};

void PlotBox2D::LineF(double x1, double y1, double x2, double y2)
{
  this->Canvas->MoveTo(Xr2i(x1), Yr2i(y1));
  this->Canvas->LineTo(Xr2i(x2), Yr2i(y2));
};

void PlotBox2D::RectangleF(double x1, double y1, double x2, double y2)
{
  this->Canvas->Rectangle(Xr2i(x1), Yr2i(y1),Xr2i(x2), Yr2i(y2));
};

void PlotBox2D::PlotFrame()
{
  // FrameOffsets
  int FTdH, FBdH, FLdW, FRdW;

  int H;     // Text height
  int HalfH; // H / 2
  int W;    // All other spaces
  int WY;   // Y-Label width
  int WX;   // X-Label Half-width

  int iX, iY, iHm, iWm, i, d, ws;
  double x,  y, PPUx, PPUy;
  String s;


  this->Canvas->Brush->Color = clWhite;
  this->Canvas->FillRect(Rect(0,0,Width-1,Height-1));
  rH = Height;
  rW = Width;

  H = this->Canvas->TextHeight("H");
  HalfH = H / 2;
  W = this->Canvas->TextWidth("H");

  this->Canvas->Pen->Color = FrameColor;
  WX = 0; // Half X Label Width
  BdH = W+W;
  if (XCaption)
  {
    for (i = 1; i <= NXTicks; i++)
    {
      x = fXmin +double(i-1)*(fXmax-fXmin)/double(NXTicks-1);
      s = CompactFloatToStr(x,XDec);
      ws = this->Canvas->TextWidth(s) / 2;
      if (ws > WX) WX = ws;
    };
    BdH = BdH + H+H + W+W+W;
  };

  WY = 0; // Y Label width
  TdH = W+W;
  LdW = W+W;
  if (YCaption)
  {
    for (i = 1; i <= NYTicks; i++)
    {
      y = fYmin +(i-1)*(fYmax-fYmin)/(NYTicks-1);
      s = CompactFloatToStr(y,YDec);
      ws = this->Canvas->TextWidth(s);
      if (ws > WY) WY = ws;
    };
    TdH = TdH + H+W;
    LdW = LdW + WY + W+W;
  };
  // Adjustment
  RdW = max0(W+W,WX+W);
  LdW = max0(LdW,WX+W);
  // Adjustment for X/Y ratio
  if (XYRatio > 0.0)
  {
    iWm = Width - LdW - RdW;
    PPUx = iWm;
    PPUx = PPUx/(fXmax - fXmin); // Pixes Per Unit
    iHm = Height - TdH - BdH;
    PPUy = iHm; 
    PPUy = PPUy/(fYmax - fYmin); // Pixes Per Unit
    if (PPUx > PPUy*XYRatio)
    {
      PPUx = PPUy*XYRatio;
      d = (iWm - round0(PPUx*(fXmax-fXmin))) / 2;
      LdW = LdW + d;
      RdW = RdW + d;
    }
    else
    {
      PPUy = PPUx/XYRatio;
      d = (iHm - round0(PPUy*(fYmax-fYmin))) / 2;
      TdH = TdH + d;
      BdH = BdH + d;
    };
  };
  // Frame Offsets
  FTdH = TdH - W;
  FBdH = BdH - W;
  FLdW = LdW - W;
  FRdW = RdW - W;
  rTdH = TdH;
  rBdH = BdH;
  rLdW = LdW;
  rRdW = RdW;
  // Draw Frame
  this->Canvas->MoveTo(   FLdW,    FTdH);
  this->Canvas->LineTo(Width-FRdW,    FTdH); // Top
  this->Canvas->LineTo(Width-FRdW, Height-FBdH); // Right
  this->Canvas->LineTo(   FLdW, Height-FBdH); // Bottom
  this->Canvas->LineTo(   FLdW,    FTdH); // Left

  if (YCaption)
  {
    d = this->Canvas->TextWidth(YTitle) / 2;
    this->Canvas->TextOut(max0(W, FLdW-d), max0(W,FTdH-W-H), YTitle);

    for (i = 1; i <= NYTicks; i++)
    {
      y = fYmin +(i-1)*(fYmax-fYmin)/(NYTicks-1);
      iY = Yr2i(y);
      s = CompactFloatToStr(y,YDec);
      this->Canvas->TextOut(FLdW-W-W-WY,iY-HalfH, s);
    };
    for (i = 1; i <= NYTicks; i++)
    {
      y = fYmin +(i-1)*(fYmax-fYmin)/(NYTicks-1);
      iY = Yr2i(y);
      this->Canvas->MoveTo(FLdW,iY);   // Y-Ticks
      this->Canvas->LineTo(FLdW-W,iY);
    };
  };

  if (XCaption)
  {
    d = this->Canvas->TextWidth(XTitle) / 2;
    i = min0(Width-FRdW+d,Width-H);
    this->Canvas->TextOut(max0(i-d-d,W), Height-FBdH+W+W+W+H, XTitle);
    for (i = 1; i <= NXTicks; i++)
    {
      x = fXmin +(i-1)*(fXmax-fXmin)/(NXTicks-1);
      iX = Xr2i(x);
      s = CompactFloatToStr(x,XDec);
      d = this->Canvas->TextWidth(s) / 2;
      this->Canvas->TextOut(iX-d,Height-FBdH+W+W, s);
      this->Canvas->MoveTo(iX,Height-FBdH);   // X-Ticks
      this->Canvas->LineTo(iX,Height-FBdH+W);
    };
  };
};

void PlotBox2D::SetXmin(double Value)
{
  fXmin = Value;
};

void PlotBox2D::SetXmax(double Value)
{
  fXmax = Value;
};

void PlotBox2D::SetYmin(double Value)
{
  fYmin = Value;
};

void PlotBox2D::SetYmax(double Value)
{
  fYmax = Value;
};

double PlotBox2D::GetXmax()
{
  return fXmax;
};

double PlotBox2D::GetXmin()
{
  return fXmin;
};

double PlotBox2D::GetYmin()
{
  return fYmin;
};

double PlotBox2D::GetYmax()
{
  return fYmax;
};

int PlotBox2D::Yr2i(double Y)
{
  return round0( (Y*(rBdH+rTdH-rH)+rH*fYmax-rBdH*fYmax-rTdH*fYmin)/
                                                                (fYmax-fYmin) );
};

int PlotBox2D::Xr2i(double X)
{
  return round0( (X*(rW-rLdW-rRdW)+rLdW*fXmax+rRdW*fXmin- rW*fXmin)/
                                                                (fXmax-fXmin) );
};

int PlotBox2D::GetPenWidth()
{
  return (this->Canvas->Pen->Width);
};
void PlotBox2D::SetPenWidth(int Value)
{
  this->Canvas->Pen->Width = Value;
};

TColor PlotBox2D::GetPenColor()
{
  return (this->Canvas->Pen->Color);
};

TColor PlotBox2D::GetBrushColor()
{
  return (this->Canvas->Brush->Color);
};

void PlotBox2D::SetPenColor(  TColor Value)
{
  this->Canvas->Pen->Color = Value;
};

void PlotBox2D::SetBrushColor(TColor Value)
{
  this->Canvas->Brush->Color = Value;
};

void PlotBox2D::PlotArrays(FArr1D& x, FArr1D& y, TColor color)

{
  double x1, x2, y1, y2;
  TColor oldcolor = color;
  this->SetPenColor(color);
  for (int i = x.L1(); i < x.H1(); i++)
  {
    x1 = x(i);   y1 = y(i);
    x2 = x(i+1); y2 = y(i+1);
    this->LineF(x1,y1,x2,y2);
  };
  this->SetPenColor(oldcolor);
};

void PlotBox2D::PlotArray(FArr1D& y, TColor color)
{
  double x1, x2, y1, y2;
  TColor oldcolor = color;
  this->SetPenColor(color);
  for (int i = y.L1(); i < y.H1(); i++)
  {
    x1 = double(i);   y1 = y(i);
    x2 = double(i+1); y2 = y(i+1);
    this->LineF(x1,y1,x2,y2);
  };
  this->SetPenColor(oldcolor);
};

