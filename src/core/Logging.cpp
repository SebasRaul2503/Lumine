#include "core/Logging.hpp"

namespace lumine {

// Categories are filterable at runtime via QT_LOGGING_RULES, e.g.
//   QT_LOGGING_RULES="lumine.*=true" ./lumine
//   QT_LOGGING_RULES="lumine.perf=true" ./lumine
Q_LOGGING_CATEGORY(lcApp, "lumine.app")
Q_LOGGING_CATEGORY(lcImage, "lumine.image")
Q_LOGGING_CATEGORY(lcPerf, "lumine.perf")

} // namespace lumine
