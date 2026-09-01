#include "u2f_view.h"
#include <gui/elements.h>

struct U2fView {
    View* view;
    U2fOkCallback callback;
    void* context;
};

typedef struct {
    U2fViewMsg display_msg;
    char fingerprint[9];
} U2fModel;

static void u2f_view_draw_callback(Canvas* canvas, void* _model) {
    U2fModel* model = _model;

    canvas_set_font(canvas, FontSecondary);

    if(model->display_msg == U2fMsgDisarmed) {
        canvas_draw_str_aligned(canvas, 128 / 2, 3, AlignCenter, AlignTop, "PEPPERKEY LOCKED");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 25, AlignCenter, AlignTop, "USB U2F");
        elements_button_center(canvas, "HOLD TO ARM");
    } else if(model->display_msg == U2fMsgNotConnected) {
        canvas_draw_str_aligned(canvas, 128 / 2, 3, AlignCenter, AlignTop, "ARMED - CONNECT USB");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 25, AlignCenter, AlignTop, "WAITING");
    } else if(model->display_msg == U2fMsgIdle) {
        canvas_draw_str_aligned(canvas, 128 / 2, 3, AlignCenter, AlignTop, "ARMED - WAITING");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 25, AlignCenter, AlignTop, "USB READY");
    } else if(model->display_msg == U2fMsgRegister) {
        canvas_draw_str_aligned(canvas, 128 / 2, 2, AlignCenter, AlignTop, "REGISTER REQUEST");
        canvas_draw_str_aligned(canvas, 128 / 2, 20, AlignCenter, AlignTop, "Site fingerprint");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 32, AlignCenter, AlignTop, model->fingerprint);
        elements_button_center(canvas, "HOLD TO APPROVE");
    } else if(model->display_msg == U2fMsgAuth) {
        canvas_draw_str_aligned(canvas, 128 / 2, 2, AlignCenter, AlignTop, "SIGN-IN REQUEST");
        canvas_draw_str_aligned(canvas, 128 / 2, 20, AlignCenter, AlignTop, "Site fingerprint");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 32, AlignCenter, AlignTop, model->fingerprint);
        elements_button_center(canvas, "HOLD TO APPROVE");
    } else if(model->display_msg == U2fMsgSuccess) {
        canvas_draw_str_aligned(canvas, 128 / 2, 3, AlignCenter, AlignTop, "APPROVED - LOCKING");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 25, AlignCenter, AlignTop, "SUCCESS");
    } else if(model->display_msg == U2fMsgError) {
        canvas_draw_str_aligned(canvas, 128 / 2, 3, AlignCenter, AlignTop, "DATA ERROR - LOCKED");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 128 / 2, 25, AlignCenter, AlignTop, "NO CHANGES MADE");
        elements_button_center(canvas, "HOLD TO RETRY");
    }
}

static bool u2f_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    U2fView* u2f = context;
    bool consumed = false;

    if(event->type == InputTypeLong) {
        if(event->key == InputKeyOk) {
            consumed = true;
            if(u2f->callback != NULL) u2f->callback(InputTypeLong, u2f->context);
        }
    }

    return consumed;
}

U2fView* u2f_view_alloc(void) {
    U2fView* u2f = malloc(sizeof(U2fView));

    u2f->view = view_alloc();
    view_allocate_model(u2f->view, ViewModelTypeLocking, sizeof(U2fModel));
    view_set_context(u2f->view, u2f);
    view_set_draw_callback(u2f->view, u2f_view_draw_callback);
    view_set_input_callback(u2f->view, u2f_view_input_callback);

    with_view_model(
        u2f->view,
        U2fModel * model,
        {
            model->display_msg = U2fMsgDisarmed;
            model->fingerprint[0] = '\0';
        },
        false);

    return u2f;
}

void u2f_view_free(U2fView* u2f) {
    furi_assert(u2f);
    view_free(u2f->view);
    free(u2f);
}

View* u2f_view_get_view(U2fView* u2f) {
    furi_assert(u2f);
    return u2f->view;
}

void u2f_view_set_ok_callback(U2fView* u2f, U2fOkCallback callback, void* context) {
    furi_assert(u2f);
    furi_assert(callback);
    with_view_model(
        u2f->view,
        U2fModel * model,
        {
            UNUSED(model);
            u2f->callback = callback;
            u2f->context = context;
        },
        false);
}

void u2f_view_set_state(U2fView* u2f, U2fViewMsg msg) {
    with_view_model(u2f->view, U2fModel * model, { model->display_msg = msg; }, true);
}

void u2f_view_set_fingerprint(U2fView* u2f, const char* fingerprint) {
    furi_assert(u2f);
    furi_assert(fingerprint);
    with_view_model(
        u2f->view,
        U2fModel * model,
        { strlcpy(model->fingerprint, fingerprint, sizeof(model->fingerprint)); },
        true);
}
