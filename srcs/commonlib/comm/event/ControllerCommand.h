/*
 * Created by okamoto on 2011-03-25
 */

#ifdef DEPRECATED

#ifndef CommEvent_ControllerCommand_h
#define CommEvent_ControllerCommand_h

class Command;

class ControllerCommandEvent
{
private:
	Command *m_cmd;
private:
	void free_();
public:
	ControllerCommandEvent() : m_cmd(0) {;}
	~ControllerCommandEvent() { free_(); }
		
public:
	bool	set(int packetNum, int seq, char *data, int n);

	Command * command() { return m_cmd; }
	Command * release() {
		Command *tmp = m_cmd;
		m_cmd = 0;
		return tmp;
	}
};


#endif // CommEvent_ControllerCommand_h

#endif // DEPRECATED 
