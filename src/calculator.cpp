#include "calculator.hpp"

#include "const_1.hpp"
#include "const_2.hpp"

namespace services {
    auto calculator::value() const -> int { return m_multiplier * (m_c1->value() + m_c2->value()); }
}