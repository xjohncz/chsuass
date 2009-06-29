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

        private class NetworkStruct
        {
            public string address;
            public int port;
            public string userName;
        }

        private string app_path;
        private string config_xml;
        private string students_xml;

        private NetworkManager netManager;

        private System.Threading.Timer pingTimer;
        //private int pingCount = 0;
        private const int pingTimeOut = 5000;

        private System.Threading.Timer timer;
        private int timerCount = 10;

        private SoundPlayer sndPlayer = null;

        private delegate void UpdateHeaderHandler(Student.StudentUpdate stUpd);
        private delegate void ShowMessageHandler(string message, string caption);

        public void StartTimer(Student.StudentUpdate stUpd)
        {
            sndPlayer.Play();
            SetProgressBarMax(10);
            TimerCallback timerDelegate = new TimerCallback(timerTick);
            timer = new System.Threading.Timer(timerDelegate, stUpd, 0, 1000);
        }

        private void timerTick(Object state)
        {
            Student.StudentUpdate stUpd = (Student.StudentUpdate)state;
            this.Invoke(new UpdateHeaderHandler(this.UpdateHeader), new Object[] { stUpd });
        }

        private void timerPing(Object state)
        {
            NetworkStruct netStr = (NetworkStruct)state;
            int pongTime = netManager.PongTime;
            pingTimer.Dispose();

            if (pongTime > pingTimeOut)
            {                
                netManager.CloseConnection();

                //bool conn = netManager.ConnectToServer(netStr.address, netStr.port, netStr.userName);
                //if (conn)
                //{
                //    TimerCallback timerDelegate = new TimerCallback(timerPing);
                //    pingTimer = new System.Threading.Timer(timerDelegate, netStr, pingTimeOut, pingTimeOut);
                //}
                //else
                //{
                    this.Invoke(new ShowMessageHandler(this.ShowMessage), new Object[] { "Соединение было утеряно. Переподключение не удалось.\nПроверьте настройки подключения и подключитесь вручную.", "Ошибка подключения" });
                //}
            }
            else
            {
                netManager.SendPing();

                TimerCallback timerDelegate = new TimerCallback(timerPing);
                pingTimer = new System.Threading.Timer(timerDelegate, netStr, pingTimeOut, pingTimeOut);
            }
        }

        private void UpdateHeader(Student.StudentUpdate stUpd)
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

                SaveStudent(stUpd.student);
                UpdateStudentInfo(stUpd.newStudent);
            
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

        private void ShowMessage(string message, string caption)
        {
            MessageBox.Show(message, caption, MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
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
                txtTheme.Height = 213;
                lvMarks.Height = 154;                
                tabControl1.Height = 239;

                lblState.Visible = true;
                prgBar.Visible = true;
            }
            else
            {
                txtTheme.Height = 242;
                lvMarks.Height = 183;
                tabControl1.Height = 268;

                lblState.Visible = false;
                prgBar.Visible = false;
            }
        }

        public void UpdateStudentInfo(Student st) 
        {

            txtStudent.Text = st.FIO;

            lvMarks.Items.Clear();
            nudMark1.Value = st.Mark1;
            nudMark2.Value = st.Mark2;
            nudMark3.Value = st.Mark3;
            nudMark4.Value = st.Mark4;
            nudMark5.Value = st.Mark5;
            nudMark6.Value = st.Mark6;

            foreach (Student.SubjectMark mark in st.subjectMarks)
            {
                ListViewItem item = new ListViewItem(mark.subject);
                item.SubItems.Add(mark.mark.ToString());
                lvMarks.Items.Add(item);
            }

            txtTheme.Text = st.Theme;
        }

        public void FillStudentResults(List<Student> students)
        {
            //lvResults.Items.Clear();
            foreach (Student st in students)
            {
                ListViewItem item = new ListViewItem(st.FIO);
                item.SubItems.Add(st.Mark1.ToString());
                item.SubItems.Add(st.Mark2.ToString());
                item.SubItems.Add(st.Mark3.ToString());
                item.SubItems.Add(st.Mark4.ToString());
                item.SubItems.Add(st.Mark5.ToString());
                item.SubItems.Add(st.Mark6.ToString());

                int mark1 = st.Mark1;
                int mark2 = st.Mark2;
                int mark3 = st.Mark3;
                int mark4 = st.Mark4;
                int mark5 = st.Mark5;
                int mark6 = st.Mark6;

                int count = 6;
                if (mark1 == 0) count--;
                if (mark2 == 0) count--;
                if (mark3 == 0) count--;
                if (mark4 == 0) count--;
                if (mark5 == 0) count--;
                if (mark6 == 0) count--;

                float middle = 0;
                if(count != 0)
                    middle = (float)(mark1 + mark2 + mark3 + mark4 + mark5 + mark6) / (float)count;
                item.SubItems.Add(middle.ToString("0.#"));

                lvResults.Items.Add(item);
            }
        }

        public void SaveStudent(Student st)
        {
            int index = -1;
            foreach (ListViewItem item in lvResults.Items)
            {
                if (item.Text == st.FIO &&
                    item.SubItems[1].Text == st.Mark1.ToString() &&
                    item.SubItems[2].Text == st.Mark2.ToString() &&
                    item.SubItems[3].Text == st.Mark3.ToString() &&
                    item.SubItems[4].Text == st.Mark4.ToString() &&
                    item.SubItems[5].Text == st.Mark5.ToString() &&
                    item.SubItems[6].Text == st.Mark6.ToString())
                {
                    index = item.Index;
                    break;
                }
            }
            
            st.Mark1 = Convert.ToInt32(nudMark1.Value);
            st.Mark2 = Convert.ToInt32(nudMark2.Value);
            st.Mark3 = Convert.ToInt32(nudMark3.Value);
            st.Mark4 = Convert.ToInt32(nudMark4.Value);
            st.Mark5 = Convert.ToInt32(nudMark5.Value);
            st.Mark6 = Convert.ToInt32(nudMark6.Value);

            XMLFileManager.SaveStudent(st, students_xml);

            if (index != -1)
            {
                ListViewItem item = lvResults.Items[index];
                item.SubItems[1].Text = st.Mark1.ToString();
                item.SubItems[2].Text = st.Mark2.ToString();
                item.SubItems[3].Text = st.Mark3.ToString();
                item.SubItems[4].Text = st.Mark4.ToString();
                item.SubItems[5].Text = st.Mark5.ToString();
                item.SubItems[6].Text = st.Mark6.ToString();

                int mark1 = st.Mark1;
                int mark2 = st.Mark2;
                int mark3 = st.Mark3;
                int mark4 = st.Mark4;
                int mark5 = st.Mark5;
                int mark6 = st.Mark6;

                int count = 6;
                if (mark1 == 0) count--;
                if (mark2 == 0) count--;
                if (mark3 == 0) count--;
                if (mark4 == 0) count--;
                if (mark5 == 0) count--;
                if (mark6 == 0) count--;

                float middle = 0;
                if (count != 0)
                    middle = (float)(mark1 + mark2 + mark3 + mark4 + mark5 + mark6) / (float)count;
                item.SubItems[7].Text = middle.ToString("0.#");
            }
        }

        public MainForm()
        {
            InitializeComponent();
            
            app_path = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase);
            config_xml = app_path + @"\config.xml";
            students_xml = app_path + @"\students_gak.xml";

            netManager = new NetworkManager(this, students_xml);
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
            string userName = txtUserName.Text;

            bool conn = netManager.ConnectToServer(address, port, userName);
            if (conn)
            {
                NetworkStruct netStr = new NetworkStruct();
                netStr.address = address;
                netStr.port = port;
                netStr.userName = userName;
                
                TimerCallback timerDelegate = new TimerCallback(timerPing);
                pingTimer = new System.Threading.Timer(timerDelegate, netStr, pingTimeOut, pingTimeOut);
            }
            else
            {
                MessageBox.Show("Не удалось подключиться к серверу!\nПроверьте параметры подключения.", "Ошибка подключения", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
            }
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
                Student student = netManager.GetCurrentStudent();
                if (student != null)
                {
                    SaveStudent(student);
                }
                netManager.CloseConnection();

                Application.Exit();
            }
        }

        private void nudMark1_ValueChanged(object sender, EventArgs e)
        {
            int mark1 = (int)nudMark1.Value;
            int mark2 = (int)nudMark2.Value;
            int mark3 = (int)nudMark3.Value;
            int mark4 = (int)nudMark4.Value;
            int mark5 = (int)nudMark5.Value;
            int mark6 = (int)nudMark6.Value;

            int count = 6;
            if (mark1 == 0) count--;
            if (mark2 == 0) count--;
            if (mark3 == 0) count--;
            if (mark4 == 0) count--;
            if (mark5 == 0) count--;
            if (mark6 == 0) count--;

            float middle = 0;
            if (count != 0)
                middle = (float)(mark1 + mark2 + mark3 + mark4 + mark5 + mark6) / (float)count;
            lblMiddleMark.Text = "Средний балл: " + middle.ToString("0.#");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //pingCount = netManager.PongCount;
            //netManager.pong = false;
            //netManager.PongTime = pingTimeOut;
        }
    }
}
