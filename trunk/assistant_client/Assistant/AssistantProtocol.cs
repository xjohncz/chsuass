using System;

namespace Assistant
{
    public class AssistantProtocol
    {
        public const int OpcodeGreeting = 100;
        public const int OpcodeAccessGranted = 200;
        public const int OpcodeUserNotFound = 210;
        public const int OpcodeUserIsAlreadyEntered = 215;
        public const int OpcodeUserPassIncorrect = 220;
        public const int OpcodeStudentRequest = 150;
        public const int OpcodeStudentRequestGranted = 160;
        public const int OpcodeStudentRequestDenied = 170;
        public const int OpcodeStudentInfo = 300;
        public const int OpcodeStudentResults = 320;
        public const int OpcodeByeMsg = 500;
    }
}