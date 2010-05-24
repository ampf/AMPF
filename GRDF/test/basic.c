/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdio.h>
#include <check.h>
#include <grdf.h>

START_TEST(make_graph)
{
  GrdfWorld *world;
  GrdfGraph *graph;

  world = grdf_world_new();
  fail_unless(world != NULL);
  fail_unless(GRDF_IS_WORLD(world));

  graph = grdf_graph_new(world);
  fail_unless(graph != NULL);
  fail_unless(GRDF_IS_GRAPH(graph));

  grdf_graph_unref(graph);
  grdf_world_unref(world);
}
END_TEST

START_TEST(make_graph_with_stuff)
{
  GrdfWorld *world;
  GrdfGraph *graph;
  GrdfNode *jojo, *mayor, *whos, *elephants, *horton;

  world = grdf_world_new();
  fail_unless(world != NULL);
  fail_unless(GRDF_IS_WORLD(world));

  graph = grdf_graph_new(world);
  fail_unless(graph != NULL);
  fail_unless(GRDF_IS_GRAPH(graph));

  // Horton hears a who (Dr. Seuss)
  whos = grdf_node_anon_new(graph, "species");
  grdf_stmt_new_nuu(graph, whos, "name", "whos");

  mayor = grdf_node_anon_new(graph,"mayors");

  grdf_stmt_new_nun(graph, mayor, "isA", whos);
  grdf_stmt_new_nuu(graph, mayor, "name", "the mayor");

  jojo = grdf_node_anon_new(graph,"children");
  grdf_stmt_new_nuu(graph, jojo, "name", "jojo");
  grdf_stmt_new_nun(graph, jojo, "father", mayor);
  grdf_stmt_new_nun(graph, jojo, "isA", whos);

  elephants = grdf_node_anon_new(graph, "species");
  grdf_stmt_new_nuu(graph, elephants, "name", "elephant");

  horton = grdf_node_anon_new(graph, "animals");
  grdf_stmt_new_nun(graph, horton, "species", elephants);
  grdf_stmt_new_nuu(graph, horton, "name", "horton");
  grdf_stmt_new_nun(graph, horton, "hears a", mayor);

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
  Suite *s = suite_create("grdf_basic");
  TCase *tc;

  tc = tcase_create("make_graph");
  tcase_add_test(tc, make_graph);
  suite_add_tcase(s,tc);

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

  srunner_set_xml(sr, "basic-tests.xml"); // INSERT THIS LINE
  srunner_set_fork_status(sr, CK_NOFORK);

  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? 0 : 1;
}
