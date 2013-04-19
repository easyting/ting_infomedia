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
        $link = _ting_infomedia_get_link($entity->ding_entity_id, $relation->relationUri);
        $element[$delta]['#markup'] = theme('ting_infomedia_button', array('ting_infomedia_button' => $link));
      }
    }
  }

  return $element;
}


/**
 * Set parameters for Infomedia link.
 * Check if user is a ding_user, and has access to Infomedia
 *
 * @param $id
 * @param $relationUri
 * @return array
 */
function _ting_infomedia_get_link($id, $relationUri) {

  $title = t('infomedia_link_text', array(), array('context' => 'ting_infomedia'));

  $user_status = _ting_infomedia_get_user_status();

  $link = array(
    '#href' => _ting_infomedia_get_href($id, $user_status, $relationUri),
    '#title' => $title,
    '#popup' => (($user_status == 'user_is_infomedia_user') ? 'bibdk-popup-link ' : ''),
    '#data-pid' => $id,
    '#user-message' => _ting_infomedia_get_user_message($user_status, $relationUri),
  );

  return $link;
}


/**
 * @return user status
 */
function _ting_infomedia_get_user_status() {
  global $user;

  if ( !ding_user_is_provider_user($user) ) {
    return 'user_is_not_provider_user';
  }

  $favourites = _ting_infomedia_get_favourites();

  if (empty($favourites)) {
    return 'user_has_no_favourite_library';
  }

  $libraryCode = $userId = $userPinCode = NULL;

  foreach ($favourites as $fav) {
    $favourite = unserialize($fav);
    if ( $favourite->getOrderAgency() ) {
      $libraryCode = $favourite->getAgencyId();
      $userId = $favourite->getUserId();
      $userPinCode = $favourite->getPinCode();
      break;
    }
  }

  if ( !$libraryCode && !$userId && !$userPinCode ) {
    return 'user_no_userdata';
  }

  if ( !($libraryCode && $userId && $userPinCode) ) {
    return 'user_incomplete_userdata';
  }

  $libraryList = bibdk_navfors_get_library_list('infomedia');

  if ( !in_array($libraryCode, $libraryList) ) {
    return 'not_infomedia_library';
  }

  $libraryCode = ( (int)$libraryCode === 0 ) ? $libraryCode : 'DK-' . $libraryCode;
  $borchk = bibdk_borchk_request($userId, $userPinCode, $libraryCode);
  $borchk_message = _ting_infomedia_borchk_parse_response($borchk->borrowerCheckResponse->requestStatus);
  if ( $borchk_message ) {
    return t($borchk_message, array(), array('context' => 'ting_infomedia'));
  }

  return 'user_is_infomedia_user';
}


/**
 * Parse response from borchk and return messages
 *
 * @param string $response
 * @return array
 */
function _ting_infomedia_borchk_parse_response($response) {
  dpm($response);
  switch ($response) {
    case 'ok' :
      return;
      break;
    case 'borrower_not_found' :
      return 'borchk_borrower_not_found';
      break;
    case 'error_in_request' :
    case 'service unavailable' :
    case 'borrowercheck_not_allowed' :
    default :
      return 'borchk_internal_error';
  }
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
function _ting_infomedia_get_user_message($user_status, $relationUri) {
  global $user;

  switch ( $user_status ) {
    CASE 'user_is_not_provider_user': // link to login
          return l(t($user_status, array(), array('context' => 'ting_infomedia')), 'user');
          break;
    CASE 'user_has_no_favourite_library': // link to favorites
    CASE 'user_no_userdata':
    CASE 'user_incomplete_userdata':
          return l( t($user_status, array(), array('context' => 'ting_infomedia')), 'user/' . $user->uid . '/edit/bibdk_favourite_list');
          break;
    CASE 'not_infomedia_library':
          return
            l( t($user_status, array(), array('context' => 'ting_infomedia')), 'user/' . $user->uid . '/edit/bibdk_favourite_list') . '<br/>' .
            l( t('infomedia_library_list', array(), array('context' => 'ting_infomedia')), 'overlay/infomedia/library_list', array('attributes'=>array('rel'=>'help', 'class'=>array('bibdk-popup-link'))));
          break;
    DEFAULT:
          return  t($user_status, array(), array('context' => 'ting_infomedia'));
  }

}


/**
 * Get infomedia link href: Local anchor for error message, or link to popup.
 *
 * @param $id
 * @param $user_status
 * @param $relationUri
 *
 * @return string
 */
function _ting_infomedia_get_href($id, $user_status, $relationUri) {

  switch ( $user_status ) {
    CASE 'user_is_infomedia_user':
          // NOTE url is of type   [useraccessinfomedia]?action=getArticle&faust=89724244&libraryCode=[libraryCode]&userId=[userId]&userPinCode=[userPinCode]
          $parts = explode('&', $relationUri);
          foreach ( $parts as $part ) {
            $part = explode('=', $part);
            if ( $part[0] == 'faust' ) {
              $faust = $part[1];
            }
          }
          return url('overlay/infomedia/' . $faust); break;
    DEFAULT:
          return '#' . $id;
  }

}


// translate status strings, so they are available when the module is enabled.
t('user_is_not_provider_user', array(), array('context' => 'ting_infomedia'));
t('user_has_no_favourite_library', array(), array('context' => 'ting_infomedia'));
t('user_no_userdata', array(), array('context' => 'ting_infomedia'));
t('user_incomplete_userdata', array(), array('context' => 'ting_infomedia'));
t('user_is_infomedia_user', array(), array('context' => 'ting_infomedia'));
t('infomedia_library_list', array(), array('context' => 'ting_infomedia'));

t('borchk_borrower_not_found', array(), array('context' => 'ting_infomedia'));
t('borchk_internal_error', array(), array('context' => 'ting_infomedia'));
