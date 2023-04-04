/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../adapters/f1ap_adapters.h"
#include "../adapters/mac_adapters.h"
#include "../adapters/rlc_adapters.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/rlc/rlc_entity.h"
#include <map>

namespace srsran {
namespace srs_du {

struct du_manager_params;

/// \brief Connector of the MAC, RLC and F1 for a given DU UE SRB bearer.
struct du_srb_connector {
  mac_sdu_rx_adapter              mac_rx_sdu_notifier;
  mac_sdu_tx_adapter              mac_tx_sdu_notifier;
  rlc_rx_rrc_sdu_adapter          rlc_rx_sdu_notif;
  rlc_f1c_tx_data_notifier        rlc_tx_data_notif;
  rlc_tx_control_notifier         rlc_tx_ctrl_notif;
  rlc_tx_mac_buffer_state_updater rlc_tx_buffer_state_notif;
  f1c_rx_sdu_rlc_adapter          f1c_rx_sdu_notif;

  /// \brief Connect bearers of MAC, RLC and F1AP layers.
  void connect(du_ue_index_t                       ue_index,
               srb_id_t                            srb_id,
               f1c_bearer&                         f1_bearer,
               rlc_entity&                         rlc_bearer,
               mac_ue_control_information_handler& mac_ue_info_handler);
};

/// \brief Connector of the MAC, RLC and F1 for a given DU UE DRB bearer.
struct du_drb_connector {
  mac_sdu_rx_adapter              mac_rx_sdu_notifier;
  mac_sdu_tx_adapter              mac_tx_sdu_notifier;
  rlc_f1u_tx_sdu_adapter          rlc_rx_sdu_notif;
  rlc_f1u_tx_data_notifier        rlc_tx_data_notif;
  rlc_tx_control_notifier         rlc_tx_ctrl_notif;
  rlc_tx_mac_buffer_state_updater rlc_tx_buffer_state_notif;
  f1u_rx_rlc_sdu_adapter          f1u_rx_sdu_notif;

  /// \brief Connect MAC, RLC and F1AP layers if bearer is a DRB.
  void connect(du_ue_index_t                       ue_index,
               drb_id_t                            drb_id,
               lcid_t                              lcid,
               f1u_bearer&                         f1_bearer,
               rlc_entity&                         rlc_bearer,
               mac_ue_control_information_handler& mac_ue_info_handler);
};

/// \brief SRB instance in DU manager. It contains SRB configuration information, RLC entity and adapters between
/// layers.
struct du_ue_srb {
  srb_id_t                    srb_id;
  rlc_config                  rlc_cfg;
  std::unique_ptr<rlc_entity> rlc_bearer;
  du_srb_connector            connector;

  lcid_t lcid() const { return srb_id_to_lcid(srb_id); }
};

/// \brief DRB instance in DU manager. It contains DRB configuration information, RLC entity and adapters between
/// layers.
struct du_ue_drb {
  drb_id_t                                                      drb_id;
  lcid_t                                                        lcid;
  std::vector<up_transport_layer_info>                          uluptnl_info_list;
  std::vector<up_transport_layer_info>                          dluptnl_info_list;
  rlc_config                                                    rlc_cfg;
  std::unique_ptr<rlc_entity>                                   rlc_bearer;
  std::unique_ptr<f1u_bearer, std::function<void(f1u_bearer*)>> drb_f1u;
  du_drb_connector                                              connector;
};

/// \brief Creates a DRB instance.
std::unique_ptr<du_ue_drb> create_drb(du_ue_index_t                       ue_index,
                                      du_cell_index_t                     pcell_index,
                                      drb_id_t                            drb_id,
                                      lcid_t                              lcid,
                                      const rlc_config&                   rlc_cfg,
                                      span<const up_transport_layer_info> uluptnl_info_list,
                                      const du_manager_params&            du_params);

/// \brief Bearer container for a UE object in the DU manager.
class du_ue_bearer_manager
{
public:
  du_ue_srb& add_srb(srb_id_t srb_id, const rlc_config& rlc_cfg);
  void       add_drb(std::unique_ptr<du_ue_drb> drb);

  std::unique_ptr<du_ue_drb> remove_drb(drb_id_t drb_id);

  const slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>& srbs() const { return srbs_; }
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>&       srbs() { return srbs_; }
  const std::map<drb_id_t, std::unique_ptr<du_ue_drb>>&      drbs() const { return drbs_; };

  optional<lcid_t> allocate_lcid() const;

private:
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS> srbs_;
  std::map<drb_id_t, std::unique_ptr<du_ue_drb>>      drbs_;
};

} // namespace srs_du
} // namespace srsran
