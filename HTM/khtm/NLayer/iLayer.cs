using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm.NLayer
{
    public interface iLayer
    {

        void SetUplayer(iLayer upLayer, int[,] upWeight);


        void SetDownlayer(iLayer downLayer, int[,] downWeight);
    }
}
