namespace HopfieldRecognizer
{
    partial class frmAddDistortion
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.nudDistortionLevel = new System.Windows.Forms.NumericUpDown();
            this.lblPercent = new System.Windows.Forms.Label();
            this.btnSetDistortionLevel = new System.Windows.Forms.Button();
            this.lblDistortionLevelSelectOffer = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.nudDistortionLevel)).BeginInit();
            this.SuspendLayout();
            // 
            // nudDistortionLevel
            // 
            this.nudDistortionLevel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.nudDistortionLevel.Location = new System.Drawing.Point(59, 31);
            this.nudDistortionLevel.Name = "nudDistortionLevel";
            this.nudDistortionLevel.Size = new System.Drawing.Size(73, 20);
            this.nudDistortionLevel.TabIndex = 0;
            this.nudDistortionLevel.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nudDistortionLevel.ValueChanged += new System.EventHandler(this.nudDistortionLevel_ValueChanged);
            this.nudDistortionLevel.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.nudDistortionLevel_KeyPress);
            // 
            // lblPercent
            // 
            this.lblPercent.AutoSize = true;
            this.lblPercent.Location = new System.Drawing.Point(138, 33);
            this.lblPercent.Name = "lblPercent";
            this.lblPercent.Size = new System.Drawing.Size(15, 13);
            this.lblPercent.TabIndex = 1;
            this.lblPercent.Text = "%";
            // 
            // btnSetDistortionLevel
            // 
            this.btnSetDistortionLevel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSetDistortionLevel.Location = new System.Drawing.Point(12, 57);
            this.btnSetDistortionLevel.Name = "btnSetDistortionLevel";
            this.btnSetDistortionLevel.Size = new System.Drawing.Size(189, 23);
            this.btnSetDistortionLevel.TabIndex = 2;
            this.btnSetDistortionLevel.Text = "Set distortion level";
            this.btnSetDistortionLevel.UseVisualStyleBackColor = true;
            this.btnSetDistortionLevel.Click += new System.EventHandler(this.btnSetDistortionLevel_Click);
            // 
            // lblDistortionLevelSelectOffer
            // 
            this.lblDistortionLevelSelectOffer.AutoSize = true;
            this.lblDistortionLevelSelectOffer.Location = new System.Drawing.Point(21, 8);
            this.lblDistortionLevelSelectOffer.Name = "lblDistortionLevelSelectOffer";
            this.lblDistortionLevelSelectOffer.Size = new System.Drawing.Size(171, 13);
            this.lblDistortionLevelSelectOffer.TabIndex = 1;
            this.lblDistortionLevelSelectOffer.Text = "Please define the level of distortion";
            // 
            // frmAddDistortion
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(213, 82);
            this.Controls.Add(this.btnSetDistortionLevel);
            this.Controls.Add(this.lblDistortionLevelSelectOffer);
            this.Controls.Add(this.lblPercent);
            this.Controls.Add(this.nudDistortionLevel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "frmAddDistortion";
            this.Opacity = 0.9;
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Distortion Level";
            ((System.ComponentModel.ISupportInitialize)(this.nudDistortionLevel)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.NumericUpDown nudDistortionLevel;
        private System.Windows.Forms.Label lblPercent;
        private System.Windows.Forms.Button btnSetDistortionLevel;
        private System.Windows.Forms.Label lblDistortionLevelSelectOffer;
    }
}