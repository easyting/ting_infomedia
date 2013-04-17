<?php

/**
 * Field definition for ting_infomedia_manifestation_link is moved to
 * ting_infomedia_field module in order to be able to disable modules that
 * implements the field.
 * see http://drupal.org/node/1285540
 */

/**
 * Implements hook_field_formatter_info().
 */
function ting_infomedia_field_formatter_info() {
  return array(
    'ting_infomedia_link_default' => array(
      'label' => t('Default'),
      'field types' => array(
        'ting_infomedia_link',
      ),
    )
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function ting_infomedia_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  foreach ($items as $delta => $item) {
    foreach ($entity->manifestation->relationsData as $relation) {
      if ($relation->relationType == 'dbcaddi:hasOnlineAccess' && $entity->manifestation->main->type->{'$'} == 'Avisartikel') {
        // $relation->relationUri:
        // [useraccessinfomedia]?action=getArticle&faust=89724244&libraryCode=[libraryCode]&userId=[userId]&userPinCode=[userPinCode]
        $link = ting_infomedia_get_link($entity->ding_entity_id, $relation->relationUri);
        $element[$delta]['#markup'] = theme('ting_infomedia_button', array('ting_infomedia_button' => $link));
      }
    }
  }

  return $element;
}

/**
 * @param $url
 * @internal param $id
 * @return array
 */
function ting_infomedia_get_link($url) {
  $title = t('infomedia_link_text', array(), array('context' => 'ting_infomedia'));

  $link = array(
    '#href' => ting_infomedia_convert_url_to_link($url),
    '#title' => $title,
    'data-pid' => $id,
    'user-message' => ting_infomedia_get_user_message(),
  );
  return $link;
}

function ting_infomedia_convert_url_to_link($url) {
  // NOTE url is of type   [useraccessinfomedia]?action=getArticle&faust=89724244&libraryCode=[libraryCode]&userId=[userId]&userPinCode=[userPinCode]
  $wsurl = variable_get('ting_infomedia_url', FALSE);
  $favourites = _ting_infomedia_get_favourites();
  $libraryCode = NULL;
  $userId = NULL;
  $userPinCode = NULL;

  foreach ($favourites as $fav) {
    $favourite = unserialize($fav);
    if ($favourite->getOrderAgency()) {
      $libraryCode = $favourite->getAgencyId();
      // userId can be cpr, userId, customId
      //      $userId = $favourite->getUserId();
      //      $userPinCode = $favourite->getPinCode();
      break;
    }
  }
  if ($libraryCode && $userId && $userPinCode) {
    $ret = str_replace(array('[useraccessinfomedia]', '[libraryCode]', '[userId]', '[userPinCode]'), array($wsurl, $libraryCode, $userId, $userPinCode), $url);
  }
  return $ret;
}

/**
 * @return TingClientAgencyBranch
 */
function _ting_infomedia_get_favourites() {
  if (isset($_SESSION['bibdk_favourites'])) {
    $favourites = $_SESSION['bibdk_favourites'];
  }
  else {
    $favourites = _bibdk_favourite_get_from_webservice();
    foreach ($favourites as $favourite) {
      bibdk_favourite_add_favourite_to_session($favourite);
    }
  }
  return $favourites;
}

/**
 * Determine user message, depending on logged in status
 *
 * @return String
 */
function ting_infomedia_get_user_message() {
  global $user;

  if (!ding_user_is_provider_user($user)) {
    return l(t('user_is_not_provider_user', array(), array('context' => 'ting_infomedia')), 'user');
  }

  return t('user_is_provider_user', array(), array('context' => 'ting_infomedia'));
}

