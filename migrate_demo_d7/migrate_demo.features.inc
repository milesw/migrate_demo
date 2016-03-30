<?php
/**
 * @file
 * migrate_demo.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function migrate_demo_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function migrate_demo_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function migrate_demo_node_info() {
  $items = array(
    'organisation' => array(
      'name' => t('Organisation'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Name'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
