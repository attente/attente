#include <math.h>

#include <clutter/clutter.h>



static ClutterActor *stage;

static ClutterActor *actor;

static ClutterActor *point;



static void
animate (gfloat t)
{
  clutter_actor_set_anchor_point (actor, 200 * cos (0.3 * t), 100 * cos (0.2 * t));
  clutter_actor_set_position (actor, 300 * sin (0.2 * t) + 150, 100);
  clutter_actor_set_rotation (actor, CLUTTER_X_AXIS, 6 * t, 0, 0, 0);
  clutter_actor_set_rotation (actor, CLUTTER_Y_AXIS, 2 * t, 0, 0, 0);
  clutter_actor_set_scale_full (actor, 0.5 * sin (0.1 * t) + 1, 1, 50, 25);
  clutter_actor_set_depth (actor, -100);
}



static gboolean
update (gpointer data)
{
  const ClutterVertex x = { 100, 50, 0 };
  static gfloat t = 0;
  ClutterVertex y;
  CoglMatrix    m;

  animate (t);

  clutter_actor_get_transformation_matrix (actor, &m);

  y.x = m.xx * x.x + m.xy * x.y + m.xz * x.z + m.xw;
  y.y = m.yx * x.x + m.yy * x.y + m.yz * x.z + m.yw;
  y.z = m.zx * x.x + m.zy * x.y + m.zz * x.z + m.zw;

  clutter_actor_set_position (point, y.x - 5, y.y - 5);
  clutter_actor_set_depth (point, y.z + 5);

  g_print ("(%f, %f, %f)\n", y.x, y.y, y.z);

  t += 0.05;

  return TRUE;
}



int
main (int   argc,
      char *argv[])
{
  ClutterColor c[] = {
    { 0, 0, 0, 255 },
    { 0, 0, 255, 255 },
    { 255, 255, 255, 255 }
  };

  clutter_init (&argc, &argv);

  cogl_set_depth_test_enabled (TRUE);

  stage = clutter_stage_get_default ();
  clutter_stage_set_color (CLUTTER_STAGE (stage), c);
  clutter_actor_show_all (stage);

  actor = clutter_rectangle_new_with_color (c + 1);
  clutter_actor_set_position (actor, 0, 0);
  clutter_actor_set_size (actor, 200, 100);
  clutter_actor_set_depth (actor, 0);
  clutter_container_add (CLUTTER_CONTAINER (stage), actor, NULL);

  point = clutter_rectangle_new_with_color (c + 2);
  clutter_actor_set_position (point, 0, 0);
  clutter_actor_set_size (point, 10, 10);
  clutter_actor_set_depth (point, 5);
  clutter_container_add (CLUTTER_CONTAINER (stage), point, NULL);

  g_timeout_add (20, update, NULL);

  clutter_main ();

  return 0;
}
