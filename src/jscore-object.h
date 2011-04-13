/*
 * jscore-object.h - Header for JSCoreObject
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

#ifndef __JSCORE_OBJECT_H__
#define __JSCORE_OBJECT_H__

#include "jscore-value.h"
#include "jscore-context.h"
#include "jscore-class.h"

#include <glib-object.h>


G_BEGIN_DECLS

#define JSCORE_TYPE_OBJECT                      \
  (jscore_object_get_type())
#define JSCORE_OBJECT(obj)                              \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               JSCORE_TYPE_OBJECT,      \
                               JSCoreObject))
#define JSCORE_OBJECT_CLASS(klass)              \
  (G_TYPE_CHECK_CLASS_CAST ((klass),            \
                            JSCORE_TYPE_OBJECT, \
                            JSCoreObjectClass))
#define IS_JSCORE_OBJECT(obj)                           \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               JSCORE_TYPE_OBJECT))
#define IS_JSCORE_OBJECT_CLASS(klass)                   \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            JSCORE_TYPE_OBJECT))
#define JSCORE_OBJECT_GET_CLASS(obj)                    \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              JSCORE_TYPE_OBJECT,       \
                              JSCoreObjectClass))

typedef struct _JSCoreObject      JSCoreObject;
typedef struct _JSCoreObjectClass JSCoreObjectClass;
typedef struct _JSCoreObjectPrivate JSCoreObjectPrivate;

/*!
@enum JSPropertyAttribute
@constant kJSPropertyAttributeNone         Specifies that a property has no special attributes.
@constant kJSPropertyAttributeReadOnly     Specifies that a property is read-only.
@constant kJSPropertyAttributeDontEnum     Specifies that a property should not be enumerated by JSPropertyEnumerators and JavaScript for...in loops.
@constant kJSPropertyAttributeDontDelete   Specifies that the delete operation should fail on a property.
*/
enum {
  JS_CORE_PROPERTY_ATTRIBUTE_NONE  = 0,
  JS_CORE_PROPERTY_ATTRIBUTE_READONLY = 1 << 1,
  JS_CORE_PROPERTY_ATTRIBUTE_DONTENUM  = 1 << 2,
  JS_CORE_PROPERTY_ATTRIBUTE_DONTDELETE  = 1 << 3
};

/*!
@typedef JSPropertyAttributes
@abstract A set of JSPropertyAttributes. Combine multiple attributes by logically ORing them together.
*/
typedef unsigned JSCorePropertyAttributes;

typedef JSCoreObject*
(*JSCoreObjectCallAsConstructorCallback) (JSCoreObject *constructor, GVariant *arguments);

struct _JSCoreObjectClass
{
  GObjectClass parent_class;
};

struct _JSCoreObject
{
  GObject parent;
  JSCoreObjectPrivate *priv;
};

GType jscore_object_get_type (void) G_GNUC_CONST;

JSCoreObject *jscore_object_new (JSCoreContext *ctx, JSCoreClass *jsClass, void* data);
JSCoreObject *jscore_object_new_from_function (JSCoreContext *ctx, gchar *name, GArray *parameters_names, gchar *body, gchar *source_url, GError **error);
JSCoreObject *jscore_object_new_from_function_with_callback (JSCoreContext *ctx, gchar *name);
JSCoreObject *jscore_object_new_from_constructor (JSCoreContext *ctx, JSCoreClass *jsClass, JSCoreObjectCallAsConstructorCallback callAsConstructor);
JSCoreObject *jscore_object_new_from_array (JSCoreContext *ctx,const gpointer elements[],gsize num_elements, GError **error);
JSCoreObject *jscore_object_new_from_date (JSCoreContext *ctx, GDateTime *date, GError **error);
JSCoreObject *jscore_object_new_from_error (JSCoreContext *ctx, GError *source_error, GError **error);
JSCoreObject *jscore_object_new_from_regexp (JSCoreContext *ctx, GRegex *regex, GError **error);

JSCoreValue *jscore_object_get_prototype (JSCoreObject * object);
void        jscore_object_set_prototype (JSCoreObject * object, JSCoreValue *value);
gboolean    jscore_object_has_property (JSCoreObject * object, gchar *name);
JSCoreValue *jscore_object_get_property (JSCoreObject *object, gchar *name, GError** error);
void       jscore_object_set_property (JSCoreObject * object, gchar *name, JSCoreValue *value, JSCorePropertyAttributes attributes, GError** error);
gboolean   jscore_object_delete_property (JSCoreObject *object, gchar *propertyName, GError **error);
JSCoreValue *jscore_object_get_property_at_index (JSCoreObject * object, guint propertyIndex, GError **error);
void    jscore_object_set_property_at_index (JSCoreObject * object,guint propertyIndex, JSCoreValue *value, GError **error);
gpointer jscore_object_get_private (JSCoreObject *object);
gboolean jscore_object_set_private (JSCoreObject *object, gpointer data);
gboolean jscore_object_is_function (JSCoreObject *object);
JSCoreValue *jscore_object_call_as_function (JSCoreObject * object, JSCoreObject * thisObject, GVariant *arguments, GError **error);
gboolean jscore_object_is_constructor (JSCoreObject *object);
JSCoreObject *jscore_object_call_as_constructor (JSCoreObject *self, GVariant *arguments, GError **error);

G_END_DECLS

#endif /* __JSCORE_OBJECT_H__ */
