/*
 * jscore-context.h - Header for JSCoreContext
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

#ifndef __JSCORE_CONTEXT_H__
#define __JSCORE_CONTEXT_H__

#include "jscore-context-group.h"
#include "jscore-class.h"
#include <glib-object.h>


G_BEGIN_DECLS

#define JSCORE_TYPE_CONTEXT                       \
  (jscore_context_get_type())
#define JSCORE_CONTEXT(obj)                               \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               JSCORE_TYPE_CONTEXT,       \
                               JSCoreContext))
#define JSCORE_CONTEXT_CLASS(klass)               \
  (G_TYPE_CHECK_CLASS_CAST ((klass),            \
                            JSCORE_TYPE_CONTEXT,  \
                            JSCoreContextClass))
#define IS_JSCORE_CONTEXT(obj)                            \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               JSCORE_TYPE_CONTEXT))
#define IS_JSCORE_CONTEXT_CLASS(klass)            \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),            \
                            JSCORE_TYPE_CONTEXT))
#define JSCORE_CONTEXT_GET_CLASS(obj)                     \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              JSCORE_TYPE_CONTEXT,        \
                              JSCoreContextClass))

typedef struct _JSCoreContext      JSCoreContext;
typedef struct _JSCoreContextClass JSCoreContextClass;
typedef struct _JSCoreContextPrivate JSCoreContextPrivate;

struct _JSCoreContextClass
{
  GObjectClass parent_class;
};

struct _JSCoreContext
{
  GObject parent;
  JSCoreContextPrivate *priv;
};

GType jscore_context_get_type (void) G_GNUC_CONST;

JSCoreContext* jscore_context_new (void);
JSCoreContext* jscore_context_new_with_class (JSCoreClass *global_object_class);
JSCoreContext* jscore_context_new_in_group (JSCoreClass *global_object_class, JSCoreContextGroup *group);

G_END_DECLS

#endif /* __JSCORE_CONTEXT_H__ */
