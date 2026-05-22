#include "core/ScopedTimer.hpp"

#include "core/Logging.hpp"

#include <utility>

namespace lumine::core {

ScopedTimer::ScopedTimer(QString label) : m_label(std::move(label))
{
    m_timer.start();
}

ScopedTimer::~ScopedTimer()
{
    qCDebug(lcPerf) << m_label << "took" << m_timer.elapsed() << "ms";
}

qint64 ScopedTimer::elapsedMs() const
{
    return m_timer.elapsed();
}

} // namespace lumine::core
