<?php

/**
 * @file
 * Contains helper functions common to multiple files.
 */

/**
 * Returns the name of the Commerce shipping service name for the given
 * Shippo carrier service name.
 */
function shippo_commerce_shipping_service_name($service) {
  $service_names = shippo_carrier_service_names();
  return 'shippo_' . $service_names[$service];
}

/**
 * Returns an array of shipping service names keyed by Shippo shipping
 * service name.
 */
function shippo_carrier_service_names() {
  return array(
    'dhl_ecommerce_first_class_expedited' => 'dhl_ec_fce',
    'dhl_ecommerce_priority_expedited' => 'dhl_ec_pe',
    'dhl_ecommerce_bpm_ground' => 'dhl_ec_bpm_g',
    'dhl_ecommerce_bpm_expedited' => 'dhl_ec_bpm_e',
    'dhl_express_domestic_express_doc' => 'dhl_ex_ded',
    'dhl_express_economy_select_doc' => 'dhl_ex_esd',
    'dhl_express_worldwide_nondoc' => 'dhl_ex_wn',
    'dhl_express_worldwide_doc' => 'dhl_ex_wd',
    'ups_standard' => 'ups_st',
    'ups_ground' => 'ups_g',
    'ups_saver' => 'ups_sv',
    'ups_3_day_select' => 'ups_tds',
    'ups_second_day_air' => 'ups_sda',
    'ups_second_day_air_am' => 'ups_sdaa',
    'ups_next_day_air' => 'ups_nda',
    'ups_next_day_air_saver' => 'ups_ndas',
    'ups_next_day_air_early_am' => 'ups_ndaea',
    'ups_mail_innovations_domestic' => 'ups_mid',
    'ups_surepost' => 'ups_sp',
    'ups_surepost_lightweight' => 'ups_spl',
    'ups_express' => 'ups_ex',
    'ups_express_plus' => 'ups_exp',
    'ups_expedited' => 'ups_ep',
  );
}

/**
 * Returns an options list of shipping services provided by specified carriers.
 */
function shippo_carrier_services($carriers = array()) {
  $package_services = array(
    'DHL eCommerce' => array(
      'dhl_ecommerce_first_class_expedited' => t('DHL eCommerceFirst Class Expedited'),
      'dhl_ecommerce_priority_expedited' => t('DHL eCommerce Priority Expedited'),
      'dhl_ecommerce_bpm_ground' => t('DHL eCommerce BPM Ground'),
      'dhl_ecommerce_bpm_expedited' => t('DHL eCommerce BPM Expedited'),
    ),
    'DHL Express' => array(
      'dhl_express_domestic_express_doc' => t('DHL Express Domestic Express Doc'),
      'dhl_express_economy_select_doc' => t('DHL Express Economy Select Doc'),
      'dhl_express_worldwide_nondoc' => t('DHL Express Express Worldwide Nondoc'),
      'dhl_express_worldwide_doc' => t('DHL Express Express Worldwide Doc'),
    ),
    'UPS' => array(
      'ups_standard' => t('UPS Standard℠'),
      'ups_ground' => t('UPS Ground'),
      'ups_saver' => t('UPS Saver®'),
      'ups_3_day_select' => t('UPS Three-Day Select®'),
      'ups_second_day_air' => t('UPS Second Day Air®'),
      'ups_second_day_air_am' => t('UPS Second Day Air A.M.®'),
      'ups_next_day_air' => t('UPS Next Day Air®'),
      'ups_next_day_air_saver' => t('UPS Next Day Air Saver®'),
      'ups_next_day_air_early_am' => t('UPS Next Day Air Early A.M.®'),
      'ups_mail_innovations_domestic' => t('UPS Mail Innovations (domestic)'),
      'ups_surepost' => t('UPS Surepost'),
      'ups_surepost_lightweight' => t('UPS Surepost Lightweight'),
      'ups_express' => t('UPS Express®'),
      'ups_express_plus' => t('UPS Express Plus®'),
      'ups_expedited' => t('UPS Expedited®'),
    ),
  );

  return empty($carriers) ? $package_services : array_intersect_key($package_services, drupal_map_assoc($carriers));
}

/**
 * Loads all enabled Shippo shipping services from the database.
 */
function shippo_load_services() {
  $shippo_services = &drupal_static(__FUNCTION__);

  // If the services haven't been loaded yet, do so now.
  if (!isset($shippo_services)) {
    $shippo_services = db_query('SELECT * FROM {shippo_service}')->fetchAllAssoc('name', PDO::FETCH_ASSOC);
  }

  return $shippo_services;
}

/**
 * Saves an enabled Shippo shipping service.
 *
 * @param $service
 *   An array containing a 'name', 'title', and 'weight' of the service.
 * @param $update
 *   Boolean indicating whether or not the save should be processed as an update
 *   to an existing enabled service.
 * @param $skip_reset
 *   Boolean indicating whether or not the menu item cache and default rule
 *   configurations should be reset after the delete.
 *
 * @return
 *   The return value of the call to drupal_write_record() to save the txn type,
 *     either FALSE on failure or SAVED_NEW or SAVED_UPDATED indicating the type
 *     of query performed to save the txn type.
 */
function shippo_save_service($service, $update = FALSE, $skip_reset = FALSE) {
  $op = drupal_write_record('shippo_service', $service, $update ? array('name') : array());

  // Clear the necessary caches and rebuild the menu items.
  if (!$skip_reset) {
    drupal_static_reset('shippo_load_services');
    commerce_shipping_services_reset();
    entity_defaults_rebuild();
    rules_clear_cache(TRUE);
    menu_rebuild();
  }

  return $op;
}

/**
 * Disables an enabled Shippo shipping service by deleting its database row.
 *
 * @param $name
 *   The name of the package service to delete.
 * @param $skip_reset
 *   Boolean indicating whether or not the menu item cache and default rule
 *   configurations should be reset after the delete.
 */
function shippo_delete_service($name, $skip_reset = FALSE) {
  db_delete('shippo_service')
    ->condition('name', $name)
    ->execute();

  rules_config_delete(array('commerce_shipping_service_' . $name));

  // Clear the necessary caches and rebuild the menu items.
  if (!$skip_reset) {
    drupal_static_reset('shippo_load_services');
    commerce_shipping_services_reset();
    rules_clear_cache(TRUE);
    menu_rebuild();
  }
}
