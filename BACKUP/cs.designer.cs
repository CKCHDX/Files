namespace SimpleCalculator
{
    partial class Form1
    {
        private System.ComponentModel.IContainer components = null;
        private TextBox textBox_Result;
        private Button button0, button1, button2, button3, button4, button5, button6, button7, button8, button9;
        private Button buttonAdd, buttonSub, buttonMul, buttonDiv, buttonEqual, buttonC, buttonCE;

        private void InitializeComponent()
        {
            this.textBox_Result = new TextBox();
            this.button0 = new Button();
            this.button1 = new Button();
            this.button2 = new Button();
            this.button3 = new Button();
            this.button4 = new Button();
            this.button5 = new Button();
            this.button6 = new Button();
            this.button7 = new Button();
            this.button8 = new Button();
            this.button9 = new Button();
            this.buttonAdd = new Button();
            this.buttonSub = new Button();
            this.buttonMul = new Button();
            this.buttonDiv = new Button();
            this.buttonEqual = new Button();
            this.buttonC = new Button();
            this.buttonCE = new Button();
            this.SuspendLayout();
            
            // textBox_Result
            this.textBox_Result.Location = new System.Drawing.Point(12, 12);
            this.textBox_Result.Name = "textBox_Result";
            this.textBox_Result.Size = new System.Drawing.Size(260, 20);
            this.textBox_Result.Text = "0";
            this.textBox_Result.TextAlign = HorizontalAlignment.Right;

            // Digit buttons
            Button[] digits = { button0, button1, button2, button3, button4, button5, button6, button7, button8, button9 };
            for (int i = 0; i <= 9; i++)
            {
                digits[i].Text = i.ToString();
                digits[i].Size = new System.Drawing.Size(50, 50);
                digits[i].Click += new EventHandler(this.button_Click);
            }

            // Arrange digit buttons manually
            button1.Location = new System.Drawing.Point(12, 50);
            button2.Location = new System.Drawing.Point(68, 50);
            button3.Location = new System.Drawing.Point(124, 50);

            button4.Location = new System.Drawing.Point(12, 106);
            button5.Location = new System.Drawing.Point(68, 106);
            button6.Location = new System.Drawing.Point(124, 106);

            button7.Location = new System.Drawing.Point(12, 162);
            button8.Location = new System.Drawing.Point(68, 162);
            button9.Location = new System.Drawing.Point(124, 162);

            button0.Location = new System.Drawing.Point(68, 218);

            // Operator buttons
            buttonAdd.Text = "+";
            buttonAdd.Size = new System.Drawing.Size(50, 50);
            buttonAdd.Location = new System.Drawing.Point(180, 50);
            buttonAdd.Click += new EventHandler(this.operator_Click);

            buttonSub.Text = "-";
            buttonSub.Size = new System.Drawing.Size(50, 50);
            buttonSub.Location = new System.Drawing.Point(180, 106);
            buttonSub.Click += new EventHandler(this.operator_Click);

            buttonMul.Text = "*";
            buttonMul.Size = new System.Drawing.Size(50, 50);
            buttonMul.Location = new System.Drawing.Point(180, 162);
            buttonMul.Click += new EventHandler(this.operator_Click);

            buttonDiv.Text = "/";
            buttonDiv.Size = new System.Drawing.Size(50, 50);
            buttonDiv.Location = new System.Drawing.Point(180, 218);
            buttonDiv.Click += new EventHandler(this.operator_Click);

            buttonEqual.Text = "=";
            buttonEqual.Size = new System.Drawing.Size(50, 106);
            buttonEqual.Location = new System.Drawing.Point(236, 162);
            buttonEqual.Click += new EventHandler(this.buttonEqual_Click);

            buttonC.Text = "C";
            buttonC.Size = new System.Drawing.Size(50, 50);
            buttonC.Location = new System.Drawing.Point(236, 50);
            buttonC.Click += new EventHandler(this.buttonC_Click);

            buttonCE.Text = "CE";
            buttonCE.Size = new System.Drawing.Size(50, 50);
            buttonCE.Location = new System.Drawing.Point(236, 106);
            buttonCE.Click += new EventHandler(this.buttonCE_Click);

            // Form1
            this.ClientSize = new System.Drawing.Size(300, 280);
            this.Controls.Add(this.textBox_Result);
            this.Controls.AddRange(digits);
            this.Controls.Add(this.buttonAdd);
            this.Controls.Add(this.buttonSub);
            this.Controls.Add(this.buttonMul);
            this.Controls.Add(this.buttonDiv);
            this.Controls.Add(this.buttonEqual);
            this.Controls.Add(this.buttonC);
            this.Controls.Add(this.buttonCE);
            this.Name = "Form1";
            this.Text = "Simple Calculator";
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}
