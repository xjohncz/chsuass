namespace Assistant
{
    public class AssistantProtocol
    {
        public const int OpcodeGreeting = 100;
        public const int OpcodeAccessGranted = 200;
        public const int OpcodeUserNotFound = 210;

        public const int OpcodeStudentInfo = 300;
        
        public const int OpcodeNeedStudents = 400;
        public const int OpcodeNeedCards = 410;
        public const int OpcodeServerSendsStudents = 420;
        public const int OpcodeServerSendsCards = 430;

        public const int OpcodeResultsRequest = 450;
        public const int OpcodeClientSendsResults = 460;

        public const int OpcodeByeMsg = 500;
    }
}