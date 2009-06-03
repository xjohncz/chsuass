using System;

using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Threading;
using System.Media;


namespace Assistant
{
    public partial class MainForm : Form
    {

        private string app_path;
        private string config_xml;
        private string students_xml;
        private string cards_xml;

        private NetworkManager netManager;

        private System.Threading.Timer timer;
        private int timerCount = 10;

        private SoundPlayer sndPlayer = null;

        private delegate void UpdateHeaderHandler(Student.StudentUpdate stCard);

        public void StartTimer(Student.StudentUpdate stCard)
        {
            sndPlayer.Play();
            SetProgressBarMax(10);
            TimerCallback timerDelegate = new TimerCallback(timerTick);
            timer = new System.Threading.Timer(timerDelegate, stCard, 0, 1000);
        }

        private void timerTick(Object state)
        {
            Student.StudentUpdate stCard = (Student.StudentUpdate)state;
            this.Invoke(new UpdateHeaderHandler(this.UpdateHeader), new Object[] { stCard });
        }

        private void UpdateHeader(Student.StudentUpdate stCard)
        {
            if (timerCount > 0)
            {
                lblState.Text = "Сохр. баллов через: " + timerCount.ToString() + " сек!";
                UpdateProgressBar(10 - timerCount);

                timerCount--;
            }
            else
            {
                timer.Dispose();

                lblState.Text = "Сохр. баллов!";
                UpdateProgressBar(10);

                timerCount = 10;

                SaveStudent(stCard.student);
                UpdateStudentInfo(stCard.newStudent, stCard.card);
            
            }
        }

        public void SetProgressBarMax(int maxValue)
        {
            prgBar.Maximum = maxValue;
            ShowStatus(true);
        }

        public void UpdateProgressBar(int value)
        {
            prgBar.Value = value;
            if(prgBar.Value == prgBar.Maximum) 
            {
                ShowStatus(false);
            }
        }

        public void SetLabelState(string state)
        {
            lblState.Text = state;
        }

        public void ShowReadBodyResult(int opcode)
        {
            switch (opcode)
            {
                case AssistantProtocol.OpcodeUserNotFound:
                    MessageBox.Show("Пользователь не найден!\nПодключение будет разорвано.", "Ошибка аутентификации", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    return;

                case AssistantProtocol.OpcodeAccessGranted:
                    MessageBox.Show("Соединение успешно установлено.");
                    break;

                case AssistantProtocol.OpcodeByeMsg:
                    MessageBox.Show("Сервер разорвал соединение");
                    return;

                default:
                    MessageBox.Show("Неизвестный пакет!\nПодключение будет разорвано.", "Ошибка сетевого протокола", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    return;
            }
        }

        private void ShowStatus(bool enabled)
        {
            if (enabled)
            {
                txtQuestion1.Height = 182;
                txtQuestion2.Height = 182;
                txtQuestion3.Height = 182;
                lblMark1.Top = 191;
                lblMark2.Top = 191;
                lblMark3.Top = 191;
                nudMark1.Top = 191;
                nudMark2.Top = 191;
                nudMark3.Top = 191;
                lvMarks.Height = 154;                
                tabControl1.Height = 239;

                lblState.Visible = true;
                prgBar.Visible = true;
            }
            else
            {
                txtQuestion1.Height = 211;
                txtQuestion2.Height = 211;
                txtQuestion3.Height = 211;
                lblMark1.Top = 220;
                lblMark2.Top = 220;
                lblMark3.Top = 220;
                nudMark1.Top = 220;
                nudMark2.Top = 220;
                nudMark3.Top = 220;
                lvMarks.Height = 183;
                tabControl1.Height = 268;

                lblState.Visible = false;
                prgBar.Visible = false;
            }
        }

        public void UpdateStudentInfo(Student st, Card card) 
        {
            //Student st = stCard.newStudent;
            //Card card = stCard.card;

            string[] strList = card.CardContents.Split('\n');
            string ticketNum = card.CardNumber.ToString();
            string question1 = strList[0];
            string question2 = strList[1];
            string question3 = strList[2];

            txtStudent.Text = st.FIO;

            lvMarks.Items.Clear();
            nudMark1.Value = st.Mark1;
            nudMark2.Value = st.Mark2;
            nudMark3.Value = st.Mark3;
            nudMark11.Value = st.Mark1;
            nudMark22.Value = st.Mark2;
            nudMark33.Value = st.Mark3;
            nudResultMark.Value = st.ResultMark;            

            foreach (Student.SubjectMark mark in st.subjectMarks)
            {
                ListViewItem item = new ListViewItem(mark.subject);
                item.SubItems.Add(mark.mark.ToString());
                lvMarks.Items.Add(item);
            }

            txtQuestion1.Text = "Билет №" + ticketNum.ToString() + "\r\n" + question1;
            txtQuestion2.Text = "Билет №" + ticketNum.ToString() + "\r\n" + question2;
            txtQuestion3.Text = "Билет №" + ticketNum.ToString() + "\r\n" + question3;
        }

        public void SaveStudent(Student st)
        {
            st.Mark1 = Convert.ToInt32(nudMark1.Value);
            st.Mark2 = Convert.ToInt32(nudMark2.Value);
            st.Mark3 = Convert.ToInt32(nudMark3.Value);
            st.ResultMark = Convert.ToInt32(nudResultMark.Value);

            XMLFileManager.SaveStudent(st, students_xml);
        }

        public MainForm()
        {
            InitializeComponent();
            
            app_path = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase);
            config_xml = app_path + @"\config.xml";
            cards_xml = app_path + @"\cards.xml";
            students_xml = app_path + @"\students.xml";

            netManager = new NetworkManager(this, cards_xml, students_xml);
            sndPlayer = new SoundPlayer(app_path + @"\beep.wav");
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (File.Exists(config_xml))
            {
                string ip, userName;
                int port;

                XMLFileManager.LoadConfig(config_xml, out ip, out port, out userName);
                if (ip != null && port != 0 && userName != null)
                {
                    txtIP.Text = ip;
                    txtPort.Text = port.ToString();
                    txtUserName.Text = userName;
                }
                else
                {
                    ip = "127.0.0.1";
                    port = 2500;
                    userName = "user";

                    txtIP.Text = ip;
                    txtPort.Text = port.ToString();
                    txtUserName.Text = userName;
                    XMLFileManager.SaveConfig(config_xml, ip, port, userName);
                }
            }
            else
            {
                string ip, userName;
                int port;

                ip = "127.0.0.1";
                port = 2500;
                userName = "user";

                txtIP.Text = ip;
                txtPort.Text = port.ToString();
                txtUserName.Text = userName;
                XMLFileManager.SaveConfig(config_xml, ip, port, userName);
            }
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            string address = txtIP.Text;
            int port = Convert.ToInt32(txtPort.Text);

            netManager.ConnectToServer(address, port, txtUserName.Text);
        }

        private void nudMark11_ValueChanged(object sender, EventArgs e)
        {
            nudMark1.Value = nudMark11.Value;
        }

        private void nudMark22_ValueChanged(object sender, EventArgs e)
        {
            nudMark2.Value = nudMark22.Value;
        }

        private void nudMark33_ValueChanged(object sender, EventArgs e)
        {
            nudMark3.Value = nudMark33.Value;
        }

        private void nudMark1_ValueChanged(object sender, EventArgs e)
        {
            nudMark11.Value = nudMark1.Value;
        }

        private void nudMark2_ValueChanged(object sender, EventArgs e)
        {
            nudMark22.Value = nudMark2.Value;
        }

        private void nudMark3_ValueChanged(object sender, EventArgs e)
        {
            nudMark33.Value = nudMark3.Value;
        }

        private void btnChange_Click(object sender, EventArgs e)
        {
            if (btnChange.Text == "Изменить")
            {
                txtIP.Enabled = true;
                txtPort.Enabled = true;
                txtUserName.Enabled = true;
                btnChange.Text = "Сохранить";
            }
            else
            {
                string ip, userName;
                int port;

                ip = txtIP.Text;
                port = Convert.ToInt32(txtPort.Text);
                userName = txtUserName.Text;

                XMLFileManager.SaveConfig(config_xml, ip, port, userName);
                
                txtIP.Enabled = false;
                txtPort.Enabled = false;
                txtUserName.Enabled = false;
                btnChange.Text = "Изменить";
            }
        }

        private void menuItemExit_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Вы уверены?", "Подтверждение выхода", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
            {
                netManager.CloseConnection();
                Application.Exit();
            }
        }
    }
}
