/*
 * jscore-object.c - Source for JSCoreObject
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

#include "jscore-object.h"
#include "jscore-context-private.h"
#include "jscore-class-private.h"
#include "jscore-value-private.h"

static void
jscore_object_class_init (JSCoreObjectClass *klass);
static void
jscore_object_init (JSCoreObject *self);

#define JS_CORE_ERROR jscore__error_quark ()
GQuark
jscore_error_quark (void)
{
  return g_quark_from_static_string ("jscore-gobject-error");
}

G_DEFINE_TYPE (JSCoreObject, jscore_object, G_TYPE_OBJECT)

static void
jscore_object_dispose (GObject *object);

/* signal enum */
enum
{
  SIGNAL_FUNCTION_CALLED, LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] =
  { 0 };

struct _JSCoreObjectPrivate
{
  JSObjectRef  object;
  JSCoreContext *context;
  gboolean dispose_has_run;
};

static void
set_error_from_js_exception (GError **error, JSValueRef exception, JSContextRef context)
{
  JSStringRef message_property_name = JSStringCreateWithUTF8CString("message");
  JSStringRef name_property_name = JSStringCreateWithUTF8CString("name");
  gchar *name;
  gchar *message;

  g_assert ((exception));
  if (!JSValueIsObject (context, (JSValueRef)exception))
    return;

  message = jscore_value_get_string_real (context,
                                          JSObjectGetProperty(context,
                                                              (JSObjectRef) exception,
                                                              message_property_name, NULL));

  name = jscore_value_get_string_real (context,
                                       JSObjectGetProperty(context,
                                                           (JSObjectRef) exception,
                                                           name_property_name, NULL));

  g_set_error (error, JS_CORE_ERROR, 42, "%s: %s", name, message);
}

static JSContextRef
get_real_context (JSCoreObject *object)
{
  return object->priv->context->priv->real;
}

JSCoreObject *
jscore_object_new (JSCoreContext *ctx, JSCoreClass *jsClass, void* data)
{
  GObject *gobject = g_object_new (JSCORE_TYPE_OBJECT, NULL);
  JSCoreObject *jsObject = JSCORE_OBJECT (gobject);
  jsObject->priv->object = JSObjectMake (ctx->priv->real, jsClass->priv->class, data);
  jsObject->priv->context = ctx;

  JSObjectSetPrivate (jsObject->priv->object, jsObject);

  return jsObject;
}

// TODO: use param specs ? GVariant ? GValue ?
JSCoreObject *
jscore_object_new_from_function (JSCoreContext *ctx,
                                 gchar *name,
                                 GArray *parameters_names,
                                 gchar *body,
                                 gchar *source_url,
                                 GError **error)
{
  JSValueRef exception = 0;

  GObject *gobject = g_object_new (JSCORE_TYPE_OBJECT, NULL);
  JSCoreObject *jsObject = JSCORE_OBJECT (gobject);

  JSStringRef js_name = JSStringCreateWithUTF8CString (name);
  JSStringRef js_body = JSStringCreateWithUTF8CString (body);
  JSStringRef js_source = JSStringCreateWithUTF8CString (source_url);

  GArray *js_parameters_names = g_array_sized_new(FALSE, FALSE,
                                                 sizeof(JSStringRef),
                                                 parameters_names->len);
  int i;
  for (i = 0; i <parameters_names->len; i++)
  {
    JSStringRef js_parameter_name = JSStringCreateWithUTF8CString (
        g_array_index(parameters_names, gpointer, i)
        );
    js_parameters_names = g_array_append_val(js_parameters_names,
                                             js_parameter_name);
  }

  jsObject->priv->context = ctx;
  jsObject->priv->object = JSObjectMakeFunction(ctx->priv->real,
                                                js_name,
                                                js_parameters_names->len,
                                                (JSStringRef *)js_parameters_names->data,
                                                js_body,
                                                js_source,
                                                0,
                                                &exception);

  JSObjectSetPrivate (jsObject->priv->object, jsObject);

  JSStringRelease (js_name);
  JSStringRelease (js_body);
  JSStringRelease (js_source);

  for (i = 0; i <js_parameters_names->len; i++)
    JSStringRelease (g_array_index(js_parameters_names, JSStringRef, i));

  if (exception)
    set_error_from_js_exception (error, exception, ctx->priv->real);
  return jsObject;
}

static JSObjectRef
delegating_Callback (JSContextRef ctx, JSObjectRef constructor,
                     size_t argumentCount,
                     const JSValueRef arguments[],
                     JSValueRef* exception, guint signal_id)
{
  JSCoreObject *jsObject = JSObjectGetPrivate (constructor);

  GVariantBuilder builder;

  g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);

  int i;
  for (i = 0; i < argumentCount; i++)
  {
    GVariant *value = jscore_value_to_variant ((JSCoreValue *)arguments[i], jsObject->priv->context);
    g_variant_builder_add_value(&builder, value);
  }

  GVariant *arguments_variant = g_variant_builder_end(&builder);

  g_signal_emit (jsObject, signal_id, signals[SIGNAL_FUNCTION_CALLED], arguments_variant);
}

static JSObjectRef
delegating_JSObjectCallAsConstructorCallback (JSContextRef ctx, JSObjectRef constructor,
                                              size_t argumentCount,
                                              const JSValueRef arguments[],
                                              JSValueRef* exception)
{
  delegating_Callback (ctx,constructor,
                       argumentCount,
                       arguments,
                       exception, signals[SIGNAL_FUNCTION_CALLED]);
}

JSValueRef
delegating_JSObjectCallAsFunctionCallback (JSContextRef ctx, JSObjectRef function,
                                           JSObjectRef thisObject, size_t argumentCount,
                                           const JSValueRef arguments[], JSValueRef* exception)
{
  delegating_Callback (ctx,function,
                       argumentCount,
                       arguments,
                       exception, signals[SIGNAL_FUNCTION_CALLED]);
}


JSCoreObject *
jscore_object_new_from_function_with_callback (JSCoreContext *ctx,
                                               gchar *name)
{
  GObject *gobject = g_object_new (JSCORE_TYPE_OBJECT, NULL);
  JSCoreObject *jsObject = JSCORE_OBJECT (gobject);

  JSStringRef jname = JSStringCreateWithUTF8CString (name);
  jsObject->priv->context = ctx;
  jsObject->priv->object = JSObjectMakeFunctionWithCallback(ctx->priv->real, jname, delegating_JSObjectCallAsFunctionCallback);

  JSObjectSetPrivate (jsObject->priv->object, jsObject);

  JSStringRelease (jname);

  return jsObject;
}


JSCoreObject *
jscore_object_new_from_constructor (JSCoreContext *ctx,
                                    JSCoreClass *jsClass,
                                    JSCoreObjectCallAsConstructorCallback callAsConstructor)
{
  GObject *gobject = g_object_new (JSCORE_TYPE_OBJECT, NULL);
  JSCoreObject *jsObject = JSCORE_OBJECT (gobject);

  jsObject->priv->context = ctx;
  jsObject->priv->object = JSObjectMakeConstructor (ctx->priv->real,
                                                    jsClass->priv->class,
                                                    delegating_JSObjectCallAsConstructorCallback);

  JSObjectSetPrivate (jsObject->priv->object, jsObject);

  return jsObject;
}

JSCoreObject *
jscore_object_new_from_array (JSCoreContext *ctx,
                              const JSCoreValue elements[],
                              gsize num_elements,
                              GError **error)
{
  JSValueRef exception = 0;
  GObject *gobject = g_object_new (JSCORE_TYPE_OBJECT, NULL);
  JSCoreObject *jsObject = JSCORE_OBJECT (gobject);

  jsObject->priv->context = ctx;
  jsObject->priv->object = JSObjectMakeArray (ctx->priv->real,
                                              num_elements,
                                              (JSValueRef *)elements,
                                              &exception);

  JSObjectSetPrivate (jsObject->priv->object, jsObject);

  if (exception)
    set_error_from_js_exception (error, exception, ctx->priv->real);

  return jsObject;
}

JSCoreObject *
jscore_object_new_from_date (JSCoreContext *ctx,
                             GDateTime *date,
                             GError **error)
{
  JSValueRef exception = 0;
  //return JSObjectMakeDate(ctx->priv->real, )
  if (exception)
    set_error_from_js_exception (error, exception, ctx->priv->real);
}

JSCoreObject *
jscore_object_new_from_error (JSCoreContext *ctx,
                              GError *source_error,
                              GError **error)
{
  JSValueRef* exception;
 // JSObjectMakeError()
}

JSCoreObject *
jscore_object_new_from_regexp (JSCoreContext *ctx,
                               GRegex *regex,
                               GError **error)
{
  JSValueRef exception = 0;
  //
  if (exception)
    set_error_from_js_exception (error, exception, ctx->priv->real);
}

JSCoreValue *
jscore_object_get_prototype (JSCoreObject * object)
{

  return (JSCoreValue *)JSObjectGetPrototype (get_real_context(object),
                                              object->priv->object);
}

void
jscore_object_set_prototype (JSCoreObject * object,
                             JSCoreValue *value)
{
  JSObjectSetPrototype(get_real_context(object),
                       object->priv->object,
                       (JSValueRef)value);
}

gboolean
jscore_object_has_property (JSCoreObject * object,
                            gchar *name)
{
  JSStringRef jname = JSStringCreateWithUTF8CString (name);
  gboolean ret = JSObjectHasProperty(get_real_context(object), object->priv->object, jname);
  JSStringRelease (jname);

  return ret;
}

JSCoreValue *
jscore_object_get_property (JSCoreObject *object,
                            gchar *name,
                            GError** error)
{
  JSValueRef exception = 0;
  JSCoreObjectPrivate *priv = object->priv;

  JSStringRef jname = JSStringCreateWithUTF8CString (name);
  JSValueRef ret = JSObjectGetProperty (get_real_context(object),
                                        priv->object,
                                        jname,
                                        &exception);

  JSStringRelease (jname);

  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(object));

  return (JSCoreValue *)ret;
}

void
jscore_object_set_property (JSCoreObject * object,
                            gchar *name,
                            JSCoreValue *value,
                            JSCorePropertyAttributes attributes,
                            GError** error)
{
  JSCoreObjectPrivate *priv = object->priv;

  JSStringRef jname = JSStringCreateWithUTF8CString (name);
  JSValueRef exception = 0;

  if (value)
  {
    JSObjectSetProperty (get_real_context(object),
                         priv->object, jname,
                         (JSValueRef)value, 0, &exception);
  }

  JSStringRelease (jname);

  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(object));
}

gboolean
jscore_object_delete_property (JSCoreObject *object,
                               gchar *name,
                               GError **error)
{
  JSValueRef exception = 0;
  JSCoreObjectPrivate *priv = object->priv;
  JSStringRef jname = JSStringCreateWithUTF8CString (name);
  gboolean ret = JSObjectDeleteProperty (get_real_context(object),
                                 priv->object,
                                 jname,
                                 &exception);
  JSStringRelease (jname);

  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(object));

  return ret;
}

JSCoreValue *
jscore_object_get_property_at_index (JSCoreObject * object,
                                     guint index,
                                     GError **error)
{
  JSCoreObjectPrivate *priv = object->priv;
  JSValueRef exception = 0;
  JSValueRef value = JSObjectGetPropertyAtIndex (get_real_context(object),
                                     priv->object, index, &exception);
  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(object));

  return (JSCoreValue *)value;
}

void
jscore_object_set_property_at_index (JSCoreObject * object,
                                     guint index,
                                     JSCoreValue *value,
                                     GError **error)
{
  JSCoreObjectPrivate *priv = object->priv;
  JSValueRef exception = 0;
  JSObjectSetPropertyAtIndex (get_real_context(object),
                              priv->object, index, (JSValueRef)value, &exception);
  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(object));
}

gpointer
jscore_object_get_private (JSCoreObject *object)
{
  return (gpointer) JSObjectGetPrivate (object->priv->object);
}

gboolean
jscore_object_set_private (JSCoreObject *object, gpointer data)
{
  JSObjectSetPrivate (object->priv->object, data);
}

gboolean
jscore_object_is_function (JSCoreObject *object)
{
  return JSObjectIsFunction (get_real_context(object), object->priv->object);
}


static GArray*
gvariant_to_js_value_array (JSCoreContext *context, GVariant *variant, GError **error)
{
  if (g_variant_is_container(variant) == FALSE)
    return NULL;

  GVariantIter *iter = g_variant_iter_new (variant);
  GVariant *child;
  GArray *js_arguments = g_array_new (FALSE, FALSE, sizeof (JSValueRef));

  while ((child = g_variant_iter_next_value (iter)))
   {
     JSCoreValue *argument = jscore_value_new_variant (context, child, error);
     g_array_append_val (js_arguments, argument);
     g_variant_unref (child);
   }

  return js_arguments;
}

JSCoreValue *
jscore_object_call_as_function (JSCoreObject * object,
                        JSCoreObject * thisObject,
                        GVariant *arguments,
                        GError **error)
{
  JSValueRef exception = 0;

  if (thisObject == NULL)
    thisObject = object;


  GArray *js_arguments = gvariant_to_js_value_array (object->priv->context, arguments, error);

  JSCoreValue *value = (JSCoreValue *)JSObjectCallAsFunction (get_real_context(object),
                                                              object->priv->object,
                                                              thisObject->priv->object,
                                                              js_arguments->len,
                                                              (JSValueRef *)js_arguments->data,
                                                              &exception);
  g_array_free (js_arguments, FALSE);

  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(object));
}

gboolean
jscore_object_is_constructor (JSCoreObject *object)
{
  JSObjectIsConstructor (get_real_context(object), object->priv->object);
}

// TODO: Use varargs, GVariant or GValue ?
JSCoreObject *
jscore_object_call_as_constructor (JSCoreObject *self,
                                   GVariant *arguments,
                                   GError **error)
{
  JSCoreObjectPrivate *priv = self->priv;
  JSValueRef exception = 0;
  GArray *js_arguments = gvariant_to_js_value_array (priv->context, arguments, error);

  GObject *gobject = g_object_new (JSCORE_TYPE_OBJECT, NULL);
  JSCoreObject *jsObject = JSCORE_OBJECT (gobject);
  jsObject->priv->context = self->priv->context;
  jsObject->priv->object =
      JSObjectCallAsConstructor (get_real_context(self),
                                 priv->object,
                                 js_arguments->len,
                                 (JSValueRef *)js_arguments->data,
                                 &exception);


  if (exception)
    set_error_from_js_exception (error, exception, get_real_context(self));

  return jsObject;
}

static void
jscore_object_class_init (JSCoreObjectClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof(JSCoreObjectPrivate));

  gobject_class->dispose = jscore_object_dispose;

  signals[SIGNAL_FUNCTION_CALLED] = g_signal_new ("constructor-called",
                                        G_OBJECT_CLASS_TYPE (klass),
                                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                                        0,
                                        NULL, NULL,
                                        g_cclosure_marshal_VOID__VARIANT,
                                        G_TYPE_NONE, 0, G_TYPE_VARIANT);

}

static void
jscore_object_init (JSCoreObject *self)
{
  JSCoreObjectPrivate *priv =
      G_TYPE_INSTANCE_GET_PRIVATE (self, JSCORE_TYPE_OBJECT,
          JSCoreObjectPrivate);

  priv->context = NULL;
  priv->object = NULL;
  self->priv = priv;
  priv->dispose_has_run = FALSE;
}


static void
jscore_object_dispose (GObject *object)
{
  JSCoreObject *self = (JSCoreObject *) object;
  JSCoreObjectPrivate *priv = self->priv;

  if (priv->dispose_has_run)
    return;

  priv->dispose_has_run = TRUE;
  JSValueUnprotect(get_real_context(self), priv->object);

  G_OBJECT_CLASS (jscore_object_parent_class)->dispose (object);
}
