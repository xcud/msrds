namespace ZGControlTest
{
	partial class Form2
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if ( disposing && ( components != null ) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.zg1 = new ZedGraph.ZedGraphControl();
			this.SuspendLayout();
			// 
			// zg1
			// 
			this.zg1.EditButtons = System.Windows.Forms.MouseButtons.Left;
			this.zg1.EditModifierKeys = ( (System.Windows.Forms.Keys)( ( System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.None ) ) );
			this.zg1.IsAutoScrollRange = false;
			this.zg1.IsEnableHEdit = false;
			this.zg1.IsEnableHPan = true;
			this.zg1.IsEnableHZoom = true;
			this.zg1.IsEnableVEdit = false;
			this.zg1.IsEnableVPan = true;
			this.zg1.IsEnableVZoom = true;
			this.zg1.IsPrintFillPage = true;
			this.zg1.IsPrintKeepAspectRatio = true;
			this.zg1.IsScrollY2 = false;
			this.zg1.IsShowContextMenu = true;
			this.zg1.IsShowCopyMessage = true;
			this.zg1.IsShowCursorValues = false;
			this.zg1.IsShowHScrollBar = false;
			this.zg1.IsShowPointValues = false;
			this.zg1.IsShowVScrollBar = false;
			this.zg1.IsSynchronizeXAxes = false;
			this.zg1.IsSynchronizeYAxes = false;
			this.zg1.IsZoomOnMouseCenter = false;
			this.zg1.LinkButtons = System.Windows.Forms.MouseButtons.Left;
			this.zg1.LinkModifierKeys = ( (System.Windows.Forms.Keys)( ( System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.None ) ) );
			this.zg1.Location = new System.Drawing.Point( 12, 12 );
			this.zg1.Name = "zg1";
			this.zg1.PanButtons = System.Windows.Forms.MouseButtons.Left;
			this.zg1.PanButtons2 = System.Windows.Forms.MouseButtons.Middle;
			this.zg1.PanModifierKeys = ( (System.Windows.Forms.Keys)( ( System.Windows.Forms.Keys.Shift | System.Windows.Forms.Keys.None ) ) );
			this.zg1.PanModifierKeys2 = System.Windows.Forms.Keys.None;
			this.zg1.PointDateFormat = "g";
			this.zg1.PointValueFormat = "G";
			this.zg1.ScrollMaxX = 0;
			this.zg1.ScrollMaxY = 0;
			this.zg1.ScrollMaxY2 = 0;
			this.zg1.ScrollMinX = 0;
			this.zg1.ScrollMinY = 0;
			this.zg1.ScrollMinY2 = 0;
			this.zg1.Size = new System.Drawing.Size( 499, 333 );
			this.zg1.TabIndex = 0;
			this.zg1.ZoomButtons = System.Windows.Forms.MouseButtons.Left;
			this.zg1.ZoomButtons2 = System.Windows.Forms.MouseButtons.None;
			this.zg1.ZoomModifierKeys = System.Windows.Forms.Keys.None;
			this.zg1.ZoomModifierKeys2 = System.Windows.Forms.Keys.None;
			this.zg1.ZoomStepFraction = 0.1;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 523, 357 );
			this.Controls.Add( this.zg1 );
			this.Name = "Form1";
			this.Text = "Form1";
			this.Resize += new System.EventHandler( this.Form1_Resize );
			this.Load += new System.EventHandler( this.Form1_Load );
			this.ResumeLayout( false );

		}

		#endregion

		private ZedGraph.ZedGraphControl zg1;
	}
}

