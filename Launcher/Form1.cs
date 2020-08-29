using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Launcher
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            Log("Launcher started.");
        }

        public void Log(string msg)
        {
            richTextBox1.AppendText('[' + DateTime.Now.ToLongTimeString() + "]: " + msg + "\r\n");
        }
    }
}
