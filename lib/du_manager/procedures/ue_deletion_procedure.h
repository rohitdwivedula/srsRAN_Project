
#ifndef SRSGNB_UE_DELECTION_PROCEDURE_H
#define SRSGNB_UE_DELECTION_PROCEDURE_H

#include "../../lib/ran/gnb_format.h"
#include "du_manager_config.h"
#include "du_manager_interfaces.h"
#include "du_ue_context.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/support/async/lazy_task.h"

namespace srsgnb {

class ue_deletion_procedure
{
public:
  ue_deletion_procedure(const du_ue_delete_message& msg_,
                        const du_manager_config_t&  cfg_,
                        ue_manager_ctrl_configurer& ue_mng_) :
    msg(msg_), cfg(cfg_), logger(cfg.logger), ue_mng(ue_mng_)
  {}

  void operator()(coro_context<lazy_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, msg.ue_index, name());

    ue = ue_mng.find_ue(msg.ue_index);
    if (ue == nullptr) {
      log_proc_failure(logger, msg.ue_index, name(), "ueId does not exist.");
      CORO_EARLY_RETURN();
    }

    // 1. Remove UE from MAC
    CORO_AWAIT_VALUE(mac_ue_delete_response_message mac_resp, launch_mac_ue_delete());
    if (not mac_resp.result) {
      log_proc_failure(logger, ue->ue_index, ue->rnti, name(), "Failed to remove UE from MAC.");
    }

    // 2. Remove UE from F1AP
    // TODO

    // 3. Remove UE object from DU UE manager
    ue_mng.remove_ue(msg.ue_index);

    log_proc_completed(logger, msg.ue_index, ue->rnti, name());
    CORO_RETURN();
  }

  static const char* name() { return "UE Delete"; }

private:
  lazy_task<mac_ue_delete_response_message> launch_mac_ue_delete()
  {
    mac_ue_delete_request_message mac_msg{};
    mac_msg.ue_index   = ue->ue_index;
    mac_msg.rnti       = ue->rnti;
    mac_msg.cell_index = ue->pcell_index;
    return cfg.mac->ue_delete_request(mac_msg);
  }

  du_ue_delete_message        msg;
  const du_manager_config_t&  cfg;
  srslog::basic_logger&       logger;
  ue_manager_ctrl_configurer& ue_mng;

  du_ue_context* ue = nullptr;
};

} // namespace srsgnb

#endif // SRSGNB_UE_DELECTION_PROCEDURE_H
