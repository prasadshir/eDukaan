<?php
/**
 * @file
 * droob_testimonials.features.inc
 */

/**
 * Implementation of hook_ctools_plugin_api().
 */
function droob_testimonials_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implementation of hook_views_api().
 */
function droob_testimonials_views_api() {
  list($module, $api) = func_get_args();
  if ($module == "views" && $api == "views_default") {
    return array("version" => "3.0");
  }
}

/**
 * Implementation of hook_node_info().
 */
function droob_testimonials_node_info() {
  $items = array(
    'testimonial' => array(
      'name' => t('Testimonial'),
      'base' => 'node_content',
      'description' => t('Client testimonial for publishing on the site'),
      'has_title' => '1',
      'title_label' => t('One liner'),
      'help' => '',
    ),
  );
  return $items;
}
