<?php /**
 * @file
 * Contains \Drupal\redhen_donation\Plugin\Field\FieldFormatter\RedhenDonationLink.
 */

namespace Drupal\redhen_donation\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;

/**
 * @FieldFormatter(
 *  id = "redhen_donation_link",
 *  label = @Translation("Donation Link"),
 *  field_types = {"redhen_donation"}
 * )
 */
class RedhenDonationLink extends FormatterBase {

  /**
   * @FIXME
   * Move all logic relating to the redhen_donation_link formatter into this
   * class. For more information, see:
   *
   * https://www.drupal.org/node/1805846
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterInterface.php/interface/FormatterInterface/8
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterBase.php/class/FormatterBase/8
   */

}
