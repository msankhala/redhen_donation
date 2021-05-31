<?php
namespace Drupal\redhen_donation\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_donation\RedhenDonationTypeInterface;

/**
 * Defines the Redhen Donation type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_donation_type",
 *   label = @Translation("Redhen Donation type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_donation\RedhenDonationTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_donation\Form\RedhenDonationTypeForm",
 *       "edit" = "Drupal\redhen_donation\Form\RedhenDonationTypeForm",
 *       "delete" = "Drupal\redhen_donation\Form\RedhenDonationTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_donation\RedhenDonationTypeHtmlRouteProvider",
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

  /**
   * The Redhen Donation type name.
   *
   * @var string
   */
  protected $name;

  /**
   * The Redhen Donation type label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Redhen Donation type locked state.
   *
   * @var string
   */
  protected $locked;
}
