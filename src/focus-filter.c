/*
OBS Focus Filter
Copyright (C) 2026 ycookiey <70356861+ycookiey@users.noreply.github.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <plugin-support.h>

#include <windows.h>
#include <math.h>

#include <util/dstr.h>
#include <util/windows/window-helpers.h>

/* フォアグラウンド判定の周期(秒)。GetForegroundWindow と EnumWindows は
 * この頻度なら無視できる負荷 */
#define CHECK_INTERVAL 0.1f

enum focus_match_mode {
	/* キャプチャ対象ウィンドウ(とその所有ダイアログ)がフォアグラウンドの時のみ表示 */
	MATCH_WINDOW = 0,
	/* 同じプロセスのウィンドウがフォアグラウンドなら表示 */
	MATCH_PROCESS = 1,
};

struct focus_filter {
	obs_source_t *context;
	enum focus_match_mode mode;
	bool show;
	float elapsed;

	/* フェード。alpha は 0(透明)〜1(不透明) を fade_ms かけて遷移する */
	float fade_ms;
	float alpha;
	gs_effect_t *effect;
	gs_eparam_t *opacity_param;
};

static const char *ff_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("FocusFilter");
}

static void ff_update(void *data, obs_data_t *settings)
{
	struct focus_filter *f = data;
	f->mode = (enum focus_match_mode)obs_data_get_int(settings, "match_mode");
	f->fade_ms = (float)obs_data_get_int(settings, "fade_ms");
}

static void *ff_create(obs_data_t *settings, obs_source_t *context)
{
	struct focus_filter *f = bzalloc(sizeof(struct focus_filter));
	f->context = context;
	/* 初回判定までの1フレームに意図しない映像が出ないよう非表示で開始 */
	f->show = false;
	f->alpha = 0.0f;

	char *effect_path = obs_module_file("focus-fade.effect");
	if (effect_path) {
		obs_enter_graphics();
		f->effect = gs_effect_create_from_file(effect_path, NULL);
		obs_leave_graphics();
		bfree(effect_path);
	}
	if (f->effect) {
		f->opacity_param = gs_effect_get_param_by_name(f->effect, "opacity");
	} else {
		obs_log(LOG_WARNING, "failed to load focus-fade.effect; fade disabled");
	}

	ff_update(f, settings);
	return f;
}

static void ff_destroy(void *data)
{
	struct focus_filter *f = data;
	if (f->effect) {
		obs_enter_graphics();
		gs_effect_destroy(f->effect);
		obs_leave_graphics();
	}
	bfree(f);
}

/* 親ソースの window 設定("タイトル:クラス:exe")からキャプチャ対象の HWND を
 * OBS本体と同じロジック(ms_find_window)で解決する。設定が無ければ NULL */
static HWND resolve_target_window(obs_source_t *parent, bool *has_setting)
{
	obs_data_t *settings = obs_source_get_settings(parent);
	const char *window = obs_data_get_string(settings, "window");
	enum window_priority priority = (enum window_priority)obs_data_get_int(settings, "priority");

	if (!window || !*window) {
		*has_setting = false;
		obs_data_release(settings);
		return NULL;
	}
	*has_setting = true;

	char *wclass = NULL;
	char *title = NULL;
	char *exe = NULL;
	ms_build_window_strings(window, &wclass, &title, &exe);

	HWND target = ms_find_window(INCLUDE_MINIMIZED, priority, wclass, title, exe);

	bfree(wclass);
	bfree(title);
	bfree(exe);
	obs_data_release(settings);
	return target;
}

static void update_show_state(struct focus_filter *f)
{
	obs_source_t *parent = obs_filter_get_parent(f->context);
	if (!parent) {
		f->show = false;
		return;
	}

	bool has_setting = false;
	HWND target = resolve_target_window(parent, &has_setting);

	if (!has_setting) {
		/* ウィンドウ指定を持たない親(画面キャプチャや
		 * フルスクリーンモードのゲームキャプチャ等)には干渉しない */
		f->show = true;
		return;
	}

	if (!target) {
		f->show = false;
		return;
	}

	HWND fg = GetForegroundWindow();
	if (!fg) {
		f->show = false;
		return;
	}

	/* UWPアプリはフレームウィンドウが前面に来るため実体に読み替える */
	if (ms_is_uwp_window(fg)) {
		HWND actual = ms_get_uwp_actual_window(fg);
		if (actual)
			fg = actual;
	}

	if (f->mode == MATCH_PROCESS) {
		DWORD target_pid = 0;
		DWORD fg_pid = 0;
		GetWindowThreadProcessId(target, &target_pid);
		GetWindowThreadProcessId(fg, &fg_pid);
		f->show = target_pid != 0 && target_pid == fg_pid;
	} else {
		f->show = fg == target || GetAncestor(fg, GA_ROOTOWNER) == target;
	}
}

static void ff_tick(void *data, float seconds)
{
	struct focus_filter *f = data;

	f->elapsed += seconds;
	if (f->elapsed >= CHECK_INTERVAL) {
		f->elapsed = 0.0f;
		update_show_state(f);
	}

	float target = f->show ? 1.0f : 0.0f;
	if (f->fade_ms <= 0.0f || !f->effect) {
		f->alpha = target;
	} else if (f->alpha != target) {
		float step = seconds * 1000.0f / f->fade_ms;
		if (f->alpha < target)
			f->alpha = fminf(target, f->alpha + step);
		else
			f->alpha = fmaxf(target, f->alpha - step);
	}
}

static void ff_render(void *data, gs_effect_t *effect)
{
	struct focus_filter *f = data;
	UNUSED_PARAMETER(effect);

	if (f->alpha >= 1.0f) {
		obs_source_skip_video_filter(f->context);
		return;
	}
	if (f->alpha <= 0.0f)
		return; /* 何も描画しない = 透明 */

	/* フェード中: 不透明度を掛けて描画 */
	if (!obs_source_process_filter_begin(f->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING))
		return;
	gs_effect_set_float(f->opacity_param, f->alpha);
	obs_source_process_filter_end(f->context, f->effect, 0, 0);
}

static obs_properties_t *ff_properties(void *data)
{
	UNUSED_PARAMETER(data);

	obs_properties_t *props = obs_properties_create();

	obs_property_t *p = obs_properties_add_list(props, "match_mode", obs_module_text("MatchMode"),
						    OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(p, obs_module_text("MatchMode.Window"), MATCH_WINDOW);
	obs_property_list_add_int(p, obs_module_text("MatchMode.Process"), MATCH_PROCESS);

	obs_property_t *fade =
		obs_properties_add_int_slider(props, "fade_ms", obs_module_text("FadeDuration"), 0, 2000, 10);
	obs_property_int_set_suffix(fade, " ms");

	return props;
}

static void ff_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, "match_mode", MATCH_WINDOW);
	obs_data_set_default_int(settings, "fade_ms", 0);
}

struct obs_source_info focus_filter_info = {
	.id = "focus_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = ff_get_name,
	.create = ff_create,
	.destroy = ff_destroy,
	.update = ff_update,
	.get_defaults = ff_defaults,
	.get_properties = ff_properties,
	.video_tick = ff_tick,
	.video_render = ff_render,
};
