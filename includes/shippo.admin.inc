<?php

/**
 * @file
 * Handles admin settings page for Shippo module.
 */
function shippo_settings_form($form, &$form_state) {
  $goshippo_link = l(t('goShippo.com'), 'https://app.goshippo.com/api', array('attributes' => array('target' => '_blank')));

  // Shippo API related configuration.
  $form['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shippo API Information'),
    '#collapsible' => TRUE,
  );
  $form['api']['shippo_api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Shippo API Token'),
    '#description' => t('Enter the API live token associated with your account on !goshippo. You can feed in the test token here for testing purposes.', array('!goshippo' => $goshippo_link)),
    '#default_value' => variable_get('shippo_api_token'),
    '#required' => TRUE,
  );

  // Shipping sender address fields.
  $form['origin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ship From Address'),
    '#collapsible' => TRUE,
  );
  $form['origin']['shippo_sender_address_line_1'] = array(
    '#type' => 'textfield',
    '#title' => t('Address Line 1'),
    '#default_value' => variable_get('shippo_sender_address_line_1'),
  );
  $form['origin']['shippo_sender_address_line_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Address Line 2'),
    '#default_value' => variable_get('shippo_sender_address_line_2'),
  );
  $form['origin']['shippo_sender_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => variable_get('shippo_sender_city'),
  );
  $form['origin']['shippo_sender_state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#default_value' => variable_get('shippo_sender_state'),
    '#options' => array(
      '' => t('-- Please Select --'),
      'AL' => t('Alabama'),
      'AK' => t('Alaska'),
      'AZ' => t('Arizona'),
      'AR' => t('Arkansas'),
      'CA' => t('California'),
      'CO' => t('Colorado'),
      'CT' => t('Connecticut'),
      'DE' => t('Delaware'),
      'DC' => t('District of Columbia'),
      'FL' => t('Florida'),
      'GA' => t('Georgia'),
      'HI' => t('Hawaii'),
      'ID' => t('Idaho'),
      'IL' => t('Illinois'),
      'IN' => t('Indiana'),
      'IA' => t('Iowa'),
      'KS' => t('Kansas'),
      'KY' => t('Kentucky'),
      'LA' => t('Louisiana'),
      'ME' => t('Maine'),
      'MD' => t('Maryland'),
      'MA' => t('Massachusetts'),
      'MI' => t('Michigan'),
      'MN' => t('Minnesota'),
      'MS' => t('Mississippi'),
      'MO' => t('Missouri'),
      'MY' => t('Montana'),
      'NE' => t('Nebraska'),
      'NV' => t('Nevada'),
      'NH' => t('New Hampshire'),
      'NJ' => t('New Jersey'),
      'NM' => t('New Mexico'),
      'NY' => t('New York'),
      'NC' => t('North Carolina'),
      'ND' => t('North Dakota'),
      'OH' => t('Ohio'),
      'OK' => t('Oklahoma'),
      'OR' => t('Oregon'),
      'PA' => t('Pennsylvania'),
      'RI' => t('Rhode Island'),
      'SC' => t('South Carolina'),
      'SD' => t('South Dakota'),
      'TN' => t('Tennessee'),
      'TX' => t('Texas'),
      'UT' => t('Utah'),
      'VT' => t('Vermont'),
      'VA' => t('Virginia'),
      'WA' => t('Washington'),
      'WV' => t('West Virginia'),
      'WI' => t('Wisconsin'),
      'WY' => t('Wyoming'),
    ),
  );
  $form['origin']['shippo_sender_postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
    '#size' => 10,
    '#default_value' => variable_get('shippo_sender_postal_code'),
  );
  $form['origin']['shippo_sender_country_code'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#default_value' => variable_get('shippo_sender_country_code'),
    '#options' => array(
      '' => t('-- Please Select --'),
      'US' => t('United States of America'),
    ),
  );

  // Shipping service type options.
  $form['shippo_ship_services'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shipping Service Options'),
    '#collapsible' => TRUE,
  );
  $form['shippo_ship_services']['service_type'] = array(
    '#type' => 'radios',
    '#title' => t('Shipping Service Type'),
    '#default_vlaue' => variable_get('shippo_ship_service_type'),
    '#options' => array(
      'cheapest' => t('Cheapest'),
      'fastest' => t('Fastest'),
    ),
    '#description' => t('Based on your choice, the cheapest or the fastest available shipping service will be used for shipping cost estimation.'),
  );

  // Fields for default package size (inches).
  $form['default_package_size'] = array(
    '#type' => 'fieldset',
    '#title' => t('Package size (inches)'),
    '#collapsible' => TRUE,
    '#description' => t('Shippo requires a package size when determining estimates.'),
  );
  $form['default_package_size']['shippo_default_package_size_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Length'),
    '#size' => 5,
    '#required' => TRUE,
    '#default_value' => variable_get('shippo_default_package_size_length'),
  );
  $form['default_package_size']['shippo_default_package_size_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#size' => 5,
    '#required' => TRUE,
    '#default_value' => variable_get('shippo_default_package_size_width'),
  );
  $form['default_package_size']['shippo_default_package_size_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#size' => 5,
    '#required' => TRUE,
    '#default_value' => variable_get('shippo_default_package_size_height'),
  );
  $form['default_package_size']['details'] = array(
    '#markup' => 'The package size is used to determine the number of packages necessary to
      create a Shippo shipping cost estimate. <strong>If products do not have physical dimensions and
      weights associated with them, the estimates will not be accurate.</strong> The logic implemented works as:
      <ul>
      <li>Assume each order requires at least one package.</li>
      <li>Use the combined volume of all products in an order to determine the number of packages.</li>
      </ul>
      This is a simple calculation that can get close to actual shipping costs. More
      complex logic involving multiple package sizes, weights, and void space can be implemented via
      custom modules.
      <br/><br/>
      Shippo has a maximum package size similar to that of ' . l('UPS', 'http://www.ups.com/content/us/en/resources/prepare/oversize.html', array('attributes' => array('target' => '_blank'))) . '.',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

/*
 * Implements hook_form_validate().
 */
function shippo_settings_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Ensure the default package size is less than the maximum permissible size.
  //  Length (longest side) <= 108 inches
  //  Girth = 2*width + 2*height
  //  Length + Girth <= 165 inches
  $dimensions = array($values['shippo_default_package_size_length'],
                      $values['shippo_default_package_size_width'],
                      $values['shippo_default_package_size_height']);
  sort($dimensions);
  list($height, $width, $length) = $dimensions;
  $girth = 2*$width + 2*$height;
  if ($length > 108) {
    form_set_error('shippo_default_package_size_length', t('The greatest dimension of the package size must be 108 inches or less.'));
  }
  if ($length + $girth > 165) {
    form_set_error('shippo_default_package_size_length', t('The girth (2*width + 2*height) of the package size must be 165 inches or less.'));
  }
}

/**
 * Implements hook_form_submit().
 */
function shippo_settings_form_submit($form, &$form_state) {
  $fields = array(
    'shippo_api_token',
    'shippo_sender_address_line_1',
    'shippo_sender_address_line_2',
    'shippo_sender_city',
    'shippo_sender_state',
    'shippo_sender_postal_code',
    'shippo_sender_country_code',
    'shippo_ship_service_type',
    'shippo_default_package_size_length',
    'shippo_default_package_size_width',
    'shippo_default_package_size_height',
  );

  foreach ($fields as $key) {
    if (array_key_exists($key, $form_state['input'])) {
      $value = $form_state['input'][$key];
      variable_set($key, $value);
    }
  }
  drupal_set_message(t('The Shippo configuration options have been saved.'));
}
