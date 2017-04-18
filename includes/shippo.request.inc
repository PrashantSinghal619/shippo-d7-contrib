<?php

/**
 * @file
 * Handles requests to Shippo server for Shippo module.
 */
require_once(libraries_get_path('shippo-php-client') . '/lib/Shippo.php');
module_load_include('inc', 'shippo', 'shippo.helper');

/**
 * Sets the session with Shippo server through the API key.
 */
function shippo_set_api_key() {
  $api_key = variable_get('shippo_api_token');

  if (!$api_key) {
    drupal_set_message(t('Unable to connect to Shippo due to invalid or missing Shippo API key. Please contact site administrator.'), 'error', FALSE);
    watchdog('shippo', 'Missing or invalid Shippo API key in Shippo shipping method configuration form.', array(), WATCHDOG_ERROR, l(t('View'), 'admin/commerce/config/shipping/methods/shippo/edit'));
    return FALSE;
  }

  // Try to establish a Session with Shippo server.
  try {
    Shippo::setApiKey($api_key);
  }
  catch (Shippo_ApiConnectionError $e) {
    drupal_set_message(t('Unable to connect to Shippo shipment services provider. Please try again.'), 'error', FALSE);
    watchdog('shippo', 'API connection error: Unable to connect to Shippo shipment services provider.', array(), WATCHDOG_ERROR);
    return FALSE;
  }
}

/**
 * Requests Shippo for shipping quote rates.
 */
function shippo_request_quote_rates($order) {
  $purpose = 'QUOTE';

  // Ship To - Customer Shipping Address.
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Determine the shipping profile reference field name for the order.
  $field_name = commerce_physical_order_shipping_field_name($order);
  $shipping_profile = $order_wrapper->{$field_name}->value();

  // Prepare the shipping address for use in the request.
  if (!empty($order_wrapper->{$field_name}->commerce_customer_address)) {
    $shipping_address = $order_wrapper->{$field_name}->commerce_customer_address->value();
    $shipping_address['phone'] = '';
  }
  else {
    drupal_set_message(t('No customer shipping address provided.'), 'error', FALSE);
    watchdog('shippo', 'The shipping quote rates fetch request could not be completed because no customer shipping address was provided for the order no. %order_no.', array('%order_no' => $order->order_id), WATCHDOG_ERROR);
    return FALSE;
  }

  // Add the phone number if available. Since we have made the phone field
  // required, it's unnecessary to handle errors related to missing phone
  // number.
  if ($phone_field_name = commerce_physical_customer_profile_phone_number_field_name($shipping_profile)) {
    if (!empty($order_wrapper->{$field_name}->{$phone_field_name})) {
      $shipping_address['phone'] = $order_wrapper->{$field_name}->{$phone_field_name}->value();
    }
  }

  $order_customer_email = $order->mail;
  $shipto_address = get_receiver_address($purpose, $shipping_address, $order_customer_email);

  // Ship From - Sender Address.
  $shipfrom_address = get_sender_address($purpose);

  $parcel_array = get_parcel_info($order);
  if (!$parcel_array) {
    drupal_set_message(t('The number of packages could not be computed.'), 'warning', FALSE);
    watchdog('shippo', t('The number of packages could not be computed.'), array(), WATCHDOG_WARNING);
    return FALSE;
  }

  // Handle customs declarations for international shipments.
  if ($shipfrom_address['country'] != $shipto_address['country']) {
    $customs_items = array();
    $customs_items = get_customs_info($order);

    // Handling Shippo API connectivity error.
    try {
      $customs_declaration = Shippo_CustomsDeclaration::create(
        array(
          'contents_type' => variable_get('shippo_customs_contents_type'),
          'contents_explanation' => variable_get('shippo_customs_content_explain'),
          'non_delivery_option' => 'RETURN',
          'certify' => 'true',
          'certify_signer' => $shipfrom_address['name'],
          'items' => $customs_items,
        )
      );
    }
    catch (Shippo_ApiConnectionError $e) {
      drupal_set_message(t('Unable to connect to Shippo shipment services provider. Please try again.'), 'error', FALSE);
      watchdog('shippo', t('Unable to connect to Shippo while attempting to create Shippo Customs Declaration Object.'), array(), WATCHDOG_ERROR);
      return FALSE;
    }

  }
  else {
    $customs_declaration = new stdClass();
  }

  // Fetch shipment rates for the selected services.
  $shipping_quote_rate_response = retrieve_shipment_rates($purpose, $shipfrom_address, $shipto_address, $parcel_array, $customs_declaration);

  if (empty((array) $shipping_quote_rate_response)) {
    drupal_set_message(t('Either you have entered incorrect/incomplete shipping address or there are no carriers available that ship to your address.'), 'error', FALSE);
    watchdog('shippo', t('Either shipping address validation failed or none of the available carriers ship to the provided customer address.'), array(), WATCHDOG_ERROR);
    return FALSE;
  }
  if (!$shipping_quote_rate_response) {
    return FALSE;
  }

  return $shipping_quote_rate_response;
}

/**
 * Helper function to return an array storing info about sender's (ship from)
 * address to be sent in Shippo rate request.
*/
function get_sender_address($purpose) {
  $fromAddress = array();
  $fromAddress = array(
    'object_purpose' => $purpose,
    'name' => variable_get('site_name'),
    'street1' => variable_get('shippo_sender_address_line_1'),
    'street2' => variable_get('shippo_sender_address_line_2'),
    'city' => variable_get('shippo_sender_city'),
    'state' => variable_get('shippo_sender_state'),
    'zip' => variable_get('shippo_sender_postal_code'),
    'country' => variable_get('shippo_sender_country_code', 'US'),
    'phone' => variable_get('shippo_sender_phone'),
    'email' => variable_get('site_mail'),
  );

  return $fromAddress;
}

/**
 * Helper function to return an array storing info about receiver's (ship to)
 * address to be sent in Shippo rate request.
*/
function get_receiver_address($purpose, $customer_shipping_address, $customer_email) {
  $toAddress = array();
  $toAddress = array(
    'object_purpose' => $purpose,
    'name' => $customer_shipping_address['name_line'],
    'street1' => $customer_shipping_address['thoroughfare'],
    'street2' => $customer_shipping_address['premise'],
    'city' => $customer_shipping_address['locality'],
    'state' => $customer_shipping_address['administrative_area'],
    'zip' => $customer_shipping_address['postal_code'],
    'country' => $customer_shipping_address['country'],
    'phone' => $customer_shipping_address['phone'],
    'email' => $customer_email,
  );

  return $toAddress;
}

/**
 * Helper function to return an array storing info about parcels.
 */
function get_parcel_info($order) {
  // This returns $weight['unit'] and $weight['weight'].
  $weight = commerce_physical_order_weight($order, 'lb');
  // This returns $volume['unit'] and $weight['volume'].
  $volume = commerce_physical_order_volume($order, 'in');

  $default_package_length = variable_get('shippo_default_package_size_length', '0');
  $default_package_width = variable_get('shippo_default_package_size_width', '0');
  $default_package_height = variable_get('shippo_default_package_size_height', '0');

  $default_package_volume = $default_package_length * $default_package_width * $default_package_height;

  // If there is no default package volume, we cannot calculate the number of
  // packages and there is no reason to send to Shippo.
  if ($default_package_volume == 0) {
    drupal_set_message(t('There was an error with the Shippo configuration.'), 'error', FALSE);
    watchdog('shippo', 'The default measurements for the shippo module is empty or is set to zero. Please set the default package dimensions in the settings page for the shippo module. Without the default measurements this module cannot calculate the number of packages and Shippo rates will not be displayed.', array(), WATCHDOG_ERROR, l(t('View'), 'admin/commerce/config/shipping/methods/shippo/edit'));
    return FALSE;
  }

  // If there is no total volume or weight for the order, there is no reason to
  // send the request to Shippo.
  if ($volume['volume'] == NULL || $weight['weight'] == NULL) {
    return FALSE;
  }

  $package_count = ceil($volume['volume'] / $default_package_volume);

  $per_package_weight = $weight['weight'] / $package_count;

  // Based on package count, declare the parcels.
  for ($i=0; $i < $package_count; $i++) {
    $parcels[$i] = array(
      'length' => $default_package_length,
      'width' => $default_package_width,
      'height' => $default_package_height,
      'distance_unit' => 'in',
      'weight' => $per_package_weight,
      'mass_unit' => 'lb',
    );
  }

  return $parcels;
}

/**
 * Helper function to return an array storing info about customs items.
 */
function get_customs_info($order) {
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
    $line_item_quantity = $line_item_weight = $net_weight = 0;

    if (in_array($line_item_wrapper->type->value(), array('product', 'product_discount'))) {
      $product_id = $line_item_wrapper->commerce_product->raw();
      $line_item_quantity = $line_item_wrapper->quantity->value();
      $line_item_weight = $line_item_wrapper->commerce_product->field_product_weight->weight->value();
      $net_weight = round($line_item_weight * $line_item_quantity, 2);
      $product = commerce_product_load($product_id);
      $product_amount = $product->commerce_price[LANGUAGE_NONE][0]['amount'];
      $product_currency_code = $product->commerce_price[LANGUAGE_NONE][0]['currency_code'];
      $product_formatted_amount = (float) str_replace(',', '', substr(commerce_currency_format($product_amount, $product_currency_code), 1));
      $customs_items[] = array(
        'description' => $product->title,
        'quantity' => (int) $line_item_quantity,
        'net_weight' => $net_weight,
        'mass_unit' => $product->field_product_weight[LANGUAGE_NONE][0]['unit'],
        'value_amount' => $product_formatted_amount * $line_item_quantity,
        'value_currency' => $product_currency_code,
        'origin_country' => variable_get('shippo_sender_country_code', 'US'),
      );
    }
  }

  return $customs_items;
}

/**
 * Helper function to retrieve shipping rates or create a live shipment.
 *
 * @param $purpose
 *   The purpose for which shipment object is being created - QUOTE or PURCHASE.
 * @param $fromAddress
 *   Sender merchant's warehouse address.
 * @param $toAddress
 *   Recepient user's address to which the order is to be shipped.
 * @param $parcel
 *   Parcel item or an array of parcel items with dimensions.
 * @param $customs
 *   (optional) Declared customs item(s) info.
 *
 * @return $selected_rate
 *   The decoded rate object retrieved for Shippo containing info about shipment
 *   rates and a pre-requisite for creating a transaction to generate a shipment
 *   label.
 */
function retrieve_shipment_rates($purpose, $fromAddress, $toAddress, $parcel, $customs) {
  $shipment_input_info = array(
    'object_purpose' => $purpose,
    'address_from' => $fromAddress,
    'address_to' => $toAddress,
    'parcel' => $parcel,
    'async' => false,
  );

  // Declare customs for international shipments.
  if (!empty((array) $customs)) {
    $shipment_input_info['customs_declaration'] = $customs->object_id;
  }

  try {
    // Retrieve shipment object from Shippo.
    $shipment = Shippo_Shipment::create($shipment_input_info);
  }
  catch (Shippo_ApiConnectionError $e) {
    drupal_set_message(t('Unable to connect to Shippo shipment services provider. Please try again.'), 'error', FALSE);
    watchdog('shippo', 'API connection error: Unable to connect to Shippo shipment services provider.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  $rates = $shipment["rates_list"];
  $selected_rates = array();

  foreach ($rates as $rate) {
    $decoded_rate = json_decode($rate);

    // Store selected services' rates.
    foreach (shippo_load_services() as $key => $value) {
      if ($key == $decoded_rate->servicelevel_token) {
        $selected_rates[] = $decoded_rate;
      }
    }
  }

  return $selected_rates;
}
