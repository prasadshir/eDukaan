<?php
/**
 * @file
 * droob_banners.views_default.inc
 */

/**
 * Implementation of hook_views_default_views().
 */
function droob_banners_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'RotatingBanner';
  $view->description = 'Rotating Banner';
  $view->tag = 'Aadi Features';
  $view->base_table = 'node';
  $view->human_name = '';
  $view->core = 0;
  $view->api_version = '3.0-alpha1';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = 5;
  $handler->display->display_options['style_plugin'] = 'slideshow';
  $handler->display->display_options['style_options']['slideshow_type'] = 'views_slideshow_cycle';
  $handler->display->display_options['style_options']['slideshow_skin'] = 'default';
  $handler->display->display_options['style_options']['skin_info'] = array(
    'class' => 'default',
    'name' => 'Default',
    'module' => 'views_slideshow',
    'path' => '',
    'stylesheets' => array(),
  );
  $handler->display->display_options['style_options']['widgets']['top']['views_slideshow_pager']['weight'] = '1';
  $handler->display->display_options['style_options']['widgets']['top']['views_slideshow_pager']['type'] = 'views_slideshow_pager_fields';
  $handler->display->display_options['style_options']['widgets']['top']['views_slideshow_pager']['views_slideshow_pager_fields_fields'] = array(
    'field_home_banner_image' => 0,
  );
  $handler->display->display_options['style_options']['widgets']['top']['views_slideshow_controls']['weight'] = '1';
  $handler->display->display_options['style_options']['widgets']['top']['views_slideshow_controls']['type'] = 'views_slideshow_controls_text';
  $handler->display->display_options['style_options']['widgets']['top']['views_slideshow_slide_counter']['weight'] = '1';
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_pager']['weight'] = '1';
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_pager']['type'] = 'views_slideshow_pager_fields';
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_pager']['views_slideshow_pager_fields_fields'] = array(
    'field_home_banner_image' => 0,
  );
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_controls']['enable'] = 1;
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_controls']['weight'] = '1';
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_controls']['type'] = 'views_slideshow_controls_text';
  $handler->display->display_options['style_options']['widgets']['bottom']['views_slideshow_slide_counter']['weight'] = '1';
  $handler->display->display_options['style_options']['views_slideshow_cycle']['timeout'] = '5000';
  $handler->display->display_options['style_options']['views_slideshow_cycle']['speed'] = '700';
  $handler->display->display_options['style_options']['views_slideshow_cycle']['delay'] = '0';
  $handler->display->display_options['style_options']['views_slideshow_cycle']['start_paused'] = 0;
  $handler->display->display_options['style_options']['views_slideshow_cycle']['remember_slide_days'] = '1';
  $handler->display->display_options['style_options']['views_slideshow_cycle']['items_per_slide'] = '1';
  $handler->display->display_options['style_options']['views_slideshow_cycle']['cleartype'] = 0;
  $handler->display->display_options['style_options']['views_slideshow_cycle']['cleartypenobg'] = 0;
  $handler->display->display_options['row_plugin'] = 'fields';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'No results found.';
  $handler->display->display_options['empty']['area']['format'] = '1';
  $handler->display->display_options['empty']['area']['tokenize'] = 0;
  /* Field: Content: Banner Image */
  $handler->display->display_options['fields']['field_home_banner_image']['id'] = 'field_home_banner_image';
  $handler->display->display_options['fields']['field_home_banner_image']['table'] = 'field_data_field_home_banner_image';
  $handler->display->display_options['fields']['field_home_banner_image']['field'] = 'field_home_banner_image';
  $handler->display->display_options['fields']['field_home_banner_image']['label'] = '';
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_home_banner_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_home_banner_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_home_banner_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_home_banner_image']['settings'] = array(
    'image_style' => 'banner',
    'image_link' => '',
  );
  $handler->display->display_options['fields']['field_home_banner_image']['field_api_classes'] = 0;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'banner' => 'banner',
  );
  $handler->display->display_options['filters']['type']['expose']['operator'] = FALSE;

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['block_description'] = 'Block - Rotating Banner';
  $export['RotatingBanner'] = $view;

  return $export;
}
