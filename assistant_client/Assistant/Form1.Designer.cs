namespace Assistant
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.MainMenu mainMenu1;

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
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.lvMarks = new System.Windows.Forms.ListView();
            this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
            this.label13 = new System.Windows.Forms.Label();
            this.txtStudent = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.label7 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtUserName = new System.Windows.Forms.TextBox();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.txtIP = new System.Windows.Forms.TextBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.nudMark1 = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.txtQuestion1 = new System.Windows.Forms.TextBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.nudMark2 = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.txtQuestion2 = new System.Windows.Forms.TextBox();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.label9 = new System.Windows.Forms.Label();
            this.nudMark3 = new System.Windows.Forms.NumericUpDown();
            this.txtQuestion3 = new System.Windows.Forms.TextBox();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.nudMark33 = new System.Windows.Forms.NumericUpDown();
            this.nudMark22 = new System.Windows.Forms.NumericUpDown();
            this.nudMark11 = new System.Windows.Forms.NumericUpDown();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.nudResultMark = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.btnChange = new System.Windows.Forms.Button();
            this.tabPage1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.tabPage6.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.menuItem2);
            this.mainMenu1.MenuItems.Add(this.menuItem1);
            // 
            // menuItem2
            // 
            this.menuItem2.Text = "Выход";
            this.menuItem2.Click += new System.EventHandler(this.menuItem2_Click);
            // 
            // menuItem1
            // 
            this.menuItem1.Text = "Таймер";
            this.menuItem1.Click += new System.EventHandler(this.menuItem1_Click);
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "ФИО";
            this.columnHeader2.Width = 200;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "ID";
            this.columnHeader1.Width = 30;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.lvMarks);
            this.tabPage1.Controls.Add(this.label13);
            this.tabPage1.Controls.Add(this.txtStudent);
            this.tabPage1.Controls.Add(this.label4);
            this.tabPage1.Location = new System.Drawing.Point(0, 0);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(240, 242);
            this.tabPage1.Text = "Студ";
            // 
            // lvMarks
            // 
            this.lvMarks.Columns.Add(this.columnHeader3);
            this.lvMarks.Columns.Add(this.columnHeader4);
            this.lvMarks.FullRowSelect = true;
            this.lvMarks.Location = new System.Drawing.Point(7, 59);
            this.lvMarks.Name = "lvMarks";
            this.lvMarks.Size = new System.Drawing.Size(226, 180);
            this.lvMarks.TabIndex = 67;
            this.lvMarks.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Дисциплина";
            this.columnHeader3.Width = 180;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Оценка";
            this.columnHeader4.Width = 40;
            // 
            // label13
            // 
            this.label13.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label13.Location = new System.Drawing.Point(7, 43);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(60, 13);
            this.label13.Text = "Оценки:";
            // 
            // txtStudent
            // 
            this.txtStudent.Location = new System.Drawing.Point(3, 19);
            this.txtStudent.Name = "txtStudent";
            this.txtStudent.Size = new System.Drawing.Size(234, 21);
            this.txtStudent.TabIndex = 63;
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label4.Location = new System.Drawing.Point(3, 3);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(60, 13);
            this.label4.Text = "Студент:";
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.btnChange);
            this.tabPage3.Controls.Add(this.label7);
            this.tabPage3.Controls.Add(this.label3);
            this.tabPage3.Controls.Add(this.label2);
            this.tabPage3.Controls.Add(this.txtUserName);
            this.tabPage3.Controls.Add(this.txtPort);
            this.tabPage3.Controls.Add(this.txtIP);
            this.tabPage3.Controls.Add(this.btnConnect);
            this.tabPage3.Location = new System.Drawing.Point(0, 0);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(240, 242);
            this.tabPage3.Text = "Подкл";
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(7, 62);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(68, 20);
            this.label7.Text = "Имя польз:";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(8, 35);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(68, 20);
            this.label3.Text = "Порт:";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(8, 7);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(68, 20);
            this.label2.Text = "IP-адрес:";
            // 
            // txtUserName
            // 
            this.txtUserName.Enabled = false;
            this.txtUserName.Location = new System.Drawing.Point(82, 61);
            this.txtUserName.Name = "txtUserName";
            this.txtUserName.Size = new System.Drawing.Size(146, 21);
            this.txtUserName.TabIndex = 3;
            // 
            // txtPort
            // 
            this.txtPort.Enabled = false;
            this.txtPort.Location = new System.Drawing.Point(82, 34);
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(146, 21);
            this.txtPort.TabIndex = 1;
            // 
            // txtIP
            // 
            this.txtIP.Enabled = false;
            this.txtIP.Location = new System.Drawing.Point(82, 7);
            this.txtIP.Name = "txtIP";
            this.txtIP.Size = new System.Drawing.Size(147, 21);
            this.txtIP.TabIndex = 0;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(116, 115);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(112, 20);
            this.btnConnect.TabIndex = 2;
            this.btnConnect.Text = "Подключиться";
            this.btnConnect.Click += new System.EventHandler(this.button1_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Controls.Add(this.tabPage6);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 265);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.nudMark1);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this.txtQuestion1);
            this.tabPage2.Location = new System.Drawing.Point(0, 0);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(232, 239);
            this.tabPage2.Text = "Вопр1";
            // 
            // nudMark1
            // 
            this.nudMark1.Location = new System.Drawing.Point(73, 204);
            this.nudMark1.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark1.Name = "nudMark1";
            this.nudMark1.Size = new System.Drawing.Size(46, 22);
            this.nudMark1.TabIndex = 63;
            this.nudMark1.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged_2);
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label1.Location = new System.Drawing.Point(7, 208);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 18);
            this.label1.Text = "Оценка:";
            // 
            // txtQuestion1
            // 
            this.txtQuestion1.Location = new System.Drawing.Point(3, 3);
            this.txtQuestion1.Multiline = true;
            this.txtQuestion1.Name = "txtQuestion1";
            this.txtQuestion1.ReadOnly = true;
            this.txtQuestion1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtQuestion1.Size = new System.Drawing.Size(234, 195);
            this.txtQuestion1.TabIndex = 58;
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.nudMark2);
            this.tabPage4.Controls.Add(this.label6);
            this.tabPage4.Controls.Add(this.txtQuestion2);
            this.tabPage4.Location = new System.Drawing.Point(0, 0);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(232, 239);
            this.tabPage4.Text = "Вопр2";
            // 
            // nudMark2
            // 
            this.nudMark2.Location = new System.Drawing.Point(73, 204);
            this.nudMark2.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark2.Name = "nudMark2";
            this.nudMark2.Size = new System.Drawing.Size(46, 22);
            this.nudMark2.TabIndex = 63;
            this.nudMark2.ValueChanged += new System.EventHandler(this.nudMark2_ValueChanged_2);
            // 
            // label6
            // 
            this.label6.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label6.Location = new System.Drawing.Point(7, 208);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(60, 18);
            this.label6.Text = "Оценка:";
            // 
            // txtQuestion2
            // 
            this.txtQuestion2.Location = new System.Drawing.Point(3, 3);
            this.txtQuestion2.Multiline = true;
            this.txtQuestion2.Name = "txtQuestion2";
            this.txtQuestion2.ReadOnly = true;
            this.txtQuestion2.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtQuestion2.Size = new System.Drawing.Size(234, 195);
            this.txtQuestion2.TabIndex = 61;
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.label9);
            this.tabPage5.Controls.Add(this.nudMark3);
            this.tabPage5.Controls.Add(this.txtQuestion3);
            this.tabPage5.Location = new System.Drawing.Point(0, 0);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(232, 239);
            this.tabPage5.Text = "Вопр3";
            // 
            // label9
            // 
            this.label9.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label9.Location = new System.Drawing.Point(7, 208);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(60, 18);
            this.label9.Text = "Оценка:";
            // 
            // nudMark3
            // 
            this.nudMark3.Location = new System.Drawing.Point(73, 204);
            this.nudMark3.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark3.Name = "nudMark3";
            this.nudMark3.Size = new System.Drawing.Size(46, 22);
            this.nudMark3.TabIndex = 62;
            this.nudMark3.ValueChanged += new System.EventHandler(this.nudMark3_ValueChanged_1);
            // 
            // txtQuestion3
            // 
            this.txtQuestion3.Location = new System.Drawing.Point(3, 3);
            this.txtQuestion3.Multiline = true;
            this.txtQuestion3.Name = "txtQuestion3";
            this.txtQuestion3.ReadOnly = true;
            this.txtQuestion3.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtQuestion3.Size = new System.Drawing.Size(234, 195);
            this.txtQuestion3.TabIndex = 61;
            // 
            // tabPage6
            // 
            this.tabPage6.Controls.Add(this.nudMark33);
            this.tabPage6.Controls.Add(this.nudMark22);
            this.tabPage6.Controls.Add(this.nudMark11);
            this.tabPage6.Controls.Add(this.label12);
            this.tabPage6.Controls.Add(this.label11);
            this.tabPage6.Controls.Add(this.label10);
            this.tabPage6.Controls.Add(this.nudResultMark);
            this.tabPage6.Controls.Add(this.label5);
            this.tabPage6.Location = new System.Drawing.Point(0, 0);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Size = new System.Drawing.Size(232, 239);
            this.tabPage6.Text = "Итоги";
            // 
            // nudMark33
            // 
            this.nudMark33.Location = new System.Drawing.Point(89, 70);
            this.nudMark33.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark33.Name = "nudMark33";
            this.nudMark33.Size = new System.Drawing.Size(46, 22);
            this.nudMark33.TabIndex = 77;
            this.nudMark33.ValueChanged += new System.EventHandler(this.nudMark33_ValueChanged);
            // 
            // nudMark22
            // 
            this.nudMark22.Location = new System.Drawing.Point(89, 42);
            this.nudMark22.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark22.Name = "nudMark22";
            this.nudMark22.Size = new System.Drawing.Size(46, 22);
            this.nudMark22.TabIndex = 76;
            this.nudMark22.ValueChanged += new System.EventHandler(this.nudMark22_ValueChanged);
            // 
            // nudMark11
            // 
            this.nudMark11.Location = new System.Drawing.Point(89, 14);
            this.nudMark11.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark11.Name = "nudMark11";
            this.nudMark11.Size = new System.Drawing.Size(46, 22);
            this.nudMark11.TabIndex = 75;
            this.nudMark11.ValueChanged += new System.EventHandler(this.nudMark11_ValueChanged_1);
            // 
            // label12
            // 
            this.label12.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label12.Location = new System.Drawing.Point(7, 73);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(76, 18);
            this.label12.Text = "Вопрос №3:";
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label11.Location = new System.Drawing.Point(7, 46);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(76, 18);
            this.label11.Text = "Вопрос №2:";
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label10.Location = new System.Drawing.Point(7, 18);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(76, 18);
            this.label10.Text = "Вопрос №1:";
            // 
            // nudResultMark
            // 
            this.nudResultMark.Location = new System.Drawing.Point(89, 101);
            this.nudResultMark.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudResultMark.Name = "nudResultMark";
            this.nudResultMark.Size = new System.Drawing.Size(46, 22);
            this.nudResultMark.TabIndex = 64;
            // 
            // label5
            // 
            this.label5.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label5.Location = new System.Drawing.Point(7, 105);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(60, 18);
            this.label5.Text = "Результат:";
            // 
            // btnChange
            // 
            this.btnChange.Location = new System.Drawing.Point(116, 89);
            this.btnChange.Name = "btnChange";
            this.btnChange.Size = new System.Drawing.Size(112, 20);
            this.btnChange.TabIndex = 4;
            this.btnChange.Text = "Изменить";
            this.btnChange.Click += new System.EventHandler(this.btnChange_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.tabControl1);
            this.Menu = this.mainMenu1;
            this.Name = "Form1";
            this.Text = "Assistant";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabPage1.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.tabPage6.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TextBox txtStudent;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtUserName;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.TextBox txtIP;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtQuestion1;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtQuestion2;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown nudMark3;
        private System.Windows.Forms.TextBox txtQuestion3;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown nudResultMark;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.NumericUpDown nudMark33;
        private System.Windows.Forms.NumericUpDown nudMark22;
        private System.Windows.Forms.NumericUpDown nudMark11;
        private System.Windows.Forms.NumericUpDown nudMark1;
        private System.Windows.Forms.NumericUpDown nudMark2;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ListView lvMarks;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.Button btnChange;




    }
}

