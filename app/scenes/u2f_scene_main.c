#include "../u2f_app_i.h"
#include "../views/u2f_view.h"
#include <dolphin/dolphin.h>
#include <furi_hal.h>
#include "../u2f.h"

#define U2F_REQUEST_TIMEOUT 10000
#define U2F_SUCCESS_TIMEOUT 3000
#define U2F_SESSION_TIMEOUT 120000

static void u2f_scene_main_ok_callback(InputType type, void* context) {
    UNUSED(type);
    furi_assert(context);
    U2fApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, U2fCustomEventConfirm);
}

static void u2f_scene_main_event_callback(U2fNotifyEvent evt, void* context) {
    furi_assert(context);
    U2fApp* app = context;
    if(evt == U2fNotifyRegister)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyRegister);
    else if(evt == U2fNotifyRegisterSuccess)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyRegisterSuccess);
    else if(evt == U2fNotifyAuth)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyAuth);
    else if(evt == U2fNotifyAuthSuccess)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyAuthSuccess);
    else if(evt == U2fNotifyWink)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyWink);
    else if(evt == U2fNotifyConnect)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyConnect);
    else if(evt == U2fNotifyDisconnect)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyDisconnect);
    else if(evt == U2fNotifyError)
        furi_event_flag_set(app->notification_flags, U2fAppNotifyDataError);
}

static void u2f_scene_main_timer_callback(void* context) {
    furi_assert(context);
    U2fApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, U2fCustomEventTimeout);
}

static void u2f_scene_main_session_timer_callback(void* context) {
    furi_assert(context);
    U2fApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, U2fCustomEventSessionExpired);
}

static void u2f_scene_main_success_timer_callback(void* context) {
    furi_assert(context);
    U2fApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, U2fCustomEventLockAfterSuccess);
}

static void u2f_scene_main_stop_session(U2fApp* app) {
    furi_timer_stop(app->timer);
    furi_timer_stop(app->success_timer);
    furi_timer_stop(app->session_timer);

    if(app->u2f_hid) {
        u2f_hid_stop(app->u2f_hid);
        app->u2f_hid = NULL;
    }
    if(app->u2f_instance) {
        u2f_free(app->u2f_instance);
        app->u2f_instance = NULL;
    }

    app->event_cur = U2fCustomEventNone;
    app->u2f_ready = false;
    app->armed = false;
    app->lock_after_timeout = false;
    memset(app->pending_app_id, 0, sizeof(app->pending_app_id));
    notification_message_block(app->notifications, &sequence_reset_rgb);
    u2f_view_set_state(app->u2f_view, U2fMsgDisarmed);
}

static bool u2f_scene_main_start_session(U2fApp* app) {
    if(app->armed) return true;

    app->u2f_instance = u2f_alloc();
    app->u2f_ready = u2f_init(app->u2f_instance);
    if(!app->u2f_ready) {
        u2f_free(app->u2f_instance);
        app->u2f_instance = NULL;
        notification_message(app->notifications, &sequence_set_red_255);
        u2f_view_set_state(app->u2f_view, U2fMsgError);
        return false;
    }

    u2f_set_event_callback(app->u2f_instance, u2f_scene_main_event_callback, app);
    app->armed = true;
    app->u2f_hid = u2f_hid_start(app->u2f_instance);
    app->event_cur = U2fCustomEventNone;
    app->lock_after_timeout = false;
    furi_timer_start(app->session_timer, U2F_SESSION_TIMEOUT);
    notification_message(app->notifications, &sequence_single_vibro);
    u2f_view_set_state(app->u2f_view, U2fMsgNotConnected);
    return true;
}

static void u2f_scene_main_show_request(U2fApp* app, uint32_t event) {
    char fingerprint_text[9];

    if(app->lock_after_timeout) return;

    furi_timer_start(app->timer, U2F_REQUEST_TIMEOUT);
    app->lock_after_timeout = false;
    if(app->event_cur == U2fCustomEventNone) {
        app->event_cur = event;
        if(!u2f_get_pending_app_id(app->u2f_instance, app->pending_app_id)) {
            app->event_cur = U2fCustomEventNone;
            return;
        }
        snprintf(
            fingerprint_text,
            sizeof(fingerprint_text),
            "%02X%02X%02X%02X",
            app->pending_app_id[0],
            app->pending_app_id[1],
            app->pending_app_id[2],
            app->pending_app_id[3]);
        u2f_view_set_fingerprint(app->u2f_view, fingerprint_text);
        u2f_view_set_state(
            app->u2f_view, event == U2fCustomEventRegister ? U2fMsgRegister : U2fMsgAuth);
        notification_message(app->notifications, &sequence_display_backlight_on);
        notification_message(app->notifications, &sequence_single_vibro);
    }
    notification_message(app->notifications, &sequence_blink_magenta_10);
}

bool u2f_scene_main_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    U2fApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(!app->armed && (event.event >= U2fCustomEventConnect) &&
           (event.event <= U2fCustomEventWink)) {
            return true;
        }
        if(event.event == U2fCustomEventConnect) {
            if(app->lock_after_timeout) return true;
            furi_timer_stop(app->timer);
            app->event_cur = U2fCustomEventNone;
            memset(app->pending_app_id, 0, sizeof(app->pending_app_id));
            u2f_view_set_state(app->u2f_view, U2fMsgIdle);
        } else if(event.event == U2fCustomEventDisconnect) {
            if(app->lock_after_timeout) return true;
            furi_timer_stop(app->timer);
            app->event_cur = U2fCustomEventNone;
            memset(app->pending_app_id, 0, sizeof(app->pending_app_id));
            if(app->armed) u2f_view_set_state(app->u2f_view, U2fMsgNotConnected);
        } else if((event.event == U2fCustomEventRegister) || (event.event == U2fCustomEventAuth)) {
            u2f_scene_main_show_request(app, event.event);
        } else if(event.event == U2fCustomEventWink) {
            notification_message(app->notifications, &sequence_blink_magenta_10);
        } else if(
            (event.event == U2fCustomEventRegisterSuccess) ||
            (event.event == U2fCustomEventAuthSuccess)) {
            notification_message_block(app->notifications, &sequence_set_green_255);
            dolphin_deed(DolphinDeedU2fAuthorized);
            app->event_cur = U2fCustomEventNone;
            memset(app->pending_app_id, 0, sizeof(app->pending_app_id));
            app->lock_after_timeout = true;
            furi_timer_stop(app->timer);
            furi_timer_start(app->success_timer, U2F_SUCCESS_TIMEOUT);
            u2f_view_set_state(app->u2f_view, U2fMsgSuccess);
        } else if(event.event == U2fCustomEventTimeout) {
            if(!app->armed || !app->u2f_instance) return true;
            if(!app->lock_after_timeout) {
                notification_message_block(app->notifications, &sequence_reset_rgb);
                u2f_cancel_pending_request(app->u2f_instance, app->pending_app_id);
                app->event_cur = U2fCustomEventNone;
                memset(app->pending_app_id, 0, sizeof(app->pending_app_id));
                if(app->armed) u2f_view_set_state(app->u2f_view, U2fMsgIdle);
            }
        } else if(event.event == U2fCustomEventLockAfterSuccess) {
            u2f_scene_main_stop_session(app);
        } else if(event.event == U2fCustomEventSessionExpired) {
            u2f_scene_main_stop_session(app);
        } else if(event.event == U2fCustomEventConfirm) {
            if(!app->armed) {
                u2f_scene_main_start_session(app);
            } else if(app->event_cur != U2fCustomEventNone) {
                u2f_confirm_user_present(app->u2f_instance, app->pending_app_id);
            }
        } else if(event.event == U2fCustomEventDataError) {
            u2f_scene_main_stop_session(app);
            notification_message(app->notifications, &sequence_set_red_255);
            u2f_view_set_state(app->u2f_view, U2fMsgError);
        }
        consumed = true;
    }

    return consumed;
}

void u2f_scene_main_on_enter(void* context) {
    U2fApp* app = context;

    app->timer = furi_timer_alloc(u2f_scene_main_timer_callback, FuriTimerTypeOnce, app);
    app->success_timer =
        furi_timer_alloc(u2f_scene_main_success_timer_callback, FuriTimerTypeOnce, app);
    app->session_timer =
        furi_timer_alloc(u2f_scene_main_session_timer_callback, FuriTimerTypeOnce, app);
    u2f_view_set_ok_callback(app->u2f_view, u2f_scene_main_ok_callback, app);
    u2f_view_set_state(app->u2f_view, U2fMsgDisarmed);

    view_dispatcher_switch_to_view(app->view_dispatcher, U2fAppViewMain);
}

void u2f_scene_main_on_exit(void* context) {
    U2fApp* app = context;
    u2f_scene_main_stop_session(app);
    furi_timer_free(app->session_timer);
    app->session_timer = NULL;
    furi_timer_free(app->success_timer);
    app->success_timer = NULL;
    furi_timer_free(app->timer);
    app->timer = NULL;
}
