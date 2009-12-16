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


namespace OpenCVDSS
{
    [Contract(Contract.Identifier)]
    [DisplayName("OpenCVDSS")]
    [Description("OpenCVDSS service (no description provided)")]
    class OpenCVDSSService : DsspServiceBase
    {
        /// <summary>
        /// Service state
        /// </summary>
        [ServiceState]
        OpenCVDSSState _state = new OpenCVDSSState();

        /// <summary>
        /// Main service port
        /// </summary>
        [ServicePort("/OpenCVDSS", AllowMultipleInstances = true)]
        OpenCVDSSOperations _mainPort = new OpenCVDSSOperations();

        [SubscriptionManagerPartner]
        submgr.SubscriptionManagerPort _submgrPort = new submgr.SubscriptionManagerPort();

        /// <summary>
        /// Service constructor
        /// </summary>
        public OpenCVDSSService(DsspServiceCreationPort creationPort)
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



            WinFormsServicePort.Post(
                new RunForm(
                    delegate()
                    {
                        return new Form3(
                            ServiceForwarder<OpenCVDSSOperations>(ServiceInfo.Service)
                        );
                    }
                )
            );


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


        [ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        public virtual IEnumerator<ITask> ButtonPressHandler(SURFInfo buttonPress)
        {

            //buttonPress.ResponsePort.Post(new OpenCVDSSState());

            SendNotification(_submgrPort, buttonPress);

            yield break;
        }

    }
}


