/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __XIAOMI_RGB_NOTIFIER_H__
#define __XIAOMI_RGB_NOTIFIER_H__

#ifdef XIAOMI_RGB_NOTIFIER

#include <utils/RefBase.h>
#include <utils/StrongPointer.h>

#include <memory>
#include <utility>

namespace sdm {

// Xiaomi's under-display light sensor subtracts the light emitted by the panel
// above it, which it measures from a concurrent writeback of the screen. On
// stock, the composer has Xiaomi's libclient2slpi.notifier request one after
// every frame of the primary display. The library is optional and is loaded at
// runtime, so that the composer does not link against a device blob.
class XiaomiRGBNotifier {
 public:
  static std::unique_ptr<XiaomiRGBNotifier> Create();

  void SendFrameUpdate(bool is_primary);

 private:
  using SendFrameUpdateFn = void (*)(void *notifier, bool is_primary);

  XiaomiRGBNotifier(android::sp<android::RefBase> notifier, SendFrameUpdateFn send_frame_update)
      : notifier_(std::move(notifier)), send_frame_update_(send_frame_update) {}

  android::sp<android::RefBase> notifier_;
  SendFrameUpdateFn send_frame_update_;
};

}  // namespace sdm

#endif  // XIAOMI_RGB_NOTIFIER

#endif  // __XIAOMI_RGB_NOTIFIER_H__
