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


namespace Assistant
{
    public partial class Form1 : Form
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

        Socket socket;
        Thread recvThread;
        //ReceiveState recvState;

        System.Threading.Timer timer;
        int timerCount = 10;

        //private void timerTick(Object stateInfo);
        private delegate void UpdateHeaderHandler(int count);
        //private void updateHeader(int count);

        //private bool waitingStudent = false;
        private int studentID = -1;
        private string student;
        private string studentTask;

        private string ticketNum;
        private string question1;
        private string question2;
        private string question3;
   
        //int opcode = 0;
        //int fieldCount = 0;
        //int rcvMsgSize = 0;

        private ManualResetEvent recvDone = new ManualResetEvent(false);

        private delegate void UpdateStudentEventHandler();
        private delegate Int32 GetMarkValueHandler(NumericUpDown nudControl);

        private List<Student> students = null;
        //private Student currentStudent = null;

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
                this.Text = "Сбор - " + count.ToString() + " сек!";
            }
            else
            {
                timerCount = 10;
                this.Text = "Assistant";
                timer.Dispose();
            }
        }

        private void SaveStudentChanges(Student student)
        {
            if (student != null)
            {
                foreach (Student st in students)
                {
                    if (st.ID == student.ID)
                    {
                        int ind = students.IndexOf(st);
                        students.Remove(st);
                        students.Insert(ind, student);
                        break;
                    }
                }
            }
        }

        private string getMD5Hash(String input)
        {
            MD5CryptoServiceProvider provider = new MD5CryptoServiceProvider();
            byte[] bs = System.Text.Encoding.UTF8.GetBytes(input);

            bs = provider.ComputeHash(bs);
            StringBuilder strBuilder = new StringBuilder();
            foreach (byte b in bs)
                strBuilder.Append(b.ToString("x2").ToLower());

            string pass = strBuilder.ToString();
            return pass;
        }

        private void InitializeConnection(string address, int port)
        {
            try
            {              
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse(address), port);
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream,
                    ProtocolType.Tcp);
                socket.Connect(endPoint);
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

        private void SendGreetingMessage(string usrName, string password)
        {
            string passHash = getMD5Hash(password);

            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeGreeting)));
            //message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(2)));

            int msgSize = sizeof(int) + usrName.Length + sizeof(int) + passHash.Length;

            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(msgSize)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(usrName.Length)));
            message.AddRange(System.Text.Encoding.UTF8.GetBytes(usrName));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(passHash.Length)));
            message.AddRange(System.Text.Encoding.UTF8.GetBytes(passHash));

            byte[] msg = message.ToArray();
            SendMessage(msg);
        }

        private void SendStudentRequestGranted()
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeStudentRequestGranted)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

            byte[] msg = message.ToArray();
            SendMessage(msg);
        }

        private void SendStudentRequestDenied()
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeStudentRequestDenied)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

            byte[] msg = message.ToArray();
            SendMessage(msg);
        }

        private void SendStudentResults(int studentID, int mark1, int mark2, int mark3,
            int mark4, int mark5, int resMark)
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeStudentResults)));
            //message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(7)));

            int msgSize = 5 * sizeof(int);

            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(msgSize)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(studentID)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(mark1)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(mark2)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(mark3)));
            //message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(mark4)));
            //message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(mark5)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(resMark)));

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

        private void ReadCallback(IAsyncResult ar)
        {
            try
            {
                StateObject state = (StateObject)ar.AsyncState;
                Socket client = state.workSocket;

                int bytesRead = client.EndReceive(ar);
                state.bytesRead += bytesRead;

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
        }

        private void ReadBody(int opcode, byte[] buffer)
        {            
            switch (opcode)
            {
                case AssistantProtocol.OpcodeUserNotFound:
                    MessageBox.Show("Пользователь не найден!\nПодключение будет разорвано.", "Ошибка аутентификации", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeUserIsAlreadyEntered:
                    MessageBox.Show("Пользователь с таким именем уже подключен!\nПодключение будет разорвано.", "Ошибка аутентификации", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeUserPassIncorrect:
                    MessageBox.Show("Неверный пароль!\nПодключение будет разорвано.", "Ошибка аутентификации", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeAccessGranted:
                    //waitingStudent = true;
                    MessageBox.Show("Соединение успешно установлено.");
                    break;

                case AssistantProtocol.OpcodeByeMsg:
                    MessageBox.Show("Сервер разорвал соединение");
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeStudentRequest:
                    //if (waitingStudent)
                    //SendStudentRequestGranted();                    

                    int mark1 = 0, mark2 = 0, mark3 = 0;//, mark4, mark5;
                    int resMark = 0;

                    //mark1 = (Int32)nudMark1.Value;
                    //mark2 = (Int32)nudMark2.Value;
                    //mark3 = (Int32)nudMark3.Value;
                    //mark4 = (Int32) nudMark4.Value;
                    //mark5 = (Int32) nudMark5.Value;
                    //resMark = (Int32)nudResultMark.Value;
                    mark1 = (Int32)this.Invoke(new GetMarkValueHandler(this.GetMarkValue), new Object[] { nudMark1 });
                    mark2 = (Int32)this.Invoke(new GetMarkValueHandler(this.GetMarkValue), new Object[] { nudMark2 });
                    mark3 = (Int32)this.Invoke(new GetMarkValueHandler(this.GetMarkValue), new Object[] { nudMark3 });
                    SendStudentResults(studentID, mark1, mark2, mark3, 0, 0, resMark);
                    //else
                    //    SendStudentRequestDenied();
                    break;

                case AssistantProtocol.OpcodeStudentInfo:
                    ReadStudentInfo(buffer);
                    //waitingStudent = false;
                    this.Invoke(new UpdateStudentEventHandler(this.UpdateStudentInfo));
                    break;

                default:
                    MessageBox.Show("Неизвестный пакет!\nПодключение будет разорвано.", "Ошибка сетевого протокола", MessageBoxButtons.OK, MessageBoxIcon.Hand, MessageBoxDefaultButton.Button1);
                    socket.Close();
                    return;
            }
        }

        private void ReadStudentInfo(byte[] buffer)
        {
            //int index = 2 * sizeof(int);

            int index = 0;
            
            studentID = GetIntFromMessage(buffer, index);
            index += sizeof(int);
            
            int studentLen = IPAddress.NetworkToHostOrder(GetIntFromMessage(buffer, index));
            index += sizeof(int);

            student = GetStringFromMessage(buffer, index, studentLen);
            index += studentLen;

            int taskLen = IPAddress.NetworkToHostOrder(GetIntFromMessage(buffer, index));
            index += sizeof(int);

            studentTask = GetStringFromMessage(buffer, index, taskLen);

            string[] strList = studentTask.Split('\n');
            ticketNum = strList[0];
            question1 = strList[1];
            question2 = strList[2];
            question3 = strList[3];
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

        private void UpdateStudentInfo() 
        {
            txtStudent.Text = student;
            txtQuestion1.Text = ticketNum + "\n" + question1 + "\n";
            txtQuestion2.Text = ticketNum + "\n" + question2 + "\n";
            txtQuestion3.Text = ticketNum + "\n" + question3 + "\n";
            //txtStudentTask.Text = studentTask;
        }

        private Int32 GetMarkValue(NumericUpDown nudControl)
        {
            return (Int32)nudControl.Value;
        }

        public Form1()
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
            string address = txtIP.Text;
            int port = Convert.ToInt32(txtPort.Text);

            InitializeConnection(address, port);

            string usrName = txtUserName.Text;
            string password = txtPassword.Text;
            SendGreetingMessage(usrName, password);

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

        private void menuItem1_Click(object sender, EventArgs e)
        {
            TimerCallback timerDelegate = new TimerCallback(timerTick);
            timer = new System.Threading.Timer(timerDelegate, timerCount, 0, 1000);
        }
    }
}