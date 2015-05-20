# $Id: Makefile,v 1.3 2011-12-16 08:01:29 okamoto Exp $
DEPFILE = Makefile.dep

CXX = g++

DEBUG = -g -Wall
DEFINE = -DUSE_ODE -DCONTROLLER -DdDOUBLE -D_DEBUG
#DEFINE = -DUSE_ODE -DCONTROLLER -DdDOUBLE
#INCLUDE = -I../ -I../commonlib
INCLUDE = -I../include
CXXFLAGS = $(CFLAGS) $(INCLUDE) $(DEFINE) $(DEBUG)

LD = $(CXX)

.SUFFIXES: .cpp .so .o

.cpp.so:
	$(LD) -shared -o $@  $(CXXFLAGS) -fPIC $<

MODEL_SRCS = SendMsg.cpp onRecvMsg.cpp Robot.cpp Gravity.cpp GoRound.cpp Capture.cpp DetectEntities.cpp SendText.cpp CallBackText.cpp JointTest.cpp onRecvText.cpp onRecvSound.cpp SetPosition.cpp SetJointAngle.cpp onAction.cpp Watcher.cpp MovingArm.cpp NiiRobotOkonomi.cpp NiiManOkonomi.cpp GetJointForce.cpp SetForce.cpp SetTorque.cpp SetAccel.cpp ConnectJoint.cpp ReleaseJoint.cpp GetParts.cpp scMove.cpp scDetectEntities.cpp scCaptureView.cpp scVoiceRecog.cpp scTV_blink.cpp Arm.cpp Attrs.cpp GetAllEntities.cpp scSetAttr.cpp onCollision.cpp scSendText.cpp scSendSound.cpp

SRCS = $(MODEL_SRCS)

LIBS = $(MODEL_SRCS:.cpp=.so)


all:
	$(MAKE) $(LIBS)
	$(MAKE) copy

copy:
	cp *.so *.cpp export/

distclean: clean

clean:
	$(RM) *.o *.so *~

install:
	make -C export install

uninstall:
	make -C export uninstall

depend:
	gcc -MM $(INCLUDE) $(DEFINE) $(SRCS) > $(DEPFILE)

include $(DEPFILE)
