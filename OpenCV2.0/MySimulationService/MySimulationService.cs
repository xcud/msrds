using System;
using System.Collections.Generic;
using System.ComponentModel;
using Microsoft.Ccr.Core;
using Microsoft.Dss.Core.Attributes;
using Microsoft.Dss.ServiceModel.Dssp;
using Microsoft.Dss.ServiceModel.DsspServiceBase;
using W3C.Soap;
using submgr = Microsoft.Dss.Services.SubscriptionManager;

using Microsoft.Ccr.Adapters.WinForms;

namespace MySimulationService
{
    [Contract(Contract.Identifier)]
    [DisplayName("MySimulationService")]
    [Description("MySimulationService service (no description provided)")]
    class MySimulationService : DsspServiceBase
    {
        /// <summary>
        /// Service state
        /// </summary>
        [ServiceState]
        MySimulationServiceState _state = new MySimulationServiceState();

        /// <summary>
        /// Main service port
        /// </summary>
        [ServicePort("/MySimulationService", AllowMultipleInstances = true)]
        MySimulationServiceOperations _mainPort = new MySimulationServiceOperations();

        [SubscriptionManagerPartner]
        submgr.SubscriptionManagerPort _submgrPort = new submgr.SubscriptionManagerPort();

        /// <summary>
        /// Service constructor
        /// </summary>
        public MySimulationService(DsspServiceCreationPort creationPort)
            : base(creationPort)
        {
        }

        /// <summary>
        /// Service start
        /// </summary>
        protected override void Start()
        {
             
            // 
            // Add service specific initialization here
            // 

            base.Start();

            
            
        }

        MySimulationDSS _s;

        IEnumerator<ITask> xnaTask()
        {


            //MySimulation.Game1 _g = new MySimulation.Game1();

            //_g._EventOnDetected += new MySimulation.Game1.OnDetected(g__EventOnDetected);
            //_g.Run();

            _s = new MySimulationDSS(ServiceForwarder<MySimulationServiceOperations>(ServiceInfo.Service));
            _s.Run();
              


            //_s.Run();
            yield break;
        }

        void g__EventOnDetected(surfCLR.SURF surf)
        {
            MySimulationServiceState s = new MySimulationServiceState();
            s._CX = surf.GetCX();
            s._CY = surf.GetCY();
            //s._CS = surf.GetCS();

            //_mainPort.Post(new OnDetected(s));


        }



        /// <summary>
        /// Replace Handler
        /// </summary>
        /// <param name="replace"></param>
        /// <returns></returns>
        [ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        public virtual IEnumerator<ITask> StartHandler(Start2 start)
        {


            SpawnIterator(xnaTask);
            //_s  = new MySimulationDSS(_mainPort);

            //_s.Run();
           
            
            //_state._CX = 100;

            //start.ResponsePort.Post(_state);

            yield break;
        }


        [ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        public virtual IEnumerator<ITask> TurnLeftHandler(TurnRight turnLeft)
        {

            SendNotification(_submgrPort, turnLeft);
            yield break;
        }



        [ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        public virtual IEnumerator<ITask> TurnLeftHandler(Scale scale)
        {

            SendNotification(_submgrPort, scale);
            yield break;
        }


        ///// <summary>
        ///// Replace Handler
        ///// </summary>
        ///// <param name="replace"></param>
        ///// <returns></returns>
        //[ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        //public virtual IEnumerator<ITask> ReplaceHandler(OnDetected buttonRelease)
        //{

        //    //replace.ResponsePort.Post(DefaultReplaceResponseType.Instance);

        //    SendNotification(_submgrPort, buttonRelease);
        //    yield break;
        //}

        /// <summary>
        /// Handles Subscribe messages
        /// </summary>
        /// <param name="subscribe">the subscribe request</param>
        [ServiceHandler]
        public void SubscribeHandler(Subscribe subscribe)
        {
            SubscribeHelper(_submgrPort, subscribe.Body, subscribe.ResponsePort);
        }
    }
}


