/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef XIAOMI_RGB_NOTIFIER

#include "xiaomi_rgb_notifier.h"

#include <dlfcn.h>
#include <utils/debug.h>

#define __CLASS__ "XiaomiRGBNotifier"

namespace sdm {

namespace {

constexpr const char *kLibrary = "libclient2slpi.notifier.so";

// clientRGBNotifier::clientRGBNotifier()
constexpr const char *kConstructor = "_ZN17clientRGBNotifierC1Ev";
// clientRGBNotifier::SendFrameUpdateSingnal(bool)
constexpr const char *kSendFrameUpdate = "_ZN17clientRGBNotifier22SendFrameUpdateSingnalEb";

using ConstructorFn = void (*)(void *notifier);

}  // namespace

std::unique_ptr<XiaomiRGBNotifier> XiaomiRGBNotifier::Create() {
  // The library starts a thread that outlives the notifier, so it stays loaded.
  void *library = dlopen(kLibrary, RTLD_NOW);
  if (!library) {
    DLOGW("Failed to load %s: %s", kLibrary, dlerror());
    return nullptr;
  }

  auto construct = reinterpret_cast<ConstructorFn>(dlsym(library, kConstructor));
  auto send_frame_update = reinterpret_cast<SendFrameUpdateFn>(dlsym(library, kSendFrameUpdate));
  if (!construct || !send_frame_update) {
    DLOGW("%s does not export the notifier", kLibrary);
    return nullptr;
  }

  // clientRGBNotifier derives from android::RefBase without adding members, and
  // its virtual destructor releases it with operator delete once the last
  // strong reference goes away.
  void *storage = ::operator new(sizeof(android::RefBase));
  construct(storage);
  android::sp<android::RefBase> notifier = static_cast<android::RefBase *>(storage);

  return std::unique_ptr<XiaomiRGBNotifier>(
      new XiaomiRGBNotifier(std::move(notifier), send_frame_update));
}

void XiaomiRGBNotifier::SendFrameUpdate(bool is_primary) {
  send_frame_update_(notifier_.get(), is_primary);
}

}  // namespace sdm

#endif  // XIAOMI_RGB_NOTIFIER
