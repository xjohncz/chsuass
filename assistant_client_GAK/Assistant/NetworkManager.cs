using System;

using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.IO;

namespace Assistant
{
    class NetworkManager
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

        private MainForm parentForm = null;

        private string students_xml;

        private int examID = -1;
        private int studentID = -1;
        private int newStudentID = -1;
        private int memberID = -1;
        private int studentCount = 0;

        private Socket socket;
        private Thread recvThread;
        private bool connected = false;

        private System.Threading.Timer pongTimer;

        private int pongTime = 0;
        public int PongTime
        {
            get { return pongTime; }
            set { pongTime = value; }
        }

        //public bool pong = true;

        private List<Student> students = null;

        private ManualResetEvent recvDone = new ManualResetEvent(false);

        private delegate void UpdateStudentEventHandler(Student st);
        private delegate void FillStudentResultsEventHandler(List<Student> students);
        private delegate void SaveStudentHandler(Student st);

        private delegate void ReadBodyHandler(int opcode);

        private delegate void SetProgressBarMaxHandler(int maxValue);
        private delegate void UpdateProgressBarHandler(int value);
        private delegate void SetStateLabelHandler(string state);

        private delegate void StartTimerHandler(Student.StudentUpdate stUpd);

        public NetworkManager(MainForm form, string students_xml)
        {
            this.parentForm = form;
            this.students_xml = students_xml;
        }

        private void timerTick(Object state)
        {
            pongTime += 25;
        }

        public Student GetCurrentStudent()
        {
            Student student = null;
            if (studentID != -1 && students != null)
            {
                student = Student.GetStudentById(students, studentID);
            }

            return student;
        }

        public bool ConnectToServer(string address, int port, string userName)
        {
            if (!connected)
            {
                if (!InitializeConnection(address, port))
                    return false;

                if (!SendGreetingMessage(userName))
                    return false;

                studentID = -1;

                recvThread = new Thread(new ThreadStart(ReadMessages));
                recvThread.IsBackground = true;
                recvThread.Start();

                connected = true;
            }

            return true;
        }

        public void CloseConnection()
        {
            try
            {
                if (socket != null)
                    if (socket.Connected)
                    {
                        SendBye();
                        //socket.Shutdown(SocketShutdown.Both);
                        socket.Close();
                    }
            }
            catch (SocketException socketEx)
            {
                Console.WriteLine("Исключение, вызванное сокетом: " + socketEx.Message);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Исключение: " + ex.Message);
            }
            finally
            {
                connected = false;
            }
        }

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
                Console.WriteLine("Исключение, вызванное сокетом: " + socketEx.Message);
                return false;
            }
        }

        private bool SendMessage(byte[] buffer)
        {
            try
            {
                int bytesSent = 0;
                bytesSent = socket.Send(buffer, 0, buffer.Length, SocketFlags.None);

                if (bytesSent != buffer.Length)
                    return false;

                return true;
            }
            catch (SocketException socketEx)
            {
                Console.WriteLine("Исключение, вызванное сокетом: " + socketEx.Message);
                return false;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Исключение: " + ex.Message);
                return false;
            }
        }

        public bool SendGreetingMessage(string usrName)
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeGreeting)));

            int msgSize = sizeof(int) + usrName.Length;

            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(msgSize)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(usrName.Length)));
            message.AddRange(System.Text.Encoding.UTF8.GetBytes(usrName));

            byte[] msg = message.ToArray();
            return SendMessage(msg);
        }

        private bool SendStudents()
        {
            Student st = Student.GetStudentById(students, studentID);
            parentForm.Invoke(new SaveStudentHandler(parentForm.SaveStudent), new Object[] { st });
            
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
            
            parentForm.Invoke(new SetProgressBarMaxHandler(parentForm.SetProgressBarMax), new Object[] { 1 });
            bool ret = SendMessage(msg);
            parentForm.Invoke(new UpdateProgressBarHandler(parentForm.UpdateProgressBar), new Object[] { 1 });

            return ret;
        }

        public bool SendPing()
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodePing)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

            byte[] msg = message.ToArray();
            bool res = SendMessage(msg);

            pongTime = 0;
            TimerCallback timerDelegate = new TimerCallback(timerTick);
            pongTimer = new System.Threading.Timer(timerDelegate, null, 0, 25);

            return res;
        }

        private bool SendBye()
        {
            List<byte> message = new List<byte>();
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(AssistantProtocol.OpcodeByeMsg)));
            message.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(0)));

            byte[] msg = message.ToArray();
            return SendMessage(msg);
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

                if (state.opcode == AssistantProtocol.OpcodeServerSendsStudents &&
                    state.recvState == ReceiveState.RecvBody)
                    parentForm.Invoke(new UpdateProgressBarHandler(parentForm.UpdateProgressBar), new Object[] { state.bytesRead });

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
                parentForm.Invoke(new SetStateLabelHandler(parentForm.SetLabelState), new Object[] { "Получение списка студентов..." });
                parentForm.Invoke(new SetProgressBarMaxHandler(parentForm.SetProgressBarMax), new Object[] { recvMsgSize });
            }
        }

        private void ReadBody(int opcode, byte[] buffer)
        {
            switch (opcode)
            {
                case AssistantProtocol.OpcodeUserNotFound:
                    parentForm.Invoke(new ReadBodyHandler(parentForm.ShowReadBodyResult), new Object[] { opcode });
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeAccessGranted:
                    parentForm.Invoke(new ReadBodyHandler(parentForm.ShowReadBodyResult), new Object[] { opcode });
                    ReadExamInfo(buffer);                    
                    break;

                case AssistantProtocol.OpcodeByeMsg:
                    parentForm.Invoke(new ReadBodyHandler(parentForm.ShowReadBodyResult), new Object[] { opcode });
                    socket.Close();
                    return;

                case AssistantProtocol.OpcodeStudentInfo:
                    ReadStudentInfo(buffer);
                    break;

                case AssistantProtocol.OpcodeServerSendsStudents:
                    ReadStudents(buffer);
                    break;

                case AssistantProtocol.OpcodeResultsRequest:
                    SendStudents();
                    break;

                case AssistantProtocol.OpcodePong:
                    //pong = true;
                    pongTimer.Dispose();
                    break;

                default:
                    parentForm.Invoke(new ReadBodyHandler(parentForm.ShowReadBodyResult), new Object[] { opcode });
                    socket.Close();
                    return;
            }
        }

        private void ReadStudentInfo(byte[] buffer)
        {
            int index = 0;

            if (studentID == -1)
                studentID = GetIntFromMessage(buffer, index);
            else
                newStudentID = GetIntFromMessage(buffer, index);
            index += sizeof(int);

            GetIntFromMessage(buffer, index);

            Student.StudentUpdate stUpd = new Student.StudentUpdate();
            stUpd.student = Student.GetStudentById(students, studentID);

            if (newStudentID != -1)
            {
                stUpd.newStudent = Student.GetStudentById(students, newStudentID);
                parentForm.Invoke(new StartTimerHandler(parentForm.StartTimer), new Object[] { stUpd });
                studentID = newStudentID;
            }
            else
            {
                parentForm.Invoke(new UpdateStudentEventHandler(parentForm.UpdateStudentInfo), new Object[] { stUpd.student });
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
            index += sizeof(int);

            studentID = GetIntFromMessage(buffer, index);

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
                parentForm.Invoke(new FillStudentResultsEventHandler(parentForm.FillStudentResults), new Object[] { students });

                if (studentID != -1)
                {
                    Student.StudentUpdate stUpd = new Student.StudentUpdate();
                    stUpd.student = Student.GetStudentById(students, studentID);
                    parentForm.Invoke(new UpdateStudentEventHandler(parentForm.UpdateStudentInfo), new Object[] { stUpd.student });
                }
            }
        }

        private void ReadStudents(byte[] buffer)
        {
            int index = 0;

            int studentsLen = IPAddress.NetworkToHostOrder(GetIntFromMessage(buffer, index));
            index += sizeof(int);

            if (File.Exists(students_xml))
                File.Delete(students_xml);

            FileStream fs = File.Create(students_xml);
            fs.Write(buffer, index, studentsLen);
            fs.Close();

            XMLFileManager.SaveMemberId(students_xml, memberID);

            students = XMLFileManager.GetStudentList(students_xml);

            parentForm.Invoke(new FillStudentResultsEventHandler(parentForm.FillStudentResults), new Object[] { students });
            if (studentID != -1)
            {
                Student.StudentUpdate stUpd = new Student.StudentUpdate();
                stUpd.student = Student.GetStudentById(students, studentID);
                parentForm.Invoke(new UpdateStudentEventHandler(parentForm.UpdateStudentInfo), new Object[] { stUpd.student });
            }
        }
    }
}
