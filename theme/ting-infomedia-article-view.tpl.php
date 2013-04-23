<div  class="infomedia-article">
  <div class="infomedia-image">
    <?php print theme('image', array('path' => drupal_get_path('module', 'ting_infomedia') . '/img/ting_infomedia_logo.jpg')); ?>
  </div>
  <?php echo $variables['element']['#markup']; ?>
  <div class="infomedia-footer">
    <p><?php print t('infomedia_copyright_notice', array(), array('context' => 'ting_infomedia')); ?></p>
  </div>
</div>