<?php

/*
 * @file
 * Provides various hooks for other modules to modify Shippo module's
 * functionality.
 */

/**
 * Allow alterations to shipping quote rate response from Shippo.
 * @param json $shipping_quote_rate_response
 *   Serialised data response from Shippo on quote rate request. Alterable.
 * @param object $order
 *   The commerce_order object corresponding to current user's order.
 * @param array $context
 *   Array containing parameters that $shipping_quote_rate_response depends on.
 *   Possible array keys to retrieve corresponding values:
 *   - shipfrom_address: Sender's address info. Returns array.
 *   - shipto_address: Receiver's address info. Returns array.
 *   - parcel_array: Info about parcel(s) involved in shipment. Returns array.
 *   - customs_declaration: Customs declaration info related to shipment.
 *     Returns json object.
 */
function hook_shippo_request_quote_rates_alter(&$shipping_quote_rate_response, $order, $context) {
  // No example.
}

/**
 * Allow alterations to the construction of receiver's shipping address.
 * @param array $shipping_address
 *   In context customer/receiver's shipping address. Alterable.
 * @param object $order
 *   The commerce_order object corresponding to current user's order.
 */
function hook_shippo_prepare_shipping_address_alter(&$shipping_address, $order) {
  // No example.
}

/**
 * Allow alterations to parcels array storing information about parcel(s)
 * involved in the shipment.
 * @param array $parcels
 *   Array containing information about parcel(s) involved in the shipment.
 * @param object $order
 *   The commerce_order object corresponding to current user's order.
 * @param array $context
 *   Array containing parameters that $parcels depends on.
 *   Possible array keys to retrieve corresponding values:
 *   - default_package_length: Default length of a single package.
 *   - default_package_width: Default width of a single package.
 *   - default_package_height: Default height of a single package.
 *   - order_weight: Total weight of the goods involved in the order.
 *   - order_volume: Total volume of the goods involved in the order.
 */
function hook_shippo_get_parcel_info_alter(&$parcels, $order, $context) {
  // No example.
}

/**
 * Allow alterations to the customs items array storing information about the
 * customs items involved in the shipment.
 * @param array $customs_items
 *   Array containing information about the customs items involved in shipment.
 * @param object $order
 *   The commerce_order object corresponding to current user's order.
 */
function hook_shippo_get_customs_info_alter(&$customs_items, $order) {
  // No example.
}
