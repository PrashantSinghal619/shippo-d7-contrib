<?php

/**
 * @file
 * Handles admin settings page for Shippo module.
 */
module_load_include('inc', 'shippo', 'shippo.helper');

/**
 * Shippo admin configuration form.
 */
function shippo_settings_form($form, &$form_state) {

  // Shippo API related configuration.
  $form['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shippo API Information'),
    '#collapsible' => TRUE,
  );
  $form['api']['shippo_api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Shippo API Token'),
    '#description' => t('Enter the API live token associated with your account on <a href="@goshippo" target="_blank">goShippo.com</a>. You can feed in the test token here for testing purposes.', array('@goshippo' => 'https://app.goshippo.com/api')),
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
    '#required' => TRUE,
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
    '#required' => TRUE,
  );
  $form['origin']['shippo_sender_state'] = array(
    '#type' => 'textfield',
    '#title' => t('State'),
    '#description' => t('State is mandatory for US and CA only. Please use two character abbreviation provided <a href="@url" target="_blank">here</a>.', array('@url' => url('http://www.advertserve.com/docs/2.0/html/manual/api_reference_state.html'))),
    '#size' => 10,
    '#maxlength' => 2,
    '#default_value' => variable_get('shippo_sender_state'),
    '#required' => variable_get('shippo_sender_state_require', TRUE),
    '#prefix' => '<div id="sender-address-placeholder">',
    '#suffix' => '</div>',
  );
  $form['origin']['shippo_sender_postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
    '#size' => 10,
    '#default_value' => variable_get('shippo_sender_postal_code'),
    '#required' => TRUE,
  );
  $form['origin']['shippo_sender_phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone Number'),
    '#default_value' => variable_get('shippo_sender_phone'),
    '#required' => TRUE,
  );

  // Get countries list from Drupal core's locale.inc using country_get_list().
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $form['origin']['shippo_sender_country_code'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#default_value' => variable_get('shippo_sender_country_code'),
    '#required' => TRUE,
    '#options' => country_get_list(),
    '#ajax' => array(
      'callback' => 'shippo_config_form_ajax_callback',
      'wrapper' => 'sender-address-placeholder',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  // Make the State field 'required' based on the Country field.
  if (isset($form_state['values']['shippo_sender_country_code'])) {

    if (in_array($form_state['values']['shippo_sender_country_code'], array('US', 'CA'))) {
      $form['origin']['shippo_sender_state']['#required'] = TRUE;
    }
    else {
      $form['origin']['shippo_sender_state']['#required'] = FALSE;
    }

    variable_set('shippo_sender_state_require', $form['origin']['shippo_sender_state']['#required']);
  }

  // Fields for customs info for international shipments.
  $form['customs_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('International Customs Info'),
    '#collapsible' => TRUE,
    '#description' => t('Customs information must be declared for international shipments.'),
  );
  $form['customs_info']['shippo_customs_contents_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of goods/content to ship'),
    '#required' => TRUE,
    '#default_value' => variable_get('shippo_customs_contents_type', 'MERCHANDISE'),
    '#options' => array(
      'DOCUMENTS' => t('Documents'),
      'GIFT' => t('Gift'),
      'SAMPLE' => t('Sample'),
      'MERCHANDISE' => t('Merchandise'),
      'RETURN_MERCHANDISE' => t('Return merchandise'),
      'HUMANITARIAN_DONATION' => t('Humanitarian Donation'),
      'OTHER' => t('Other'),
    ),
  );
  $form['customs_info']['shippo_customs_content_explain'] = array(
    '#type' => 'textfield',
    '#title' => t('Content explanation'),
    '#required' => TRUE,
    '#default_value' => variable_get('shippo_customs_content_explain'),
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
      Shippo has a maximum package size similar to that of ' . l(t('UPS'), 'http://www.ups.com/content/us/en/resources/prepare/oversize.html', array('attributes' => array('target' => '_blank'))) . '.',
  );

  // Shipping service options.
  $form['services'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enable Shippo shipping services'),
    '#description' => t('Please ensure that you select the services provided by only the enabled carriers on your Shippo account at www.goshippo.com.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );

  // Build an options array of all available Shippo shipping services.
  $shipping_services = shippo_carrier_services();

  foreach ($shipping_services as $optgroup => $optgroup_options) {
    $form['services'][strtr($optgroup, ' ', '-')] = array(
      '#type' => 'checkboxes',
      '#title' => check_plain($optgroup),
      '#options' => $optgroup_options,
      '#default_value' => array_keys(shippo_load_services()),
    );
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

/**
 * Shippo configuration form AJAX callback.
 */
function shippo_config_form_ajax_callback($form, $form_state) {
  return $form['origin']['shippo_sender_state'];
}

/*
 * Implements hook_form_validate().
 */
function shippo_settings_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Allow only alphabet letters in Sender From Address' State field.
  if (isset($values['shippo_sender_state']) && !ctype_alpha($values['shippo_sender_state'])) {
    form_set_error('origin][shippo_sender_state', t('Only alphabet characters allowed for State field under Ship From Address.'));
  }

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
    'shippo_sender_state_require',
    'shippo_sender_postal_code',
    'shippo_sender_phone',
    'shippo_sender_country_code',
    'shippo_customs_contents_type',
    'shippo_customs_content_explain',
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

  // Load the enabled services.
  $enabled_services = shippo_load_services();
  $all_services = shippo_carrier_services();

  // Loop over the shipping services...
  foreach ($form_state['values']['services'] as $optgroup => $options) {
    foreach ($options as $key => $value) {
      // If a service was marked to be enabled and hasn't been saved yet, save
      // it to the database now.
      if ($key === $value && !isset($enabled_services[$key])) {
        $service = array(
          'name' => $key,
          'title' => $all_services[strtr($optgroup, '-', ' ')][$key],
        );

        shippo_save_service($service);
      }
      // Otherwise if a service that is enabled was unchecked, delete it from
      // the database if possible now.
      elseif ($value == '0' && isset($enabled_services[$key])) {
        // Check to make sure no line items are depending on this line item.
        $query = new EntityFieldQuery();

        $query
          ->entityCondition('entity_type', 'commerce_line_item', '=')
          ->fieldCondition('commerce_shipping_service', 'value', shippo_commerce_shipping_service_name($key), '=')
          ->count();

        // If no line items were found...
        if ($query->execute() == 0) {
          // Delete the record from the database.
          shippo_delete_service($key);
        }
        else {
          // Otherwise display a message indicating why it couldn't be disabled.
          $title = check_plain($all_services[strtr($optgroup, '-', ' ')][$key]);
          drupal_set_message(t('Could not disable the @title service, because a shipping line item exists for it.', array('@title' => $title)), 'error');
        }
      }
    }
  }

  drupal_set_message(t('Shippo configuration saved.'));

  $form_state['redirect'] = 'admin/commerce/config/shipping/methods';
}
