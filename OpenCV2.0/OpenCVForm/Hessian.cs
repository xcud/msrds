using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
            /*
Numerical derivation
http://en.wikipedia.org/wiki/Numerical_differentiation
*/

//float pi = 3.141592654;
//double h  = 0.00001;


namespace OpenCVForm
{


    public class HarrisInfo
    {
        public double _Min = double.MaxValue;
        public double _Max = double.MinValue;
    }


    class Hessian
    {
        double h = 0.00001;

        // define the function you want to use
        double F(double x, double y, double T)
        {

            return (1.0 / (2.0 * Math.PI * T * T)) * Math.Exp(-((x * x + y * y) / (2.0 * T * T)));
        }

        // first derivative to x
        double Fx(double x, double y, double T)
        {
            double fl = F(x - h, y, T) ;
            double fr = F(x + h, y, T) ;    
            return (fr - fl) / (2.0 * h);
        }

        // first derivative to y
        double Fy(double x, double y, double T)
        {
            double fl = F(x, y - h, T) ;
            double fr = F(x, y + h, T) ;    
            return (fr - fl) / (2.0 * h);
        }

        // second derivative of fx to x
        double Fxx(double x, double y, double T)
        {
            double fl = Fx(x - h, y, T) ;
            double fr = Fx(x + h, y, T) ;    
            return (fr - fl) / (2.0 * h);
        }

        // second derivative of fx to y
        double Fxy(double x, double y, double T)
        {
            double fl = Fx(x, y - h, T) ;
            double fr = Fx(x, y + h, T) ;    
            return (fr - fl) / (2.0 * h);
        }

        // second derivative of fy to x
        double Fyx(double x, double y, double T)
        {
            double fl = Fy(x - h, y, T) ;
            double fr = Fy(x + h, y, T) ;    
            return (fr - fl) / (2.0 * h);
        }

        // second derivative of fy to x
        double Fyy(double x, double y, double T)
        {
            double fl = Fy(x, y - h, T) ;
            double fr = Fy(x, y + h, T) ;
            return (fr - fl) / (2.0 * h);
        }

        ///**integral to calculate int(K(x), x=x_start..x_end)*/
        //double Kint(double x_start, double x_end){
        //    // making the step smaller gives more accurate results (and requires more computation)
        //    double x_step = (x_end - x_start) / 100;      
        //    double area = 0.0;   
        //    // we start at x_start plus half the step width to calculate the function 
        //    // at the middle of the elements  
        //    for (double x = x_start + x_step/2; x < x_end; x += x_step)  
        //    {        area += K(x) * x_step;    }    
        //    return area;
        //}

        ///**double integral to calculate int(K(x,y), x=x_start..x_end, y=y_start..y_end)*/
        //double Kint(double x_start, double x_end, double y_start, double y_end)
        //{   
        //    // making the x_step smaller gives more accurate results (and requires more computation) 
        //    double x_step = (x_end - x_start) / 100;    double y_step = (y_end - y_start) / 100;  
        //    double volume = 0.0;    
        //    // we start at x_start and y_start plus half the step width to
        //    // calculate the function at the middle of the elements   
        //    for (double x = x_start + x_step/2; x < x_end; x += x_step) 
        //    {      
        //        for (double y = y_start + y_step/2; y < y_end; y += y_step)   
        //        {           
        //            volume += K(x, y) * x_step * y_step;      
        //        }   
        //    }    return volume;
        //}

        ///**double convolution of Img(x,y) with F(x,y) around (x_center, y_center)*/
        //double Kint(double x_center, double y_center){  
        //    // choose appropriate start and end values,
        //    // such that F(x_center - x, y_center - y) is about zero at the 
        //    // boundaries (in this case F(-100, -100) and F(100,100), etc.) 
        //    double x_start = x_center - 100;    double x_end = x_center + 100; 
        //    double y_start = y_center - 100;    double y_end = y_center + 100; 
        //    // making the x_step smaller gives more accurate results (and requires more computation)
        //    double x_step = (x_end - x_start) / 100; 
        //    double y_step = (y_end - y_start) / 100; 
        //    // we start at x_start and y_start plus half the step width to  
        //    // calculate the function at the middle of the elements     
        //    double volume = 0.0;    
        //    for (double x = x_start + x_step/2; x < x_end; x += x_step)   
        //    {       
        //        for (double y = y_start + y_step/2; y < y_end; y += y_step) 
        //        {           
        //            volume += (Img(x, y) * F(x_center - x, y_center - y)) * x_step * y_step;       
        //        }   
        //    }   
        //    return volume;
        //}



        void FirstDerivative(Bitmap bmp)
        {

        }

        public void Gaussian(Bitmap bmp,Bitmap bmp2)
        {



            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            System.Drawing.Imaging.BitmapData data = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                bmp.PixelFormat);

            Rectangle rect2 = new Rectangle(0, 0, bmp2.Width, bmp.Height);
            System.Drawing.Imaging.BitmapData data2 = bmp2.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                bmp2.PixelFormat);

            double[][] g = new double[][] { new double[] { 1, 4, 6, 4, 1 }, new double[] { 4, 16, 24, 16, 4 }
                ,new double[] { 6, 24, 36, 24, 6 },new double[] { 4, 16, 24, 16, 4 },new double[] { 1, 4, 6, 4, 1 } };


            for (int y = 0; y < 5; y++)
            {
                for (int x = 0; x < 5; x++)
                {
                    g[y][x] /= 256;
                }

            }


            unsafe
            {
                byte* imgPtr = (byte*)(data.Scan0);
                byte* imgPtr2 = (byte*)(data2.Scan0);

                //imgPtr += data.Stride * 2;
                

                for (int y = 0; y < data.Height - 4; y++)
                {
                    


                    for (int x = 0; x < data.Width - 4; x++)
                    {

                        

                        double t = 0;

                        for (int j = 0; j < 5; j++)
                        {
                            for (int i = 0; i < 5; i++)
                            {
                                t += *(imgPtr + data.Stride * j + i) * g[j][i];
                            }

                        }

                        *(imgPtr2 + data.Stride * 2 - 2) = (byte)t;

                        



                        imgPtr++;
                        imgPtr2++;
                    }


                    imgPtr += data.Stride - data.Width + 4;
                    imgPtr2 += data2.Stride - data2.Width + 4;
                }

            }


            // Unlock the bits.
            bmp.UnlockBits(data);
            bmp2.UnlockBits(data);

            // Draw the modified image.







        }


        public void Laplace(Bitmap bmp, Bitmap bmp2)
        {



            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            System.Drawing.Imaging.BitmapData data = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                bmp.PixelFormat);

            Rectangle rect2 = new Rectangle(0, 0, bmp2.Width, bmp2.Height);
            System.Drawing.Imaging.BitmapData data2 = bmp2.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                bmp2.PixelFormat);


            unsafe
            {
                byte* imgPtr = (byte*)(data.Scan0);
                byte* imgPtr2 = (byte*)(data2.Scan0);

                imgPtr += data.Stride;
                imgPtr2 += data.Stride;

                for (int y = 1; y < data.Height - 1; y++)
                {
                    imgPtr += 1;
                    imgPtr2 += 1;
                    for (int x = 1; x < data.Width - 1; x++)
                    {

                        // write the logic implementation here

                        int t = -*(imgPtr - data.Stride - 1);
                        t -= *(imgPtr - data.Stride);
                        t -= *(imgPtr - data.Stride + 1);
                        t -= *(imgPtr - 1);
                        t += *(imgPtr) * 8;
                        t -= *(imgPtr + 1);
                        t -= *(imgPtr + data.Stride - 1);
                        t -= *(imgPtr + data.Stride);
                        t -= *(imgPtr + data.Stride + 1);

                        //t /= 9;

                        if (t < 0)
                        {
                            *imgPtr2 = 0;
                        }
                        else if (t < 255)
                        {
                            *imgPtr2 = (byte)t;
                        }
                        else
                        {
                            *imgPtr2 = 255;
                        }

                        imgPtr++;
                        imgPtr2++;
                    }


                    imgPtr += data.Stride - data.Width + 1;
                    imgPtr2 += data.Stride - data.Width + 1;
                }

            }


            // Unlock the bits.
            bmp.UnlockBits(data);
            bmp2.UnlockBits(data);

            // Draw the modified image.





        }


        public void Harris(Bitmap bmp, Bitmap bmp2)
        {

            HarrisInfo hi = new HarrisInfo();

            Bitmap temp = new Bitmap(bmp.Width,bmp.Height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);

            //Laplace(bmp, temp);
            Laplace(bmp, temp);

            //double [,] temp = new double[bmp.Height,bmp.Width];

            double[] dx2 = new double[bmp.Width * bmp.Height];
            double[] dy2 = new double[bmp.Width * bmp.Height];
            double[] dxy = new double[bmp.Width * bmp.Height];

            double [] harrMap = new double[bmp.Width * bmp.Height];

            Rectangle rect = new Rectangle(0, 0, temp.Width, temp.Height);
            System.Drawing.Imaging.BitmapData data = temp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                temp.PixelFormat);

            Rectangle rect2 = new Rectangle(0, 0, bmp2.Width, bmp2.Height);
            System.Drawing.Imaging.BitmapData data2 = bmp2.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                bmp2.PixelFormat);


            //double max = -0000000.1;

            SortedList<double, double> sList = new SortedList<double, double>();

            unsafe
            {
                byte* imgPtr = (byte*)(data.Scan0);
                byte* imgPtr2 = (byte*)(data2.Scan0);
                
                fixed (double* fpdx2 = dx2)
                fixed (double* fpdy2 = dy2)
                fixed (double* fpdxy = dxy)
                fixed (double* fharrMap = harrMap)
                {
                    double* pdx2 = fpdx2;
                    double* pdy2 = fpdy2;
                    double* pdxy = fpdxy;
                    double* ph = fharrMap;

                    double tx ,ty;


                    for (int y = 0; y < data.Height - 2; y++)
                    {

                        for (int x = 0; x < data.Width - 2; x++)
                        {
                            
                            tx = *(imgPtr  +2);
                            tx += *(imgPtr + data.Width + 2);
                            tx += *(imgPtr + data.Width*2 + 2);

                            
                            tx = *(imgPtr );
                            tx -= *(imgPtr + data.Width );
                            tx -= *(imgPtr + data.Width *2);

                            tx /= 6;


                            ty = *(imgPtr + data.Width*2 );
                            ty += *(imgPtr + data.Width*2 +1 );
                            ty += *(imgPtr + data.Width *2+ 2);

                            ty -= *(imgPtr );
                            ty -= *(imgPtr + 1);
                            ty -= *(imgPtr + 2);

                            ty /= 6;

                            *(pdx2 + data.Width +1) = tx * tx;
                            *(pdy2  + data.Width +1)= ty * ty;
                            *(pdxy  + data.Width +1)= tx * ty;


                            pdx2++;
                            pdy2++;
                            pdxy++;
                            imgPtr++;
                        


                        }
                        imgPtr += data.Stride - data.Width + 2;
                        pdx2 +=2;
                        pdy2 +=2;
                        pdxy +=2;



                    }



                    pdx2 = fpdx2;
                    pdy2 = fpdy2;
                    pdxy = fpdxy;

                    pdx2 += data.Width;
                    pdy2 += data.Width;
                    pdxy += data.Width;
                    ph += data.Width;


                    for (int y = 1; y < data.Height - 1; y++)
                    {
                        pdx2++;
                        pdy2++;
                        pdxy++;
                        ph++;


                        for (int x = 1; x < data.Width - 1; x++)
                        {

                            double d = (*pdx2 * *pdy2 - 2 * *pdxy) - 0.04 * (*pdx2 + *pdy2) * (*pdx2 + *pdy2);


                            if( d < hi._Min )
                                hi._Min = d ;

                            if (hi._Max < d)
                                hi._Max = d ;
                            *ph = d;


                            pdx2++;
                            pdy2++;
                            pdxy++;
                            ph++;

                        }
                        
                        pdx2++;
                        pdy2++;
                        pdxy++;
                        ph++;

                    }


                    double scale = (255 - 0)/(hi._Max-hi._Min);
	                double  shift = -hi._Min * scale + 0;

                    ph = fharrMap;
                    
                    ph += data.Width;
                    imgPtr2 += data2.Stride;

                    for (int y = 1; y < data.Height - 1; y++)
                    {
                        imgPtr2++;
                        ph++;


                        for (int x = 1; x < data.Width - 1; x++)
                        {

                            


                            *imgPtr2 = (byte)((*ph )* scale + shift);


                            
                            imgPtr2++;
                            ph++;

                        }
                        
                        
                        imgPtr2 += data2.Stride - data.Width + 1;
                            ph++;

                    }


                }

            }


            // Unlock the bits.
            temp.UnlockBits(data);
            bmp2.UnlockBits(data);

            // Draw the modified image.

            



            
        }


        public void Do(Bitmap bmp)
        {

            

            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            System.Drawing.Imaging.BitmapData data = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                bmp.PixelFormat);



            unsafe
            {
                byte* imgPtr = (byte*)(data.Scan0);

                imgPtr += data.Stride ;

                for (int y = 1; y < data.Height-1; y++)
                {
                    imgPtr += 1;
                    for (int x = 1; x < data.Width-1 ; x++)
                    {

                        // write the logic implementation here

                        int t = -*(imgPtr - data.Stride - 1);
                        t -= *(imgPtr - data.Stride);
                        t -= *(imgPtr - data.Stride + 1);
                        t -= *(imgPtr - 1);
                        t += *(imgPtr) * 8;
                        t -= *(imgPtr + 1);
                        t -= *(imgPtr + data.Stride - 1);
                        t -= *(imgPtr + data.Stride);
                        t -= *(imgPtr + data.Stride + 1);

                        //*imgPtr = (byte)(t / 9);

                        if (t < 0)
                        {
                            *imgPtr = 0;
                        }else if(t < 255)
                        {
                            *imgPtr = (byte)t;
                        }else{
                            *imgPtr = 255;
                        }

                        imgPtr++;
                    }
                                        

                    imgPtr += data.Stride - data.Width +1;
                }

            }


            // Unlock the bits.
            bmp.UnlockBits(data);

            // Draw the modified image.





        }
    }
}
