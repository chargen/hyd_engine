/**
 * \file property.h
 */

#ifndef HYD_PROPERTY_H
#define HYD_PROPERTY_H

#include "list.h"
#include <stdint.h>
#include <jansson.h>

/**
 * \struct hyd_property
 *
 * A property is value with a name,
 * that are intended to be set by name,
 * and get by name
 */
struct hyd_property {
	char *name;
	enum {
		NUMBER,
		STRING,
		BOOL
	} type;
	union {
		float n;
		uint8_t b;
		char *s;
	} value;
	struct hyd_list list;
};

/**
 * \return A new number property
 */
struct hyd_property *hyd_property_create_number(float value, const char *name);

/**
 * \return A new string property
 */
struct hyd_property *hyd_property_create_string(const char *value, const char *name);

/**
 * \return A new boolean property
 */
struct hyd_property *hyd_property_create_bool(uint8_t value, const char *name);

/**
 * \param[in] property The property to destroy
 */
void hyd_property_destroy(struct hyd_property *property);

/**
 * \brief Fill a list of properties from a JSON object
 *
 * \param[in] list List to fill
 * \param[in] root The JSON object
 *
 * \return 0 if success. Non-zero if failure
 */
uint8_t hyd_property_list_create_json(struct hyd_list *list, json_t *root);

#endif