/*
 * jscore-value.c - Source for JSCoreValue
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

#include "jscore-value.h"
#include "jscore-context.h"
#include "jscore-context-private.h"
#include "jscore-class-private.h"

#include <glib.h>
#include <JavaScriptCore/JavaScript.h>

/* JavascriptCore API */

void
jscore_value_ref (JSCoreContext *context,
                      JSCoreValue *value)
{
  JSValueProtect(context->priv->real, value);
}

void
jscore_value_unref (JSCoreContext *context,
                      JSCoreValue *value)
{
  JSValueUnprotect (context->priv->real, value);
}

int
jscore_value_get_type (JSCoreContext *context,
                       const JSCoreValue *value)
{
  return JSValueGetType(context->priv->real, value);
}

gboolean
jscore_value_is_boolean (JSCoreContext *context,
                         JSCoreValue *value)
{
  return JSValueIsBoolean(context->priv->real, value);
}

gboolean
jscore_value_is_number (JSCoreContext *context,
                         JSCoreValue *value)
{
  return JSValueIsNumber (context->priv->real, value);
}

gboolean
jscore_value_is_null (JSCoreContext *context,
                      JSCoreValue *value)
{
  return JSValueIsNull (context->priv->real, value);
}

gboolean
jscore_value_is_string (JSCoreContext *context,
                      JSCoreValue *value)
{
  return JSValueIsString (context->priv->real, value);
}

gboolean
jscore_value_is_undefined (JSCoreContext *context,
                      JSCoreValue *value)
{
  return JSValueIsUndefined (context->priv->real, value);
}

gboolean
jscore_value_is_object (JSCoreContext *context,
                        JSCoreValue *value)
{
  return JSValueIsObject (context->priv->real, value);
}

gboolean
jscore_value_is_object_of_class (JSCoreContext *context,
                                 JSCoreValue *value, JSCoreClass *class)
{
  return JSValueIsObjectOfClass (context->priv->real, value, class->priv->class);
}

gboolean
jscore_value_is_equal (JSCoreContext *context,
                       JSCoreValue *value, JSCoreValue *value2)
{
  return JSValueIsEqual (context->priv->real, value, value2, NULL);
}

JSCoreValue *
jscore_value_new_null (JSCoreContext *context)
{
  return JSValueMakeNull(context->priv->real);
}

JSCoreValue *
jscore_value_new_undefined (JSCoreContext *context)
{
  return JSValueMakeUndefined(context->priv->real);
}

JSCoreValue *
jscore_value_new_string (JSCoreContext *context, const gchar *string)
{
  JSStringRef jsstr = JSStringCreateWithUTF8CString (string);
  JSValueRef valstr = JSValueMakeString (context->priv->real, jsstr);
  JSStringRelease (jsstr);

  return valstr;
}

JSCoreValue *
jscore_value_new_number (JSCoreContext *context, gdouble number)
{
  return JSValueMakeNumber (context->priv->real, (gdouble) number);
}

JSCoreValue *
jscore_value_new_boolean (JSCoreContext *context, gboolean boolean)
{
 return JSValueMakeBoolean (context->priv->real, boolean);
}

JSCoreValue *
jscore_value_new_json (JSCoreContext *context, const gchar* json)
{
  JSStringRef jsstr = JSStringCreateWithUTF8CString (json);
  JSValueRef val = JSValueMakeFromJSONString(context->priv->real, jsstr);
  JSStringRelease (jsstr);

  return val;
}


gchar *jscore_value_get_string_real (JSContextRef context, JSValueRef value)
{
  JSStringRef jsstr = NULL;
  JSValueRef func, str;
  gchar *buf = NULL;
  gint length;

  jsstr = JSValueToStringCopy (context, value, NULL);
  length = JSStringGetMaximumUTF8CStringSize (jsstr);
  if (length > 0)
    {
      buf = g_malloc (length * sizeof (gchar));
      JSStringGetUTF8CString (jsstr, buf, length);
    }
  if (jsstr)
    JSStringRelease (jsstr);

  return buf;
}

gchar *jscore_value_get_string (JSCoreContext *context, JSCoreValue *value)
{
  return jscore_value_get_string_real (context->priv->real, (JSValueRef) value);
}

gdouble jscore_value_get_number (JSCoreContext *context, JSCoreValue *value)
{
  return JSValueToNumber (context->priv->real, value, NULL);
}

gboolean jscore_value_get_boolean (JSCoreContext *context, JSCoreValue *value)
{
  return JSValueToBoolean(context->priv->real, value);
}

gchar *jscore_value_get_json (JSCoreContext *context, const JSCoreValue *value)
{
  return JSValueCreateJSONString(context->priv->real, value, 2, NULL);
}

/* Extensions */

JSCoreValue *
jscore_value_new_gvalue (JSCoreContext *context,
                        GValue * gval, GError **error)
{
  JSValueRef exception = NULL;

  if (!G_IS_VALUE (gval))
    {
      return false;
    }
  switch (G_VALUE_TYPE (gval))
    {
    case G_TYPE_BOOLEAN:
      return jscore_value_new_boolean (context,
                                       g_value_get_boolean (gval));
    case G_TYPE_CHAR:
      return jscore_value_new_char (context, g_value_get_char (gval), error);
    case G_TYPE_UCHAR:
      return jscore_value_new_uchar (context, g_value_get_uchar (gval), error);
    case G_TYPE_INT:
      return jscore_value_new_int (context, g_value_get_int (gval), error);
    case G_TYPE_UINT:
      return jscore_value_new_uint (context, g_value_get_uint (gval), error);
    case G_TYPE_LONG:
      return jscore_value_new_long (context, g_value_get_long (gval), error);
    case G_TYPE_ULONG:
      return jscore_value_new_ulong (context, g_value_get_ulong (gval), error);
    case G_TYPE_INT64:
      return jscore_value_new_int64 (context, g_value_get_int64 (gval), error);
    case G_TYPE_UINT64:
      return jscore_value_new_uint64 (context, g_value_get_uint64 (gval),
                                     error);
    case G_TYPE_FLOAT:
      return jscore_value_new_float (context, g_value_get_float (gval), exception);
    case G_TYPE_DOUBLE:
      return jscore_value_new_double (context, g_value_get_double (gval),
                                     exception);
    case G_TYPE_STRING:
      return jscore_value_new_string (context, (gchar *)
                                     g_value_get_string (gval));
    }
  return NULL;
}

JSCoreValue *
jscore_value_new_variant (JSCoreContext *context,
                        GVariant * gval, GError **error)
{
  JSValueRef exception = NULL;

  if (!G_IS_VARIANT (gval))
    {
      return false;
    }
  GVariantType *type = g_variant_get_type (gval);

  if (g_variant_type_equal(type, G_VARIANT_TYPE_BOOLEAN))
      return jscore_value_new_boolean (context,
                                       g_variant_get_boolean (gval));
  else if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE))
    return jscore_value_new_number (context,
                                      g_variant_get_byte(gval));
  else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT16))
     return jscore_value_new_number (context,
                                       g_variant_get_int16(gval));
  else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT32))
     return jscore_value_new_number (context,
                                       g_variant_get_int32(gval));
  else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT64))
     return jscore_value_new_number (context,
                                       g_variant_get_int64(gval));
  else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT16))
      return jscore_value_new_number (context,
                                        g_variant_get_uint16(gval));
   else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT32))
      return jscore_value_new_number (context,
                                        g_variant_get_uint32(gval));
   else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT64))
      return jscore_value_new_number (context,
                                        g_variant_get_uint64(gval));

   else if (g_variant_type_equal(type, G_VARIANT_TYPE_DOUBLE))
       return jscore_value_new_number (context,
                                         g_variant_get_double(gval));

    else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT32))
       return jscore_value_new_string (context,
                                         g_variant_get_bytestring(gval));

  return NULL;
}

GVariant *
jscore_value_to_variant (JSCoreValue *value,JSCoreContext *context)
{
  JSValueRef exception = NULL;

  switch (JSValueGetType(context->priv->real, value))
  {
  case kJSTypeUndefined:
  case kJSTypeNull:
    return NULL;
  case kJSTypeBoolean:
    return g_variant_new_boolean (JSValueToBoolean(context, value));
  case kJSTypeNumber:
    return g_variant_new_double (JSValueToNumber(context, value, &exception));
  case kJSTypeString:
    return g_variant_new_string (JSValueToStringCopy(context, value, &exception));
  case kJSTypeObject:
    return g_variant_new_string(JSValueToStringCopy(context, value, &exception));
  }

  return NULL;
}

