class CtrlFoo
{
public:
    CtrlFoo():i(0){}
    void Entry(int state, int eventid, void *msg, int msgLen, void* data);
private:
    int i;
};
