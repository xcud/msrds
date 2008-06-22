using System;
using System.Collections.Generic;
using System.Text;

namespace Robotics.Dssopencv2
{
    public class DSSOpenCVForm : CLROpenCV.MovingDetect
    {

        Dssopencv2Operations _eventsPort;

        public DSSOpenCVForm(Dssopencv2Operations eventsPort)
        {
            _eventsPort = eventsPort;

        }
        
    }
}
