#pragma once

#include <QHostAddress>

namespace default_network_parameters {

inline constexpr quint16 serverPort = 12345;
inline const QHostAddress serverAddress = QHostAddress::LocalHost;
inline constexpr int reconnectionPeriod = 1000;

}  // namespace default_network_parameters