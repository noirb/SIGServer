RM = rm -f
CODE2HTML = /usr/bin/code2html
CODE2HTML_FLAGS = -n -l c++

CPP_SRCS = onAction.cpp \
	onRecvText.cpp \
	onRecvSound.cpp \
	SetPosition.cpp \
	SetJointAngle.cpp \
	DetectEntities.cpp \
	Capture.cpp \
	SendText.cpp \
	TestController.cpp \
	Move.cpp \
	Watcher.cpp \
	onRecvMsg.cpp \
	Attrs.cpp \
	SetForce.cpp \
	SetAccel.cpp \
	SetTorque.cpp \
	GetParts.cpp \
	ConnectJoint.cpp \
	ReleaseJoint.cpp \
	SendMsg.cpp \
	GetAllEntities.cpp \
	onCollision.cpp

MODEL_SAMPLEDIR = ../../srcs/model/samples
CPPHTML_SRCS = $(CPP_SRCS:.cpp=.html)

.SUFFIXES: .cpp .html

.cpp.html:
	cat $< | nkf --sjis | $(CODE2HTML) $(CODE2HTML_FLAGS) > $@
#	cat $< | tr -d '\r' | $(CODE2HTML) $(CODE2HTML_FLAGS) > $@

all: $(CPPHTML_SRCS)

init:
	@echo Creating links...;
	for i in $(CPP_SRCS) ; do \
		if [ ! -e $$i ] ;then \
			ln -s $(MODEL_SAMPLEDIR)/$$i ./; \
		fi; \
	done

clean:
	$(RM) $(CPPHTML_SRCS)
