<?php
namespace Drupal\redhen_donation;

use Drupal\redhen_donation\RedhenDonationTypeInterface;

/**
 * Defines the Redhen Donation type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_donation_type",
 *   label = @Translation("Redhen Donation type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_donation\ContactTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_donation\Form\ContactTypeForm",
 *       "edit" = "Drupal\redhen_donation\Form\ContactTypeForm",
 *       "delete" = "Drupal\redhen_donation\Form\ContactTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_donation\ContactTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_donation_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_donation",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/redhen-donation-type/{redhen_donation_type}",
 *     "add-form" = "/admin/structure/redhen/redhen-donation-type/add",
 *     "edit-form" = "/admin/structure/redhen/redhen-donation-type/{redhen_donation_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/redhen-donation-type/{redhen_donation_type}/delete",
 *     "collection" = "/admin/structure/redhen/redhen-donation-type"
 *   }
 * )
 */
class RedhenDonationType extends ConfigEntityBundleBase implements RedhenDonationTypeInterface {
}
class RedhenDonationType extends Entity {

  public $name;
  public $label;
  public $locked;

  /**
   * Type constructor.
   *
   * @param array $values
   *   Values array.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_donation_type');
  }

}
