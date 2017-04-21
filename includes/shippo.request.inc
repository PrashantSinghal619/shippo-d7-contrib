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
  $shipping_address = shippo_prepare_shipping_address($order);

  if (!$shipping_address) {
    return FALSE;
  }

  $order_customer_email = $order->mail;
  $shipto_address = get_receiver_address($purpose, $shipping_address, $order_customer_email);

  // Ship From - Sender Address.
  $shipfrom_address = get_sender_address($purpose);

  $parcel_array = shippo_get_parcel_info($order);
  if (!$parcel_array) {
    drupal_set_message(t('The number of packages could not be computed.'), 'error', FALSE);
    watchdog('shippo', 'The number of packages could not be computed.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // Handle customs declarations for international shipments.
  if ($shipfrom_address['country'] != $shipto_address['country']) {
    $customs_declaration = shippo_declare_customs($shipfrom_address, $order);

    if (!$customs_declaration) {
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
    watchdog('shippo', 'Either shipping address validation failed or none of the available carriers ship to the provided customer address.', array(), WATCHDOG_ERROR);
    return FALSE;
  }
  if (!$shipping_quote_rate_response) {
    return FALSE;
  }

  $context = array(
    'shipfrom_address' => $shipfrom_address,
    'shipto_address' => $shipto_address,
    'parcel_array' => $parcel_array,
    'customs_declaration' => $customs_declaration,
  );
  // Allow other modules to make changes to shipping quote rate response.
  drupal_alter('shippo_request_quote_rates', $shipping_quote_rate_response, $order, $context);
  return $shipping_quote_rate_response;
}

/**
 * Purchase a shipping rate thus returning shipping label(s) and
 * tracking info for the shipment.
 */
function shippo_purchase_shipping_rate($order) {
  $purpose = 'PURCHASE';
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {

    if ($line_item_wrapper->type->value() == 'shipping') {
      $service = shippo_carrier_service_key($line_item_wrapper->commerce_shipping_service->value());
    }

  }

  // Ship To - Customer Shipping Address.
  $shipping_address = shippo_prepare_shipping_address($order);

  if (!$shipping_address) {
    return FALSE;
  }

  $order_customer_email = $order->mail;
  $shipto_address = get_receiver_address($purpose, $shipping_address, $order_customer_email);

  // Ship From - Sender Address.
  $shipfrom_address = get_sender_address($purpose);

  $parcel_array = shippo_get_parcel_info($order);
  if (!$parcel_array) {
    drupal_set_message(t('The number of packages could not be computed.'), 'error', FALSE);
    watchdog('shippo', 'The number of packages could not be computed.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // Handle customs declarations for international shipments.
  if ($shipfrom_address['country'] != $shipto_address['country']) {
    $customs_declaration = shippo_declare_customs($shipfrom_address, $order);

    if (!$customs_declaration) {
      return FALSE;
    }

  }
  else {
    $customs_declaration = new stdClass();
  }

  // Fetch available shipment purchase rates.
  $shipping_purchase_rate_response = retrieve_shipment_rates($purpose, $shipfrom_address, $shipto_address, $parcel_array, $customs_declaration);

  if (empty((array) $shipping_purchase_rate_response)) {
    drupal_set_message(t('Either you have entered incorrect/incomplete shipping address or there are no carriers available that ship to your address.'), 'error', FALSE);
    watchdog('shippo', 'Either shipping address validation failed or none of the available carriers ship to the provided customer address.', array(), WATCHDOG_ERROR);
    return FALSE;
  }
  if (!$shipping_purchase_rate_response) {
    return FALSE;
  }

  foreach ($shipping_purchase_rate_response as $rate_object) {

    if (isset($service) && $rate_object->servicelevel_token == $service) {
      $rate_object_id = $rate_object->object_id;
      // Purchase the selected rate.
      $transaction = shippo_create_shipment_transaction($rate_object_id);
      // Retrieve info about the purchased shipment rate.
      $purchase_shipment_data = shippo_get_post_purchase_shipping_info($transaction, sizeof($parcel_array), $rate_object_id);
    }

  }

  foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {

    if ($line_item_wrapper->type->value() == 'shipping') {
      $shipping_line_item = commerce_line_item_load($line_item_wrapper->line_item_id->value());
    }

  }

  if (isset($shipping_line_item) && isset($purchase_shipment_data)) {
    // Store post purchase shipment info into shipping line item data.
    $shipping_line_item->data['shipment_details'] = $purchase_shipment_data;
    entity_save('commerce_line_item', $shipping_line_item);
  }

}

/**
 * Prepare customer shipping address info.
 */
function shippo_prepare_shipping_address($order) {
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

  // Allow other modules to alter the construction of receiver shipping address.
  drupal_alter('shippo_prepare_shipping_address', $shipping_address, $order);
  return $shipping_address;
}

/**
 * Helper function to return an array storing info about sender's (ship from)
 * address to be sent in Shippo rate request.
*/
function get_sender_address($purpose) {
  $from_address = array();
  $from_address = array(
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

  return $from_address;
}

/**
 * Helper function to return an array storing info about receiver's (ship to)
 * address to be sent in Shippo rate request.
*/
function get_receiver_address($purpose, $customer_shipping_address, $customer_email) {
  $to_address = array();
  $to_address = array(
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

  return $to_address;
}

/**
 * Helper function to return an array storing info about parcels.
 */
function shippo_get_parcel_info($order) {
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

  $context = array(
    'default_package_length' => $default_package_length,
    'default_package_width' => $default_package_width,
    'default_package_height' => $default_package_height,
    'order_weight' => $weight,
    'order_volume' => $volume,
  );
  // Allow other modules to modify the shipments parcel info.
  drupal_alter('shippo_get_parcel_info', $parcels, $order, $context);
  return $parcels;
}

/**
 * Helper function to return an array storing info about customs items.
 */
function shippo_get_customs_info($order) {
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

  // Allow other modules to modify the customs items array.
  drupal_alter('shippo_get_customs_info', $customs_items, $order);
  return $customs_items;
}

/**
 * Customs declaration for international shipments.
 */
function shippo_declare_customs($shipfrom_address, $order) {
  $customs_items = array();
  $customs_items = shippo_get_customs_info($order);

  // Handling Shippo API connectivity error while attempting to declare customs
  // information.
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
    watchdog('shippo', 'Unable to connect to Shippo while attempting to create Shippo Customs Declaration Object.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  return $customs_declaration;
}

/**
 * Helper function to create Shippo transaction object to purchase a shipment
 * rate.
 * @param $rate_object_id
 *   Object ID of the selected rate from among the Shipment response rates from
 *   Shippo.
 * @return $transaction
 *   Decoded Transaction object returned by Shippo.
 */
function shippo_create_shipment_transaction($rate_object_id) {
  try {
    $transaction = Shippo_Transaction::create(
      array(
        'rate' => $rate_object_id,
        'label_file_type' => "PDF",
        'async' => FALSE,
      )
    );
  }
  catch (Shippo_ApiConnectionError $e) {
    drupal_set_message(t('Unable to connect to Shippo shipment services provider. Please try again.'), 'error', FALSE);
    watchdog('shippo', 'Unable to connect to Shippo while attempting to create Shippo Transaction Object.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  $transaction = json_decode($transaction);
  return $transaction;
}

/**
 * Helper function to get post shipping rate purchase shipment data e.g.
 * shipping label(s) url, tracking url, etc.
 *
 * @param $master_transaction
 *   The transaction object corresponding to the master (first) parcel involved
 *   in the shipment.
 * @param $package_count
 *   The number of packages involved in the shipment.
 * @param $rate_object_id
 *   Object ID of the selected rate from among the Shipment response rates from
 *   Shippo.
 * @return $shipment_line_item_data
 *   Return post-purchase shipment data.
 */
function shippo_get_post_purchase_shipping_info($master_transaction, $package_count, $rate_object_id) {

  if ($master_transaction->object_status === 'SUCCESS') {

    if ($package_count > 1) {
      $transactions = Shippo_Transaction::all(
        array(
          'rate' => $rate_object_id,
        )
      );

      $transactions = json_decode($transactions);
      $slave_transactions = array();
      $slave_tracking_numbers = array();
      $slave_tracking_urls = array();
      $slave_label_urls = array();

      foreach ($transactions->results as $transaction) {

        if ($transaction->object_status !== 'SUCCESS') {
          drupal_set_message(t('There was a problem processing a Shipment transaction. Please contact the site administrator.'), 'error', FALSE);
          $failure_messages = array();

          foreach ($transaction->messages as $message) {
            $failure_messages[] = $message->text;
          }

          $failure_messages = implode("\n", $failure_messages);
          watchdog('shippo', $failure_messages, array(), WATCHDOG_ERROR);
          return FALSE;
        }

        if ($transaction->tracking_number !== $master_transaction->tracking_number) {
          $slave_transactions[] = $transaction;
          $slave_tracking_numbers[] = $transaction->tracking_number;
          $slave_tracking_urls[] = $transaction->tracking_url_provider;
          $slave_label_urls[] = $transaction->label_url;
        }

      }

    }

    // Populate post-purchase data to be stored with the shipping line item.
    $shipment_line_item_data = array(
      'master_tracking_number' => $master_transaction->tracking_number,
      'master_tracking_url' => $master_transaction->tracking_url_provider,
      'master_label_url' => $master_transaction->label_url,
    );

    if (!empty($slave_transactions)) {
      $shipment_line_item_data['slave_tracking_numbers'] = $slave_tracking_numbers;
      $shipment_line_item_data['slave_tracking_urls'] = $slave_tracking_urls;
      $shipment_line_item_data['slave_label_urls'] = $slave_label_urls;
    }

    if (!empty($master_transaction->commercial_invoice_url)) {
      $shipment_line_item_data['commercial_invoice_url'] = $master_transaction->commercial_invoice_url;
    }

  }
  else {
    drupal_set_message(t('There was a problem processing a Shipment transaction. Please contact the site administrator.'), 'error', FALSE);
    $failure_messages = array();

    foreach ($master_transaction->messages as $message) {
      $failure_messages[] = $message->text;
    }

    $failure_messages = implode("\n", $failure_messages);
    watchdog('shippo', $failure_messages, array(), WATCHDOG_ERROR);
    return FALSE;
  }

  return $shipment_line_item_data;
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
function retrieve_shipment_rates($purpose, $from_address, $to_address, $parcel, $customs) {
  $shipment_input_info = array(
    'object_purpose' => $purpose,
    'address_from' => $from_address,
    'address_to' => $to_address,
    'parcel' => $parcel,
    'async' => FALSE,
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
