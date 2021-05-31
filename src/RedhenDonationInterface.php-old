<?php

namespace Drupal\redhen_donation;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;

/**
 * Provides an interface for defining Redhen donation entities.
 *
 * @ingroup redhen_donation
 */
interface RedhenDonationInterface extends ContentEntityInterface, EntityChangedInterface {
  /**
   * Gets the Redhen Donation type.
   *
   * @return string
   *   The Redhen Donation type.
   */
  public function getType();


  /**
   * Sets the Contact name.
   *
   * @param string $name
   *   The Contact name.
   *
   * @return \Drupal\redhen_donation\RedhanDonationInterface
   *   The called Contact entity.
   */
  public function setName($name);

  /**
   * Gets the Redhen Donation creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Redhen Donation.
   */
  public function getCreatedTime();

  /**
   * Sets the Redhen Donation creation timestamp.
   *
   * @param int $timestamp
   *   The Redhen Donation creation timestamp.
   *
   * @return \Drupal\redhen_donation\RedhanDonationInterface
   *   The called Redhen Donation entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns a label for the contact.
   */
  public function label();

  /**
   * Returns the Contact active status indicator.
   *
   * @return bool
   *   TRUE if the Contact is active.
   */
  // public function isActive();

  /**
   * Sets the active status of a Contact.
   *
   * @param bool $active
   *   TRUE to set this Contact to active, FALSE to set it to inactive.
   *
   * @return \Drupal\redhen_donation\RedhanDonationInterface
   *   The called Contact entity.
   */
  // public function setActive($active);

}
