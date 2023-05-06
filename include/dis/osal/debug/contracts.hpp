#ifndef DIS_OSAL_DEBUG_CONTRACTS_HPP
#define DIS_OSAL_DEBUG_CONTRACTS_HPP

#include "dis/osal/debug/assert.hpp"

#define dis_contract_check(msg, cond) \
    dis_assert(cond, dis::printf_abort_handler{}, dis::assert_level<3>{}, msg)

#define dis_expects(cond) dis_contract_check("Precondition", cond)
#define dis_ensures(cond) dis_contract_check("Postcondition", cond)

#endif  // DIS_OSAL_DEBUG_CONTRACTS_HPP
