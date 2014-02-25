/* $Id: CommDataEncoder.h,v 1.7 2012-03-27 04:10:40 noma Exp $ */ 
#ifndef CommDataEncoder_h
#define CommDataEncoder_h

#include "comm/encoder/Attach.h"
#include "comm/encoder/Entities.h"
#include "comm/encoder/InvokeMethod.h"
#include "comm/encoder/LogMsg.h"
#include "comm/encoder/Result.h"
#include "comm/encoder/Entity.h"
#include "comm/encoder/SimCtrl.h"
#include "comm/encoder/CaptureViewImage.h"
#include "comm/encoder/InvokeOnRecvSound.h"
#include "comm/encoder/DetectEntities.h"
//added by okamoto@tome (2011/9/8)
#include "comm/encoder/DistanceSensor.h"
//added by okamoto@tome (2011/12/19)
#include "comm/encoder/MoveControl.h"
#ifdef DEPRECATED
#include "comm/encoder/ControllerCommand.h"
#endif
#include "comm/encoder/NameService.h"
#include "comm/encoder/GetObjectNames.h"

#include "comm/encoder/Attrs.h"
#include "comm/encoder/JointAngle.h"


//added by okamoto@tome (2011/2/18)
#include "comm/encoder/JointQuaternion.h"
//added by okamoto@tome (2011/3/3)
#include "comm/encoder/JointTorque.h"

#include "comm/encoder/SoundRecog.h"
#include "comm/encoder/GetJointForce.h"

#include "comm/encoder/ConnectJoint.h"
#include "comm/encoder/ReleaseJoint.h"

//added by okamoto@tome (2011/8/1)
#include "comm/encoder/DisplayText.h"

//added by noma@tome (2012/02/20)
#include "comm/encoder/GetPointedObject.h"


#endif // CommDataEncoder_h

