#include "util.h"

void utils::sendTransformPose2d(tf::TransformBroadcaster &broadcaster, const Pose2D &pose)
{
  tf::Transform transformMapToBaseLink;
  transformMapToBaseLink.setOrigin( tf::Vector3(pose.x, -pose.y, 0.0) );
  tf::Quaternion qMapToBaseLink;
  qMapToBaseLink.setRPY(0, 0, -pose.theta);
  transformMapToBaseLink.setRotation(qMapToBaseLink);
  broadcaster.sendTransform(tf::StampedTransform(transformMapToBaseLink, ros::Time::now(), "map", "base_link"));
}
