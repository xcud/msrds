using System;
using System.Collections.Generic;
using System.ComponentModel;
using Microsoft.Ccr.Core;
using Microsoft.Dss.Core.Attributes;
using Microsoft.Dss.ServiceModel.Dssp;
using Microsoft.Dss.ServiceModel.DsspServiceBase;
using W3C.Soap;

namespace MyListBox2
{
    /// <summary>
    /// MyListBox2 contract class
    /// </summary>
    public sealed class Contract
    {
        /// <summary>
        /// DSS contract identifer for MyListBox2
        /// </summary>
        [DataMember]
        public const string Identifier = "http://schemas.tempuri.org/2009/12/mylistbox2.html";
    }

    /// <summary>
    /// MyListBox2 state
    /// </summary>
    [DataContract]
    public class MyListBox2State
    {
        [DataMember]
        public string msg;
    }

    /// <summary>
    /// MyListBox2 main operations port
    /// </summary>
    [ServicePort]
    public class MyListBox2Operations : PortSet<DsspDefaultLookup, DsspDefaultDrop, Get, Subscribe, ButtonPress>
    {
    }


    /// <summary>
    /// ButtonPress - Message sent when a button is pressed
    /// </summary>
    [DisplayName("(User) ButtonPress")]
    [Description("Indicates when a button in the dialog is pressed.")]
    public class ButtonPress : Update<MyListBox2State, PortSet<DefaultUpdateResponseType, Fault>>
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public ButtonPress()
        {
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="body">Button press request message</param>
        public ButtonPress(MyListBox2State body)
            : base(body)
        {
        }
    }


    /// <summary>
    /// MyListBox2 get operation
    /// </summary>
    public class Get : Get<GetRequestType, PortSet<MyListBox2State, Fault>>
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
        public Get(GetRequestType body, PortSet<MyListBox2State, Fault> responsePort)
            : base(body, responsePort)
        {
        }
    }

    /// <summary>
    /// MyListBox2 subscribe operation
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


