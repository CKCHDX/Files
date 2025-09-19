using System;
using System.Windows.Forms;

namespace SimpleCalculator
{
    public partial class Form1 : Form
    {
        double value = 0;
        string operation = "";
        bool operationPressed = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void button_Click(object sender, EventArgs e)
        {
            if ((textBox_Result.Text == "0") || operationPressed)
                textBox_Result.Clear();

            operationPressed = false;
            Button b = (Button)sender;
            textBox_Result.Text = textBox_Result.Text + b.Text;
        }

        private void operator_Click(object sender, EventArgs e)
        {
            Button b = (Button)sender;
            operation = b.Text;
            value = double.Parse(textBox_Result.Text);
            operationPressed = true;
        }

        private void buttonCE_Click(object sender, EventArgs e)
        {
            textBox_Result.Text = "0";
            value = 0;
        }

        private void buttonC_Click(object sender, EventArgs e)
        {
            textBox_Result.Text = "0";
        }

        private void buttonEqual_Click(object sender, EventArgs e)
        {
            double secondValue = double.Parse(textBox_Result.Text);
            switch (operation)
            {
                case "+":
                    textBox_Result.Text = (value + secondValue).ToString();
                    break;
                case "-":
                    textBox_Result.Text = (value - secondValue).ToString();
                    break;
                case "*":
                    textBox_Result.Text = (value * secondValue).ToString();
                    break;
                case "/":
                    if (secondValue != 0)
                        textBox_Result.Text = (value / secondValue).ToString();
                    else
                        textBox_Result.Text = "Error";
                    break;
            }
            operation = "";
        }
    }
}
