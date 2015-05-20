
DEPFILE = Makefile.dep

INCLUDE_LOCAL = -I ../../ -I ../ 

.SUFFIXES: .cpp .o

.cpp.o :
	$(CXX) $(CXXFLAGS) $(INCLUDE_LOCAL) $(INCLUDE) $(DEFINE) -c -o $@ $<


SRCS = Attach.cpp \
	Attrs.cpp \
	Encoder.cpp \
	Entities.cpp \
	InvokeMethod.cpp \
	LogMsg.cpp \
	Result.cpp \
	SimCtrl.cpp \
	Entity.cpp \
	CaptureViewImage.cpp \
	DetectEntities.cpp \
	InvokeOnRecvSound.cpp \
	NameService.cpp \
	JointAngle.cpp \
	JointQuaternion.cpp \
	JointTorque.cpp \
	SoundRecog.cpp \
	GetJointForce.cpp \
	ConnectJoint.cpp \
	ReleaseJoint.cpp \
	GetObjectNames.cpp \
	DisplayText.cpp \
	DistanceSensor.cpp \
	GetPointedObject.cpp \
	MoveControl.cpp	

OBJS = $(SRCS:.cpp=.o)


all: $(OBJS)

depend:
	gcc -Wall -MM $(DEFINE) $(INCLUDE_LOCAL) $(INCLUDE) $(SRCS) > $(DEPFILE)

clean:
	$(RM) *.o *~ core

include $(DEPFILE)
