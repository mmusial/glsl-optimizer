/**************************************************************************
 *
 * Copyright 2009, VMware, Inc.
 * All Rights Reserved.
 * Copyright 2010 George Sapountzis <gsapountzis@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "pipe/p_compiler.h"
#include "pipe/p_format.h"
#include "util/u_inlines.h"
#include "util/u_format.h"
#include "util/u_math.h"
#include "util/u_memory.h"

#include "state_tracker/sw_winsys.h"
#include "dri_sw_winsys.h"


struct dri_sw_displaytarget
{
   void *data;
   void *mapped;
};

struct dri_sw_winsys
{
   struct sw_winsys base;
};

static INLINE struct dri_sw_displaytarget *
dri_sw_displaytarget( struct sw_displaytarget *dt )
{
   return (struct dri_sw_displaytarget *)dt;
}

static INLINE struct dri_sw_winsys *
dri_sw_winsys( struct sw_winsys *ws )
{
   return (struct dri_sw_winsys *)ws;
}


static boolean
dri_sw_is_displaytarget_format_supported( struct sw_winsys *ws,
                                          unsigned tex_usage,
                                          enum pipe_format format )
{
   /* TODO: check visuals or other sensible thing here */
   return TRUE;
}

static struct sw_displaytarget *
dri_sw_displaytarget_create(struct sw_winsys *winsys,
                            unsigned tex_usage,
                            enum pipe_format format,
                            unsigned width, unsigned height,
                            unsigned alignment,
                            unsigned *stride)
{
   struct dri_sw_displaytarget *dri_sw_dt;
   unsigned nblocksy, size, dri_sw_stride, format_stride;

   dri_sw_dt = CALLOC_STRUCT(dri_sw_displaytarget);
   if(!dri_sw_dt)
      goto no_dt;

   format_stride = util_format_get_stride(format, width);
   dri_sw_stride = align(format_stride, alignment);

   nblocksy = util_format_get_nblocksy(format, height);
   size = dri_sw_stride * nblocksy;

   dri_sw_dt->data = align_malloc(size, alignment);
   if(!dri_sw_dt->data)
      goto no_data;

   *stride = dri_sw_stride;
   return (struct sw_displaytarget *)dri_sw_dt;

no_data:
   FREE(dri_sw_dt);
no_dt:
   return NULL;
}

static void
dri_sw_displaytarget_destroy(struct sw_winsys *ws,
                             struct sw_displaytarget *dt)
{
   struct dri_sw_displaytarget *dri_sw_dt = dri_sw_displaytarget(dt);

   if (dri_sw_dt->data) {
      FREE(dri_sw_dt->data);
   }

   FREE(dri_sw_dt);
}

static void *
dri_sw_displaytarget_map(struct sw_winsys *ws,
                         struct sw_displaytarget *dt,
                         unsigned flags)
{
   struct dri_sw_displaytarget *dri_sw_dt = dri_sw_displaytarget(dt);
   dri_sw_dt->mapped = dri_sw_dt->data;
   return dri_sw_dt->mapped;
}

static void
dri_sw_displaytarget_unmap(struct sw_winsys *ws,
                           struct sw_displaytarget *dt)
{
   struct dri_sw_displaytarget *dri_sw_dt = dri_sw_displaytarget(dt);
   dri_sw_dt->mapped = NULL;
}

static struct sw_displaytarget *
dri_sw_displaytarget_from_handle(struct sw_winsys *winsys,
                                 const struct pipe_texture *templ,
                                 struct winsys_handle *whandle,
                                 unsigned *stride)
{
   assert(0);
   return NULL;
}

static boolean
dri_sw_displaytarget_get_handle(struct sw_winsys *winsys,
                                struct sw_displaytarget *dt,
                                struct winsys_handle *whandle)
{
   assert(0);
   return FALSE;
}

static void
dri_sw_displaytarget_display(struct sw_winsys *ws,
                             struct sw_displaytarget *dt,
                             void *context_private)
{
   assert(0);
}


static void
dri_destroy_sw_winsys(struct sw_winsys *winsys)
{
   FREE(winsys);
}

struct sw_winsys *
dri_create_sw_winsys(void)
{
   struct dri_sw_winsys *ws;

   ws = CALLOC_STRUCT(dri_sw_winsys);
   if (!ws)
      return NULL;

   ws->base.destroy = dri_destroy_sw_winsys;

   ws->base.is_displaytarget_format_supported = dri_sw_is_displaytarget_format_supported;

   /* screen texture functions */
   ws->base.displaytarget_create = dri_sw_displaytarget_create;
   ws->base.displaytarget_destroy = dri_sw_displaytarget_destroy;
   ws->base.displaytarget_from_handle = dri_sw_displaytarget_from_handle;
   ws->base.displaytarget_get_handle = dri_sw_displaytarget_get_handle;

   /* texture functions */
   ws->base.displaytarget_map = dri_sw_displaytarget_map;
   ws->base.displaytarget_unmap = dri_sw_displaytarget_unmap;

   ws->base.displaytarget_display = dri_sw_displaytarget_display;

   return &ws->base;
}

/* vim: set sw=3 ts=8 sts=3 expandtab: */