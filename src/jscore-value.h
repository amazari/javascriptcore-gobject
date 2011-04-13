/*
 * jscore-value.h - Header for JscoreValue
 *
 * Copyright (C) 2010 Igalia S.L.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __JSCORE_VALUE_H__
#define __JSCORE_VALUE_H__

#include "jscore-context.h"

typedef gpointer JSCoreValue;


JSCoreValue *jscore_value_new_null (JSCoreContext *context);
JSCoreValue *jscore_value_new_undefined (JSCoreContext *context);
JSCoreValue *jscore_value_new_string (JSCoreContext *context, const gchar *string);
JSCoreValue *jscore_value_new_number (JSCoreContext *context, gdouble number);
JSCoreValue *jscore_value_new_boolean (JSCoreContext *context, gboolean boolean);
JSCoreValue *jscore_value_new_json (JSCoreContext *context, const gchar *json);

//JscoreValue *jscore_value_new_null (JSCoreContext *context);
//JscoreValue *jscore_value_new_undefined (JSCoreContext *context, gboolean boolean);
gchar *jscore_value_get_string (JSCoreContext *context,JSCoreValue *value);
gdouble jscore_value_get_number (JSCoreContext *context, JSCoreValue *value);
gboolean jscore_value_get_boolean (JSCoreContext *context, JSCoreValue *value);
gchar *jscore_value_get_json (JSCoreContext *context, const JSCoreValue *value);

int jscore_value_get_type (JSCoreContext *context, const JSCoreValue *value);


gboolean jscore_value_is_boolean (JSCoreContext *context, JSCoreValue *value);
gboolean jscore_value_is_number (JSCoreContext *context, JSCoreValue *value);
gboolean jscore_value_is_null (JSCoreContext *context, JSCoreValue *value);
gboolean jscore_value_is_string (JSCoreContext *context,JSCoreValue *value);
gboolean jscore_value_is_undefined (JSCoreContext *context, JSCoreValue *value);
gboolean jscore_value_is_object (JSCoreContext *context, JSCoreValue *value);
gboolean jscore_value_is_object_of_class (JSCoreContext *context, JSCoreValue *value, JSCoreClass *class);
gboolean jscore_value_is_equal (JSCoreContext *context, JSCoreValue *value, JSCoreValue *value2);
void jscore_value_ref (JSCoreContext *context, JSCoreValue *value);
void jscore_value_unref (JSCoreContext *context, JSCoreValue *value);


JSCoreValue *jscore_value_new_variant (JSCoreContext *context, GVariant * gval, GError **error);
GVariant *jscore_value_to_variant (JSCoreValue *value,JSCoreContext *context);
#endif /* __JSCORE_VALUE_H__ */
