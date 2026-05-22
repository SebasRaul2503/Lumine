#pragma once

#include <QLoggingCategory>

namespace lumine {

// Application lifecycle, window and input events.
Q_DECLARE_LOGGING_CATEGORY(lcApp)

// Image loading and decoding.
Q_DECLARE_LOGGING_CATEGORY(lcImage)

// Performance instrumentation (startup, decode timings).
Q_DECLARE_LOGGING_CATEGORY(lcPerf)

} // namespace lumine
