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
namespace MyListBox2
{
    [Contract(Contract.Identifier)]
    [DisplayName("MyListBox2")]
    [Description("MyListBox2 service (no description provided)")]
    class MyListBox2Service : DsspServiceBase
    {
        /// <summary>
        /// Service state
        /// </summary>
        [ServiceState]
        MyListBox2State _state = new MyListBox2State();

        /// <summary>
        /// Main service port
        /// </summary>
        [ServicePort("/MyListBox2", AllowMultipleInstances = true)]
        MyListBox2Operations _mainPort = new MyListBox2Operations();

        [SubscriptionManagerPartner]
        submgr.SubscriptionManagerPort _submgrPort = new submgr.SubscriptionManagerPort();

        /// <summary>
        /// Service constructor
        /// </summary>
        public MyListBox2Service(DsspServiceCreationPort creationPort)
            : base(creationPort)
        {
        }

        Form1 _form = null;

        /// <summary>
        /// Service start
        /// </summary>
        protected override void Start()
        {

            // 
            // Add service specific initialization here
            // 

            base.Start();

            _form = new Form1();

            WinFormsServicePort.Post(
                new RunForm(
                    delegate()
                    {
                        return _form;
                    }
                )
            );


        }

        [ServiceHandler(ServiceHandlerBehavior.Exclusive)]
        public virtual IEnumerator<ITask> ReplaceHandler(ButtonPress replace)
        {
            WinFormsServicePort.FormInvoke(
                delegate
                {
                    _form.AddMsg(replace.Body.msg);

                }
            );

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


