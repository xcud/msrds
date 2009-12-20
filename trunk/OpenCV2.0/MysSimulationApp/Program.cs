using System;

namespace MysSimulationApp
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        /// 

        [STAThread]
        static void Main(string[] args)
        {


            using (MySimulation.Game1 game = new MySimulation.Game1())
            {
                game.Run();
            }
        }
    }
}

