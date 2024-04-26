
int msgid;
struct scheduler sch;
int finishedProcesses;
struct PCB* pcb;
int recievedProcesses;
int totalProcesses;
struct Process* runningProcess;
struct Process* newProcess;
float totalWaitingTime;
float totalWTA;
int totalProcessingTime;

//Functions definition
void initializeScheduler();
void handlerINT(int);
bool checkNewProcesses(void* container);
void pushIntoConatainer(void*,int,struct Process*);
void startProcess(struct Process*);
void insertPCBSlot(struct PCB*,struct Process*,pid_t);             //
struct Process* getProcessByID(pid_t);
void stopProcess(struct Process*);
void continueProcess(struct Process*);
void updateOutfile(struct Process*);
pid_t getProcessID(struct Process*);
void updatePCB(struct PCB *,struct Process*,enum processState);    //
void finishProcessHandler(int); 
void generatePrefFile();
void SRTN_Algo();
void HPF_Algo();
void RR_Algo(char*);