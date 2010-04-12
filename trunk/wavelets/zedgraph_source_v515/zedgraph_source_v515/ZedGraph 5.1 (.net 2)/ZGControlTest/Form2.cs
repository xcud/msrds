//============================================================================
//ZedGraph demo code
//The code contained in this file (only) is released into the public domain, so you
//can copy it into your project without any license encumbrance.  Note that
//the actual ZedGraph library code is licensed under the LGPL, which is not
//public domain.
//
//This file is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using ZedGraph;

namespace ZGControlTest
{
	public partial class Form2 : Form
	{
		public Form2()
		{
			InitializeComponent();
		}

		private void Form1_Load( object sender, EventArgs e )
		{
			// Get a reference to the GraphPane instance in the ZedGraphControl
			GraphPane myPane = zg1.GraphPane;

			// Set the titles and axis labels
			myPane.Title.Text = "Demonstration of Dual Y Graph";
			myPane.XAxis.Title.Text = "Time, Days";
			myPane.YAxis.Title.Text = "Parameter A";
			myPane.Y2Axis.Title.Text = "Parameter B";

			// Make up some data points based on the Sine function
			PointPairList list = new PointPairList();
			PointPairList list2 = new PointPairList();
			for ( double i = -2; i < 2; i+=0.1 )
			{
				double x = (double)i * 5.0;
				//double y = Math.Sin( (double)i * Math.PI / 15.0 ) * 16.0;
               
                double y = ((2 / (1 + Math.Exp(  x))) - 1);
				double y2 = y * 13.5;
				list.Add( x, y );
				list2.Add( x, y2 );
			}

			// Generate a red curve with diamond symbols, and "Alpha" in the legend
			LineItem myCurve = myPane.AddCurve( "Alpha",
				list, Color.Red, SymbolType.Diamond );
			// Fill the symbols with white
			myCurve.Symbol.Fill = new Fill( Color.White );

			// Generate a blue curve with circle symbols, and "Beta" in the legend
			myCurve = myPane.AddCurve( "Beta",
				list2, Color.Blue, SymbolType.Circle );
			// Fill the symbols with white
			myCurve.Symbol.Fill = new Fill( Color.White );
			// Associate this curve with the Y2 axis
			myCurve.IsY2Axis = true;

			// Show the x axis grid
			myPane.XAxis.MajorGrid.IsVisible = true;

			// Make the Y axis scale red
			myPane.YAxis.Scale.FontSpec.FontColor = Color.Red;
			myPane.YAxis.Title.FontSpec.FontColor = Color.Red;
			// turn off the opposite tics so the Y tics don't show up on the Y2 axis
			myPane.YAxis.MajorTic.IsOpposite = false;
			myPane.YAxis.MinorTic.IsOpposite = false;
			// Don't display the Y zero line
			myPane.YAxis.MajorGrid.IsZeroLine = false;
			// Align the Y axis labels so they are flush to the axis
			myPane.YAxis.Scale.Align = AlignP.Inside;
			// Manually set the axis range
			myPane.YAxis.Scale.Min = -30;
			myPane.YAxis.Scale.Max = 30;

			// Enable the Y2 axis display
			myPane.Y2Axis.IsVisible = true;
			// Make the Y2 axis scale blue
			myPane.Y2Axis.Scale.FontSpec.FontColor = Color.Blue;
			myPane.Y2Axis.Title.FontSpec.FontColor = Color.Blue;
			// turn off the opposite tics so the Y2 tics don't show up on the Y axis
			myPane.Y2Axis.MajorTic.IsOpposite = false;
			myPane.Y2Axis.MinorTic.IsOpposite = false;
			// Display the Y2 axis grid lines
			myPane.Y2Axis.MajorGrid.IsVisible = true;
			// Align the Y2 axis labels so they are flush to the axis
			myPane.Y2Axis.Scale.Align = AlignP.Inside;

			// Fill the axis background with a gradient
			myPane.Chart.Fill = new Fill( Color.White, Color.LightGray, 45.0f );

			// Add a text box with instructions
			TextObj text = new TextObj(
				"Zoom: left mouse & drag\nPan: middle mouse & drag\nContext Menu: right mouse",
				0.05f, 0.95f, CoordType.ChartFraction, AlignH.Left, AlignV.Bottom );
			text.FontSpec.StringAlignment = StringAlignment.Near;
			myPane.GraphObjList.Add( text );

			// Enable scrollbars if needed
			zg1.IsShowHScrollBar = true;
			zg1.IsShowVScrollBar = true;
			zg1.IsAutoScrollRange = true;
			zg1.IsScrollY2 = true;

			// OPTIONAL: Show tooltips when the mouse hovers over a point
			zg1.IsShowPointValues = true;
			zg1.PointValueEvent += new ZedGraphControl.PointValueHandler( MyPointValueHandler );

			// OPTIONAL: Add a custom context menu item
			zg1.ContextMenuBuilder += new ZedGraphControl.ContextMenuBuilderEventHandler(
							MyContextMenuBuilder );

			// OPTIONAL: Handle the Zoom Event
			zg1.ZoomEvent += new ZedGraphControl.ZoomEventHandler( MyZoomEvent );

			// Size the control to fit the window
			SetSize();

			// Tell ZedGraph to calculate the axis ranges
			// Note that you MUST call this after enabling IsAutoScrollRange, since AxisChange() sets
			// up the proper scrolling parameters
			zg1.AxisChange();
			// Make sure the Graph gets redrawn
			zg1.Invalidate();
		}

		/// <summary>
		/// On resize action, resize the ZedGraphControl to fill most of the Form, with a small
		/// margin around the outside
		/// </summary>
		private void Form1_Resize( object sender, EventArgs e )
		{
			SetSize();
		}

		private void SetSize()
		{
			zg1.Location = new Point( 10, 10 );
			// Leave a small margin around the outside of the control
			zg1.Size = new Size( this.ClientRectangle.Width - 20,
					this.ClientRectangle.Height - 20 );
		}

		/// <summary>
		/// Display customized tooltips when the mouse hovers over a point
		/// </summary>
		private string MyPointValueHandler( ZedGraphControl control, GraphPane pane,
						CurveItem curve, int iPt )
		{
			// Get the PointPair that is under the mouse
			PointPair pt = curve[iPt];

			return curve.Label.Text + " is " + pt.Y.ToString( "f2" ) + " units at " + pt.X.ToString( "f1" ) + " days";
		}

		/// <summary>
		/// Customize the context menu by adding a new item to the end of the menu
		/// </summary>
		private void MyContextMenuBuilder( ZedGraphControl control, ContextMenuStrip menuStrip,
						Point mousePt, ZedGraphControl.ContextMenuObjectState objState )
		{
			ToolStripMenuItem item = new ToolStripMenuItem();
			item.Name = "add-beta";
			item.Tag = "add-beta";
			item.Text = "Add a new Beta Point";
			item.Click += new System.EventHandler( AddBetaPoint );

			menuStrip.Items.Add( item );
		}

		/// <summary>
		/// Handle the "Add New Beta Point" context menu item.  This finds the curve with
		/// the CurveItem.Label = "Beta", and adds a new point to it.
		/// </summary>
		private void AddBetaPoint( object sender, EventArgs args )
		{
			// Get a reference to the "Beta" curve IPointListEdit
			IPointListEdit ip = zg1.GraphPane.CurveList["Beta"].Points as IPointListEdit;
			if ( ip != null )
			{
				double x = ip.Count * 5.0;
				double y = Math.Sin( ip.Count * Math.PI / 15.0 ) * 16.0 * 13.5;
				ip.Add( x, y );
				zg1.AxisChange();
				zg1.Refresh();
			}
		}

		// Respond to a Zoom Event
		private void MyZoomEvent( ZedGraphControl control, ZoomState oldState,
					ZoomState newState )
		{
			// Here we get notification everytime the user zooms
		}


	}
}