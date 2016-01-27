//
// Copyright (C) 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef UPDATE_ENGINE_BINDER_SERVICE_H_
#define UPDATE_ENGINE_BINDER_SERVICE_H_

#include <utils/Errors.h>

#include <vector>

#include <utils/RefBase.h>

#include "update_engine/common_service.h"
#include "update_engine/parcelable_update_engine_status.h"

#include "android/brillo/BnUpdateEngine.h"
#include "android/brillo/IUpdateEngineStatusCallback.h"

namespace chromeos_update_engine {

class BinderUpdateEngineService : public android::brillo::BnUpdateEngine {
 public:
  BinderUpdateEngineService(SystemState* system_state)
      : common_(new UpdateEngineService(system_state)) {}
  virtual ~BinderUpdateEngineService() = default;

  void SendStatusUpdate(int64_t in_last_checked_time, double in_progress,
                        const std::string& in_current_operation,
                        const std::string& in_new_version, int64_t in_new_size);

  // android::brillo::BnUpdateEngine overrides.
  android::binder::Status AttemptUpdate(const android::String16& app_version,
                                        const android::String16& omaha_url,
                                        int flags) override;
  android::binder::Status AttemptRollback(bool powerwash) override;
  android::binder::Status CanRollback(bool* out_can_rollback) override;
  android::binder::Status ResetStatus() override;
  android::binder::Status GetStatus(
      android::brillo::ParcelableUpdateEngineStatus* status);
  android::binder::Status RebootIfNeeded() override;
  android::binder::Status SetChannel(const android::String16& target_channel,
                                     bool powerwash) override;
  android::binder::Status GetChannel(bool get_current_channel,
                                     android::String16* out_channel) override;
  android::binder::Status SetP2PUpdatePermission(bool enabled) override;
  android::binder::Status GetP2PUpdatePermission(
      bool* out_p2p_permission) override;
  android::binder::Status SetUpdateOverCellularPermission(
      bool enabled) override;
  android::binder::Status GetUpdateOverCellularPermission(
      bool* out_cellular_permission) override;
  android::binder::Status GetDurationSinceUpdate(
      int64_t* out_duration) override;
  android::binder::Status GetPrevVersion(
      android::String16* out_prev_version) override;
  android::binder::Status GetRollbackPartition(
      android::String16* out_rollback_partition) override;
  android::binder::Status RegisterStatusCallback(
      const android::sp<android::brillo::IUpdateEngineStatusCallback>& callback)
      override;

 private:
  // Generic function for dispatching to the common service.
  template<typename... Parameters, typename... Arguments>
  android::binder::Status CallCommonHandler(
      bool (UpdateEngineService::*Handler)(brillo::ErrorPtr*, Parameters...),
      Arguments... arguments);

  // To be used as a death notification handler only.
  void UnregisterStatusCallback(
      android::brillo::IUpdateEngineStatusCallback* callback);

  std::unique_ptr<UpdateEngineService> common_;

  std::vector<android::sp<android::brillo::IUpdateEngineStatusCallback>>
      callbacks_;
};  // class BinderService

}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_BINDER_SERVICE_H_