using System;
using System.Collections.Generic;
using System.ComponentModel;
using Microsoft.Ccr.Core;
using Microsoft.Dss.Core.Attributes;
using Microsoft.Dss.ServiceModel.Dssp;
using Microsoft.Dss.ServiceModel.DsspServiceBase;
using W3C.Soap;

namespace OpenCVDSS
{
    /// <summary>
    /// OpenCVDSS contract class
    /// </summary>
    public sealed class Contract
    {
        /// <summary>
        /// DSS contract identifer for OpenCVDSS
        /// </summary>
        [DataMember]
        public const string Identifier = "http://schemas.tempuri.org/2009/12/opencvdss.html";
    }

    /// <summary>
    /// OpenCVDSS state
    /// </summary>
    [DataContract]
    public class OpenCVDSSState
    {
        public int _X;
        public int _Y;
        public int _S;
    }


    /// <summary>
    /// Replace - Replaces the state
    /// </summary>
    [DisplayName("(User) DialogStateChange22")]
    [Description("Indicates when the dialog state changes.")]
    public class Replace : Replace<OpenCVDSSState, PortSet<DefaultReplaceResponseType, Fault>>
    {
    }



    /// <summary>
    /// OpenCVDSS main operations port
    /// </summary>
    [ServicePort]
    public class OpenCVDSSOperations : PortSet<DsspDefaultLookup, DsspDefaultDrop, Get, Subscribe, SURFInfo, SURFInfo2, Replace>
    {
    }

    /// <summary>
    /// OpenCVDSS get operation
    /// </summary>
    [DisplayName("(User) SURFInfo3")]
    [Description("Indicates when a button in the dialog is pressed.")]

    public class Get : Get<GetRequestType, PortSet<OpenCVDSSState, Fault>>
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
        public Get(GetRequestType body, PortSet<OpenCVDSSState, Fault> responsePort)
            : base(body, responsePort)
        {
        }
    }


    /// <summary>
    /// OpenCVDSS get operation
    /// </summary>

    [DisplayName("(User) SURFInfo2")]
    [Description("Indicates when a button in the dialog is pressed.")]
    public class SURFInfo2 : Get<GetRequestType, PortSet<OpenCVDSSState, Fault>>
    {
        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        public SURFInfo2()
        {
        }

        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        /// <param name="body">the request message body</param>
        public SURFInfo2(GetRequestType body)
            : base(body)
        {
        }

        /// <summary>
        /// Creates a new instance of Get
        /// </summary>
        /// <param name="body">the request message body</param>
        /// <param name="responsePort">the response port for the request</param>
        public SURFInfo2(GetRequestType body, PortSet<OpenCVDSSState, Fault> responsePort)
            : base(body, responsePort)
        {
        }
    }


    /// <summary>
    /// ButtonPress - Message sent when a button is pressed
    /// </summary>

    [DisplayName("(User) SURFInfo")]
    [Description("Indicates when a button in the dialog is pressed.")]
    public class SURFInfo : Update<OpenCVDSSState, PortSet<DefaultUpdateResponseType, Fault>>
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public SURFInfo()
        {
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="body">Button press request message</param>
        public SURFInfo(OpenCVDSSState body)
            : base(body)
        {
        }
    }



    /// <summary>
    /// OpenCVDSS subscribe operation
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


