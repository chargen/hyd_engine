/**
 * \file property.h
 */

#ifndef HYD_PROPERTY_H
#define HYD_PROPERTY_H

#include <stdint.h>
#include <jansson.h>

#undef BOOL

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
		HYD_PROP_NUMBER,
		HYD_PROP_STRING,
		HYD_PROP_BOOL
	} type;
	union {
		float n;
		uint8_t b;
		char *s;
	} value;
	struct hyd_property *next;
};

/**
 * \return A new number property
 */
struct hyd_property *hyd_property_create_number(float v, const char *n);

/**
 * \return A new string property
 */
struct hyd_property *hyd_property_create_string(const char *v, const char *n);

/**
 * \return A new boolean property
 */
struct hyd_property *hyd_property_create_bool(uint8_t v, const char *n);

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
uint8_t hyd_property_create_json(struct hyd_property *l, json_t *root);

#endif
