#include "tpa_basic/basics.hpp"
#include "tpa_basic/assign.hpp"
#include "tpa_basic/unary_op.hpp"
#include "tpa_basic/reduce_op.hpp"
#include "tpa_basic/binary_op.hpp"
#include "tpa_basic/ternary_op.hpp"
#include "tpa_basic/other.hpp"

#if defined(TP_NAMESPACE)
using TP_NAMESPACE::operator+;
using TP_NAMESPACE::operator-;
using TP_NAMESPACE::operator*;
using TP_NAMESPACE::operator/;
#endif

#include "tpa_basic/undefine.hpp"
