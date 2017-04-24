### About this module ###

Shippo module for Drupal 7 provides real-time shipping cost estimates for
various shipment carriers e.g. UPS, USPS, DHL Express, Canada Post, FedEx, etc.
and shipping label creation along with tracking information functionality.
Version: 1.0
Drupal core version: 7.x

### How do I get set up? ###

1. **Pre-requisites**

  - You will need a valid Shippo account on https://goshippo.com/ to get a
  working API key. There are two types of API keys available:
    - Test API token: For testing purposes only.
    - Live API token: For production purposes only.
  - Shippo levies a label creation cost of $0.05 - see https://support.goshippo.com/hc/en-us/articles/115001960683-Shippo-billing-process
  - To facilitate billing in Shippo, you will need to setup your default payment
  method on the Billing tab in your Shippo dashboard - see https://support.goshippo.com/hc/en-us/articles/115000997566-How-to-pay-for-shipments-in-Shippo
  - You will need to activate Carrier accounts on Shippo dashboard (you should
  have a valid account with the concerned carrier prior to that with all
  necessary features activated). You can also use Default carrier accounts
  activated by Shippo.
  - I encourage you to go through Shippo's help resources at https://support.goshippo.com/hc/en-us
  to better understand the Shippo workflow, features and precautions. Also,
  you can better understand which carrier services would be apt for your
  marketplace at https://support.goshippo.com/hc/en-us/categories/200384565-Carriers.


2. **Setup**

  - Download and enable all the modular dependencies mentioned below.
  - Download the 'shippo-php-client' library to your drupal site's 'libraries'
  directory. This library is available on github (https://github.com/goshippo/shippo-php-client).
  - Download and enable the module from d.o (https://www.drupal.org/project/shippo).
  - Easiest of all, if you have drush running, do 'drush pm-enable shippo' to
  download and enable all dependencies and then the module. Remember that you
  will still have to download the 'shippo-php-client' manually.


3. **Configuration**

  1. Shippo module configuration form has been provided at
  'Store > Configuration > Shipping > Shipping Methods > Shippo' in your drupal
  admin panel. Under the ENABLE SHIPPO SHIPPING SERVICES section, enable the
  desired services of only those carriers which have been activated by you on
  your Shippo dashboard as mentioned above in Pre-requisites section.
  2. You can find the enabled shipping services at
  'Store > Configuration > Shipping > Shipping Services > Shippo' in your drupal
  admin panel. You can associate rules to individual services using
  'configure component' option in front of it.
  3. You will need to add two fields of type - Physical dimensions and Physical
  weight each to the product type(s) of the products you intend to ship.
  - Note: Shippo configuration form will be accessible to only the user roles
  with 'administer shipping' permission enabled.


4. **Dependencies**

  - Commerce module (https://www.drupal.org/project/commerce)
  - Commerce Shipping module (https://www.drupal.org/project/commerce_shipping)
  - Commerce Physical module (https://www.drupal.org/project/commerce_physical)
  - Commerce Checkout module (Comes packaged with Commerce module)
  - Libraries module (https://www.drupal.org/project/libraries)
  - Address Field Phone module (https://www.drupal.org/project/addressfield_phone)
  version 7.x-1.x-dev specifically.
  - shippo-php-client library (https://github.com/goshippo/shippo-php-client)


5. **Database configuration**

  - No initial database configuration needed.


6. **How to run tests**

  - This is a simple test case to demonstrate the shipping cost estimation and
  shipment label creation for an order involving purchase of a single product.
  - Create a commerce product with necessary fields filled in including Physical
  dimensions as well as Physical weight fields.
  - Make a custom view to show the list of all the enabled products with an
  Add to Cart link. Visit the view page.
  - Add the product to cart. Visit the cart page and proceed to checkout.
  - Enter a valid shipping address under Shipping Information section. Continue
  checkout. If you enter an invalid/unserviceable shipping address, you should
  get an error message and won't be able to proceed.
  - On the next page, i.e. 'shipping' page, select an appropriate shipping
  service from among the available ones. The shipping rate estimates fetched
  from Shippo are shown against the service. Proceed to review/payments page.
  - Complete the checkout. This module will purchase the shipping rate for the
  order in the background.
  - Currently, post-purchase shipment info has been exposed programatically
  through the following functions:
    - ```php
      shippo_retrieve_shipment_master_parcel_info(...)
      ```
    - ```php
      shippo_retrieve_shipment_slave_parcel_info(...)
      ```
  Utilizing these functions would require a little custom modular development.
  We hope to better expose this information in drupal's admin UI panel in
  future or as per the feature requests that come along on this module's issue
  queue.


7. **Limitations**

  - Similar to that of Commerce UPS module.
  - Single "Ship from" address for all products.
  - Doesn't ensure product dimensions are less than default package size
  dimensions. In other words, if you have a product that is 1x1x20 (volume=20)
  and your default package size is 5x5x5 (volume=125), even though the product
  won't physically fit in the box, these values will be used to calculate the
  shipping estimate.
  - Doesn't play Tetris. For example, if you have an order with 14 products with
  a combined volume of 50 and your default package size has a volume of 60, the
  shipping estimate will be for a single box regardless of if due to the
  packaging shape they don't actually fit in the box.
  - Doesn't limit the weight of packages. If you're trying to ship a box full of
  lead that weighs 600lbs, this module will let you (instead of breaking the
  order into more packages).
  - Doesn't account for packing material. If you need to account for packing
  material, then you may want to adjust product dimensions accordingly.


8. **Methodology**

Calculating estimated shipping costs is a tricky business, and it can get
really complicated really quickly. Knowing this, we purposely designed this
module with simplicity in mind. Here's how it works:

  - Similar to that of Commerce UPS module.
  - Every order must contain at least one package.
  - The number of packages is determined by calculating the total volume of all
  products in the order, dividing by the volume of the default package size,
  and rounding up.
  - The weight of each package is determined by dividing the total weight of all
  products in the order by the number of packages.

If you need custom functionality, you have several options:

  - Determine if it is something that can be generalized to suit a number of
  users and submit it via the issue queue as a suggestion for inclusion in
  this module.
  - Hire one of the maintainers to create a custom module that interfaces with
  Commerce UPS to add your custom functionality.
  - Break open a text editor and start coding your own custom module.


### Contribution guidelines ###

* Writing tests

It would be great if you can contribute test cases for this module. Need help
writing test cases? Visit https://www.drupal.org/docs/7/testing/unit-testing-with-simpletest


* Code review

This module's code has been through a basic review by the Coder module - another
great module btw. On top of that, this code has been thoroughly assessed by
a few experienced minds - my mentors at QED42 Engg. Pvt. Ltd. - ofcourse the
one's who eagerly helped me out throughout the module's development.


### Authors/Maintainers/Contributors ###

* Project author/maintainer : Prashant Singhal - https://www.drupal.org/u/prashant-singhal
  (mailto:prashant.singhal@qed42.com)
* Project supervisor : Dipen Chaudhary - https://www.drupal.org/u/dipen-chaudhary
  (mailto:dipen@qed42.com)
