#pragma once

#include <QHostAddress>

namespace default_network_parameters {

inline constexpr quint16 serverPort = 12345;
inline const QHostAddress serverAddress = QHostAddress::LocalHost;

}  // namespace default_network_parameters