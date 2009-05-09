using System;
using System.Collections.Generic;
using System.Text;

namespace Robotics.Dssopencv2
{

    /// <summary>
    /// C# opencv 폼 랩핑
    /// 
    /// </summary>
    public class DSSOpenCVForm : CLROpenCV.MovingDetect
    {

        Dssopencv2Operations _eventsPort;

        int _CurIndex = -1;

        public DSSOpenCVForm(Dssopencv2Operations eventsPort)
        {
            _eventsPort = eventsPort;

        }
         
        public override void OnDectect(int index)
        {
            if(-1 < index && _CurIndex != index)
            {
                _CurIndex = index;
                _eventsPort.Post(new IndexUpdate(new ButtonPressRequest(index)));
                
            }
        
        }

        
    }
}
