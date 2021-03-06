/**
 * \file frame.h
 */

#ifndef HYD_FRAME_H
#define HYD_FRAME_H

#include <SDL.h>
#include <jansson.h>

/**
 * \struct hyd_frame
 *
 * A frame is a rectangle
 * in a texture.
 */
struct hyd_frame {
	char *name;
	SDL_Rect rect;
};

/**
 * \param[in] name The frame's name
 * \param[in] rect The frame's rectangle
 *
 * \return The new frame
 */
struct hyd_frame *hyd_frame_create(const char *name, const SDL_Rect rect);

/**
 * \brief Create a new frame from a JSON object
 *
 * \param[in] root The JSON object
 *
 * \return The new frame
 */
struct hyd_frame *hyd_frame_create_json(json_t *root);

/**
 * \brief Create frames from a JSON array
 *
 * \param[in] root The JSON array
 * \param[out] num The number of frames created
 *
 * \return The new array of frames
 */
struct hyd_frame **hyd_frame_array_create_json(json_t *root, uint32_t *num);

/**
 * \param[in] frames The list of frames to search through
 * \param[in] num The number of frames
 * \param[in] name The name to search for
 *
 * \return The frame or NULL if not found.
 */
struct hyd_frame *hyd_frame_array_find(struct hyd_frame **frames, uint32_t num, const char *name);

/**
 * \param[in] frame The frame to destroy
 */
void hyd_frame_destroy(struct hyd_frame *frame);

#endif
