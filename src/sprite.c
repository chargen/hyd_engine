#include "sprite.h"
#include <physfs.h>
#include <SDL.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "transform.h"
#include "engine.h"

struct hyd_spr *hyd_spr_create(struct hyd_tex *tex, struct hyd_frame **frames,
		uint32_t num_frames, struct hyd_anim **anims,
		uint32_t num_anims)
{
	struct hyd_spr *spr = malloc(sizeof(*spr));
	if (spr == NULL) {
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Failed to allocate data for sprite"
				);
		return NULL;
	}

	spr->tex = tex;
	spr->frames = frames;
	spr->anims = anims;
	spr->num_frames = num_frames;
	spr->num_anims = num_anims;

	return spr;
}

struct hyd_spr *hyd_spr_create_json(json_t *root)
{
	uint32_t i = 0;
	struct hyd_tex *tex = NULL;
	struct hyd_frame **frames = NULL;
	struct hyd_anim **anims = NULL;
	uint32_t num_frames;
	uint32_t num_anims;
	json_t *iter_json;
	struct hyd_spr *ret;

	if (!json_is_object(root)) {
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Invalid sprite file format\n"
				);
		return NULL;
	}

	iter_json = json_object_get(root, "img");
	if (!json_is_string(iter_json)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_APPLICATION,
			"Could not find 'img' value\n"
			);
		return NULL;
	}

	struct hyd_tex_list *tex_l = hyd_engine_get_tex_list();
	tex = hyd_tex_list_find(tex_l, json_string_value(iter_json));
	if (tex == NULL) {
		tex = hyd_tex_create_file(json_string_value(iter_json));
		struct hyd_tex_list *l = malloc(sizeof(*l));
		l->tex = tex;
		l->next = tex_l->next;
		tex_l->next = l;
	}

	iter_json = json_object_get(root, "frames");
	if (!json_is_array(iter_json)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_APPLICATION,
			"Could not find 'frames' array\n"
			);
		return NULL;
	}

	frames = hyd_frame_array_create_json(iter_json, &num_frames);

	iter_json = json_object_get(root, "animations");
	if (json_is_array(iter_json))
		anims = hyd_anim_array_create_json(iter_json,
				frames, num_frames, &num_anims);

	ret = hyd_spr_create(hyd_tex_copy(tex),
			frames, num_frames, anims, num_anims);

	iter_json = json_object_get(root, "default_frame");
	if (json_is_string(iter_json))
		ret->default_frame = hyd_frame_array_find(ret->frames, ret->num_frames, json_string_value(iter_json));

	iter_json = json_object_get(root, "default_animation");
	if (json_is_string(iter_json))
		ret->default_anim = hyd_anim_array_find(ret->anims, ret->num_anims, json_string_value(iter_json));


	json_decref(root);
	return ret;
}

struct hyd_spr *hyd_spr_create_file(const char *fname)
{
	PHYSFS_sint64 file_len = 0;
	uint8_t *buf = NULL;
	json_t *root = NULL;
	json_error_t err;

	file_len = hyd_fs_read_buffer(fname, &buf);
	if (file_len == 0) {
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Failed to read sprite from file: '%s'\n",
				fname
				);
		return NULL;
	}

	//SDL_Log("Reading sprite - %s - size: %i", fname, file_len);

	root = json_loadb(buf, file_len, 0, &err);
	free(buf);

	if (root == NULL) {
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"JSON error for file '%s' - line: %d - message: %s.",
				fname, err.line, err.text
				);
		return NULL;
	}

	return hyd_spr_create_json(root);
}

void hyd_spr_destroy(struct hyd_spr *s)
{
	if (s == NULL)
		return;

	uint32_t i;
	for (i = 0; i < s->num_anims; i++)
		hyd_anim_destroy(s->anims[i]);

	free(s->anims);

	for (i = 0; i < s->num_frames; i++)
		hyd_frame_destroy(s->frames[i]);

	free(s->frames);

	hyd_tex_destroy(s->tex);
	free(s);
}

void hyd_spr_draw_point(struct hyd_spr *spr, struct hyd_frame *frame, SDL_Point point)
{
	if (spr == NULL || frame == NULL)
		return;

	SDL_Rect rect;
	rect.x = point.x;
	rect.y = point.y;
	rect.w = frame->rect.w;
	rect.h = frame->rect.h;

	struct hyd_quad quad = {
		point.x, point.y,
		point.x + frame->rect.w,
		point.y + frame->rect.h
	};

	struct hyd_color col = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	
	struct hyd_quad uv = {
		((float)frame->rect.x) / ((float)spr->tex->w),
		((float)frame->rect.y) / ((float)spr->tex->h),
		((float)frame->rect.x + frame->rect.w) / ((float)spr->tex->w),
		((float)frame->rect.y + frame->rect.h) / ((float)spr->tex->h)
	};

	hyd_quad_tex_draw(&quad, &col, spr->tex, &uv);
}

struct hyd_spr *hyd_spr_copy(struct hyd_spr *s) {
	uint32_t i,j;
	struct hyd_spr *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;

	memcpy(ret, s, sizeof(*s));
	ret->tex = hyd_tex_copy(s->tex);

	ret->frames = calloc(s->num_frames, sizeof(struct hyd_frame));
	ret->num_frames = s->num_frames;
	for (i = 0; i < ret->num_frames; i++) {
		ret->frames[i] = malloc(sizeof(struct hyd_frame));
		memcpy(ret->frames[i], s->frames[i], sizeof(struct hyd_frame));
		ret->frames[i]->name = malloc(strlen(s->frames[i]->name) + 1);
		strcpy(ret->frames[i]->name, s->frames[i]->name);
	}

	ret->anims = calloc(s->num_anims, sizeof(struct hyd_anim));
	ret->num_anims = s->num_anims;
	for (i = 0; i < ret->num_anims; i++) {
		ret->anims[i] = malloc(sizeof(struct hyd_anim));
		memcpy(ret->anims[i], s->anims[i], sizeof(struct hyd_anim));
		ret->anims[i]->name = malloc(strlen(s->anims[i]->name) + 1);
		strcpy(ret->anims[i]->name, s->anims[i]->name);

		ret->anims[i]->frames = calloc(ret->anims[i]->num_frames, sizeof(struct hyd_anim));
		ret->anims[i]->num_frames = 0;
	}

	return ret;
}
