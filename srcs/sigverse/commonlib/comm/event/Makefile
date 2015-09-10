DEPFILE = Makefile.dep

INCLUDE_LOCAL =  -I ../../ -I ../

.SUFFIXES: .cpp .o

.cpp.o :
	$(CXX) $(CXXFLAGS) $(INCLUDE_LOCAL) $(INCLUDE) $(DEFINE) -c -o $@ $<


SRCS = RecvEntities.cpp \
	RequestAttach.cpp \
	RequestGetEntity.cpp \
	RequestSimCtrl.cpp \
	Result.cpp \
	ResultGetEntity.cpp \
	LogMsg.cpp \
	RequestCaptureViewImage.cpp \
	ResultCaptureViewImage.cpp \
	DetectEntities.cpp \
	Attrs.cpp \
	NameService.cpp \
	JointAngle.cpp \
	JointQuaternion.cpp \
	JointTorque.cpp \
	SoundRecog.cpp \
	GetJointForce.cpp \
	ResultAttach.cpp \
	ConnectJoint.cpp \
	ReleaseJoint.cpp \
	GetObjectNames.cpp \
	DisplayText.cpp \
	RequestDistanceSensor.cpp \
	ResultDistanceSensor.cpp \
	GetPointedObject.cpp \
	MoveControl.cpp

OBJS = $(SRCS:.cpp=.o)


all: $(OBJS)

depend:
	gcc -Wall -MM $(DEFINE) $(INCLUDE_LOCAL) $(INCLUDE) $(SRCS) > $(DEPFILE)

clean:
	$(RM) *.o *~ core

include $(DEPFILE)
