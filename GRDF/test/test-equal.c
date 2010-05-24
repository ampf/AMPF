/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdio.h>
#include <check.h>
#include <grdf.h>

START_TEST(make_graph_with_stuff)
{
  GrdfWorld *world;
  GrdfGraph *graph;
  GrdfNode *node1, *node2, *pred1, *pred2;

  world = grdf_world_new();
  fail_unless(world != NULL);
  fail_unless(GRDF_IS_WORLD(world));

  graph = grdf_graph_new(world);
  fail_unless(graph != NULL);
  fail_unless(GRDF_IS_GRAPH(graph));

  node1 = grdf_node_anon_new(graph,"node1");
  node2 = grdf_node_anon_new(graph,"node1");
  pred1 = grdf_node_uri_new(graph, "predicate");
  pred2 = grdf_node_uri_new(graph, "predicate");

  grdf_graph_dump(stdout, graph);

  fail_unless(grdf_node_equal(pred1, pred2));
  fail_unless(grdf_node_equal(pred1, pred1));

  fail_unless(grdf_node_equal(node1, node2));
  fail_unless(grdf_node_equal(node1, node1));

  {
      gchar *n3 = NULL;
      gint len = 0;

      // Have libgrdf write the graph to a string it will allocate
      grdf_graph_to_n3(graph,&n3,&len);
      printf(n3);
  }

//  grdf_graph_unref(graph);
  grdf_world_unref(world);
}
END_TEST

Suite *grdf_basic_suite() {
  Suite *s = suite_create("test_equal");
  TCase *tc;

  tc = tcase_create("make_graph_with_stuff");
  tcase_add_test(tc, make_graph_with_stuff);
  suite_add_tcase(s,tc);

  return s;
}

int main() {
  int number_failed;
  Suite *s = grdf_basic_suite ();
  SRunner *sr = srunner_create (s);

  grdf_init();

  srunner_set_xml(sr, "test-equal.xml"); // INSERT THIS LINE
  srunner_set_fork_status(sr, CK_NOFORK);

  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? 0 : 1;
}
