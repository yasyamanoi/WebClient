using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace TestXML
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try {
                if (textBox1.Text != "")
                {

                    var xmlDoc = new XmlDocument();
                    xmlDoc.Load(textBox1.Text);
                    MessageBox.Show("XMLファイルの検証に成功しました。");
                }
                else
                {
                    MessageBox.Show("XMLファイル名を入力してください。");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("XMLのロードに失敗しました。" + ex.ToString());
            }
        }
    }
}
