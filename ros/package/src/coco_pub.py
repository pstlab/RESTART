#!/usr/bin/env python
import rospy
from std_msgs.msg import String

def callback(data):
    rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)

rospy.init_node('CoCo', anonymous=True)
pub = rospy.Publisher('chatter', String, queue_size=10)
sub = rospy.Subscriber('chatter', String, callback)

pub.publish('Hello! I am CoCo')

if __name__ == '__main__':
    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass