# $Id: Makefile,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $
RM = rm -f
LD = g++

INCLUDE = -I../ -I../comm -I../comm/encoder -I../comm/event

DEBUG = -g  -DUNIT_TEST -DUSE_ODE -DdDouble

CFLAGS = $(INCLUDE) $(DEBUG) -pipe

.SUFFIXES: .cpp .o 

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $< 

ENC_SRCS = ../comm/encoder/Encoder.cpp ../comm/encoder/DetectEntities.cpp ../comm/encoder/InvokeMethod.cpp ../comm/encoder/InvokeOnRecvSound.cpp ../comm/encoder/SimCtrl.cpp ../comm/encoder/Attach.cpp ../comm/encoder/Result.cpp ../comm/encoder/LogMsg.cpp ../comm/encoder/ControllerCommand.cpp ../comm/encoder/Entity.cpp ../comm/encoder/CaptureViewImage.cpp ../comm/encoder/Attrs.cpp ../comm/encoder/NameService.cpp ../comm/encoder/JointAngle.cpp ../comm/encoder/SoundRecog.cpp ../comm/encoder/GetJointForce.cpp ../comm/encoder/ConnectJoint.cpp ../comm/encoder/ReleaseJoint.cpp ../comm/encoder/GetObjectNames.cpp
#ENC_SRCS = ../comm/encoder/*.cpp
EVENT_SRCS = ../comm/event/DetectEntities.cpp ../comm/event/RequestSimCtrl.cpp ../comm/event/RequestAttach.cpp ../comm/event/Result.cpp ../comm/event/LogMsg.cpp ../comm/event/ControllerCommand.cpp ../comm/event/RequestGetEntity.cpp ../comm/event/ResultGetEntity.cpp ../comm/event/RequestCaptureViewImage.cpp ../comm/event/ResultCaptureViewImage.cpp ../comm/event/RecvEntities.cpp ../comm/event/Attrs.cpp ../comm/event/NameService.cpp  ../comm/event/JointAngle.cpp ../comm/event/SoundRecog.cpp ../comm/event/GetJointForce.cpp  ../comm/event/ResultAttach.cpp ../comm/event/ConnectJoint.cpp ../comm/event/ReleaseJoint.cpp ../comm/event/GetObjectNames.cpp 
CTRL_SRCS = ../comm/controller/Action.cpp ../comm/controller/RecvText.cpp ../comm/controller/RecvSound.cpp ../comm/controller/RecvMessage.cpp ../comm/controller/Collision.cpp
CMD_SRCS = ../command/JointControl.cpp ../command/Move.cpp ../command/decode.cpp

CT_SRCS = ../ct/NSLookup.cpp ../ct/CTReader.cpp

LIB_SRCS = ../Logger.cpp ../CommUtil.cpp ../ViewImage.cpp ../Value.cpp ../Rotation.cpp ../SimWorld.cpp ../SimObj.cpp ../SimObjBase.cpp ../Attribute.cpp ../Parts.cpp ../CParts.cpp ../Parts_Binary.cpp ../RawSound.cpp ../binary.cpp ../CommDataDecoder.cpp ../comm/Header.cpp $(EVENT_SRCS) $(CTRL_SRCS) $(ENC_SRCS) $(CMD_SRCS) $(CT_SRCS)


TEST_SRCS = getnames.cpp attach.cpp decoder.cpp ctrl.cpp decoder_entity.cpp parts.cpp nameservice.cpp attrs.cpp SimObj.cpp soundrecog_res.cpp soundrecog_res_sjis.cpp soundrecog_res_utf8.cpp soundrecog_res_eucjp.cpp  soundrecog_req.cpp jointforce.cpp cjoint.cpp rjoint.cpp
SRCS = $(TEST_SRCS) $(LIB_SRCS)  

TEST_OBJS = $(TEST_SRCS:.cpp=.o)
LIB_OBJS = $(LIB_SRCS:.cpp=.o)

TESTS = $(TEST_SRCS:.cpp=)

OBJS = $(SRCS:.cpp=.o)

TARGET = ctrl

LIBS = -lode

target: 
	$(MAKE) compile "TEST_OBJ=$(TARGET).o"
	./$(TARGET)


all: $(TEST_OBJS)
	for i in $(TESTS); \
	do ( $(MAKE) compile "TEST_OBJ=$$i.o") \
	done
	$(MAKE) run

compile: $(LIB_OBJS) $(TEST_OBJ)
	$(LD) -o $(TEST_OBJ:.o=) $(TEST_OBJ) $(LIB_OBJS) -lcppunit $(LIBS)


run : 
	@echo run tests
	@for i in $(TESTS); \
	do ( ./$$i ) done

soundrecog_res_utf8.cpp:
	cat soundrecog_res.cpp | nkf --utf8 | sed 's/SoundRecogTest/SoundRecogTestUTF8/g' > $@

soundrecog_res_eucjp.cpp:
	cat soundrecog_res.cpp | nkf --euc | sed 's/SoundRecogTest/SoundRecogTestEUCJP/g' > $@

soundrecog_res_sjis.o: soundrecog_res_sjis.cpp

soundrecog_res_sjis.cpp:
	cat soundrecog_res.cpp | nkf --sjis | sed 's/SoundRecogTest/SoundRecogTestSJIS/g' > $@

ns: $(LIB_OBJS) ns.cpp
	$(LD) -o $@ $(CFLAGS) ns.cpp $(LIB_OBJS) $(LIBS)

clean:
	$(RM) $(TESTS) $(OBJS) *.o *~ soundrecog_res_*.cpp

