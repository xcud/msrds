namespace OpenCVForm
{
    partial class Form3
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
            this.pictureBoxIpl1 = new OpenCvSharp.UserInterface.PictureBoxIpl();
            this.pictureBoxIpl2 = new OpenCvSharp.UserInterface.PictureBoxIpl();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxIpl1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxIpl2)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBoxIpl1
            // 
            this.pictureBoxIpl1.Location = new System.Drawing.Point(36, 270);
            this.pictureBoxIpl1.Name = "pictureBoxIpl1";
            this.pictureBoxIpl1.Size = new System.Drawing.Size(320, 240);
            this.pictureBoxIpl1.TabIndex = 1;
            this.pictureBoxIpl1.TabStop = false;
            // 
            // pictureBoxIpl2
            // 
            this.pictureBoxIpl2.Location = new System.Drawing.Point(36, 12);
            this.pictureBoxIpl2.Name = "pictureBoxIpl2";
            this.pictureBoxIpl2.Size = new System.Drawing.Size(320, 240);
            this.pictureBoxIpl2.TabIndex = 2;
            this.pictureBoxIpl2.TabStop = false;
            // 
            // Form3
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(480, 687);
            this.Controls.Add(this.pictureBoxIpl2);
            this.Controls.Add(this.pictureBoxIpl1);
            this.Name = "Form3";
            this.Text = "Form3";
            this.Load += new System.EventHandler(this.Form3_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxIpl1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxIpl2)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public OpenCvSharp.UserInterface.PictureBoxIpl pictureBoxIpl1;
        public OpenCvSharp.UserInterface.PictureBoxIpl pictureBoxIpl2;
    }
}