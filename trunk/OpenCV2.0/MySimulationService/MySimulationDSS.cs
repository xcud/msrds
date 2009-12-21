using System;
using System.Collections.Generic;

using System.Text;

namespace MySimulationService
{
    class MySimulationDSS
    {
        MySimulationServiceOperations _mainport = null;
        MySimulation.Game1 _g;
        public MySimulationDSS(MySimulationServiceOperations mainport)
        {
            _mainport = mainport;

            _g = new MySimulation.Game1();
            

            

        }

        public void Run()
        {
            _g._EventOnDetected += new MySimulation.Game1.OnDetected(g__EventOnDetected);
            _g._EventOnDetected2 += new MySimulation.Game1.OnDetected2(_g__EventOnDetected2);

            _g.Run();
        }

        void _g__EventOnDetected2(int scale)
        {
            MySimulationServiceState s = new MySimulationServiceState();
            s._CS = scale;
          //  s._CX = 0;
            s._CY = 0;

            _mainport.Post(new Scale(s));
        }

        void g__EventOnDetected(surfCLR.SURF surf)
        {
            MySimulationServiceState s = new MySimulationServiceState();
            s._CS = surf.GetCS();
            s._CX = surf.GetCX();
            s._CY = surf.GetCY();

            _mainport.Post(new TurnRight(s));
        }
    }
}
