/*
 * Modified by noma on 2012-03-27
 * TODO: comparison of header order between RecvEvent.h
 */

#ifndef CommDataEncoder_h
#define CommDataEncoder_h

#include <sigverse/commonlib/comm/encoder/Attach.h>
#include <sigverse/commonlib/comm/encoder/Entities.h>
#include <sigverse/commonlib/comm/encoder/InvokeMethod.h>
#include <sigverse/commonlib/comm/encoder/LogMsg.h>
#include <sigverse/commonlib/comm/encoder/Result.h>
#include <sigverse/commonlib/comm/encoder/Entity.h>
#include <sigverse/commonlib/comm/encoder/SimCtrl.h>
#include <sigverse/commonlib/comm/encoder/CaptureViewImage.h>
#include <sigverse/commonlib/comm/encoder/InvokeOnRecvSound.h>
#include <sigverse/commonlib/comm/encoder/DetectEntities.h>
//added by okamoto@tome (2011/9/8)
#include <sigverse/commonlib/comm/encoder/DistanceSensor.h>
//added by okamoto@tome (2011/12/19)
#include <sigverse/commonlib/comm/encoder/MoveControl.h>
#include <sigverse/commonlib/comm/encoder/NameService.h>
#include <sigverse/commonlib/comm/encoder/GetObjectNames.h>

#include <sigverse/commonlib/comm/encoder/Attrs.h>
#include <sigverse/commonlib/comm/encoder/JointAngle.h>


//added by okamoto@tome (2011/2/18)
#include <sigverse/commonlib/comm/encoder/JointQuaternion.h>
//added by okamoto@tome (2011/3/3)
#include <sigverse/commonlib/comm/encoder/JointTorque.h>

#include <sigverse/commonlib/comm/encoder/SoundRecog.h>
#include <sigverse/commonlib/comm/encoder/GetJointForce.h>

#include <sigverse/commonlib/comm/encoder/ConnectJoint.h>
#include <sigverse/commonlib/comm/encoder/ReleaseJoint.h>

//added by okamoto@tome (2011/8/1)
#include <sigverse/commonlib/comm/encoder/DisplayText.h>

//added by noma@tome (2012/02/20)
#include <sigverse/commonlib/comm/encoder/GetPointedObject.h>


#endif // CommDataEncoder_h

