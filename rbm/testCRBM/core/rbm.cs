using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// http://www.scipy.org/Numpy_Example_List#head-f6e1f97602be88279491a78c2d8ae067140cf4d3


namespace testCRBM.core
{
    class rbm
    {
        private double sig;
        private double epsW;
        private double epsA;
        private int nvis;
        private int nhid;
        private int Ndat;
        private double cost;
        private double moment;
        private float[] Svis0;
        private float[] Svis;
        private float[] Shid;
        private float[,] W;
        private float[,] dW;
        private float[] Avis;
        private float[] Ahid;
        private float[] dA;
        

        Random _R = new Random();
        private float[,] dat;
        // 정규분포 를 뽑아주어야한다.


        float[,] standard_normal(int n,int m,int div)
        {
            float[,] nm = new float [n,m];



            return nm;
        }
        float [] standard_normal(int n)
        {
            float[] arr = new float [n];

            return arr;
        }

        void ones(float [] arr,float value)
        {
            for (int i = 0; i < arr.Length; i++)
            {
                arr[i] = value;
            }
        }

        void init(float self, int nvis, int nhid)
        {
            this.sig = 0.2;
            this.epsW = 0.5;
            this.epsA  = 0.5;
            this.nvis = nvis;
            this.nhid = nhid;
            this.Ndat = 500;
            this.cost = 0.00001;
            this.moment = 0.90;
            this.Svis0 = new float[nvis+1];
            this.Svis0[nvis] = 1.0f;
            this.Svis = new float[nvis+1];
            this.Svis[nvis] = 1.0f;
            this.Shid = new float[nhid+1];//zeros( nhid+1, dtype=float32);
            this.W  = standard_normal(nvis+1, nhid+1,10);
            this.dW = standard_normal(nvis+1, nhid+1,1000);
            this.Avis  =  new float[nvis+1 ] ;
            ones(Avis, 0.1f);
            this.Ahid  = new float[nhid+1];// dtype=float32);
            ones(Ahid, 1);

            this.dA = new float[nvis + 1];//dtype=float32);
            this.dat = this.genData();
                

        }

        float [] random_sample(int len )
        {
            float []  arr = new float[len];
            for(int i = 0 ;i< len ;i++)
            {
                arr[i] = (float)_R.NextDouble();
            }

            return arr;
        }
        
        float [,] genData()
        {
            //float c1 = 0.5f;
            //float r1 = 0.4f;
            //float r2 = 0.3f;
            ////# generate enough data to filter
            //int N = 20* this.Ndat;
            //var X = random_sample(N);
            //var Y = random_sample(N);
            //var X1 = X[(X-c1)*(X-c1) + (Y-c1)*(Y-c1) < r1*r1]
            //Y1 = Y[(X-c1)*(X-c1) + (Y-c1)*(Y-c1) < r1*r1]
            //X2 = X1[(X1-c1)*(X1-c1) + (Y1-c1)*(Y1-c1) > r2*r2]
            //Y2 = Y1[(X1-c1)*(X1-c1) + (Y1-c1)*(Y1-c1) > r2*r2]
            //X3 = X2[ abs(X2-Y2)>0.05 ]
            //Y3 = Y2[ abs(X2-Y2)>0.05 ]
            //#X3 = X2[ X2-Y2>0.15 ]
            //#Y3 = Y2[ X2-Y2>0.15]
            //X4=zeros( this.Ndat, dtype=float32)
            //Y4=zeros( this.Ndat, dtype=float32)
            //for i in xrange(self.Ndat):
            //    if (X3[i]-Y3[i]) >0.05:
            //        X4[i] = X3[i] + 0.08
            //        Y4[i] = Y3[i] + 0.18
            //    else:
            //        X4[i] = X3[i] - 0.08
            //        Y4[i] = Y3[i] - 0.18
            //print "X", size(X3[0:self.Ndat]), "Y", size(Y3)
            //return(vstack((X4[0:self.Ndat],Y4[0:self.Ndat])))
            return new float[this.Ndat, this.Ndat];
        }

        // # Sigmoidal Function
        float sigFun(float  X,float  A)
        {

        
            float lo = 0.0f;
            float hi = 1.0f;
            return (lo + (hi - lo) / (1.0f + (float)Math.Exp(-A * X)));
        }

       //    # visible=0, hidden=1
        void activ(int who)
        {
        if(who==0)
        {
            float [] temp = standard_normal(this.nvis +1);
            for(int i = 0; i< this.Svis.Length;i++)
            {
               // this.Svis = dot(self.W, self.Shid) + self.sig*standard_normal(self.nvis+1)         
                this.Svis[i] = this.W * this.Shid + this.sig * temp[i];
            }
                        self.Svis = self.sigFun(self.Svis, self.Avis)
            self.Svis[-1] = 1.0 # bias
        }
        if(who==1)
        {
            self.Shid = dot(self.Svis, self.W) + self.sig*standard_normal(self.nhid+1)
            self.Shid = self.sigFun(self.Shid, self.Ahid)
            self.Shid[-1] = 1.0 # bias
        }
    }
}
