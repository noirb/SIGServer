/*
 * Modified by noma on 2012-03-27
 * TODO: comparison of header order between CommDataEncoder.h
 */

#ifndef RecvEvent_h
#define RecvEvent_h

#include <sigverse/commonlib/comm/event/LogMsg.h>
#include <sigverse/commonlib/comm/event/RecvEntities.h>
#include <sigverse/commonlib/comm/event/RequestAttach.h>
#include <sigverse/commonlib/comm/event/RequestGetEntity.h>
#include <sigverse/commonlib/comm/event/RequestSimCtrl.h>
#include <sigverse/commonlib/comm/event/ResultGetEntity.h>
#include <sigverse/commonlib/comm/event/Result.h>
#include <sigverse/commonlib/comm/event/RequestCaptureViewImage.h>
#include <sigverse/commonlib/comm/event/ResultCaptureViewImage.h>
#include <sigverse/commonlib/comm/event/DetectEntities.h>

// added by okamoto@tome (2011/9/9)
#include <sigverse/commonlib/comm/event/RequestDistanceSensor.h>
#include <sigverse/commonlib/comm/event/ResultDistanceSensor.h>

#include <sigverse/commonlib/comm/event/NameService.h>

#include <sigverse/commonlib/comm/event/Attrs.h>
#include <sigverse/commonlib/comm/event/JointAngle.h>

// added by okamoto@tome (2011/2/18)
#include <sigverse/commonlib/comm/event/JointQuaternion.h>

// added by okamoto@tome (2011/3/3)
#include <sigverse/commonlib/comm/event/JointTorque.h>

#include <sigverse/commonlib/comm/event/SoundRecog.h>
#include <sigverse/commonlib/comm/event/GetJointForce.h>
#include <sigverse/commonlib/comm/event/ResultAttach.h>

#include <sigverse/commonlib/comm/event/ConnectJoint.h>
#include <sigverse/commonlib/comm/event/ReleaseJoint.h>

#include <sigverse/commonlib/comm/event/GetObjectNames.h>

// added by okamoto@tome (2011/8/2)
#include <sigverse/commonlib/comm/event/DisplayText.h>

// added by noma@tome (2012/02/20)
#include <sigverse/commonlib/comm/event/GetPointedObject.h>


#include <sigverse/commonlib/comm/event/MoveControl.h>




#endif // RecvEvent_h
