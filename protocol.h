#ifndef PROTOCOL_H
#define PROTOCOL_H

static const int OpcodeGreeting = 100;

static const int OpcodeAccessGranted = 200;
static const int OpcodeUserNotFound = 210;

static const int OpcodeStudentInfo = 300;

static const int OpcodeNeedStudents = 400;
static const int OpcodeNeedCards = 410;
static const int OpcodeServerSendsStudents = 420;
static const int OpcodeServerSendsCards = 430;

static const int OpcodeResultsRequest = 450;
static const int OpcodeClientSendsResults = 460;

static const int OpcodeByeMsg = 500;

#endif // PROTOCOL_H
