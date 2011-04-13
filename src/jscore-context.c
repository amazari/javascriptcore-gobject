/*
 * jscore-context.c - Source for JSCoreContext
 *
 * Copyright (C) 2010 Igalia S.I.

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

#include "jscore-context.h"
#include "jscore-class.h"
#include "jscore-context-group.h"
#include "jscore-context-group-private.h"
#include "jscore-context-private.h"
#include "jscore-class-private.h"

#include <JavaScriptCore/JavaScript.h>

G_DEFINE_TYPE (JSCoreContext, jscore_context, G_TYPE_OBJECT);

static void
jscore_context_constructed (GObject *object);
static void
jscore_context_dispose (GObject *object);
static void
jscore_context_finalize (GObject *object);
static void
jscore_context_get_property (GObject *object,
                             guint property_id,
                             GValue *value,
                             GParamSpec *pspec);
static void
jscore_context_set_property (GObject *object,
                             guint property_id,
                             const GValue *value,
                             GParamSpec *pspec);

/* properties */
enum
{
  PROP_DUMMY = 1, LAST_PROPERTY
};

JSCoreContext*
jscore_context_new_in_group (JSCoreClass *class,
                             JSCoreContextGroup *group)
{
  GObject *object = g_object_new (JSCORE_TYPE_CONTEXT, NULL);

  JSCoreContext *context = JSCORE_CONTEXT (object);

  context->priv->real =
      JSGlobalContextCreateInGroup (group ? group->priv->real : NULL,
                                    class->priv->class);

  return context;
}

JSCoreContext*
jscore_context_new_with_class (JSCoreClass *class)
{
  return jscore_context_new_in_group (class, NULL);
}

JSCoreContext*
jscore_context_new ()
{
  return jscore_context_new_in_group (NULL, NULL);
}

JSCoreContextGroup *
jscore_context_get_group (JSCoreContext *context)
{
  return JSContextGetGroup (context->priv->real);
}

JSObjectRef
jscore_context_get_global_object (JSCoreContext *context)
{
  return JSContextGetGlobalObject (context->priv->real);
}

static void
jscore_context_class_init (JSCoreContextClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof(JSCoreContextPrivate));

  gobject_class->get_property = jscore_context_get_property;
  gobject_class->set_property = jscore_context_set_property;
  gobject_class->constructed = jscore_context_constructed;
  gobject_class->dispose = jscore_context_dispose;
  gobject_class->finalize = jscore_context_finalize;
}

static void
jscore_context_init (JSCoreContext *self)
{
  JSCoreContextPrivate *priv =
      G_TYPE_INSTANCE_GET_PRIVATE (self,
                                   JSCORE_TYPE_CONTEXT,
                                   JSCoreContextPrivate);

  self->priv = priv;
  priv->dispose_has_run = FALSE;
}

static void
jscore_context_get_property (GObject *object,
                             guint property_id,
                             GValue *value,
                             GParamSpec *pspec)
{
  JSCoreContext *self = JSCORE_CONTEXT (object);
  JSCoreContextPrivate *priv = self->priv;

  /* Make compiler happy */
  (void) priv;

  switch (property_id)
  {
  case PROP_DUMMY:
    g_value_set_uint (value, 0);
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  break;
  }
}

static void
jscore_context_set_property (GObject *object,
                             guint property_id,
                             const GValue *value,
                             GParamSpec *pspec)
{
  JSCoreContext *self = JSCORE_CONTEXT (object);
  JSCoreContextPrivate *priv = self->priv;

  /* Make compiler happy */
  (void) priv;

  switch (property_id)
  {
  case PROP_DUMMY:
    g_value_get_uint (value);
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  break;
  }
}

static void
jscore_context_constructed (GObject *object)
{
  void
  (*chain_up) (GObject *) =
      G_OBJECT_CLASS (jscore_context_parent_class)->constructed;
  JSCoreContext *self = JSCORE_CONTEXT (object);
  JSCoreContextPrivate *priv = self->priv;

  /* Make compiler happy */
  (void) priv;

  if (chain_up != NULL)
    chain_up (object);

}
static void
jscore_context_dispose (GObject *object)
{
  JSCoreContext *self = (JSCoreContext *) object;
  JSCoreContextPrivate *priv = self->priv;

  if (priv->dispose_has_run)
    return;

  priv->dispose_has_run = TRUE;
  JSGlobalContextRelease (priv->real);

  G_OBJECT_CLASS (jscore_context_parent_class)->dispose (object);
}

static void
jscore_context_finalize (GObject *object)
{
  JSCoreContext *self = (JSCoreContext *) object;

  /* Make compiler happy */
  (void) self;

  G_OBJECT_CLASS (jscore_context_parent_class)->finalize (object);
}

