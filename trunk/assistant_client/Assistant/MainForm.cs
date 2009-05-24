using System;

using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.Threading;
using System.Security.Cryptography;
using System.Xml;


namespace Assistant
{
    public partial class MainForm : Form
    {

        enum ReceiveState
        {
            RecvHeader = 0,
            RecvBody
        }
        
        private class StateObject
        {
            public const int bufferSize = 1024;
            public byte[] buffer = new byte[bufferSize];
            public int bytesRead = 0;
            public List<byte> listBuffer = new List<byte>();
            
            public Socket workSocket = null;
            public ReceiveState recvState;

            public int opcode = 0;
            public int recvMsgSize = 0;
        }

        const string config_xml = @"\Program Files\Assistant\config.xml";
        const string students_xml = @"\Program Files\Assistant\students.xml";
        const string cards_xml = @"\Program Files\Assistant\cards.xml";

        Socket socket;
        Thread recvThread;

        System.Threading.Timer timer;
        int timerCount = 10;

        private delegate void UpdateHeaderHandler(int count);

        private int examID = -1;
        private int studentID = -1;
        private int newStudentID = -1;
        //private int prevStudentID = -1;
        private int memberID = -1;
        private int studentCount = 0;

        private string ticketNum;
        private string question1;
        private string question2;
        private string question3;
   
        private ManualResetEvent recvDone = new ManualResetEvent(false);

        private delegate void UpdateStudentEventHandler();
        private delegate void SaveStudentHandler();
        private delegate Int32 GetMarkValueHandler(NumericUpDown nudControl);

        private delegate void SetProgressBarMaxHandler(int maxValue);
        private delegate void UpdateProgressBarHandler(int value);
        private delegate void SetStateLabelHandler(string state);

        List<Card> cards = null;
        List<Student> students = null;

        private void timerTick(Object stateInfo)
        {
            int count = (int)stateInfo;
            this.Invoke(new UpdateHeaderHandler(this.UpdateHeader), new Object[] {timerCount});
        }

        private void UpdateHeader(int count)
        {
            if (timerCount > 0)
            {
                timerCount--;
                this.Text = "!Сохр.-" + count.ToString() + " сек!";
            }
            else
            {
                timer.Dispose();

                timerCount = 10;
                this.Text = "Assistant";
                this.Invoke(new SaveStudentHandler(this.SaveStudent));
                this.Invoke(new UpdateStudentEventHandler(this.UpdateStudentInfo));                
            }
        }

        private void SetProgressBarMax(int maxValue)
        {
            prgBar.Maximum = maxValue;
            prgBar.Visible = true;
            lblState.Visible = true;
            //Cursor.Current = Cursors.WaitCursor;
        }

        private void UpdateProgressBar(int value)
        {
            prgBar.Value = value;
            if(prgBar.Value == prgBar.Maximum) 
            {
                prgBar.Visible = false;
                lblState.Visible = false;
                //Cursor.Current = Cursors.Default;
            }
        }

        private void SetLabelState(string state)
        {
            lblState.Text = "Состояние: " + state;
        }

        //private void SaveStudentChanges(Student student)
        //{
        //    if (student != null)
        //    {
        //        foreach (Student st in students)
        //        {
        //            if (st.ID == student.ID)
        //            {
        //                int ind = students.IndexOf(st);
        //                students.Remove(st);
        //                students.Insert(ind, student);
        //                break;
        //            }
        //        }
        //    }
        //}

        //private string getMD5Hash(String input)
        //{
        //    MD5CryptoServiceProvider provider = new MD5CryptoServiceProvider();
        //    byte[] bs = System.Text.Encoding.UTF8.GetBytes(input);

        //    bs = provider.ComputeHash(bs);
        //    StringBuilder strBuilder = new StringBuilder();
        //    foreach (byte b in bs)
        //        strBuilder.Append(b.ToString("x2").ToLower());

        //    string pass = strBuilder.ToString();
        //    return pass;
        //}

        private bool InitializeConnection(string address, int port)
        {
            try
            {              
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse(address), port);
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream,
                    ProtocolType.Tcp);
                socket.Connect(endPoint);

                return true;
            }
            catch (SocketException socketEx)
            {
                MessageBox.Show("Исключение, вызванное сокетом: " + socketEx.Message);
                return false;
            }
        }

        private void SendMessage(byte[] buffer)
        {
            try
            {
                socket.Send(buffer, 0, buffer.Length, SocketFlags.None);
            }
            catch (SocketException socketEx)
            {
                MessageBox.Show("Исключение, вызванное сокетом: " + socketEx.Message);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Исключение: " + ex.Message);
            }
        }

        private void SendGreetingMessage(string usrName)//, string password)
        {
            //string passHash = getMD5Hash(password);

            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeGreeting)));
            //message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(2)));

            int msgSize = sizeof(int) + usrName.Length; //+ sizeof(int) + passHash.Length;

            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(msgSize)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(usrName.Length)));
            message.AddRange(System.Text.Encoding.UTF8.GetBytes(usrName));
            //message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(passHash.Length)));
            //message.AddRange(System.Text.Encoding.UTF8.GetBytes(passHash));

            byte[] msg = message.ToArray();
            SendMessage(msg);
        }

        private void SendStudents()
        {
            this.Invoke(new SaveStudentHandler(this.SaveStudent));

            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeClientSendsResults)));

            FileStream fs = File.OpenRead(students_xml);
            int fsLen = (int)fs.Length;
            int msgSize = fsLen + sizeof(int);
            byte[] buffer = new byte[fsLen];
            fs.Read(buffer, 0, fsLen);
            fs.Close();

            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(msgSize)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(fsLen)));
            message.AddRange(buffer);

            byte[] msg = message.ToArray();
            SendMessage(msg);
        }

        private void SendBye()
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeByeMsg)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

            byte[] msg = message.ToArray();
            SendMessage(msg);
        }

        private int GetIntFromMessage(byte[] buffer, int index)
        {
            int opcode = BitConverter.ToInt32(buffer, index);

            return opcode;
        }

        private string GetStringFromMessage(byte[] buffer, int index, int length)
        {
            return System.Text.Encoding.UTF8.GetString(buffer, index, length);
        }

        private void ReadMessages()
        {
            while (true)
            {
                try
                {
                    recvDone.Reset();

                    StateObject state = new StateObject();
                    state.workSocket = socket;
                    state.recvState = ReceiveState.RecvHeader;

                    if (!socket.Connected)
                    {
                        socket.Close();
                        break;
                    }

                    socket.BeginReceive(state.buffer, 0, 2 * sizeof(int), SocketFlags.None,
                        new AsyncCallback(ReadCallback), state);

                    recvDone.WaitOne();
                }
                catch (SocketException socketEx)
                {
                    Console.WriteLine("Исключение, вызванное сокетом: " + socketEx.Message);
                }
                catch (ThreadAbortException)
                {
                    socket.Close();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Исключение: " + ex.Message);
                }
            }
        }

        private void ReadCallback(IAsyncResult ar)
        {
            try
            {
                StateObject state = (StateObject)ar.AsyncState;
                Socket client = state.workSocket;

                int bytesRead = client.EndReceive(ar);
                state.bytesRead += bytesRead;

                if ((state.opcode == AssistantProtocol.OpcodeServerSendsCards ||
                    state.opcode == AssistantProtocol.OpcodeServerSendsStudents) &&
                    state.recvState == ReceiveState.RecvBody)
                    this.Invoke(new UpdateProgressBarHandler(this.UpdateProgressBar), new Object[] { state.bytesRead });

                // !!!CHECK!!!
                /*if (bytesRead == 0)
                {
                    MessageBox.Show("Подключение было разорвано!", "Разрыв соединения");
                    client.Shutdown(SocketShutdown.Both);
                    client.Close();
                }*/

                if (state.recvState == ReceiveState.RecvHeader)
                {
                    if (state.bytesRead < 2 * sizeof(int))
                    {
                        client.BeginReceive(state.buffer, state.bytesRead, 2 * sizeof(int) - state.bytesRead,
                            SocketFlags.None, new AsyncCallback(ReadCallback), state);
                    }
                    else
                    {
                        ReadHeader(state.buffer, out state.opcode, out state.recvMsgSize);

                        state.recvState = ReceiveState.RecvBody;
                        state.bytesRead = 0;
                        client.BeginReceive(state.buffer, 0, Math.Min(StateObject.bufferSize, state.recvMsgSize - state.bytesRead),
                            SocketFlags.None, new AsyncCallback(ReadCallback), state);
                    }
                }
                else
                {
                    byte[] meaningBuffer = new byte[bytesRead];
                    Array.Copy(state.buffer, meaningBuffer, bytesRead);

                    state.listBuffer.AddRange(meaningBuffer);

                    if (state.bytesRead < state.recvMsgSize)
                    {                        
                        client.BeginReceive(state.buffer, 0, Math.Min(StateObject.bufferSize, state.recvMsgSize - state.bytesRead),
                            SocketFlags.None, new AsyncCallback(ReadCallback), state);
                    }
                    else
                    {
                        ReadBody(state.opcode, state.listBuffer.ToArray());
                        recvDone.Set();
                    }
                }
                
            }
            catch (Exception ex)
            {
                Console.WriteLine("Исключение: " + ex.Message);
            }
        }

        private void ReadHeader(byte[] buffer, out int opcode, out int recvMsgSize)
        {
            int index = 0;
            opcode = GetIntFromMessage(buffer, index);
            index += sizeof(int);
            recvMsgSize = GetIntFromMessage(buffer, index);

            if (opcode == AssistantProtocol.OpcodeServerSendsStudents)
            {
                this.Invoke(new SetStateLabelHandler(this.SetLabelState), new Object[] { "получение списка студентов..." });
                this.Invoke(new SetProgressBarMaxHandler(this.SetProgressBarMax), new Object[] { recvMsgSize });
            } 
            else if (opcode == AssistantProtocol.OpcodeServerSendsCards)
            {
                this.Invoke(new SetStateLabelHandler(this.SetLabelState), new Object[] { "получение списка билетов..." });
                this.Invoke(new SetProgressBarMaxHandler(this.SetProgressBarMax), new Object[] { recvMsgSize });
            }
        }

        private void ReadBody(int opcode, byte[] buffer)
        {            
            switch (opcode)
            {
                case AssistantProtocol.OpcodeUserNotFound:
                    MessageBox.Show("Пользователь не найден!\nПодключение будет разорвано.", "Ошибка аутентификации", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeAccessGranted:
                    ReadExamInfo(buffer);
                    MessageBox.Show("Соединение успешно установлено.");
                    break;

                case AssistantProtocol.OpcodeByeMsg:
                    MessageBox.Show("Сервер разорвал соединение");
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeStudentInfo:
                    ReadStudentInfo(buffer);
                    break;

                case AssistantProtocol.OpcodeServerSendsStudents:
                    ReadStudents(buffer);
                    break;
                    
                case AssistantProtocol.OpcodeServerSendsCards:
                    ReadCards(buffer);
                    break;

                case AssistantProtocol.OpcodeResultsRequest:
                    SendStudents();
                    break;

                default:
                    MessageBox.Show("Неизвестный пакет!\nПодключение будет разорвано.", "Ошибка сетевого протокола", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    socket.Close();
                    return;
            }
        }

        private void ReadStudentInfo(byte[] buffer)
        {
            int index = 0;

            //prevStudentID = studentID;
            if(studentID == -1)
                studentID = GetIntFromMessage(buffer, index);
            else
                newStudentID = GetIntFromMessage(buffer, index);
            index += sizeof(int);

            int cardNumber = GetIntFromMessage(buffer, index);
            Student.GetStudentById(students, studentID).CardNumber = cardNumber;

            if (newStudentID != -1)
            {
                TimerCallback timerDelegate = new TimerCallback(timerTick);
                timer = new System.Threading.Timer(timerDelegate, timerCount, 0, 1000);
            }
            else
            {
                this.Invoke(new UpdateStudentEventHandler(this.UpdateStudentInfo));
            }
        }

        private void ReadExamInfo(byte[] buffer)
        {
            int index = 0;
            
            examID = GetIntFromMessage(buffer, index);
            index += sizeof(int);

            memberID = GetIntFromMessage(buffer, index);
            index += sizeof(int);

            studentCount = GetIntFromMessage(buffer, index);

            bool needStudents = false;

            if (File.Exists(students_xml))
            {
                ExamInfo examInfo = XMLFileManager.GetExamInfo(students_xml);
                if (examInfo.examId != examID || examInfo.studentCount != studentCount)
                    needStudents = true;
            }
            else
            {
                needStudents = true;
            }

            if (needStudents)
            {
                List<byte> message = new List<byte>();
                message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeNeedStudents)));
                message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

                byte[] msg = message.ToArray();
                SendMessage(msg);
            }
            else
            {
                XMLFileManager.SaveMemberId(students_xml, memberID);
                students = XMLFileManager.GetStudentList(students_xml);
            }

            if (!File.Exists(cards_xml))
            {
                List<byte> message = new List<byte>();
                message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeNeedCards)));
                message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

                byte[] msg = message.ToArray();
                SendMessage(msg);
            }
            else
            {
                cards = XMLFileManager.GetCardList(cards_xml);
            }
        }

        private void ReadStudents(byte[] buffer)
        {
            int index = 0;

            int studentsLen = IPAddress.NetworkToHostOrder(GetIntFromMessage(buffer, index));
            index += sizeof(int);
            
            if(File.Exists(students_xml))
                File.Delete(students_xml);

            FileStream fs = File.Create(students_xml);
            fs.Write(buffer, index, studentsLen);
            fs.Close();

            XMLFileManager.SaveMemberId(students_xml, memberID);
            
            students = XMLFileManager.GetStudentList(students_xml);

        }

        private void ReadCards(byte[] buffer)
        {
            int index = 0;

            int cardsLen = IPAddress.NetworkToHostOrder(GetIntFromMessage(buffer, index));
            index += sizeof(int);

            if (File.Exists(cards_xml))
                File.Delete(cards_xml);

            FileStream fs = File.Create(cards_xml);
            fs.Write(buffer, index, cardsLen);
            fs.Close();
            
            cards = XMLFileManager.GetCardList(cards_xml);

        }

        private void UpdateStudentInfo() 
        {
            Student st = Student.GetStudentById(students, studentID);
            Card card = Card.GetCardByNum(cards, st.CardNumber);

            string[] strList = card.CardContents.Split('\n');
            ticketNum = card.CardNumber.ToString();
            question1 = strList[0];
            question2 = strList[1];
            question3 = strList[2];

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

        private void SaveStudent()
        {
            Student st = Student.GetStudentById(students, studentID);
            st.Mark1 = Convert.ToInt32(nudMark1.Value);
            st.Mark2 = Convert.ToInt32(nudMark2.Value);
            st.Mark3 = Convert.ToInt32(nudMark3.Value);
            st.ResultMark = Convert.ToInt32(nudResultMark.Value);

            XMLFileManager.SaveStudent(st, students_xml);
            studentID = newStudentID;
        }

        private Int32 GetMarkValue(NumericUpDown nudControl)
        {
            return (Int32)nudControl.Value;
        }

        public MainForm()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //Cursor.Current = Cursors.WaitCursor;
            //students = Student.GetStudentList(@"\Program Files\Assistant\Students.xml");

            //foreach (Student record in students)
            //{
            //    ListViewItem item = new ListViewItem(record.ID.ToString());
            //    item.SubItems.Add(record.FIO);
            //    listView1.Items.Add(item);
            //    cmbStudents.Items.Add(record.ID.ToString() + ": " + record.FIO);
            //}

            //Cursor.Current = Cursors.Default;
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

        private void cmbStudents_SelectedIndexChanged(object sender, EventArgs e)
        {
            //string item = cmbStudents.Text;
            //int ind = item.IndexOf(":");
            //string stID = item.Substring(0, ind);
            //int id = Convert.ToInt32(stID);

            //SaveStudentChanges(currentStudent);
            //currentStudent = null;

            //foreach (Student st in students)
            //{
            //    if (st.ID == id)
            //    {
            //        txtStudentTask.Text = st.Theme;
            //        txtInstructor.Text = st.Instructor;
            //        txtConsultant.Text = st.Consultant;
            //        nudMark1.Value = st.Mark1;
            //        nudMark2.Value = st.Mark2;
            //        nudMark3.Value = st.Mark3;
            //        nudMark4.Value = st.Mark4;
            //        nudMark5.Value = st.Mark5;
            //        nudResultMark.Value = st.ResultMark;

            //        currentStudent = st;
            //        break;
            //    }
            //}
        }

        private void menuItem2_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Вы уверены?", "Подтверждение выхода", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
            {
                try
                {
                    if (socket != null)
                        if (socket.Connected)
                        {
                            SendBye();
                            socket.Shutdown(SocketShutdown.Both);
                            socket.Close();
                        }
                    Application.Exit();
                }
                catch (SocketException socketEx)
                {
                    Console.WriteLine("Исключение, вызванное сокетом: " + socketEx.Message);
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Исключение: " + ex.Message);
                }
            }
        }

        private void nudMark1_ValueChanged(object sender, EventArgs e)
        {
            //if (currentStudent != null && cmbStudents.Text != "")
            //    currentStudent.Mark1 = (Int32)nudMark1.Value;
        }

        private void nudMark2_ValueChanged(object sender, EventArgs e)
        {
            //if (currentStudent != null && cmbStudents.Text != "")    
            //    currentStudent.Mark2 = (Int32)nudMark2.Value;
        }

        private void nudMark3_ValueChanged(object sender, EventArgs e)
        {
            //if (currentStudent != null && cmbStudents.Text != "") 
            //    currentStudent.Mark3 = (Int32)nudMark3.Value;
        }

        private void nudMark4_ValueChanged(object sender, EventArgs e)
        {
            //if (currentStudent != null && cmbStudents.Text != "")
            //    currentStudent.Mark4 = (Int32)nudMark4.Value;
        }

        private void nudMark5_ValueChanged(object sender, EventArgs e)
        {
            //if (currentStudent != null && cmbStudents.Text != "")
            //    currentStudent.Mark5 = (Int32)nudMark5.Value;
        }

        private void nudResultMark_ValueChanged(object sender, EventArgs e)
        {
            //if (currentStudent != null && cmbStudents.Text != "")
            //    currentStudent.ResultMark = (Int32)nudResultMark.Value;
        }

        private void menuItem3_Click(object sender, EventArgs e)
        {
            //SaveStudentChanges(currentStudent);
            //Student.SaveStudentList(students, @"\Program Files\Assistant\Students.xml");
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //if (listView1.SelectedIndices.Count > 0)
            //    cmbStudents.SelectedIndex = listView1.FocusedItem.Index;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            studentID = -1;
            
            string address = txtIP.Text;
            int port = Convert.ToInt32(txtPort.Text);

            if (!InitializeConnection(address, port))
                return;

            string usrName = txtUserName.Text;
            //string password = txtPassword.Text;
            SendGreetingMessage(usrName);
            //SendGreetingMessage(usrName, password);

            tabControl1.TabIndex = 1;

            recvThread = new Thread(new ThreadStart(ReadMessages));
            recvThread.IsBackground = true;
            recvThread.Start();
        }

        private void menuItem4_Click(object sender, EventArgs e)
        {
            //int mark1, mark2, mark3;//, mark4, mark5;
            //int resMark;

            //mark1 = (Int32) nudMark1.Value;
            //mark2 = (Int32) nudMark2.Value;
            //mark3 = (Int32) nudMark3.Value;
            ////mark4 = (Int32) nudMark4.Value;
            ////mark5 = (Int32) nudMark5.Value;
            //resMark = (Int32) nudResultMark.Value;

            //SendStudentResults(studentID, mark1, mark2, mark3, 0, 0, resMark);
        }

        private void nudMark11_ValueChanged_1(object sender, EventArgs e)
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

        private void nudMark1_ValueChanged_2(object sender, EventArgs e)
        {
            nudMark11.Value = nudMark1.Value;
        }

        private void nudMark2_ValueChanged_2(object sender, EventArgs e)
        {
            nudMark22.Value = nudMark2.Value;
        }

        private void nudMark3_ValueChanged_1(object sender, EventArgs e)
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
    }
}