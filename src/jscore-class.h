/*
 * jscore-class.h - Header for JSCoreClass
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

#ifndef __JSCORE_CLASS_H__
#define __JSCORE_CLASS_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define JSCORE_TYPE_CLASS                       \
  (jscore_class_get_type())
#define JSCORE_CLASS(obj)                               \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               JSCORE_TYPE_CLASS,       \
                               JSCoreClass))
#define JSCORE_CLASS_CLASS(klass)               \
  (G_TYPE_CHECK_CLASS_CAST ((klass),            \
                            JSCORE_TYPE_CLASS,  \
                            JSCoreClassClass))
#define IS_JSCORE_CLASS(obj)                            \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               JSCORE_TYPE_CLASS))
#define IS_JSCORE_CLASS_CLASS(klass)            \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),            \
                            JSCORE_TYPE_CLASS))
#define JSCORE_CLASS_GET_CLASS(obj)                     \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              JSCORE_TYPE_CLASS,        \
                              JSCoreClassClass))

typedef struct _JSCoreClass      JSCoreClass;
typedef struct _JSCoreClassClass JSCoreClassClass;
typedef struct _JSCoreClassPrivate JSCoreClassPrivate;

typedef struct JSClassDefinition JSCoreClassDefinition;

struct _JSCoreClassClass
{
  GObjectClass parent_class;
};

struct _JSCoreClass
{
  GObject parent;
  JSCoreClassPrivate *priv;
};

GType jscore_class_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __JSCORE_CLASS_H__ */
