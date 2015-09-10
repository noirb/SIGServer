#!/bin/bash

mkdir $1
cd $1

cat <<EOF > ControllerSample.cpp
#include "ControllerEvent.h"  
#include "Controller.h"  
#include "Logger.h"  
  
class MyController : public Controller {  
public:  
  void onInit(InitEvent &evt);  
  double onAction(ActionEvent&);  
  void onRecvMsg(RecvMsgEvent &evt); 
  void onCollision(CollisionEvent &evt); 
};  
  
void MyController::onInit(InitEvent &evt) {  
}  
  
double MyController::onAction(ActionEvent &evt) {  
  return 1.0;      
}  
  
void MyController::onRecvMsg(RecvMsgEvent &evt) {  
}  

void MyController::onCollision(CollisionEvent &evt) { 
}
  
extern "C" Controller * createController() {  
  return new MyController;  
}  

EOF

cat <<EOF > WorldSample.xml
<?xml version="1.0" encoding="utf8"?>
<world name="myworld1">
  <gravity x="0.0" y="-980.7" z="0.0"/>
  <instanciate class="seToy_D.xml">
    <set-attr-value name="name" value="toy_D"/>
    <set-attr-value name="language" value="c++"/>
    <set-attr-value name="implementation"
                    value="./ControllerSample.so"/>
    <set-attr-value name="dynamics" value="true"/>
    <set-attr-value name="x" value="0.0"/>
    <set-attr-value name="y" value="60.0"/>
    <set-attr-value name="z" value="0.0"/>
    <set-attr-value name="mass" value="1.0"/>
    <set-attr-value name="collision" value="true"/>
  </instanciate>
</world>
EOF

cat <<EOF > Makefile
#sigverse header
SIG_SRC  = \$(SIGVERSE_PATH)/include/sigverse

all: \$(OBJS)

#compile
./%.so: ./%.cpp
	g++ -Wall -DCONTROLLER -DNDEBUG -DUSE_ODE -DdDOUBLE -I\$(SIG_SRC) -I\$(SIG_SRC)/comm/controller -fPIC -shared -o \$@ $<

clean:
	rm ./*.so
EOF

cat<<EOF > sigmake.sh
#################################
# compile SIGVerse controller
# e.g.
# $ ./sigmake.sh Controller.cpp
# $ ./sigmake.sh clean
#################################
#!/bin/sh

# make clean
arg1=\$1
if [ "\${arg1}" = "clean" ]; then
make clean

# set OBJS
else
for arg in \$@
do
case \${arg} in 
*\.cpp)
export OBJS="\$OBJS \${arg%.cpp}.so";
esac
done

# complile
make
fi
EOF

chmod 744 sigmake.sh