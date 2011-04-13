/*
 * jscore-context-group.c - Source for JSCoreContextGroup
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

#include "jscore-context-group.h"
#include "jscore-context-group-private.h"
#include <JavaScriptCore/JavaScript.h>

G_DEFINE_TYPE (JSCoreContextGroup, jscore_context_group, G_TYPE_OBJECT);

static void jscore_context_group_constructed (GObject *object);
static void jscore_context_group_dispose (GObject *object);
static void jscore_context_group_finalize (GObject *object);


static void
jscore_context_group_class_init (JSCoreContextGroupClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (JSCoreContextGroupPrivate));

  gobject_class->constructed = jscore_context_group_constructed;
  gobject_class->dispose = jscore_context_group_dispose;
  gobject_class->finalize = jscore_context_group_finalize;

}

static void
jscore_context_group_init (JSCoreContextGroup *self)
{
  JSCoreContextGroupPrivate *priv =
    G_TYPE_INSTANCE_GET_PRIVATE (self, JSCORE_TYPE_CONTEXT_GROUP,
                                 JSCoreContextGroupPrivate);

  self->priv = priv;
  priv->dispose_has_run = FALSE;
}


static void
jscore_context_group_constructed (GObject *object)
{
  void (*chain_up) (GObject *) =
    G_OBJECT_CLASS (jscore_context_group_parent_class)->constructed;
  JSCoreContextGroup *self = JSCORE_CONTEXT_GROUP (object);
  JSCoreContextGroupPrivate *priv = self->priv;

  priv->real = JSContextGroupCreate();

  if (chain_up != NULL)
    chain_up (object);

}
static void
jscore_context_group_dispose (GObject *object)
{
  JSCoreContextGroup *self = (JSCoreContextGroup *)object;
  JSCoreContextGroupPrivate *priv = self->priv;

  if (priv->dispose_has_run)
    return;

  JSContextGroupRelease(priv->real);

  priv->dispose_has_run = TRUE;

  G_OBJECT_CLASS (jscore_context_group_parent_class)->dispose (object);
}

static void
jscore_context_group_finalize (GObject *object)
{
  JSCoreContextGroup *self = (JSCoreContextGroup *)object;

  /* Make compiler happy */
  (void)self;

  G_OBJECT_CLASS (jscore_context_group_parent_class)->finalize (object);
}

