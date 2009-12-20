using System;
using System.Collections.Generic;
using System.ComponentModel;
using Microsoft.Ccr.Core;
using Microsoft.Dss.Core.Attributes;
using Microsoft.Dss.ServiceModel.Dssp;
using Microsoft.Dss.ServiceModel.DsspServiceBase;
using W3C.Soap;

namespace MySimulationService
{
    /// <summary>
    /// MySimulationService contract class
    /// </summary>
    public sealed class Contract
    {
        /// <summary>
        /// DSS contract identifer for MySimulationService
        /// </summary>
        [DataMember]
        public const string Identifier = "http://schemas.tempuri.org/2009/12/mysimulationservice.html";
    }

    /// <summary>
    /// MySimulationService state
    /// </summary>
    [DataContract]
    public class MySimulationServiceState
    {
        [DataMember]
        public int _CX;
        [DataMember]
        public int _CY;
        [DataMember]
        public int _CS;

    }

    /// <summary>
    /// MySimulationService main operations port
    /// </summary>
    [ServicePort]
    public class MySimulationServiceOperations : PortSet<DsspDefaultLookup, DsspDefaultDrop, Get, Subscribe, Start2, TurnRight, Scale>
    {
    }


    public class Scale : Update<MySimulationServiceState, PortSet<DefaultSubmitResponseType, Fault>>
    {
        public Scale()
            : base(new MySimulationServiceState())
        {
        }

        public Scale(MySimulationServiceState body)
            : base(body)
        {

        }

    }

    public class TurnRight : Update<MySimulationServiceState, PortSet<DefaultSubmitResponseType, Fault>>
    {
        public TurnRight()
            : base(new MySimulationServiceState())
        {
        }

        public TurnRight(MySimulationServiceState body)
            : base(body)
        {

        }

    }

     
    ///// <summary>
    ///// ButtonRleases - Message sent when a button is released
    ///// </summary>
    //[DisplayName("(User) ButtonRelease")]
    //[Description("Indicates when a button in the dialog is released.")]
    //public class OnDetected : Update<MySimulationServiceState, PortSet<DefaultUpdateResponseType, Fault>>
    //{
    //    /// <summary>
    //    /// Constructor
    //    /// </summary>
    //    public OnDetected()
    //    {
    //    }

    //    /// <summary>
    //    /// Constructor
    //    /// </summary>
    //    /// <param name="body">Button release request message</param>
    //    public OnDetected(MySimulationServiceState body)
    //        : base(body)
    //    {
    //    }
    //}




    /// <summary>
    /// ButtonRleases - Message sent when a button is released
    /// </summary>
    [DisplayName("Star22xxt")]
    [Description("Indicates when a button in the dialog is released.")]
    public class Start2 : Get<GetRequestType, PortSet<MySimulationServiceState, Fault>>
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public Start2()
        {
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="body">Button release request message</param>
        public Start2(GetRequestType body)
            : base(body)
        {
        }


        
        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        /// <param name="body">the request message body</param>
        /// <param name="responsePort">the response port for the request</param>
        public Start2(GetRequestType body, PortSet<MySimulationServiceState, Fault> responsePort)
            : base(body, responsePort)
        {
        }
    }





    /// <summary>
    /// MySimulationService get operation
    /// </summary>
    public class Get : Get<GetRequestType, PortSet<MySimulationServiceState, Fault>>
    {
        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        public Get()
        {
        }

        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        /// <param name="body">the request message body</param>
        public Get(GetRequestType body)
            : base(body)
        {
        }

        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        /// <param name="body">the request message body</param>
        /// <param name="responsePort">the response port for the request</param>
        public Get(GetRequestType body, PortSet<MySimulationServiceState, Fault> responsePort)
            : base(body, responsePort)
        {
        }
    }

    /// <summary>
    /// MySimulationService subscribe operation
    /// </summary>
    public class Subscribe : Subscribe<SubscribeRequestType, PortSet<SubscribeResponseType, Fault>>
    {
        /// <summary>
        /// Creates a new instance of Subscribe
        /// </summary>
        public Subscribe()
        {
        }

        /// <summary>
        /// Creates a new instance of Subscribe
        /// </summary>
        /// <param name="body">the request message body</param>
        public Subscribe(SubscribeRequestType body)
            : base(body)
        {
        }

        /// <summary>
        /// Creates a new instance of Subscribe
        /// </summary>
        /// <param name="body">the request message body</param>
        /// <param name="responsePort">the response port for the request</param>
        public Subscribe(SubscribeRequestType body, PortSet<SubscribeResponseType, Fault> responsePort)
            : base(body, responsePort)
        {
        }
    }
}


