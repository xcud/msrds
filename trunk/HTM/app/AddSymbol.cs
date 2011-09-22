using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace app
{
    public partial class AddSymbol : Form
    {
        private List<SymbolInfo> _SymbolInfoList;

        public AddSymbol()
        {
            InitializeComponent();


        }

        public AddSymbol(List<SymbolInfo> _SymbolInfoList)
        {
            // TODO: Complete member initialization
            this._SymbolInfoList = _SymbolInfoList;

            InitializeComponent();

        }

        private void AddSymbol_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            
        }
    }
}
