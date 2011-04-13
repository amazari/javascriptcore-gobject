/*
 * jscore-class.c - Source for JSCoreClass
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

#include "jscore-class.h"
#include "jscore-class-private.h"
#include <JavaScriptCore/JavaScript.h>


G_DEFINE_TYPE (JSCoreClass, jscore_class, G_TYPE_OBJECT);

static void jscore_class_dispose (GObject *object);

JSCoreClass *
jscore_class_new (const JSClassDefinition *definition)
{
  GObject *object = g_object_new (JSCORE_TYPE_CLASS, NULL);

  JSCoreClass *js_class = JSCORE_CLASS (object);
  js_class->priv->class = JSClassCreate(definition);

  return js_class;
}

static void
jscore_class_class_init (JSCoreClassClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (JSCoreClassPrivate));

 gobject_class->dispose = jscore_class_dispose;
}

static void
jscore_class_init (JSCoreClass *self)
{
  JSCoreClassPrivate *priv =
    G_TYPE_INSTANCE_GET_PRIVATE (self, JSCORE_TYPE_CLASS,
                                 JSCoreClassPrivate);

  self->priv = priv;
  priv->dispose_has_run = FALSE;
}

static void
jscore_class_dispose (GObject *object)
{
  JSCoreClass *self = (JSCoreClass *)object;
  JSCoreClassPrivate *priv = self->priv;

  if (priv->dispose_has_run)
    return;

  priv->dispose_has_run = TRUE;
  JSClassRelease(priv->class);

  G_OBJECT_CLASS (jscore_class_parent_class)->dispose (object);
}


