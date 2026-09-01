#pragma once

#include "u2f_app.h"
#include "scenes/u2f_scene.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include "views/u2f_view.h"
#include "u2f_hid.h"
#include "u2f.h"

typedef enum {
    U2fAppErrorNoFiles,
    U2fAppErrorCloseRpc,
} U2fAppError;

typedef enum {
    U2fCustomEventNone,

    U2fCustomEventConnect,
    U2fCustomEventDisconnect,
    U2fCustomEventDataError,

    U2fCustomEventRegister,
    U2fCustomEventRegisterSuccess,
    U2fCustomEventAuth,
    U2fCustomEventAuthSuccess,
    U2fCustomEventWink,

    U2fCustomEventTimeout,
    U2fCustomEventLockAfterSuccess,
    U2fCustomEventSessionExpired,

    U2fCustomEventConfirm,

    U2fCustomEventErrorBack,

} GpioCustomEvent;

typedef enum {
    U2fAppNotifyConnect = (1U << 0),
    U2fAppNotifyDisconnect = (1U << 1),
    U2fAppNotifyDataError = (1U << 2),
    U2fAppNotifyRegister = (1U << 3),
    U2fAppNotifyRegisterSuccess = (1U << 4),
    U2fAppNotifyAuth = (1U << 5),
    U2fAppNotifyAuthSuccess = (1U << 6),
    U2fAppNotifyWink = (1U << 7),
    U2fAppNotifyAll = (1U << 8) - 1,
} U2fAppNotifyFlag;

typedef enum {
    U2fAppViewError,
    U2fAppViewMain,
} U2fAppView;

struct U2fApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    FuriEventFlag* notification_flags;
    NotificationApp* notifications;
    Widget* widget;
    FuriTimer* timer;
    FuriTimer* success_timer;
    FuriTimer* session_timer;
    U2fHid* u2f_hid;
    U2fView* u2f_view;
    U2fData* u2f_instance;
    GpioCustomEvent event_cur;
    bool u2f_ready;
    bool armed;
    bool lock_after_timeout;
    uint8_t pending_app_id[32];
    U2fAppError error;
};
