/*
 * CtrlFoo.h
 *
 *  Created on: 2022��4��23��
 *      Author: daniel
 */
class CtrlFoo
{
public:
	CtrlFoo():i(0){}
    void Entry(int state, int eventid, void *msg, int msgLen, void* data);
private:
    void Print(void *);
private:
    int i;
};
