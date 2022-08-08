/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/messages.h"

namespace srsgnb {

struct prach_buffer_context;

namespace fapi_adaptor {

/// \brief Helper function that converts from a UL PRACH FAPI PDU to a PRACH buffer context.
///
/// \param[out] context Context for the PRACH buffer.
/// \param[in] fapi_pdu FAPI PRACH PDU.
/// \param[in] prach_config_tlv Multi-PRACH configuration TLV.
void convert_prach_fapi_to_phy(prach_buffer_context&           context,
                               const fapi::ul_prach_pdu&       fapi_pdu,
                               const fapi::multi_prach_config& prach_config_tlv);

} // namespace fapi_adaptor
} // namespace srsgnb
