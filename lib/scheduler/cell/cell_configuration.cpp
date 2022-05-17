/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_configuration.h"
#include "srsgnb/asn1/rrc_nr/tdd_cfg_helper.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/resource_block.h"

using namespace srsgnb;

cell_configuration::cell_configuration(const sched_cell_configuration_request_message& msg) :
  cell_index(msg.cell_index),
  pci(msg.pci),
  nof_dl_prbs(get_max_Nprb(msg.dl_carrier.carrier_bw_mhz, msg.scs_common, false)),
  nof_ul_prbs(get_max_Nprb(msg.ul_carrier.carrier_bw_mhz, msg.scs_common, false)),
  nof_slots_per_frame(get_nof_slots_per_subframe(msg.dl_cfg_common.init_dl_bwp.generic_params.scs)),
  dl_cfg_common(msg.dl_cfg_common),
  ul_cfg_common(msg.ul_cfg_common),
  tdd_cfg_common(msg.tdd_ul_dl_cfg_common),
  dl_carrier(msg.dl_carrier),
  ssb_cfg(msg.ssb_config),
  // SSB derived params.
  ssb_case(ssb_get_ssb_pattern(msg.ssb_config.scs, msg.dl_carrier.arfcn)),
  paired_spectrum(band_helper::is_paired_spectrum(band_helper::get_band_from_dl_arfcn(msg.dl_carrier.arfcn))),
  L_max(ssb_get_L_max(msg.ssb_config.scs, msg.dl_carrier.arfcn))
{
  using namespace tdd_cfg_helper;

  if (tdd_cfg_common.has_value()) {
    // Cache list of DL and UL slots in case of TDD
    uint8_t numerology_idx = msg.tdd_ul_dl_cfg_common->ref_subcarrier_spacing.value;
    dl_enabled_slot_lst.resize(nof_slots_per_period(*msg.tdd_ul_dl_cfg_common));
    ul_enabled_slot_lst.resize(nof_slots_per_period(*msg.tdd_ul_dl_cfg_common));
    for (unsigned i = 0; i < dl_enabled_slot_lst.size(); ++i) {
      dl_enabled_slot_lst[i] = (uint8_t)slot_is_dl(*msg.tdd_ul_dl_cfg_common, slot_point{numerology_idx, i});
      ul_enabled_slot_lst[i] = (uint8_t)slot_is_ul(*msg.tdd_ul_dl_cfg_common, slot_point{numerology_idx, i});
    }
  }
}

#define CHECK(cond, ...)                                                                                               \
  if (not(cond))                                                                                                       \
    return {fmt::format(__VA_ARGS__)};

error_type<std::string> srsgnb::is_cell_configuration_request_valid(const sched_cell_configuration_request_message& msg)
{
  CHECK(msg.cell_index < MAX_NOF_DU_CELLS, "Invalid cell index={}", msg.cell_index);
  CHECK(msg.pci <= MAX_PCI, "Invalid pci={}", msg.pci);
  // TODO
  return {};
}
