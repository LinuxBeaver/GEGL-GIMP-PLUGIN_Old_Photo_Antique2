/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * 2025 Beaver Old Photo Antique Effect 2
* 
* 
id=1
gaussian-blur std-dev-x=0.9 std-dev-y=0.9
gray
gamma value=1.2
over aux=[ ref=1 inner-glow value=white radius=30 opacity=2
noise-spread amount-x=20 amount-y=30
gaussian-blur std-dev-x=1 std-dev-y=1 ]
sepia scale=1
id=2
overlay srgb=true aux=[  ref=2 noise-solid detail=15 x-size=8 y-size=8 tileable=true turbulent=true opacity value=0.1 opacity value=1 ]
over aux=[ simplex-noise scale=20  opacity value=0.02 opacity value=1 ]

over aux=[ perlin-noise scale=17   opacity value=0.05 opacity value=1 ]
shadows-highlights highlights=-50
crop aux=[ ref=1 ]
			
box-blur radius=1 
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_boolean (stress_mode, _("Stress toning policy (slow but better)"), TRUE)
     description (_("Enable or disable stress toning, this enhances the antique photo at the cost of speed, if disabled you may need to experiment with higher noise and lighting tweaks"))

property_boolean (sepia_mode, _("Sepia policy"), TRUE)
     description (_("Enable or disable the sepia"))

property_boolean (saturation_mode, _("Saturation policy"), FALSE)
     description (_("Enable or disable the saturation"))

property_boolean (archaic_mode, _("Archaic policy"), FALSE)
     description (_("Enable or disable archaic mode, requires sepia mode"))
   ui_meta ("visible", " sepia_mode")

property_boolean (tempshift_mode, _("Temperature Shift policy"), FALSE)
     description (_("Enable or disable tempshift"))
   ui_meta ("visible", " sepia_mode")



property_double (saturation, _("Saturation"), 0.5)
   description (_("Saturation control"))
   value_range (0.0, 0.5)
   ui_range    (0.0, 0.5)
   ui_meta ("visible", " saturation_mode")

property_double (sepia, _("Sepia"), 1.0)
    description(_("Strength of the sepia effect. At 0 there is no sepia effect and the image will be gray to 0.5 color depending on what the saturation slider is"))
    value_range (0.25, 1.0)
  ui_steps      (0.05, 0.5)
   ui_meta ("visible", " sepia_mode")

property_double (temperature, _("Temperature shift"), 4000.0)
   description  (_("Temperature shift, shift from (blue to orange) 6500 is the non-effect point "))
   value_range  (3000.0, 12000.0)
   ui_range     (3000.0, 12000.0)
  ui_steps      (0.5, 4.0)
   ui_meta ("visible", "sepia_mode")
   ui_meta ("visible", "tempshift_mode")



property_double (noise, _("Noise"), 0.2)
    description(_("Grayscaled RGB noise"))
    value_range (0.0, 0.35)

property_double (gaus, _("Blur"), 1.0)
   description (_("Mild gaussian blur to mimic a dated photo's lower quality"))
   value_range (0.0, 2.5)
   ui_range    (0.0, 2.5)
   ui_steps      (0.5, 1.0)


property_double (lightness, _("Lighting"), -10.0)
   description  (_("Lightness control"))
   value_range  (-25.0, 10.0)
   ui_range     (-25.0, 10.0)
   ui_steps      (0.5, 1.0)

property_double (stress, _("Stess toning presence"), 0.85)
  value_range (0.5, 1.0)
  ui_range (0.5, 1.0)
  ui_gamma (1.5)
   ui_steps      (0.5, 1.0)
   ui_meta ("visible", " stress_mode")

property_double (solid_n, _("Solid noise presence"), 1.0)
  value_range (0.0, 3.0)
  ui_range (0.0, 3.0) 
  ui_gamma (1.5)
   ui_steps      (0.5, 1.0)

property_double (simplex_n, _("Simplex noise presence"), 1.0)
  value_range (0.0, 2.0)
  ui_range (0.0, 2.0)
  ui_gamma (1.5)
   ui_steps      (0.5, 1.0)

property_double (perlin_n, _("Perlin noise presence"), 0.6)
  value_range (0.0, 1.0)
  ui_range (0.0, 1.0)
  ui_gamma (1.5)
   ui_steps      (0.5, 1.0)

property_seed    (seed, _("Random seed"), rand)
    description(_("Seed randomization of four noises as well as the border's spread seed"))

property_double (solid_noise_scale, _("Solid noise scale"), 2.0)
   description (_("Scale of the solid noise, lower numbers make it larger"))
   value_range (1.0, 8.0)
   ui_range    (1.0, 8.0)
  ui_steps      (0.5, 1.0)


property_double (border_radius, _("Border feather radius"), 30.0)
   description  (_("Border radius control"))
   value_range  (0.0, 40.0)
   ui_range     (0.0, 40.0)
   ui_steps      (0.5, 1.0)

property_double (border_opacity, _("Border opacity"), 0.2)
   description  (_("Border opacity control"))
   value_range  (0.0, 1.0)
   ui_range     (0.0, 1.0)
  ui_steps      (0.5, 1.0)

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     old2
#define GEGL_OP_C_SOURCE old2.c

#include "gegl-op.h"

typedef struct
{
  GeglNode *input;
  GeglNode *gamma;
  GeglNode *spread;
  GeglNode *innerglow;
  GeglNode *idref1;
  GeglNode *idref2;
  GeglNode *crop;
  GeglNode *saturation;
  GeglNode *sepia;
  GeglNode *overlay;
  GeglNode *opacity;
  GeglNode *opacity2;
  GeglNode *opacity3;
  GeglNode *opacity4;
  GeglNode *opacity5;
  GeglNode *opacity6;
  GeglNode *normal;
  GeglNode *normal2;
  GeglNode *normal3;
  GeglNode *gaus;
  GeglNode *gaus2;
  GeglNode *gaus3;
  GeglNode *luminance;
  GeglNode *solidnoise;
  GeglNode *simplexnoise;
  GeglNode *perlinnoise;
  GeglNode *stress;
  GeglNode *lightness;
  GeglNode *shadowhighlights;
  GeglNode *srcatop;
  GeglNode *nopstress;
  GeglNode *nopsepia;
  GeglNode *nopsaturation;
  GeglNode *noparchaic;
  GeglNode *noptempshift;
  GeglNode *tempshift;
  GeglNode *archaic;
  GeglNode *noisergb;
  GeglNode *noplight;
  GeglNode *light;
  GeglNode *output;
}State;






static void update_graph (GeglOperation *operation)

{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  GeglNode *stressx = state->stress;
  GeglNode *saturationx = state->saturation;
  GeglNode *sepiax = state->sepia;

  GeglNode *lightx = state->noplight;

if (o->stress_mode) stressx = state->stress; 
if (!o->stress_mode) stressx = state->nopstress; 

if (o->sepia_mode) sepiax = state->sepia; 
if (!o->sepia_mode) sepiax = state->nopsepia; 


if (o->sepia_mode) lightx = state->noplight; 
if (!o->sepia_mode) lightx = state->light; 

if (o->saturation_mode) saturationx = state->saturation; 
if (!o->saturation_mode) saturationx = state->nopsaturation; 


{


  gegl_node_link_many (state->input, state->srcatop, state->output, NULL);
  gegl_node_link_many (state->input, state->noisergb, state->gaus, saturationx, state->gamma,  state->normal, sepiax, state->idref1, state->overlay, state->normal2, state->normal3, state->shadowhighlights, state->crop, state->idref2, state->luminance, state->lightness, lightx, state->gaus3, NULL);
  gegl_node_connect (state->srcatop, "aux", state->gaus3, "output");
  gegl_node_connect (state->normal, "aux", state->gaus2, "output");
  gegl_node_connect (state->overlay, "aux", state->opacity2, "output");
  gegl_node_link_many (state->input, state->innerglow, state->spread, state->gaus2,   NULL);
  gegl_node_link_many (state->solidnoise, state->opacity, state->opacity2,   NULL);
  gegl_node_connect (state->normal2, "aux", state->opacity4, "output");
  gegl_node_link_many (state->simplexnoise, state->opacity3, state->opacity4,   NULL);
  gegl_node_connect (state->normal3, "aux", state->opacity6, "output");
  gegl_node_link_many (state->perlinnoise, state->opacity5, state->opacity6,   NULL);
  gegl_node_connect (state->crop, "aux", state->input, "output");
  gegl_node_connect (state->luminance, "aux", stressx, "output");
  gegl_node_link_many (state->idref2, stressx,   NULL);
}
  if (!o->tempshift_mode)
  if (o->archaic_mode)
  if (o->sepia_mode)
{

  gegl_node_link_many (state->input, state->srcatop, state->output, NULL);
  gegl_node_link_many (state->input, state->noisergb, state->gaus, saturationx, state->gamma,  state->normal, sepiax, state->idref1, state->overlay, state->normal2, state->normal3, state->shadowhighlights, state->crop, state->idref2, state->luminance, state->lightness, lightx, state->gaus3, state->archaic, NULL);
  gegl_node_connect (state->srcatop, "aux", state->archaic, "output");
  gegl_node_connect (state->normal, "aux", state->gaus2, "output");
  gegl_node_connect (state->overlay, "aux", state->opacity2, "output");
  gegl_node_link_many (state->input, state->innerglow, state->spread, state->gaus2,   NULL);
  gegl_node_link_many (state->solidnoise, state->opacity, state->opacity2,   NULL);
  gegl_node_connect (state->normal2, "aux", state->opacity4, "output");
  gegl_node_link_many (state->simplexnoise, state->opacity3, state->opacity4,   NULL);
  gegl_node_connect (state->normal3, "aux", state->opacity6, "output");
  gegl_node_link_many (state->perlinnoise, state->opacity5, state->opacity6,   NULL);
  gegl_node_connect (state->crop, "aux", state->input, "output");
  gegl_node_connect (state->luminance, "aux", stressx, "output");
  gegl_node_link_many (state->idref2, stressx,   NULL);
}
  if (o->tempshift_mode)
  if (!o->archaic_mode)
  if (o->sepia_mode)
{

  gegl_node_link_many (state->input, state->srcatop, state->output, NULL);
  gegl_node_link_many (state->input, state->noisergb, state->gaus, saturationx, state->gamma,  state->normal, sepiax, state->idref1, state->overlay, state->normal2, state->normal3, state->shadowhighlights, state->crop, state->idref2, state->luminance, state->lightness, lightx, state->gaus3, state->tempshift, NULL);
  gegl_node_connect (state->srcatop, "aux", state->tempshift, "output");
  gegl_node_connect (state->normal, "aux", state->gaus2, "output");
  gegl_node_connect (state->overlay, "aux", state->opacity2, "output");
  gegl_node_link_many (state->input, state->innerglow, state->spread, state->gaus2,   NULL);
  gegl_node_link_many (state->solidnoise, state->opacity, state->opacity2,   NULL);
  gegl_node_connect (state->normal2, "aux", state->opacity4, "output");
  gegl_node_link_many (state->simplexnoise, state->opacity3, state->opacity4,   NULL);
  gegl_node_connect (state->normal3, "aux", state->opacity6, "output");
  gegl_node_link_many (state->perlinnoise, state->opacity5, state->opacity6,   NULL);
  gegl_node_connect (state->crop, "aux", state->input, "output");
  gegl_node_connect (state->luminance, "aux", stressx, "output");
  gegl_node_link_many (state->idref2, stressx,   NULL);
}
  if (o->tempshift_mode)
  if (o->archaic_mode)
  if (o->sepia_mode)
{

  gegl_node_link_many (state->input, state->srcatop, state->output, NULL);
  gegl_node_link_many (state->input, state->noisergb, state->gaus, saturationx, state->gamma,  state->normal, sepiax, state->idref1, state->overlay, state->normal2, state->normal3, state->shadowhighlights, state->crop, state->idref2, state->luminance, state->lightness, lightx, state->gaus3, state->tempshift, state->archaic, NULL);
  gegl_node_connect (state->srcatop, "aux", state->archaic, "output");
  gegl_node_connect (state->normal, "aux", state->gaus2, "output");
  gegl_node_connect (state->overlay, "aux", state->opacity2, "output");
  gegl_node_link_many (state->input, state->innerglow, state->spread, state->gaus2,   NULL);
  gegl_node_link_many (state->solidnoise, state->opacity, state->opacity2,   NULL);
  gegl_node_connect (state->normal2, "aux", state->opacity4, "output");
  gegl_node_link_many (state->simplexnoise, state->opacity3, state->opacity4,   NULL);
  gegl_node_connect (state->normal3, "aux", state->opacity6, "output");
  gegl_node_link_many (state->perlinnoise, state->opacity5, state->opacity6,   NULL);
  gegl_node_connect (state->crop, "aux", state->input, "output");
  gegl_node_connect (state->luminance, "aux", stressx, "output");
  gegl_node_link_many (state->idref2, stressx,   NULL);
}
    }


static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);


  State *state = o->user_data = g_malloc0 (sizeof (State));


state->input    = gegl_node_get_input_proxy (gegl, "input");
state->output   = gegl_node_get_output_proxy (gegl, "output");


state->gamma = gegl_node_new_child (gegl,
                                  "operation", "gegl:gamma", "value", 2.0, 
                                  NULL);

state->spread = gegl_node_new_child (gegl,
                                  "operation", "gegl:noise-spread", "amount-x", 20, "amount-y", 20, 
                                  NULL);

    GeglColor *whites = gegl_color_new ("#ffffff");
state->innerglow    = gegl_node_new_child (gegl,
                                  "operation", "gegl:inner-glow", "value", whites,
                                  NULL);


state->idref1    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

state->idref2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

state->crop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);

state->saturation    = gegl_node_new_child (gegl,
                                  "operation", "gegl:saturation",
                                  NULL);

state->sepia = gegl_node_new_child (gegl,
                                  "operation", "gegl:sepia",
                                  NULL);


state->overlay = gegl_node_new_child (gegl,
                                  "operation", "gegl:overlay", "srgb", TRUE, 
                                  NULL);

state->normal = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);
state->normal2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);

state->normal3 = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);

state->opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 0.1, 
                                  NULL);

state->opacity2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", 
                                  NULL);

state->opacity3 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 0.1, 
                                  NULL);

state->opacity4 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", 
                                  NULL);

state->opacity5 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 0.02, 	
                                  NULL);

state->opacity6 = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", 
                                  NULL);


state->luminance = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 56, "composite-mode", 0,   
                                  NULL);
state->gaus = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur", 
                                  NULL);

state->stress = gegl_node_new_child (gegl,
                                  "operation", "gegl:stress", 
                                  NULL);

state->lightness = gegl_node_new_child (gegl,
                                  "operation", "gegl:hue-chroma", 
                                  NULL);


state->gaus2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur", "std-dev-x", 1.0, "std-dev-y", 1.0,
                                  NULL);

state->gaus3 = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur", "std-dev-x", 1.0, "std-dev-y", 1.0,
                                  NULL);

state->shadowhighlights    = gegl_node_new_child (gegl,
                                  "operation", "gegl:shadows-highlights", "highlights", -50.0, 
                                  NULL);


state->solidnoise = gegl_node_new_child (gegl,
                                  "operation", "gegl:noise-solid", "detail", 15, "x-size", 8.0, "y-size", 8.0, "tileable", TRUE, "turbulent", TRUE,   
                                  NULL);

state->simplexnoise = gegl_node_new_child (gegl,
                                  "operation", "gegl:simplex-noise", "scale", 20.0,     
                                  NULL);

state->perlinnoise = gegl_node_new_child (gegl,
                                  "operation", "gegl:perlin-noise", "scale", 17.0,     
                                  NULL);

state->srcatop = gegl_node_new_child (gegl,
                                  "operation", "gegl:src-atop", 
                                  NULL);

state->nopstress = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop", 
                                  NULL);

state->nopsepia = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop", 
                                  NULL);

state->nopsaturation = gegl_node_new_child (gegl,
                                  "operation", "gegl:saturation", "scale", 0.0, 
                                  NULL);

state->noparchaic = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop", 
                                  NULL);

state->noptempshift = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop", 
                                  NULL);

#define archaics \
" median-blur radius=0 abyss-policy=none id=1 gimp:layer-mode layer-mode=overlay blend-space=rgb-perceptual opacity=0.7 aux=[ ref=1 difference-of-gaussians radius1=5.6 radius2=6.1  ] "\

state->archaic = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", archaics, 
                                  NULL);


state->tempshift = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-temperature", 
                                  NULL);


state->noisergb = gegl_node_new_child (gegl,
                                  "operation", "gegl:noise-rgb",
                                  NULL);

state->noplight = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop", 
                                  NULL);

state->light = gegl_node_new_child (gegl,
                                  "operation", "gegl:hue-chroma", "lightness", 5.5,
                                  NULL);


  gegl_operation_meta_redirect (operation, "sepia", state->sepia, "scale");
  gegl_operation_meta_redirect (operation, "gaus", state->gaus, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaus", state->gaus, "std-dev-y");
  gegl_operation_meta_redirect (operation, "solid_n", state->opacity2, "value");
  gegl_operation_meta_redirect (operation, "solid_noise_scale", state->solidnoise, "x-size");
  gegl_operation_meta_redirect (operation, "solid_noise_scale", state->solidnoise, "y-size");
  gegl_operation_meta_redirect (operation, "simplex_n", state->opacity4, "value");
  gegl_operation_meta_redirect (operation, "perlin_n", state->opacity6, "value");
  gegl_operation_meta_redirect (operation, "seed", state->solidnoise, "seed"); 
  gegl_operation_meta_redirect (operation, "seed", state->simplexnoise, "seed");
  gegl_operation_meta_redirect (operation, "seed", state->spread, "seed");
  gegl_operation_meta_redirect (operation, "seed", state->noisergb, "seed");
  gegl_operation_meta_redirect (operation, "border_radius", state->innerglow, "radius");
  gegl_operation_meta_redirect (operation, "border_opacity", state->innerglow, "opacity");
  gegl_operation_meta_redirect (operation, "lightness", state->lightness, "lightness");
  gegl_operation_meta_redirect (operation, "saturation", state->saturation, "scale");
  gegl_operation_meta_redirect (operation, "temperature", state->tempshift, "intended-temperature");
  gegl_operation_meta_redirect (operation, "noise", state->noisergb, "red");
  gegl_operation_meta_redirect (operation, "noise", state->noisergb, "green");
  gegl_operation_meta_redirect (operation, "noise", state->noisergb, "blue");
  gegl_operation_meta_redirect (operation, "stress", state->luminance, "opacity");


 }




static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:antique2",
    "title",       _("Old Photo Antique 2"),
    "reference-hash", "istronglyopposeisraelswarsandgenocide",
    "description", _("A revision of the classic antique old photo filter, more realistic but slower"),
    "gimp:menu-path", "<Image>/Filters/Decor/",
    "gimp:menu-label", _("Old Photo Antique 2..."),
    NULL);
}

#endif
