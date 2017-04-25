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
    'dhl_ecommerce_marketing_parcel_ground' => 'dhl_ec_mpg',
    'dhl_ecommerce_marketing_parcel_expedited' => 'dhl_ec_mpe',
    'dhl_ecommerce_parcel_plus_ground' => 'dhl_ec_ppg',
    'dhl_ecommerce_parcel_plus_expedited' => 'dhl_ec_ppe',
    'dhl_ecommerce_parcel_plus_expedited_max' => 'dhl_ec_ppem',
    'dhl_ecommerce_parcels_ground' => 'dhl_ec_pg',
    'dhl_ecommerce_parcels_expedited' => 'dhl_ec_pe',
    'dhl_ecommerce_parcels_expedited_max' => 'dhl_ec_pem',
    'dhl_ecommerce_ground' => 'dhl_ec_g',
    'dhl_ecommerce_expedited' => 'dhl_ec_e',
    'dhl_ecommerce_light' => 'dhl_ec_l',
    'dhl_ecommerce_plus' => 'dhl_ec_p',
    'dhl_express_domestic_express_doc' => 'dhl_ex_ded',
    'dhl_express_economy_select_doc' => 'dhl_ex_esd',
    'dhl_express_worldwide_nondoc' => 'dhl_ex_wn',
    'dhl_express_worldwide_doc' => 'dhl_ex_wd',
    'dhl_express_worldwide' => 'dhl_ex_w',
    'dhl_express_worldwide_eu_doc' => 'dhl_ex_wed',
    'dhl_express_break_bulk_express_doc' => 'dhl_ex_bbed',
    'dhl_express_express_9_00_nondoc' => 'dhl_ex_enn',
    'dhl_express_express_10_30_doc' => 'dhl_ex_etd',
    'dhl_express_express_10_30_nondoc' => 'dhl_ex_etn',
    'dhl_express_express_12_00_doc' => 'dhl_ex_etwd',
    'dhl_express_europack_nondoc' => 'dhl_ex_epn',
    'dhl_express_express_envelope_doc' => 'dhl_ex_eed',
    'dhl_express_express_12_00_nondoc' => 'dhl_ex_etwn',
    'dhl_express_worldwide_b2c_doc' => 'dhl_ex_wbd',
    'dhl_express_worldwide_b2c_nondoc' => 'dhl_ex_wbn',
    'dhl_express_medical_express' => 'dhl_ex_me',
    'dhl_express_express_easy_nondoc' => 'dhl_ex_een',
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
    'usps_priority' => 'usps_p',
    'usps_priority_express' => 'usps_pe',
    'usps_first' => 'usps_f',
    'usps_parcel_select' => 'usps_ps',
    'usps_media_mail' => 'usps_mm',
    'usps_priority_mail_international' => 'usps_pmi',
    'usps_priority_mail_express_international' => 'usps_pmei',
    'usps_first_class_package_international_service' => 'usps_fcpis',
    'fedex_ground' => 'fedex_g',
    'fedex_home_delivery' => 'fedex_hd',
    'fedex_smart_post' => 'fedex_sp',
    'fedex_2_day' => 'fedex_td',
    'fedex_2_day_am' => 'fedex_tda',
    'fedex_express_saver' => 'fedex_es',
    'fedex_standard_overnight' => 'fedex_so',
    'fedex_priority_overnight' => 'fedex_po',
    'fedex_first_overnight' => 'fedex_fo',
    'fedex_international_economy' => 'fedex_ie',
    'fedex_international_priority' => 'fedex_ip',
    'fedex_international_first' => 'fedex_if',
    'fedex_europe_first_international_priority' => 'fedex_efip',
    'canada_post_regular_parcel' => 'cp_rp',
    'canada_post_expedited_parcel' => 'cp_ep',
    'canada_post_priority' => 'cp_p',
    'canada_post_xpresspost' => 'cp_xp',
    'canada_post_xpresspost_international' => 'cp_xpi',
    'canada_post_xpresspost_usa' => 'cp_xu',
    'canada_post_expedited_parcel_usa' => 'cp_epu',
    'canada_post_tracked_packet_usa' => 'cp_tpu',
    'canada_post_small_packet_usa_air' => 'cp_spua',
    'canada_post_tracked_packet_international' => 'cp_tpi',
    'canada_post_small_packet_international_air' => 'cp_spia',
    'purolator_ground' => 'pur_g',
    'purolator_ground9_am' => 'pur_gn',
    'purolator_ground1030_am' => 'pur_gt',
    'purolator_ground_distribution' => 'pur_gd',
    'purolator_ground_evening' => 'pur_ge',
    'purolator_ground_us' => 'pur_gu',
    'purolator_express' => 'pur_ex',
    'purolator_express9_am' => 'pur_exn',
    'purolator_express1030_am' => 'pur_ext',
    'purolator_express_evening' => 'pur_exe',
    'purolator_express_us' => 'pur_exu',
    'purolator_express_us9_am' => 'pur_exun',
    'purolator_express_us1030_am' => 'pur_exut',
    'purolator_express_us1200' => 'pur_exutw',
    'purolator_express_international' => 'pur_exi',
    'purolator_express_international9_am' => 'pur_exin',
    'purolator_express_international1030_am' => 'pur_exit',
    'purolator_express_international1200' => 'pur_exitw',
    'asendia_us_international_priority_airmail' => 'as_uipa',
    'asendia_us_international_surface_airlift' => 'as_uisa',
    'australia_post_express_post' => 'ap_exp',
    'australia_post_parcel_post' => 'ap_pp',
    'dhl_paket_business' => 'dhl_gr_pb',
    'dhl_weltpaket_business' => 'dhl_gr_wpb',
    'dhl_europaket_business' => 'dhl_epb',
    'deutsche_post_postkarte' => 'dp_pk',
    'deutsche_post_standardbrief' => 'dp_sb',
    'deutsche_post_kompaktbrief' => 'dp_kb',
    'deutsche_post_grossbrief' => 'dp_gb',
    'deutsche_post_maxibrief' => 'dp_mb',
    'deutsche_post_maxibrief_plus' => 'dp_mbp',
    'gls_deutschland_business_parcel' => 'glsd_bp',
    'gls_france_business_parcel' => 'glsf_bp',
    'mondial_relay_pointrelais' => 'mr_pr',
    'parcelforce_express48' => 'pf_exf',
    'parcelforce_express24' => 'pf_ext',
    'parcelforce_expressam' => 'pf_exa',
    'newgistics_parcel_select_lightweight' => 'ng_psl',
    'newgistics_parcel_select' => 'ng_ps',
    'newgistics_priority_mail' => 'ng_pm',
    'newgistics_first_class_mail' => 'ng_fcm',
    'ontrac_ground' => 'ot_g',
    'ontrac_sunrise_gold' => 'ot_sg',
    'ontrac_sunrise' => 'ot_s',
    'lasership_routed_delivery' => 'ls_rd',
    'uber_on_demand' => 'ur_od',
    'hermes_uk_parcelshop' => 'hu_ps',
    'hermes_uk_home_delivery' => 'hu_hd',
  );
}

/**
 * Returns an options list of shipping services provided by specified carriers.
 */
function shippo_carrier_services($carriers = array()) {
  $package_services = array(
    'DHL eCommerce' => array(
      'dhl_ecommerce_first_class_expedited' => t('DHL eCommerce First Class Expedited'),
      'dhl_ecommerce_priority_expedited' => t('DHL eCommerce Priority Expedited'),
      'dhl_ecommerce_bpm_ground' => t('DHL eCommerce BPM Ground'),
      'dhl_ecommerce_bpm_expedited' => t('DHL eCommerce BPM Expedited'),
      'dhl_ecommerce_marketing_parcel_ground' => t('DHL eCommerce Marketing Parcel Ground'),
      'dhl_ecommerce_marketing_parcel_expedited' => t('DHL eCommerce Marketing Parcel Expedited'),
      'dhl_ecommerce_parcel_plus_ground' => t('DHL eCommerce Parcel Plus Ground'),
      'dhl_ecommerce_parcel_plus_expedited' => t('DHL eCommerce Parcel Plus Expedited'),
      'dhl_ecommerce_parcel_plus_expedited_max' => t('DHL eCommerce Parcel Plus Expedited Max'),
      'dhl_ecommerce_parcels_ground' => t('DHL eCommerce Parcels Ground Expedited'),
      'dhl_ecommerce_parcels_expedited' => t('DHL eCommerce Parcels Expedited'),
      'dhl_ecommerce_parcels_expedited_max' => t('DHL eCommerce Parcels Expedited Max'),
      'dhl_ecommerce_ground' => t('DHL eCommerce Ground'),
      'dhl_ecommerce_expedited' => t('DHL eCommerce Expedited (for Flats)'),
      'dhl_ecommerce_light' => t('DHL eCommerce Light (for EZ Return)'),
      'dhl_ecommerce_plus' => t('DHL eCommerce Plus (for EZ Return)'),
    ),
    'DHL Express' => array(
      'dhl_express_domestic_express_doc' => t('DHL Express Domestic Express Doc'),
      'dhl_express_economy_select_doc' => t('DHL Express Economy Select Doc'),
      'dhl_express_worldwide_nondoc' => t('DHL Express Express Worldwide Nondoc'),
      'dhl_express_worldwide_doc' => t('DHL Express Express Worldwide Doc'),
      'dhl_express_worldwide' => t('DHL Express Worldwide'),
      'dhl_express_worldwide_eu_doc' => t('DHL Express Express Worldwide EU Doc'),
      'dhl_express_break_bulk_express_doc' => t('DHL Express Break Bulk Express Doc'),
      'dhl_express_express_9_00_nondoc' => t('DHL Express Express 9:00 Doc'),
      'dhl_express_express_10_30_doc' => t('DHL Express Express 10:30 Doc'),
      'dhl_express_express_10_30_nondoc' => t('DHL Express Express 10:30 NonDoc'),
      'dhl_express_express_12_00_doc' => t('DHL Express Express 12:00 Doc'),
      'dhl_express_europack_nondoc' => t('DHL Express Europack NonDoc'),
      'dhl_express_express_envelope_doc' => t('DHL Express Express Envelope Doc'),
      'dhl_express_express_12_00_nondoc' => t('DHL Express Express 12:00 NonDoc'),
      'dhl_express_worldwide_b2c_doc' => t('DHL Express Express Worldwide (B2C) Doc'),
      'dhl_express_worldwide_b2c_nondoc' => t('DHL Express Express Worldwide (B2C) NonDoc'),
      'dhl_express_medical_express' => t('DHL Express Medical Express'),
      'dhl_express_express_easy_nondoc' => t('DHL Express Express Easy NonDoc'),
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
    'USPS' => array(
      'usps_priority' => t('USPS Priority Mail'),
      'usps_priority_express' => t('USPS Priority Mail Express'),
      'usps_first' => t('USPS First Class Mail/Package'),
      'usps_parcel_select' => t('USPS Parcel Select'),
      'usps_media_mail' => t('USPS Media Mail, only for existing Shippo customers with grandfathered Media Mail option.'),
      'usps_priority_mail_international' => t('USPS Priority Mail International'),
      'usps_priority_mail_express_international' => t('USPS Priority Mail Express International'),
      'usps_first_class_package_international_service' => t('USPS First Class Package International'),
    ),
    'FedEx' => array(
      'fedex_ground' => t('FedEx Ground'),
      'fedex_home_delivery' => t('FedEx Home Delivery'),
      'fedex_smart_post' => t('FedEx Smartpost'),
      'fedex_2_day' => t('FedEx 2 Day'),
      'fedex_2_day_am' => t('FedEx 2 Day A.M.'),
      'fedex_express_saver' => t('FedEx Express Saver'),
      'fedex_standard_overnight' => t('FedEx Standard Overnight'),
      'fedex_priority_overnight' => t('FedEx Priority Overnight'),
      'fedex_first_overnight' => t('FedEx First Overnight'),
      'fedex_international_economy' => t('FedEx International Economy'),
      'fedex_international_priority' => t('FedEx International Priority'),
      'fedex_international_first' => t('FedEx International First'),
      'fedex_europe_first_international_priority' => t('FedEx Europe First International Priority'),
    ),
    'Canada Post' => array(
      'canada_post_regular_parcel' => t('Canada Post Regular Parcel'),
      'canada_post_expedited_parcel' => t('Canada Post Expedited Parcel'),
      'canada_post_priority' => t('Canada Post Priority'),
      'canada_post_xpresspost' => t('Canada Post Xpresspost'),
      'canada_post_xpresspost_international' => t('Canada Post Xpresspost International'),
      'canada_post_xpresspost_usa' => t('Canada Post Xpresspost USA'),
      'canada_post_expedited_parcel_usa' => t('Canada Post Expedited Parcel USA'),
      'canada_post_tracked_packet_usa' => t('Canada Post Tracked Packet USA'),
      'canada_post_small_packet_usa_air' => t('Canada Post Small Packet USA Air'),
      'canada_post_tracked_packet_international' => t('Canada Post Tracked Packet International'),
      'canada_post_small_packet_international_air' => t('Canada Post Small Package International Air'),
    ),
    'Purolator' => array(
      'purolator_ground' => t('Purolator Ground'),
      'purolator_ground9_am' => t('Purolator Ground 9am'),
      'purolator_ground1030_am' => t('Purolator Ground 10:30am'),
      'purolator_ground_distribution' => t('Purolator Ground Distribution'),
      'purolator_ground_evening' => t('Purolator Ground Evening'),
      'purolator_ground_us' => t('Purolator Ground US'),
      'purolator_express' => t('Purolator Express'),
      'purolator_express9_am' => t('Purolator Express 9am'),
      'purolator_express1030_am' => t('Purolator Express 10am'),
      'purolator_express_evening' => t('Purolator Express Evening'),
      'purolator_express_us' => t('Purolator Express US'),
      'purolator_express_us9_am' => t('Purolator Express US 9am'),
      'purolator_express_us1030_am' => t('Purolator Express US 10:30am'),
      'purolator_express_us1200' => t('Purolator Express US 12pm'),
      'purolator_express_international' => t('Purolator Express International'),
      'purolator_express_international9_am' => t('Purolator Express International 9am'),
      'purolator_express_international1030_am' => t('Purolator Express International 10:30am'),
      'purolator_express_international1200' => t('Purolator Express International 12pm'),
    ),
    'Asendia' => array(
      'asendia_us_international_priority_airmail' => t('Asendia International Priority Airmail'),
      'asendia_us_international_surface_airlift' => t('Asendia International Surface Air Lift'),
    ),
    'Australia Post' => array(
      'australia_post_express_post' => t('Australia Post Express Post'),
      'australia_post_parcel_post' => t('Australia Post Parcel Post'),
    ),
    'DHL Germany' => array(
      'dhl_paket_business' => t('DHL Germany Paket (business customers)'),
      'dhl_weltpaket_business' => t('DHL Germany Weltpaket (business customers)'),
      'dhl_europaket_business' => t('DHL Germany Europaket (business customers)'),
    ),
    'Deutsche Post' => array(
      'deutsche_post_postkarte' => t('Deutsche Post Postkarte'),
      'deutsche_post_standardbrief' => t('Deutsche Post Standardbrief'),
      'deutsche_post_kompaktbrief' => t('Deutsche Post Kompaktbrief'),
      'deutsche_post_grossbrief' => t('Deutsche Post Grossbrief'),
      'deutsche_post_maxibrief' => t('Deutsche Post Maxibrief'),
      'deutsche_post_maxibrief_plus' => t('Deutsche Post Maxibrief Plus'),
    ),
    'GLS Germany' => array(
      'gls_deutschland_business_parcel' => t('GLS Germany Business Parcel'),
    ),
    'GLS France' => array(
      'gls_france_business_parcel' => t('GLS France Business Parcel'),
    ),
    'Mondial Relay' => array(
      'mondial_relay_pointrelais' => t('Mondial Relay Point Relais'),
    ),
    'Parcelforce' => array(
      'parcelforce_express48' => t('Parcelforce Express 48'),
      'parcelforce_express24' => t('Parcelforce Express 24'),
      'parcelforce_expressam' => t('Parcelforce Express AM'),
    ),
    'Newgistics' => array(
      'newgistics_parcel_select_lightweight' => t('Newgistics Parcel Select Lightweight'),
      'newgistics_parcel_select' => t('Newgistics Parcel Select'),
      'newgistics_priority_mail' => t('Newgistics Priority Mail'),
      'newgistics_first_class_mail' => t('Newgistics First Class Mail'),
    ),
    'OnTrac' => array(
      'ontrac_ground' => t('OnTrac Ground'),
      'ontrac_sunrise_gold' => t('OnTrac Sunrise Gold'),
      'ontrac_sunrise' => t('OnTrac Sunrise'),
    ),
    'Lasership' => array(
      'lasership_routed_delivery' => t('Lasership Routed Delivery'),
    ),
    'UberRUSH' => array(
      'uber_on_demand' => t('On Demand'),
    ),
    'Hermes UK' => array(
      'hermes_uk_parcelshop' => t('Hermes UK ParcelShop'),
      'hermes_uk_home_delivery' => t('Hermes UK Home Delivery'),
    ),
  );

  return empty($carriers) ? $package_services : array_intersect_key($package_services, drupal_map_assoc($carriers));
}

/**
 * Returns the carrier service key for the corresponding service name.
 */
function shippo_carrier_service_key($service) {

  $service = str_replace('shippo_', '', $service);
  $service_key = array_search($service, shippo_carrier_service_names());
  return $service_key;
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
 * @param array $service
 *   An array containing a 'name', 'title', and 'weight' of the service.
 * @param bool $update
 *   Boolean indicating whether or not the save should be processed as an update
 *   to an existing enabled service.
 * @param bool $skip_reset
 *   Boolean indicating whether or not the menu item cache and default rule
 *   configurations should be reset after the delete.
 *
 * @return bool|string
 *   The return value of the call to drupal_write_record() to save the txn type,
 *   either FALSE on failure or SAVED_NEW or SAVED_UPDATED indicating the type
 *   of query performed to save the txn type.
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
 * @param string $name
 *   The name of the package service to delete.
 * @param bool $skip_reset
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
