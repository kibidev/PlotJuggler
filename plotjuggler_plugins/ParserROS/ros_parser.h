#ifndef ROS_PARSER_H
#define ROS_PARSER_H

#include "PlotJuggler/fmt/core.h"
#include "PlotJuggler/messageparser_base.h"
#include "PlotJuggler/special_messages.h"
#include "rosx_introspection/ros_parser.hpp"

class ParserROS : public PJ::MessageParser
{
public:
  ParserROS(const std::string& topic_name, const std::string& type_name,
            const std::string& schema, RosMsgParser::Deserializer* deserializer,
            PJ::PlotDataMapRef& data);

  bool parseMessage(const PJ::MessageRef serialized_msg, double& timestamp) override;

  void setLargeArraysPolicy(bool clamp, unsigned max_size) override;

protected:
  RosMsgParser::Parser _parser;
  std::shared_ptr<RosMsgParser::Deserializer> _deserializer;
  RosMsgParser::FlatMessage _flat_msg;
  std::string _topic;

  PJ::Msg::Header parseHeader(const std::string& prefix, double& timestamp);

  template <size_t N>
  void parseCovariance(const std::string& prefix, double& timestamp);

  void parseVector3(const std::string& prefix, double& timestamp);
  void parsePoint(const std::string& prefix, double& timestamp);
  void parseQuaternion(const std::string& prefix, double& timestamp);

  void parseTwist(const std::string& prefix, double& timestamp);
  void parseTwistWithCovariance(const std::string& prefix, double& timestamp);

  void parseTransform(const std::string& prefix, double& timestamp);
  void parseTransformStamped(const std::string& prefix, double& timestamp);

  void parsePose(const std::string& prefix, double& timestamp);
  void parsePoseStamped(const std::string& prefix, double& timestamp);
  void parsePoseWithCovariance(const std::string& prefix, double& timestamp);

  void parseImu(const std::string& prefix, double& timestamp);
  void parseOdometry(const std::string& prefix, double& timestamp);

  void parseDiagnosticMsg(const std::string& prefix, double& timestamp);
  void parseJointStateMsg(const std::string& prefix, double& timestamp);
  void parseTF2Msg(const std::string& prefix, double& timestamp);

  void parseDataTamerSchemasMsg(const std::string& prefix, double& timestamp);
  void parseDataTamerSnapshotMsg(const std::string& prefix, double& timestamp);

  std::function<void(const std::string& prefix, double&)> _customized_parser;

  bool _has_header = false;
};

template <size_t N>
inline void ParserROS::parseCovariance(const std::string& prefix, double& timestamp)
{
  std::array<double, N * N> cov;
  for (auto& val : cov)
  {
    _deserializer->deserialize(RosMsgParser::FLOAT64).convert<double>();
  }
  for (int i = 0; i < N; i++)
  {
    for (int j = i; j < N; j++)
    {
      const size_t index = i * N + j;
      getSeries(fmt::format("{}[{}][{}]", prefix, i, j))
          .pushBack({ timestamp, cov[index] });
    }
  }
}

#endif  // ROS_PARSER_H
