/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_IMPL_H
#define SRSGNB_MAC_IMPL_H

#include "mac_config.h"
#include "mac_ctrl/mac_controller.h"
#include "mac_ctrl/mac_scheduler_configurator.h"
#include "mac_dl/mac_dl_processor.h"
#include "mac_ul/mac_ul_processor.h"
#include "rach_handler.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/scheduler/mac_scheduler.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class mac_impl : public mac_interface, public mac_cell_control_information_handler
{
public:
  explicit mac_impl(mac_ul_ccch_notifier&     event_notifier,
                    du_l2_ul_executor_mapper& ul_exec_mapper,
                    du_l2_dl_executor_mapper& dl_exec_mapper,
                    task_executor&            ctrl_exec,
                    mac_result_notifier&      phy_notifier_);

  mac_rach_handler& get_rach_handler(du_cell_index_t cell_index) override { return rach_hdl; }

  mac_ue_configurator& get_ue_configurator() override { return ctrl_unit; }

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override { return *this; }

  void handle_crc(const crc_indication_message& msg) override {}

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override
  {
    return dl_unit.get_slot_handler(cell_index);
  }

  mac_cell_manager& get_cell_manager() override { return ctrl_unit; }

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index) override { return ul_unit; }

private:
  mac_common_config_t cfg;

  /// Table used to convert from RNTI to UE index.
  du_rnti_table rnti_table;

  /// Handle used to await scheduler configurations.
  srs_sched_config_adapter sched_cfg_adapter;

  /// MAC Scheduler.
  std::unique_ptr<mac_scheduler> sched_obj;

  mac_dl_processor dl_unit;
  mac_ul_processor ul_unit;
  mac_controller   ctrl_unit;
  rach_handler     rach_hdl;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_IMPL_H
