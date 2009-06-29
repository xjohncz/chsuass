namespace Assistant
{
    partial class MainForm
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
            this.menuItemExit = new System.Windows.Forms.MenuItem();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.prgBar = new System.Windows.Forms.ProgressBar();
            this.lblState = new System.Windows.Forms.Label();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.btnChange = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtUserName = new System.Windows.Forms.TextBox();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.txtIP = new System.Windows.Forms.TextBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.label8 = new System.Windows.Forms.Label();
            this.lblMiddleMark = new System.Windows.Forms.Label();
            this.nudMark6 = new System.Windows.Forms.NumericUpDown();
            this.nudMark5 = new System.Windows.Forms.NumericUpDown();
            this.nudMark4 = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.nudMark3 = new System.Windows.Forms.NumericUpDown();
            this.nudMark2 = new System.Windows.Forms.NumericUpDown();
            this.nudMark1 = new System.Windows.Forms.NumericUpDown();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.txtTheme = new System.Windows.Forms.TextBox();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.lvMarks = new System.Windows.Forms.ListView();
            this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
            this.txtStudent = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.lvResults = new System.Windows.Forms.ListView();
            this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader7 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader8 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader9 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader10 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader11 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader12 = new System.Windows.Forms.ColumnHeader();
            this.tabPage3.SuspendLayout();
            this.tabPage6.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.menuItemExit);
            // 
            // menuItemExit
            // 
            this.menuItemExit.Text = "Выход";
            this.menuItemExit.Click += new System.EventHandler(this.menuItemExit_Click);
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
            // prgBar
            // 
            this.prgBar.Location = new System.Drawing.Point(4, 257);
            this.prgBar.Name = "prgBar";
            this.prgBar.Size = new System.Drawing.Size(233, 10);
            this.prgBar.Visible = false;
            // 
            // lblState
            // 
            this.lblState.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Bold);
            this.lblState.ForeColor = System.Drawing.Color.Red;
            this.lblState.Location = new System.Drawing.Point(3, 242);
            this.lblState.Name = "lblState";
            this.lblState.Size = new System.Drawing.Size(234, 12);
            this.lblState.Visible = false;
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
            this.tabPage3.Size = new System.Drawing.Size(240, 245);
            this.tabPage3.Text = "Подкл";
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
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // tabPage6
            // 
            this.tabPage6.Controls.Add(this.label8);
            this.tabPage6.Controls.Add(this.lblMiddleMark);
            this.tabPage6.Controls.Add(this.nudMark6);
            this.tabPage6.Controls.Add(this.nudMark5);
            this.tabPage6.Controls.Add(this.nudMark4);
            this.tabPage6.Controls.Add(this.label1);
            this.tabPage6.Controls.Add(this.label5);
            this.tabPage6.Controls.Add(this.label6);
            this.tabPage6.Controls.Add(this.nudMark3);
            this.tabPage6.Controls.Add(this.nudMark2);
            this.tabPage6.Controls.Add(this.nudMark1);
            this.tabPage6.Controls.Add(this.label12);
            this.tabPage6.Controls.Add(this.label11);
            this.tabPage6.Controls.Add(this.label10);
            this.tabPage6.Location = new System.Drawing.Point(0, 0);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Size = new System.Drawing.Size(232, 242);
            this.tabPage6.Text = "Баллы";
            // 
            // label8
            // 
            this.label8.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Regular);
            this.label8.Location = new System.Drawing.Point(7, 205);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(226, 40);
            this.label8.Text = "Примечание: \r\nбаллы со значение 0 не принимаются во внимание при вычислении средн" +
                "его";
            // 
            // lblMiddleMark
            // 
            this.lblMiddleMark.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.lblMiddleMark.Location = new System.Drawing.Point(7, 184);
            this.lblMiddleMark.Name = "lblMiddleMark";
            this.lblMiddleMark.Size = new System.Drawing.Size(108, 18);
            this.lblMiddleMark.Text = "Средний балл:  0";
            // 
            // nudMark6
            // 
            this.nudMark6.Location = new System.Drawing.Point(187, 153);
            this.nudMark6.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark6.Name = "nudMark6";
            this.nudMark6.Size = new System.Drawing.Size(46, 22);
            this.nudMark6.TabIndex = 86;
            this.nudMark6.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged);
            // 
            // nudMark5
            // 
            this.nudMark5.Location = new System.Drawing.Point(187, 126);
            this.nudMark5.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark5.Name = "nudMark5";
            this.nudMark5.Size = new System.Drawing.Size(46, 22);
            this.nudMark5.TabIndex = 85;
            this.nudMark5.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged);
            // 
            // nudMark4
            // 
            this.nudMark4.Location = new System.Drawing.Point(187, 98);
            this.nudMark4.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark4.Name = "nudMark4";
            this.nudMark4.Size = new System.Drawing.Size(46, 22);
            this.nudMark4.TabIndex = 84;
            this.nudMark4.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged);
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label1.Location = new System.Drawing.Point(7, 157);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(174, 18);
            this.label1.Text = "Ответы на &вопросы:";
            // 
            // label5
            // 
            this.label5.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label5.Location = new System.Drawing.Point(7, 130);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(174, 18);
            this.label5.Text = "Оформление &РПЗ:";
            // 
            // label6
            // 
            this.label6.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label6.Location = new System.Drawing.Point(7, 102);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(174, 18);
            this.label6.Text = "Пользовательский &интерфейс:";
            // 
            // nudMark3
            // 
            this.nudMark3.Location = new System.Drawing.Point(187, 69);
            this.nudMark3.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark3.Name = "nudMark3";
            this.nudMark3.Size = new System.Drawing.Size(46, 22);
            this.nudMark3.TabIndex = 77;
            this.nudMark3.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged);
            // 
            // nudMark2
            // 
            this.nudMark2.Location = new System.Drawing.Point(187, 42);
            this.nudMark2.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark2.Name = "nudMark2";
            this.nudMark2.Size = new System.Drawing.Size(46, 22);
            this.nudMark2.TabIndex = 76;
            this.nudMark2.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged);
            // 
            // nudMark1
            // 
            this.nudMark1.Location = new System.Drawing.Point(187, 14);
            this.nudMark1.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudMark1.Name = "nudMark1";
            this.nudMark1.Size = new System.Drawing.Size(46, 22);
            this.nudMark1.TabIndex = 75;
            this.nudMark1.ValueChanged += new System.EventHandler(this.nudMark1_ValueChanged);
            // 
            // label12
            // 
            this.label12.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label12.Location = new System.Drawing.Point(7, 73);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(174, 18);
            this.label12.Text = "&Проектирование и разработка:";
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label11.Location = new System.Drawing.Point(7, 46);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(174, 18);
            this.label11.Text = "Выбор &технологии и инстр.:";
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label10.Location = new System.Drawing.Point(7, 18);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(174, 18);
            this.label10.Text = "&Актуальность:";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.txtTheme);
            this.tabPage2.Location = new System.Drawing.Point(0, 0);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(232, 242);
            this.tabPage2.Text = "Тема";
            // 
            // txtTheme
            // 
            this.txtTheme.Location = new System.Drawing.Point(3, 3);
            this.txtTheme.Multiline = true;
            this.txtTheme.Name = "txtTheme";
            this.txtTheme.ReadOnly = true;
            this.txtTheme.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtTheme.Size = new System.Drawing.Size(234, 239);
            this.txtTheme.TabIndex = 58;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.lvMarks);
            this.tabPage1.Controls.Add(this.txtStudent);
            this.tabPage1.Controls.Add(this.label13);
            this.tabPage1.Controls.Add(this.label4);
            this.tabPage1.Location = new System.Drawing.Point(0, 0);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(240, 245);
            this.tabPage1.Text = "Студ";
            // 
            // lvMarks
            // 
            this.lvMarks.Columns.Add(this.columnHeader3);
            this.lvMarks.Columns.Add(this.columnHeader4);
            this.lvMarks.FullRowSelect = true;
            this.lvMarks.Location = new System.Drawing.Point(7, 59);
            this.lvMarks.Name = "lvMarks";
            this.lvMarks.Size = new System.Drawing.Size(226, 183);
            this.lvMarks.TabIndex = 67;
            this.lvMarks.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Дисциплина";
            this.columnHeader3.Width = 172;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Оценка";
            this.columnHeader4.Width = 50;
            // 
            // txtStudent
            // 
            this.txtStudent.Location = new System.Drawing.Point(3, 19);
            this.txtStudent.Name = "txtStudent";
            this.txtStudent.Size = new System.Drawing.Size(234, 21);
            this.txtStudent.TabIndex = 63;
            // 
            // label13
            // 
            this.label13.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label13.Location = new System.Drawing.Point(7, 43);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(60, 13);
            this.label13.Text = "Оценки:";
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label4.Location = new System.Drawing.Point(3, 3);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(60, 13);
            this.label4.Text = "Студент:";
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage6);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 268);
            this.tabControl1.TabIndex = 5;
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.lvResults);
            this.tabPage4.Location = new System.Drawing.Point(0, 0);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(232, 242);
            this.tabPage4.Text = "Итоги";
            // 
            // lvResults
            // 
            this.lvResults.Columns.Add(this.columnHeader5);
            this.lvResults.Columns.Add(this.columnHeader6);
            this.lvResults.Columns.Add(this.columnHeader7);
            this.lvResults.Columns.Add(this.columnHeader8);
            this.lvResults.Columns.Add(this.columnHeader9);
            this.lvResults.Columns.Add(this.columnHeader10);
            this.lvResults.Columns.Add(this.columnHeader11);
            this.lvResults.Columns.Add(this.columnHeader12);
            this.lvResults.FullRowSelect = true;
            this.lvResults.Location = new System.Drawing.Point(4, 3);
            this.lvResults.Name = "lvResults";
            this.lvResults.Size = new System.Drawing.Size(233, 239);
            this.lvResults.TabIndex = 68;
            this.lvResults.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "ФИО";
            this.columnHeader5.Width = 80;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "А";
            this.columnHeader6.Width = 20;
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "Т";
            this.columnHeader7.Width = 20;
            // 
            // columnHeader8
            // 
            this.columnHeader8.Text = "П";
            this.columnHeader8.Width = 20;
            // 
            // columnHeader9
            // 
            this.columnHeader9.Text = "И";
            this.columnHeader9.Width = 20;
            // 
            // columnHeader10
            // 
            this.columnHeader10.Text = "Р";
            this.columnHeader10.Width = 20;
            // 
            // columnHeader11
            // 
            this.columnHeader11.Text = "В";
            this.columnHeader11.Width = 20;
            // 
            // columnHeader12
            // 
            this.columnHeader12.Text = "Ср.";
            this.columnHeader12.Width = 25;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.prgBar);
            this.Controls.Add(this.lblState);
            this.Menu = this.mainMenu1;
            this.Name = "MainForm";
            this.Text = "Assistant";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabPage3.ResumeLayout(false);
            this.tabPage6.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.MenuItem menuItemExit;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ProgressBar prgBar;
        private System.Windows.Forms.Label lblState;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Button btnChange;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtUserName;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.TextBox txtIP;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.NumericUpDown nudMark3;
        private System.Windows.Forms.NumericUpDown nudMark2;
        private System.Windows.Forms.NumericUpDown nudMark1;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TextBox txtTheme;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.ListView lvMarks;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.TextBox txtStudent;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.NumericUpDown nudMark6;
        private System.Windows.Forms.NumericUpDown nudMark5;
        private System.Windows.Forms.NumericUpDown nudMark4;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ListView lvResults;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader8;
        private System.Windows.Forms.ColumnHeader columnHeader9;
        private System.Windows.Forms.ColumnHeader columnHeader10;
        private System.Windows.Forms.ColumnHeader columnHeader11;
        private System.Windows.Forms.Label lblMiddleMark;
        private System.Windows.Forms.ColumnHeader columnHeader12;
        private System.Windows.Forms.Label label8;




    }
}

