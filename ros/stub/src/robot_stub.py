#!/usr/bin/env python
import rospy
from std_msgs.msg import String

def callback(data):
    rospy.loginfo(rospy.get_caller_id() + "Command completed: %s", data.data)

rospy.init_node('CoCo', anonymous=True)
pub = rospy.Publisher('nao_command', String, queue_size=10)
sub = rospy.Subscriber('command_completed', String, callback)

pub.publish('Hello! I am CoCo')

if __name__ == '__main__':
    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass