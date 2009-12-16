using System;
using System.Collections.Generic;
using System.ComponentModel;
using Microsoft.Ccr.Core;
using Microsoft.Dss.Core.Attributes;
using Microsoft.Dss.ServiceModel.Dssp;
using Microsoft.Dss.ServiceModel.DsspServiceBase;
using W3C.Soap;
using submgr = Microsoft.Dss.Services.SubscriptionManager;

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


            MySimulation.Game1 g = new MySimulation.Game1();

            g._EventOnDetected += new MySimulation.Game1.OnDetected(g__EventOnDetected);

            g.Run();

        }

        void g__EventOnDetected(surfCLR.SURF surf)
        {
            MySimulationServiceState s = new MySimulationServiceState();
            s._CX = surf.GetCX();
            s._CY = surf.GetCY();
            s._CS = surf.GetCS();

            _mainPort.Post(new  OnDetected(s));


        }



        /// <summary>
        /// Replace Handler
        /// </summary>
        /// <param name="replace"></param>
        /// <returns></returns>
        [ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        public virtual IEnumerator<ITask> ReplaceHandler(OnDetected buttonRelease)
        {

            //replace.ResponsePort.Post(DefaultReplaceResponseType.Instance);

            SendNotification(_submgrPort, buttonRelease);
            yield break;
        }

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


